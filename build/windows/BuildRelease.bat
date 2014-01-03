set QTDIR=C:\Qt\5.2.0
set QMAKESPEC=win32-msvc2012
set PATH=%QTDIR%\msvc2012\bin;%PATH%
qmake -config release ..\..\src\USBVideoManager.pro
call "%VS110COMNTOOLS%vsvars32"
nmake