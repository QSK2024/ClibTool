#include <iostream>
#include <windows.h>
#include <string>
using namespace std;

// 纯动态加载，不依赖任何 .h 文件！
typedef int (*PFN_CLib_Pack)(const char*,const char*,const char*,const char*);
typedef int (*PFN_CLib_Extract)(const char*,const char*);
typedef char* (*PFN_CLib_ReadInfoInMem)(const char*);
typedef void* (*PFN_CLib_ReadFileInMem)(const char*,const char*,size_t*);
typedef void (*PFN_CLib_FreeString)(char*);
typedef void (*PFN_CLib_FreeMem)(void*);

int main(int argc, char* argv[])
{
    // 修复中文乱码（最终版必备）
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    cout << "==== CLib 打包/解压/内存预览工具 最终修复版 ====" << endl;

    HMODULE hDll = LoadLibraryA("CLibCore.dll");
    if(hDll == nullptr)
    {
        cout << "[错误] 找不到 CLibCore.dll，请与程序同级目录放置！" << endl;
        system("pause");
        return 1;
    }

    PFN_CLib_Pack fnPack = (PFN_CLib_Pack)GetProcAddress(hDll,"CLib_Pack");
    PFN_CLib_Extract fnExtract = (PFN_CLib_Extract)GetProcAddress(hDll,"CLib_Extract");
    PFN_CLib_ReadInfoInMem fnReadInfo = (PFN_CLib_ReadInfoInMem)GetProcAddress(hDll,"CLib_ReadInfoInMem");
    PFN_CLib_ReadFileInMem fnReadFile = (PFN_CLib_ReadFileInMem)GetProcAddress(hDll,"CLib_ReadFileInMem");
    PFN_CLib_FreeString fnFreeStr = (PFN_CLib_FreeString)GetProcAddress(hDll,"CLib_FreeString");
    PFN_CLib_FreeMem fnFreeMem = (PFN_CLib_FreeMem)GetProcAddress(hDll,"CLib_FreeMem");

    if(!fnPack || !fnExtract || !fnReadInfo || !fnReadFile || !fnFreeStr || !fnFreeMem)
    {
        cout << "[错误] DLL函数加载不完整！" << endl;
        FreeLibrary(hDll);
        system("pause");
        return 1;
    }

    if(argc < 2)
    {
        cout << "使用方法：" << endl;
        cout << "  CLibTool.exe pack info.txt real.jpg pin.png out.clib" << endl;
        cout << "  CLibTool.exe extract xxx.clib 输出目录" << endl;
        cout << "  CLibTool.exe preview xxx.clib" << endl;
        FreeLibrary(hDll);
        system("pause");
        return 0;
    }

    string cmd = argv[1];

    if(cmd == "pack")
    {
        if(argc != 6)
            cout << "参数错误！pack 需要 4 个资源参数" << endl;
        else
        {
            int ret = fnPack(argv[2],argv[3],argv[4],argv[5]);
            ret == 0 ? cout << "✅ 打包成功：" << argv[5] << endl : cout << "❌ 打包失败，错误码：" << ret << endl;
        }
    }
    else if(cmd == "extract")
    {
        if(argc != 4)
            cout << "参数错误！extract 需要 clib文件 + 输出目录" << endl;
        else
        {
            int ret = fnExtract(argv[2],argv[3]);
            ret == 0 ? cout << "✅ 解压成功，目录已自动创建：" << argv[3] << endl : cout << "❌ 解压失败" << endl;
        }
    }
    else if(cmd == "preview")
    {
        if(argc != 3)
            cout << "参数错误！preview 需要 clib文件" << endl;
        else
        {
            cout << "\n----- 内存预览模式（不写入磁盘）-----" << endl;
            char* info = fnReadInfo(argv[2]);
            if(info)
            {
                cout << "📄 info.txt 内容：\n" << info << endl;
                fnFreeStr(info);
            }
            else
            {
                cout << "! 未读取到 info.txt 内容" << endl;
            }

            size_t sz;
            void* jpg = fnReadFile(argv[2], "real.jpg", &sz);
            if(jpg)
            {
                cout << "✅ real.jpg 载入内存，大小：" << sz << " 字节" << endl;
                fnFreeMem(jpg);
            }

            void* png = fnReadFile(argv[2], "pin.png", &sz);
            if(png)
            {
                cout << "✅ pin.png 载入内存，大小：" << sz << " 字节" << endl;
                fnFreeMem(png);
            }
            cout << "------------------------------------\n";
        }
    }
    else
    {
        cout << "未知命令！" << endl;
    }

    FreeLibrary(hDll);
    system("pause");
    return 0;
}
