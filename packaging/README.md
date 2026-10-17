# 打包

需已安装 **Qt 6 MSVC 64-bit**，并设置 `CMAKE_PREFIX_PATH`（或在 `C:\Qt` 下可被脚本自动发现）。

```powershell
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.8.0\msvc2022_64"
.\packaging\build.ps1
```

会用 `windeployqt` 收集 Qt DLL 后打 ZIP / NSIS 安装包。
