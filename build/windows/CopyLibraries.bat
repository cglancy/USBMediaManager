set QTBINDIR=C:\Qt\Qt5.0.1\5.0.1\msvc2010\bin
set QTPLUGINSDIR=C:\Qt\Qt5.0.1\5.0.1\msvc2010\plugins
set VCREDIST="C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\redist\x86\Microsoft.VC100.CRT"

mkdir lib\platforms
mkdir lib\imageformats

copy %QTBINDIR%\D3DCompiler_43.dll lib
copy %QTBINDIR%\icudt49.dll lib
copy %QTBINDIR%\icuin49.dll lib
copy %QTBINDIR%\icuuc49.dll lib
copy %QTBINDIR%\libEGL.dll lib
copy %QTBINDIR%\libGLESv2.dll lib
copy %QTBINDIR%\Qt5Concurrent.dll lib
copy %QTBINDIR%\Qt5Core.dll lib
copy %QTBINDIR%\Qt5Gui.dll lib
copy %QTBINDIR%\Qt5Multimedia.dll lib
copy %QTBINDIR%\Qt5Network.dll lib
copy %QTBINDIR%\Qt5OpenGL.dll lib
copy %QTBINDIR%\Qt5Widgets.dll lib

copy %VCREDIST%\msvcp100.dll lib
copy %VCREDIST%\msvcr100.dll lib

copy %QTPLUGINSDIR%\platforms\qwindows.dll lib\platforms
copy %QTPLUGINSDIR%\platforms\qminimal.dll lib\platforms

copy %QTPLUGINSDIR%\imageformats\qgif.dll lib\imageformats
copy %QTPLUGINSDIR%\imageformats\qico.dll lib\imageformats
copy %QTPLUGINSDIR%\imageformats\qjpeg.dll lib\imageformats