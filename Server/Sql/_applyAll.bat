echo "Applying"
del _all.sql
copy /a /b *.sql _all.sql

cd ..\Database
sqlite3 data.db < ..\SQL\_all.sql
del ..\SQL\_all.sql
pause
