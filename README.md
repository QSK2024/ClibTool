# CLib Component Library Tool | CLib 元器件库资源管理工具


## 1. Project Introduction | 一、项目介绍

**English**

CLib is a self-made component resource packing, extraction and in-memory preview tool based on the miniz compression library for Windows. The custom `.clib` file is essentially a standard ZIP archive — you can change its extension and open it with any common decompression software. It is specially designed for centrally storing component documents, real-object photos and pinout diagrams, making resource management, distribution and in-memory reading convenient. The tool supports resource packing, one-click extraction and **disk-free in-memory preview**, and is suitable for secondary development, GUI integration and component inventory systems.

**中文**

CLib 是一款**自制元器件资源打包/解压/内存预览工具**，基于 miniz 压缩库开发，运行在 Windows 平台。自定义 `.clib` 后缀文件，本质是标准 ZIP 压缩包，可改后缀用解压软件打开，专门用于统一收纳元器件的资料、实拍图、引脚图，方便管理、分发与内存读取。工具全程支持：打包固化资源、一键解压、**无磁盘写入内存预览**，适配后续二次开发、GUI 集成、元器件台账系统使用。

---

## 2. Runtime Environment | 二、运行环境

**English**

- Operating System: All Windows versions
- Compiler: MinGW64
- Dependency Library: miniz (`miniz.h` + `miniz.c`)
- Encoding: UTF-8 by default, Chinese console garbled text fully fixed

**中文**

- 操作系统：Windows 全系列
- 编译工具：MinGW64
- 依赖库：miniz（miniz.h + miniz.c）
- 编码支持：默认 UTF-8，彻底修复控制台中文乱码

---

## 3. Project File Structure | 三、项目文件结构

**English**

- **CLibCore.cpp**: Core DLL source code, provides low-level interfaces for packing, extraction and in-memory reading
- **CLibTool.cpp**: Command-line tool, calls the DLL to implement operations
- **miniz.h / miniz.c**: Open-source lightweight ZIP compression library dependencies
- **CLibCore.dll**: Compiled dynamic-link library
- **CLibTool.exe**: Compiled executable tool

**中文**

- **CLibCore.cpp**：核心 DLL 源码，提供所有打包、解压、内存读取底层接口
- **CLibTool.cpp**：命令行工具，调用 DLL 实现可视化操作
- **miniz.h / miniz.c**：开源轻量 ZIP 压缩库依赖文件
- **CLibCore.dll**：编译生成的动态链接库
- **CLibTool.exe**：编译生成的可执行工具

---

## 4. Internal Format of .clib Package | 四、CLib 包内部固定格式

**English**

Every `.clib` file contains three fixed resource files with unified specification:

- `info.txt`: Component description and parameter inventory (UTF-8 encoding)
- `real.jpg`: Real-object photo of the component
- `pin.png`: Component pinout diagram

**中文**

所有 `.clib` 文件内部固定包含 3 个资源文件，结构统一规范：

- `info.txt`：元器件文字说明、参数台账（UTF-8 编码）
- `real.jpg`：元器件实物实拍图片
- `pin.png`：元器件引脚定义示意图

---

## 5. Compile Commands | 五、编译命令

**English**

Built with miniz two-file compilation. No macro errors or missing link issues, stable for use.

```bash
# Compile core DLL
g++ CLibCore.cpp miniz.c -shared -o CLibCore.dll

# Compile command-line tool
g++ CLibTool.cpp -o CLibTool.exe -static
```

**中文**

项目采用 miniz 双文件编译，无宏报错、无链接缺失、稳定可用：

```bash
# 编译核心DLL
g++ CLibCore.cpp miniz.c -shared -o CLibCore.dll

# 编译命令行工具
g++ CLibTool.cpp -o CLibTool.exe -static
```

---

## 6. Tool Usage Tutorial | 六、工具使用教程

### 6.1 Pack to Generate .clib File | 打包生成 clib 文件

**English**

```bash
CLibTool.exe pack info.txt real.jpg pin.png output.clib
```

Function: Packs the description text, real-object photo and pinout diagram into one independent `.clib` resource package.

**中文**

```bash
CLibTool.exe pack info.txt real.jpg pin.png 输出.clib
```

功能：将说明文本、实物图、引脚图三合一打包为独立 clib 资源包。

### 6.2 Extract .clib File | 解压 clib 文件

**English**

```bash
CLibTool.exe extract target.clib output_folder_name
```

Function: **Automatically creates folders if they do not exist**, extracts all resources completely without omission.

**中文**

```bash
CLibTool.exe extract 目标.clib 输出文件夹名
```

功能：**自动创建不存在的文件夹**，一键完整解压所有资源，无遗漏、不报错。

### 6.3 In-Memory Preview (No Disk Writing) | 内存预览（不解盘）

**English**

```bash
CLibTool.exe preview target.clib
```

Function: Reads resources purely in memory, **no disk files generated**. Quick preview of text and image size, suitable for real-time program reading.

**中文**

```bash
CLibTool.exe preview 目标.clib
```

功能：纯内存读取资源，**不生成任何磁盘文件**，快速预览文本与图片尺寸，适合程序实时读取展示。

---

## 7. DLL Public API Reference | 七、DLL 对外开放接口说明

**English**

All interfaces can be directly invoked by other programs for secondary development:

- **CLib_Pack**: Pass in three resource paths + output path, generates a `.clib` file. Returns 0 on success.
- **CLib_Extract**: Pass in `.clib` path + output directory, auto-creates directories and extracts all resources.
- **CLib_ReadInfoInMem**: Reads `info.txt` text in memory without extraction. Memory must be released after use.
- **CLib_ReadFileInMem**: Reads any file inside the package in memory, obtains binary data and file size.
- **CLib_FreeString**: Releases the text memory to avoid memory leaks.
- **CLib_FreeMem**: Releases the binary image memory to avoid memory leaks.

**中文**

全部接口可被其他程序直接调用，支持二次开发：

- **CLib_Pack**：传入三个资源路径 + 输出路径，打包生成 clib，0 为成功
- **CLib_Extract**：传入 clib 路径 + 输出目录，自动建目录解压全部资源
- **CLib_ReadInfoInMem**：内存读取 txt 文本，无需解压，用完需释放内存
- **CLib_ReadFileInMem**：内存读取包内任意图片/文件，获取文件大小与二进制数据
- **CLib_FreeString**：释放读取的文本内存（防止泄漏）
- **CLib_FreeMem**：释放读取的二进制图片内存（防止泄漏）

---

## 8. Project Improvement Highlights | 八、项目修复亮点（最终稳定版）

**English**

- Fully fixed miniz link errors and struct-member-missing errors
- Added automatic recursive folder creation; extraction will not produce empty output or fail silently
- Global UTF-8 encoding, completely solves Chinese console garbled characters
- Complete memory release mechanism, no memory leaks
- All interfaces standardized, compatible with subsequent GUI/software integration
- Abandoned the error-prone single-header macro mode, adopted the stable two-file compilation

**中文**

- ✅ 彻底修复 miniz 链接报错、结构体成员不存在报错
- ✅ 新增自动递归创建文件夹，解压不会空输出、不会静默失败
- ✅ 全局 UTF-8 编码，彻底解决控制台中文乱码
- ✅ 完整内存释放机制，无内存泄漏
- ✅ 接口全部标准化，兼容后续 GUI/软件集成
- ✅ 摒弃坑人的单头文件宏，采用最稳定的双文件编译

---

## 9. Notes | 九、注意事项

**English**

- `CLibCore.dll` and `CLibTool.exe` must be placed in the same directory when running
- Pointers returned by memory-reading functions **must be released using the corresponding interfaces**
- `.clib` files use standard ZIP format, high security and excellent compatibility
- Source files for packing must strictly use these names: `info.txt` / `real.jpg` / `pin.png`

**中文**

- 运行时 `CLibCore.dll` 和 `CLibTool.exe` 必须放在同一目录
- 所有内存读取返回的指针，**必须使用对应接口释放**
- clib 文件为标准 ZIP，安全性高、兼容性极强
- 打包素材必须严格对应文件名：info.txt / real.jpg / pin.png
