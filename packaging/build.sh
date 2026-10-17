#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VERSION="$(grep -E 'project\(.*VERSION' "$ROOT/CMakeLists.txt" | sed -E 's/.*VERSION\s+([0-9.]+).*/\1/')"
INSTALLER_MODE="${INSTALLER:-both}"

CMAKE="$(command -v cmake || true)"
if [[ -z "$CMAKE" ]]; then
  for p in \
    "/c/Program Files/CMake/bin/cmake.exe" \
    "$LOCALAPPDATA/Microsoft/WinGet/Packages/Kitware.CMake_Microsoft.Winget.Source_8wekyb3d8bbwe/cmake-"*/bin/cmake.exe
  do
    [[ -x "$p" ]] && CMAKE="$p" && break
  done
fi
[[ -n "$CMAKE" && -x "$CMAKE" ]] || { echo "需要 cmake"; exit 1; }

find_qt() {
  if [[ -n "${CMAKE_PREFIX_PATH:-}" && -d "$CMAKE_PREFIX_PATH" ]]; then
    echo "$CMAKE_PREFIX_PATH"
    return
  fi
  local repo_qt="$ROOT/../.qt"
  if [[ -d "$repo_qt" ]]; then
    local found
    found="$(find "$repo_qt" -maxdepth 2 -type d -name 'msvc*_64' 2>/dev/null | sort -r | head -1)"
    [[ -n "$found" ]] && { echo "$found"; return; }
  fi
  if [[ -d /c/Qt ]]; then
    find /c/Qt -maxdepth 2 -type d -name 'msvc*_64' 2>/dev/null | sort -r | head -1
  fi
}

QT="$(find_qt || true)"
[[ -n "$QT" && -f "$QT/bin/windeployqt.exe" ]] || {
  echo "请设置 CMAKE_PREFIX_PATH 为 Qt6 msvc_64 目录，例如: d:/codes/soft/.qt/6.8.2/msvc2022_64"
  exit 1
}
echo "Qt: $QT"

(cd "$ROOT" && "$CMAKE" -B build "-DCMAKE_PREFIX_PATH=$QT" && "$CMAKE" --build build --config Release)

EXE_SRC="$ROOT/build/Release/suimi-qt.exe"
[[ -f "$EXE_SRC" ]] || { echo "未找到 suimi-qt.exe"; exit 1; }

DIST="$ROOT/dist"
PORTABLE_NAME="随密-Qt-${VERSION}-win64-绿色版"
PORTABLE_DIR="$DIST/$PORTABLE_NAME"
rm -rf "$DIST"
mkdir -p "$PORTABLE_DIR"
cp -f "$EXE_SRC" "$PORTABLE_DIR/随密-Qt.exe"
cp -f "$ROOT/packaging/stage/"* "$PORTABLE_DIR/"
"$QT/bin/windeployqt.exe" --no-compiler-runtime --no-translations "$PORTABLE_DIR/随密-Qt.exe"

(cd "$DIST" && tar -a -cf "${PORTABLE_NAME}.zip" "$(basename "$PORTABLE_DIR")")

INSTALLER_STAGE="$ROOT/packaging/installer-stage"
rm -rf "$INSTALLER_STAGE" && mkdir -p "$INSTALLER_STAGE"
cp -r "$PORTABLE_DIR"/* "$INSTALLER_STAGE/"

SETUP_EXE="$DIST/随密-Qt-${VERSION}-win64-setup.exe"
SETUP_ASCII="$DIST/Suimi-Qt-${VERSION}-win64-setup.exe"
for p in "/c/Program Files (x86)/NSIS/makensis.exe" "/c/Program Files/NSIS/makensis.exe"; do
  if [[ -f "$p" && ( "$INSTALLER_MODE" == "nsis" || "$INSTALLER_MODE" == "both" ) ]]; then
    "$p" "//INPUTCHARSET" "UTF8" "//DVERSION=$VERSION" \
      "//DSTAGE_DIR=$(cygpath -w "$INSTALLER_STAGE")" \
      "//DOUTFILE=$(cygpath -w "$SETUP_ASCII")" \
      "$(cygpath -w "$ROOT/packaging/nsis/setup.nsi")"
    mv -f "$SETUP_ASCII" "$SETUP_EXE"
    break
  fi
done
echo "Done: $DIST"
