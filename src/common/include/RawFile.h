#ifndef __RAW_FILE__H
#define __RAW_FILE__H
#include <string>

#include <spdlog/spdlog.h>

#ifndef _WIN32
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#else
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#endif

#define PWRITES 6

class RawFile {
    private:
#ifndef _WIN32
        int m_fd {-1};
#else
        HANDLE m_fd;
        HANDLE m_hEvents[PWRITES];
        OVERLAPPED m_ovs[PWRITES];
#endif
        std::filesystem::path m_file;
        uint64_t m_width;
        uint64_t m_height;
        uint16_t m_count;
        uint8_t m_bitDepth;
        uint64_t m_fileSize;
        uint16_t* m_buf;

    public:
        RawFile(std::filesystem::path file, uint8_t bitDepth, uint16_t width, uint16_t height, uint32_t count) {
            spdlog::info("Opening raw file: {}", file.string());
            m_width = width;
            m_height = height;
            m_bitDepth = bitDepth;
            m_count = count;

#ifndef _WIN32
            m_fd = open(file.string().c_str(), O_WRONLY | O_CREAT, 0640);
            if (m_fd < 0) {
                spdlog::info("Async open error: {}", errno);
            }
#else
            m_fd = CreateFileA(file.string().c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_FLAG_OVERLAPPED, NULL);
            if (m_fd == INVALID_HANDLE_VALUE) {
                spdlog::error("Error opening file: {}", GetLastError());
            }

            for (auto i = 0; i < PWRITES; i++) {
                m_hEvents[i] = CreateEventA(NULL, FALSE, FALSE, NULL);
            }
#endif
        }

        ~RawFile() { };

        void Close() {
#ifndef _WIN32
            close(m_fd);
#else
            CloseHandle(m_fd);
            for (auto i = 0; i < PWRITES; i++) {
                CloseHandle(m_hEvents[i]);
            }
#endif
        };

        size_t Write(void* data, uint64_t idx) {
#ifndef _WIN32
            return pwrite(m_fd, data, m_width * m_height * (m_bitDepth / 8), ((m_bitDepth / 8) * m_width * m_height * idx));
#else

            DWORD chunk = ((m_bitDepth / 8) * m_width * m_height) / PWRITES;
            DWORD chunkRem = ((m_bitDepth / 8) * m_width * m_height) % PWRITES;
            uint64_t fileOffset = idx * static_cast<uint64_t>(m_bitDepth / 8) * m_width * m_height;

            DWORD wrote = 0;
            BOOL ovRes;

            auto _write = [&](uint64_t _idx, DWORD _chunk, DWORD count) {
                ULARGE_INTEGER uli;
                uli.QuadPart = fileOffset + static_cast<uint64_t>(_idx*count);

                m_ovs[_idx].Offset = uli.LowPart;
                m_ovs[_idx].OffsetHigh = uli.HighPart;
                m_ovs[_idx].hEvent = m_hEvents[_idx];

                WriteFile(m_fd, static_cast<uint8_t*>(data)+(_idx*_chunk), count, NULL, &m_ovs[_idx]);
                ovRes = GetOverlappedResult(m_fd, &m_ovs[_idx], &wrote, FALSE);
                if (ovRes == 0) {
                    DWORD lastError = GetLastError();
                    switch (lastError) {
                        case ERROR_IO_INCOMPLETE:
                            break;
                        default:
                            spdlog::error("GetOverlappedResult error: {}", lastError);
                    };
                }

            };

            for (uint64_t i = 0; i < PWRITES - 1; i++) {
                _write(i, chunk, chunk);
            }

            _write(PWRITES-1, chunk, chunk+chunkRem);

            if (WAIT_FAILED == WaitForMultipleObjects(PWRITES, m_hEvents, TRUE, INFINITE)) {
                spdlog::error("WaitForMultipleObjects failed: {}", GetLastError());
            }
            return chunk*PWRITES;
#endif
        }

};
#endif //__RAW_FILE__H