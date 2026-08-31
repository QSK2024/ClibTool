#include "miniz.h"

#include <windows.h>
#include <cstring>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

// 递归创建多级目录，不存在就新建
static bool MakeDirectoryRecursive(const char* pathA)
{
    std::string s(pathA);
    for(size_t i = 1; i < s.size(); i++)
    {
        if(s[i] == '\\')
        {
            char tmp[260];
            strncpy(tmp, s.c_str(), i);
            tmp[i]=0;
            CreateDirectoryA(tmp, nullptr);
        }
    }
    return CreateDirectoryA(pathA, nullptr) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

// 将磁盘单个文件添加进zip归档
static int AddSingleFile(mz_zip_archive* pZip, const char* diskFilePath, const char* zipInnerName)
{
    HANDLE hFile = CreateFileA(diskFilePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return 1;

    DWORD fSize = GetFileSize(hFile, nullptr);
    if (fSize == INVALID_FILE_SIZE)
    {
        CloseHandle(hFile);
        return 2;
    }

    void* buf = malloc(fSize);
    if (!buf)
    {
        CloseHandle(hFile);
        return 3;
    }

    DWORD readBytes;
    ReadFile(hFile, buf, fSize, &readBytes, nullptr);
    CloseHandle(hFile);

    bool ok = mz_zip_writer_add_mem(pZip, zipInnerName, buf, fSize, MZ_BEST_COMPRESSION);
    free(buf);

    return ok ? 0 : 4;
}

// CLib_Pack: 打包 info.txt real.jpg pin.png 输出为 clib
// 返回 0=成功，非0错误码
__declspec(dllexport) int CLib_Pack(const char* infoPath, const char* jpgPath, const char* pngPath, const char* outClib)
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(mz_zip_archive));

    if (!mz_zip_writer_init_file(&zip, outClib, 0))
        return 10;

    int ret;
    ret = AddSingleFile(&zip, infoPath, "info.txt");
    if(ret != 0) goto pack_end;

    ret = AddSingleFile(&zip, jpgPath, "real.jpg");
    if(ret != 0) goto pack_end;

    ret = AddSingleFile(&zip, pngPath, "pin.png");
    if(ret != 0) goto pack_end;

pack_end:
    mz_zip_writer_finalize_archive(&zip);
    mz_zip_writer_end(&zip);
    return ret;
}

// CLib_Extract：把clib全部解压到目标目录（自动创建目录）
__declspec(dllexport) int CLib_Extract(const char* clibPath, const char* destDir)
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(mz_zip_archive));

    MakeDirectoryRecursive(destDir);

    if (!mz_zip_reader_init_file(&zip, clibPath, 0))
        return 1;

    int totalFiles = (int)mz_zip_reader_get_num_files(&zip);
    for (int i = 0; i < totalFiles; i++)
    {
        mz_zip_archive_file_stat stat;
        if (!mz_zip_reader_file_stat(&zip, i, &stat))
            continue;

        size_t sz = stat.m_uncomp_size;
        char* buf = (char*)malloc(sz);
        if (!buf) continue;

        if (mz_zip_reader_extract_to_mem(&zip, i, buf, sz, 0))
        {
            std::string outPath = std::string(destDir) + "\\" + stat.m_filename;
            HANDLE hOut = CreateFileA(outPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hOut != INVALID_HANDLE_VALUE)
            {
                DWORD written;
                WriteFile(hOut, buf, sz, &written, nullptr);
                CloseHandle(hOut);
            }
        }
        free(buf);
    }

    mz_zip_reader_end(&zip);
    return 0;
}

// CLib_ReadInfoInMem: 在内存读取info.txt，返回字符串，用完必须调用CLib_FreeString释放
__declspec(dllexport) char* CLib_ReadInfoInMem(const char* clibPath)
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(mz_zip_archive));
    char* result = nullptr;

    if (!mz_zip_reader_init_file(&zip, clibPath, 0))
        return nullptr;

    int idx = mz_zip_reader_locate_file(&zip, "info.txt", nullptr, 0);
    if (idx >= 0)
    {
        mz_zip_archive_file_stat stat;
        if (mz_zip_reader_file_stat(&zip, idx, &stat))
        {
            size_t sz = stat.m_uncomp_size;
            result = (char*)malloc(sz + 1);
            if (result)
            {
                if (mz_zip_reader_extract_to_mem(&zip, idx, result, sz, 0))
                {
                    result[sz] = '\0';
                }
                else
                {
                    free(result);
                    result = nullptr;
                }
            }
        }
    }
    mz_zip_reader_end(&zip);
    return result;
}

// CLib_ReadFileInMem：内存读取包内指定文件名，返回内存指针，size输出大小；用完 CLib_FreeMem
__declspec(dllexport) void* CLib_ReadFileInMem(const char* clibPath, const char* innerFileName, size_t* outSize)
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(mz_zip_archive));
    void* ptr = nullptr;
    *outSize = 0;

    if (!mz_zip_reader_init_file(&zip, clibPath, 0))
        return nullptr;

    int idx = mz_zip_reader_locate_file(&zip, innerFileName, nullptr, 0);
    if (idx >= 0)
    {
        mz_zip_archive_file_stat stat;
        if (mz_zip_reader_file_stat(&zip, idx, &stat))
        {
            size_t sz = stat.m_uncomp_size;
            ptr = malloc(sz);
            if(ptr)
            {
                if(mz_zip_reader_extract_to_mem(&zip, idx, ptr, sz, 0))
                {
                    *outSize = sz;
                }
                else
                {
                    free(ptr);
                    ptr = nullptr;
                }
            }
        }
    }
    mz_zip_reader_end(&zip);
    return ptr;
}

// 释放字符串
__declspec(dllexport) void CLib_FreeString(char* str)
{
    free(str);
}

// 释放二进制内存块
__declspec(dllexport) void CLib_FreeMem(void* mem)
{
    free(mem);
}

#ifdef __cplusplus
}
#endif
