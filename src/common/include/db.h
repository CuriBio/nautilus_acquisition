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
 * @file  db.h
 *
 * @brief Defines class for performing sqlite3 DB operations
 *********************************************************************/
#ifndef DATABASE_H
#define DATABASE_H


#include <stdlib.h>
#include <sqlite3.h>
#include <spdlog/spdlog.h>
#include <tsl/ordered_map.h>

typedef tsl::ordered_map<std::string, std::string> dbRow;

class Database {
    private:
        sqlite3 *db;
        
    public:
        Database() {
            std::string dbFilePath = "TODO";
            spdlog::info("Opening DB connection");
            rc = sqlite3_open(dbFilePath, &db);
            if (rc) {
                spdlog::error("Failed to open DB connection");
            }
            initDB();
        }

        ~Database() {
            spdlog::info("Closing DB connection");
            sqlite3_close(db);
        }

        std::vector<dbRow> getPlateIDs() {
            std::string query = "SELECT plate_id FROM background_recordings ORDER BY updated_at DESC;";
            return exec(query, std::vector<std::string> {});
        }

        void addPlateID(
            std::string plateId,
            std::string filePath,
            std::string plateFormat,
        ) {
            spdlog::info("Adding plate ID {} with format {}", plateId, plateFormat);
            std::string query = "INSERT INTO background_recordings VALUES (?, ?, ?, ?, ?);";
            exec(query, std::vector<std::string> {plateId, filePath, now_timestamp(), now_timestamp(), plateFormat});
        }

        void overwritePlateID(std::string plateId, std::string plateFormat) {
            // TODO does this need to update the file path?
            spdlog::info("Overwriting plate ID {} to format {}", plateId, plateFormat);
            std::string query = "UPDATE background_recordings SET plate_format=?, updated_at=? WHERE plate_id=?;";
            exec(query, std::vector<std::string> {plateFormat, now_timestamp(), plateId});
        }
    
    private:
        initDB() {
            spdlog::info("Initializing DB");
            std::string query = "CREATE TABLE IF NOT EXISTS background_recordings(",
                "plate_id TEXT PRIMARY KEY, "
                "file_path TEXT NOT NULL, "
                "created_at TEXT NOT NULL, "
                "updated_at TEXT NOT NULL, "
                "plate_format TEXT NOT NULL); ";
            
            exec(query, std::vector<std::string> {});
        }

        std::vector<dbRow> exec(std::string query, std::vector<std::string> params) {
            sqlite3_stmt *pStmt;
            char *pzTail;
            int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, &pzTail);
            if (rc != SQLITE_OK) {
                spdlog::error("SQL prepare failed for query: {}", query);
            }

            int paramIdx = 1;
            for (const auto& s: params) {
                rc = sqlite3_bind_text(pStmt, paramIdx, s.c_str(), -1, SQLITE_STATIC);
                if (rc != SQLITE_OK) {
                    spdlog::error("SQL bind text failed for param {} ({}) of query: {}", s, paramIdx, query);
                }
                paramIdx++;
            }

            std::vector<dbRow> results {};

            rc = sqlite3_step(pStmt);
            while (rc == SQLITE_ROW) {
                dbRow row {};

                int numCols = sqlite3_column_count(pStmt);
                for (int colIdx = 0; colIdx < numCols; colIdx++) {
                    std::string colName = sqlite3_column_name(pStmt, colIdx);
                    std::string colVal = sqlite3_column_text(pStmt, colIdx);
                    row[colName] = colVal;
                }
                results.push(row);

                rc = sqlite3_step(pStmt);
            }
            if (rc != SQLITE_DONE) {
                spdlog::error("SQL step failed for query: {}", query);
            }

            rc = sqlite3_finalize(pStmt);
            if (rc != SQLITE_OK) {
                spdlog::error("SQL finalize failed for query: {}", query);
            }

            return results;
        }

        std::string now_timestamp() {
            std::string fmt = "yyyy-mm-dd hh:mm:ss";
            char timeString[std::size(fmt)];

            auto now = std::chrono::system_clock::now();
            auto timestamp = std::chrono::system_clock::to_time_t(now);
            std::tm *tm = std::localtime(&timestamp);
            std::strftime(std::data(timeString), std::size(timeString), fmt, tm);

            return timeString;
        }
}
#endif // DATABASE_H