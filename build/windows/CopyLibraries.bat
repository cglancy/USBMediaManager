set QTBINDIR=C:\Qt\5.2.0\msvc2012\bin
set QTPLUGINSDIR=C:\Qt\5.2.0\msvc2012\plugins
set VCREDIST="C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\redist\x86\Microsoft.VC110.CRT"

mkdir lib\platforms
mkdir lib\imageformats

copy %QTBINDIR%\d3dcompiler_46.dll lib
copy %QTBINDIR%\icudt51.dll lib
copy %QTBINDIR%\icuin51.dll lib
copy %QTBINDIR%\icuuc51.dll lib
copy %QTBINDIR%\libEGL.dll lib
copy %QTBINDIR%\libGLESv2.dll lib
copy %QTBINDIR%\Qt5Concurrent.dll lib
copy %QTBINDIR%\Qt5Core.dll lib
copy %QTBINDIR%\Qt5Gui.dll lib
copy %QTBINDIR%\Qt5Multimedia.dll lib
copy %QTBINDIR%\Qt5Network.dll lib
copy %QTBINDIR%\Qt5OpenGL.dll lib
copy %QTBINDIR%\Qt5Widgets.dll lib

copy %VCREDIST%\msvcp110.dll lib
copy %VCREDIST%\msvcr110.dll lib

copy %QTPLUGINSDIR%\platforms\qwindows.dll lib\platforms
copy %QTPLUGINSDIR%\platforms\qminimal.dll lib\platforms

copy %QTPLUGINSDIR%\imageformats\qgif.dll lib\imageformats
copy %QTPLUGINSDIR%\imageformats\qico.dll lib\imageformats
copy %QTPLUGINSDIR%\imageformats\qjpeg.dll lib\imageformats