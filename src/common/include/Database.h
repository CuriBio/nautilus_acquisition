/*
 * MIT License
 *
 * Copyright (c) 2024 Curi Bio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*********************************************************************
 * @file  database.h
 *
 * @brief Defines class for performing sqlite3 DB operations
 *********************************************************************/
#ifndef DATABASE_H
#define DATABASE_H


#include <stdlib.h>
#include <spdlog/spdlog.h>
#include <tsl/ordered_map.h>
#include <sqlite3.h>

#define DB_TIMESTAMP_STR "%Y-%m-%d %H:%M:%S"

typedef tsl::ordered_map<std::string, std::string> dbRow;

struct execRes {
    bool success;
    std::vector<dbRow> rows;
};

class Database {
    private:
        std::string dbFilePath;
        sqlite3 *db {nullptr};

    public:
        Database(std::filesystem::path userProfile) {
            dbFilePath = (userProfile / "AppData" / "Local" / "Nautilai" / "nautilai.db").string();
        }

        ~Database() {
            if (db == nullptr) {
                return;
            }
            spdlog::info("Closing DB connection");
            int rc = sqlite3_close(db);
            if (rc) {
                spdlog::error("Failed to close DB connection");
            }
        }

        bool initDB() {
            if (db != nullptr) {
                spdlog::error("DB already initialized");
                return false;
            }
            spdlog::info("Opening DB connection");
            int rc = sqlite3_open(dbFilePath.c_str(), &db);
            if (rc != SQLITE_OK) {
                spdlog::error("Failed to open DB connection");
                return false;
            }

            spdlog::info("Initializing DB");
            std::string query = "CREATE TABLE IF NOT EXISTS background_recordings("
                "plate_id TEXT PRIMARY KEY, "
                "file_path TEXT NOT NULL, "
                "created_at TEXT NOT NULL, "
                "updated_at TEXT NOT NULL, "
                "plate_format TEXT NOT NULL); ";
            auto res = exec(query, std::vector<std::string> {});
            if (!res.success) {
                spdlog::error("Failed to initialize DB");
            }

            return res.success;
        }

        std::vector<std::string> getPlateIds() {
            std::vector<std::string> plateIds {};

            if (db == nullptr) {
                spdlog::error("Cannot get plate IDs, no DB connection");
                return plateIds;
            }

            std::string query = "SELECT plate_id FROM background_recordings ORDER BY updated_at DESC;";
            auto res = exec(query, std::vector<std::string> {});


            if (res.success) {
                for (auto& row : res.rows) {
                    plateIds.push_back(row["plate_id"]);
                }
            } else {
                spdlog::error("Failed to get plate IDs");
            }

            return plateIds;
        }

        void addPlateId(std::string plateId, std::string plateFormat, std::string filePath) {
            if (db == nullptr) {
                spdlog::error("Cannot add plate ID, no DB connection");
            }
            spdlog::info("Adding plate ID '{}' with format '{}'", plateId, plateFormat);
            std::string query = "INSERT INTO background_recordings VALUES (?, ?, ?, ?, ?);";
            auto res = exec(query, std::vector<std::string> {plateId, filePath, now_timestamp(), now_timestamp(), plateFormat});
            if (!res.success) {
                spdlog::error("Failed to add plate ID '{}' with format '{}'", plateId, plateFormat);
            }
        }

        void overwritePlateId(std::string plateId, std::string plateFormat) {
            if (db == nullptr) {
                spdlog::error("Cannot overwrite plate ID, no DB connection");
            }
            spdlog::info("Overwriting plate ID '{}' to format '{}'", plateId, plateFormat);
            std::string query = "UPDATE background_recordings SET plate_format=?, updated_at=? WHERE plate_id=?;";
            auto res = exec(query, std::vector<std::string> {plateFormat, now_timestamp(), plateId});
            if (!res.success) {
                spdlog::error("Failed to overwrite plate ID '{}' to format '{}'", plateId, plateFormat);
            }
        }

    private:
        execRes exec(std::string query, std::vector<std::string> params) {
            std::vector<dbRow> results {};

            if (db == nullptr) {
                spdlog::error("Cannot execute query, no DB connection");
                return {false, results};
            }

            sqlite3_stmt *pStmt;
            const char *pzTail;
            int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, &pzTail);

            if (rc != SQLITE_OK) {
                spdlog::error("SQL prepare failed for query: {}", query);
                return {false, results};
            }

            int paramIdx = 1;
            for (const auto& s : params) {
                rc = sqlite3_bind_text(pStmt, paramIdx, s.c_str(), -1, SQLITE_STATIC);

                if (rc != SQLITE_OK) {
                    spdlog::error("SQL bind text failed for param {} ({}) of query: {}", s, paramIdx, query);
                    return {false, results};
                }

                paramIdx++;
            }

            rc = sqlite3_step(pStmt);
            while (rc == SQLITE_ROW) {
                dbRow row {};

                int numCols = sqlite3_column_count(pStmt);
                for (int colIdx = 0; colIdx < numCols; colIdx++) {
                    std::string colName = sqlite3_column_name(pStmt, colIdx);
                    // Tanner (3/6/24): this probably won't work with unicode, need to figure out how to handle that or prevent it from being entered
                    std::string colVal = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, colIdx));
                    row[colName] = colVal;
                }
                results.push_back(row);

                rc = sqlite3_step(pStmt);
            }
            if (rc != SQLITE_DONE) {
                spdlog::error("SQL step failed for query: {}", query);
                return {false, results};
            }

            rc = sqlite3_finalize(pStmt);
            if (rc != SQLITE_OK) {
                spdlog::error("SQL finalize failed for query: {}", query);
                return {false, results};
            }

            return {true, results};
        }

        std::string now_timestamp() {
            char timeString[std::size(DB_TIMESTAMP_STR) + 4] = {};

            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::system_clock::to_time_t(now);
            std::tm *tm = std::localtime(&timestamp);
            std::strftime(std::data(timeString), std::size(timeString), DB_TIMESTAMP_STR, tm);

            return timeString;
        }
};
#endif // DATABASE_H
