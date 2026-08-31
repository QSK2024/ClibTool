#include <iostream>
#include <windows.h>
#include <string>
#include <cstring>

using namespace std;

//DLL函数指针定义
typedef int (*PFN_CLib_Pack)(const char*,const char*,const char*,const char*);
typedef int (*PFN_CLib_Extract)(const char*,const char*);
typedef char* (*PFN_CLib_ReadInfoInMem)(const char*);
typedef void* (*PFN_CLib_ReadFileInMem)(const char*,const char*,size_t*);
typedef void (*PFN_CLib_FreeString)(char*);
typedef void (*PFN_CLib_FreeMem)(void*);

static PFN_CLib_Pack fnPack = nullptr;
static PFN_CLib_Extract fnExtract = nullptr;
static PFN_CLib_ReadInfoInMem fnReadInfo = nullptr;
static PFN_CLib_ReadFileInMem fnReadFile = nullptr;
static PFN_CLib_FreeString fnFreeStr = nullptr;
static PFN_CLib_FreeMem fnFreeMem = nullptr;

//判断后缀是否.clib
bool IsClibFile(const char* path)
{
    if (!path) return false;
    const char* p = strrchr(path, '.');
    if (!p) return false;
    return _stricmp(p, ".clib") == 0;
}

//读取clib，控制台打印info（中英文提示）
void ShowClibText(const char* clibFilePath)
{
    cout << "========================================" << endl;
    cout << "CLib 文件文本预览 | CLib File Text Preview" << endl;
    cout << "文件路径 | File Path: " << clibFilePath << endl;
    cout << "----------------------------------------" << endl;

    char* infoUtf8 = fnReadInfo(clibFilePath);
    if(infoUtf8 != nullptr)
    {
        cout << "【info.txt 内容 | Content】" << endl;
        cout << infoUtf8 << endl;
        fnFreeStr(infoUtf8);
    }
    else
    {
        cout << "错误：读取info失败！| Error: Failed to read info!" << endl;
    }

    size_t imgSize;
    //只打印图片大小信息，不渲染图片
    void* jpgBuf = fnReadFile(clibFilePath, "real.jpg", &imgSize);
    if(jpgBuf != nullptr)
    {
        cout << endl;
        cout << "real.jpg : " << imgSize << " 字节 | bytes (图片暂未预览 | Image preview disabled)" << endl;
        fnFreeMem(jpgBuf);
    }
    void* pngBuf = fnReadFile(clibFilePath, "pin.png", &imgSize);
    if(pngBuf != nullptr)
    {
        cout << "pin.png  : " << imgSize << " 字节 | bytes (图片暂未预览 | Image preview disabled)" << endl;
        fnFreeMem(pngBuf);
    }

    cout << "========================================" << endl;
    cout << endl;
    cout << "按任意键退出... | Press any key to exit..." << endl;
    system("pause");
}


int main(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HMODULE hDll = LoadLibraryA("CLibCore.dll");
    if(hDll == nullptr)
    {
        cout << "[错误] 同目录找不到 CLibCore.dll ! | ERROR: Cannot find CLibCore.dll!" << endl;
        system("pause");
        return 1;
    }

    fnPack      = (PFN_CLib_Pack)GetProcAddress(hDll,"CLib_Pack");
    fnExtract   = (PFN_CLib_Extract)GetProcAddress(hDll,"CLib_Extract");
    fnReadInfo  = (PFN_CLib_ReadInfoInMem)GetProcAddress(hDll,"CLib_ReadInfoInMem");
    fnReadFile  = (PFN_CLib_ReadFileInMem)GetProcAddress(hDll,"CLib_ReadFileInMem");
    fnFreeStr   = (PFN_CLib_FreeString)GetProcAddress(hDll,"CLib_FreeString");
    fnFreeMem   = (PFN_CLib_FreeMem)GetProcAddress(hDll,"CLib_FreeMem");

    if(!fnPack || !fnExtract || !fnReadInfo || !fnReadFile || !fnFreeStr || !fnFreeMem)
    {
        cout << "[错误] DLL内部函数加载失败！| ERROR: DLL function load failed!" << endl;
        FreeLibrary(hDll);
        system("pause");
        return 1;
    }

    //拖拽 /右键打开方式：传入.clib文件，直接打印文本
    if(argc >= 2 && IsClibFile(argv[1]))
    {
        ShowClibText(argv[1]);
        FreeLibrary(hDll);
        return 0;
    }

    //====原有命令行功能，全部提示改为中英文双语====
    if(argc < 2)
    {
        cout << "==== CLib 打包/解压工具 | CLib Pack/Extract Tool ====" << endl;
        cout << "用法 | Usage：" << endl;
        cout << "  CLibTool.exe pack info.txt real.jpg pin.png out.clib" << endl;
        cout << "  CLibTool.exe extract xxx.clib 输出目录 | output_dir" << endl;
        cout << "  CLibTool.exe preview xxx.clib" << endl;
        cout << "使用方式：直接将 .clib 文件拖拽到此exe上，查看文本内容" << endl;
        cout << "Hint: Drag .clib file onto this exe to view text content." << endl;
        FreeLibrary(hDll);
        system("pause");
        return 1;
    }

    string cmd = argv[1];
    if(cmd == "pack")
    {
        if(argc != 6)
        {
            cout << "pack 参数错误！| Invalid pack arguments!" << endl;
        }
        else
        {
            int ret = fnPack(argv[2],argv[3],argv[4],argv[5]);
            if(ret ==0)
                cout << "✅打包成功 | Pack success:"<<argv[5]<<endl;
            else
                cout << "❌打包失败 | Pack failed, code:"<<ret<<endl;
        }
    }
    else if(cmd == "extract")
    {
        if(argc !=4)
        {
            cout << "extract 参数错误！| Invalid extract arguments!" << endl;
        }
        else
        {
            int ret = fnExtract(argv[2], argv[3]);
            if(ret ==0)
                cout << "✅解压成功 | Extract success"<<endl;
            else
                cout << "❌解压失败 | Extract failed"<<endl;
        }
    }
    else if(cmd == "preview")
    {
        if(argc !=3)
        {
            cout << "preview 参数错误！| Invalid preview arguments!" << endl;
        }
        else
        {
            cout << "\n-----控制台预览 | Console Preview-----"<<endl;
            char* info = fnReadInfo(argv[2]);
            if(info)
            {
                cout << "info.txt:\n" << info << endl;
                fnFreeStr(info);
            }
            size_t sz;
            void* jpg = fnReadFile(argv[2],"real.jpg",&sz);
            if(jpg){cout<<"real.jpg : "<<sz<<" bytes\n"; fnFreeMem(jpg);}
            void* png = fnReadFile(argv[2],"pin.png",&sz);
            if(png){cout<<"pin.png  : "<<sz<<" bytes\n"; fnFreeMem(png);}
        }
    }
    else
    {
        cout << "未知命令 | Unknown command！" << endl;
    }

    FreeLibrary(hDll);
    system("pause");
    return 0;
}
