@echo off
cd /D %~dp0
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 40 -c charset.utf8 -p 6 -t 0.66666 -a custom_chars.ini
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 80 -c charset.utf8 -p 6 -t 0.88888 -a custom_chars.ini
font2bitmap -i %1 -o "%~d1%~p1%~n1" -s 100 -c charset.utf8 -p 6 -t 0.99999 -a custom_chars.ini
REM font2bitmap -i %1 -o "%~d1%~p1%~n1" -m mixed -s 60 -c charset.utf8 -p 5
pause
