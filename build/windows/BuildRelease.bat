set QTDIR=C:\Qt\Qt5.0.1
set QMAKESPEC=win32-msvc2010
C:\Qt\Qt5.0.1\5.0.1\msvc2010\bin\qmake -config release ..\..\src\USBVideoManager.pro
set PATH=C:\Qt\Qt5.0.1\5.0.1\msvc2010\bin;%PATH%
call "%VS100COMNTOOLS%vsvars32"
nmake