@echo OFF
CLS

SET PY=C:\Python27\python.exe
SET WS=..\..\r_flash_loader_rx\utilities\python
SET PROJ_NAME=FL_UserApp_RDKRX63N

%PY% -u "%WS%\r_fl_mot_converter.py" -i "..\..\%PROJ_NAME%\Release\%PROJ_NAME%.mot"
%PY% -u "%WS%\r_fl_serial_flash_loader.py" -p3 -f "..\..\%PROJ_NAME%\Release\%PROJ_NAME%.bch" -cload

ECHO DONE
pause
