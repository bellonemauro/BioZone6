SET QT5_BINARY_DIR= C:/Qt/6.6.0/msvc2019_64/bin
REM C:/Qt/5.15.2/msvc2019_64/bin
REM SET QT5_BINARY_DIR= C:/Qt/5.9/msvc2015_64/bin

%QT5_BINARY_DIR%/lrelease.exe ./eng.ts ./eng.qm
%QT5_BINARY_DIR%/lrelease.exe ./sve.ts ./sve.qm
%QT5_BINARY_DIR%/lrelease.exe ./ita.ts ./ita.qm
%QT5_BINARY_DIR%/lrelease.exe ./cn.ts ./cn.qm

pause
echo press any key to exit ...