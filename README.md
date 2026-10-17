# suimi-qt

[随密](https://github.com/swqztech/suimi) 的 **Qt 6 Widgets** 版本，功能与其它实现一致。

## 依赖

- CMake 3.16+
- **Qt 6**（Widgets），MSVC 套件  
  安装示例：[Qt 在线安装器](https://www.qt.io/download-qt-installer) 勾选 *Qt 6.x → MSVC 64-bit*，或已有 `C:\Qt\6.x.x\msvc2019_64`

## 构建

```powershell
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.8.0\msvc2022_64"   # 改成你的 Qt 路径
cmake -B build -DCMAKE_PREFIX_PATH=$env:CMAKE_PREFIX_PATH
cmake --build build --config Release
# build\Release\suimi-qt.exe
```

## 打包

绿色版会用 `windeployqt` 带上 Qt 运行库：

```powershell
.\packaging\build.ps1
```

详见 `packaging/README.md`。

## 说明

小项目用 **CMake + 本机 Qt** 即可，不必 vcpkg；若希望 Qt 也由 vcpkg/conan 提供，可自行改 `CMakeLists.txt`。
