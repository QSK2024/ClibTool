# CLib 元器件库工具 · 完整说明文档

## 一、项目介绍

CLib 是一款**自制元器件资源打包/解压/内存预览工具**，基于 miniz 压缩库开发，运行在 Windows 平台。自定义 `.clib` 后缀文件，本质是标准 ZIP 压缩包，可改后缀用解压软件打开，专门用于统一收纳元器件的资料、实拍图、引脚图，方便管理、分发与内存读取。

工具全程支持：打包固化资源、一键解压、**无磁盘写入内存预览**，适配后续二次开发、GUI 集成、元器件台账系统使用。

## 二、运行环境

- 操作系统：Windows 全系列

- 编译工具：MinGW64

- 依赖库：miniz（miniz\.h \+ miniz\.c）

- 编码支持：默认 UTF\-8，彻底修复控制台中文乱码

## 三、项目文件结构

- **CLibCore\.cpp**：核心 DLL 源码，提供所有打包、解压、内存读取底层接口

- **CLibTool\.cpp**：命令行工具，调用 DLL 实现可视化操作

- **miniz\.h / miniz\.c**：开源轻量 ZIP 压缩库依赖文件

- **CLibCore\.dll**：编译生成的动态链接库

- **CLibTool\.exe**：编译生成的可执行工具

## 四、CLib 包内部固定格式

所有 \.clib 文件内部固定包含 3 个资源文件，结构统一规范：

- `info.txt`：元器件文字说明、参数台账（UTF\-8 编码）

- `real.jpg`：元器件实物实拍图片

- `pin.png`：元器件引脚定义示意图

## 五、编译命令

项目采用 miniz 双文件编译，无宏报错、无链接缺失、稳定可用：

```Plain Text
# 编译核心DLL
g++ CLibCore.cpp miniz.c -shared -o CLibCore.dll

# 编译命令行工具
g++ CLibTool.cpp -o CLibTool.exe -static
```

## 六、工具使用教程

### 1\. 打包生成 clib 文件

```Plain Text
CLibTool.exe pack info.txt real.jpg pin.png 输出.clib
```

功能：将说明文本、实物图、引脚图三合一打包为独立 clib 资源包。

### 2\. 解压 clib 文件

```Plain Text
CLibTool.exe extract 目标.clib 输出文件夹名
```

功能：**自动创建不存在的文件夹**，一键完整解压所有资源，无遗漏、不报错。

### 3\. 内存预览（不解盘）

```Plain Text
CLibTool.exe preview 目标.clib
```

功能：纯内存读取资源，**不生成任何磁盘文件**，快速预览文本与图片大小，适合程序实时读取展示。

## 七、DLL 对外开放接口说明

所有接口可直接被其他程序调用，支持二次开发：

- **CLib\_Pack**：传入三个资源路径 \+ 输出路径，打包生成 clib，0 为成功

- **CLib\_Extract**：传入 clib 路径 \+ 输出目录，自动建目录解压全部资源

- **CLib\_ReadInfoInMem**：内存读取 txt 文本，无需解压，用完需释放内存

- **CLib\_ReadFileInMem**：内存读取包内任意图片/文件，获取文件大小与二进制数据

- **CLib\_FreeString**：释放读取的文本内存（防止泄漏）

- **CLib\_FreeMem**：释放读取的二进制图片内存（防止泄漏）

## 八、项目修复亮点（最终稳定版）

- ✅ 彻底修复 miniz 链接报错、结构体成员不存在报错

- ✅ 新增自动递归创建文件夹，解压不会空输出、不会静默失败

- ✅ 全局 UTF\-8 编码，彻底解决控制台中文乱码

- ✅ 完整内存释放机制，无内存泄漏

- ✅ 接口全部标准化，兼容后续 GUI/软件集成

- ✅ 摒弃坑人的单头文件宏，采用最稳定的双文件编译

## 九、注意事项

- 运行时 `CLibCore.dll` 和 `CLibTool.exe` 必须放在同一目录

- 所有内存读取返回的指针，**必须使用对应接口释放**

- clib 文件为标准 ZIP，安全性高、兼容性极强

- 打包素材必须严格对应文件名：info\.txt / real\.jpg / pin\.png