@echo off
cd /D %~dp0
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 100 -c charset.utf8 -p 6 -t 1.66666 -a custom_chars.ini
REM font2bitmap -i %1 -o "%~d1%~p1%~n1" -m mixed -s 60 -c charset.utf8 -p 5
pause
