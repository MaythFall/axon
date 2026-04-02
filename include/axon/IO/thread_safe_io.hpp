#pragma once
#include <errors.hpp>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <shobjidl.h>
#include <WinSock2.h>
#include <windows.h>
#include <winnt.h>
#include <io.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

namespace axon {

    inline static size_t read_file(uint8_t *buffer, int fd, size_t offset, size_t length) {
        #ifdef _WIN32
        HANDLE hFile = (HANDLE)_get_osfhandle(fd);
        if (hFile == INVALID_HANDLE_VALUE) throw axon::io_error("Invalid file handle");

        size_t totalBytesRead = 0;
        
        while (totalBytesRead < length) {
            OVERLAPPED overlapped = {0};
            size_t currentOffset = offset + totalBytesRead;
            overlapped.Offset = static_cast<DWORD>(currentOffset);
            overlapped.OffsetHigh = static_cast<DWORD>(currentOffset >> 32);

            size_t remaining = length - totalBytesRead;
            DWORD toRead = (remaining > 0xFFFFFFFF) ? 0xFFFFFFFF : static_cast<DWORD>(remaining);
            DWORD bytesRead = 0;

            if (!ReadFile(hFile, buffer + totalBytesRead, toRead, &bytesRead, &overlapped)) {
                DWORD err = GetLastError();
                if (err == ERROR_IO_PENDING) {
                    if (!GetOverlappedResult(hFile, &overlapped, &bytesRead, TRUE)) {
                        throw axon::io_error("GetOverlappedResult failed");
                    }
                } else if (err == ERROR_HANDLE_EOF) {
                    break;
                } else {
                    throw axon::io_error("Windows ReadFile failed");
                }
            }

            if (bytesRead == 0) break;
            totalBytesRead += bytesRead;
        }
        return totalBytesRead;

        #else
        ssize_t bytesRead = pread(fd, buffer, length, static_cast<off_t>(offset));
        if (bytesRead < 0) {
            if (errno == EINTR) return read_file(buffer, fd, offset, length);
            throw axon::io_error("pread failed");
        }
        return static_cast<size_t>(bytesRead);
        #endif
    }

    inline static size_t write_file(uint8_t *buffer, int fd, size_t offset, size_t length, bool last) {
        #ifdef _WIN32
        HANDLE hFile = (HANDLE)_get_osfhandle(fd);
        if (hFile == INVALID_HANDLE_VALUE) throw axon::io_error("Invalid file handle");
        size_t totalWritten = 0;

        while (totalWritten < length) {
            OVERLAPPED overlapped = {0};
            size_t currentOffset = offset + totalWritten;
            overlapped.Offset = static_cast<DWORD>(currentOffset);
            overlapped.OffsetHigh = static_cast<DWORD>(currentOffset >> 32);

            size_t remaining = length - totalWritten;
            DWORD toWrite = (remaining > 0xFFFFFFFF) ? 0xFFFFFFFF : static_cast<DWORD>(remaining);
            DWORD bytesWritten = 0;

            if (!WriteFile(hFile, buffer + totalWritten, toWrite, &bytesWritten, &overlapped)) {
                DWORD err = GetLastError();
                if (err == ERROR_IO_PENDING) {
                    if (!GetOverlappedResult(hFile, &overlapped, &bytesWritten, TRUE)) {
                        throw axon::io_error("Windows GetOverlappedResult failed during write");
                    }
                } else {
                    throw axon::io_error("Windows WriteFile failed");
                }
            }

            if (bytesWritten == 0 && toWrite > 0) throw axon::io_error("Windows WriteFile wrote 0 bytes (Disk Full?)");
            totalWritten += bytesWritten;
        }

        if (last) {
            FlushFileBuffers(hFile);
        }

        return totalWritten;
        #else
        ssize_t totalWritten = 0;
        while (static_cast<size_t>(totalWritten) < length) {
            ssize_t written = pwrite(fd, buffer + totalWritten, length - totalWritten, static_cast<off_t>(offset + totalWritten));
            
            if (written < 0) {
                if (errno == EINTR) continue;
                throw axon::io_error("pwrite failed");
            } 
            
            if (written == 0) throw axon::io_error("pwrite wrote 0 bytes (Disk Full?)");
            totalWritten += written;
        }

        if (last) {
            fsync(fd);
        }

        return static_cast<size_t>(totalWritten);
        #endif
    }
}