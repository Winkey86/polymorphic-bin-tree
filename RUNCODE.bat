@echo off
rem Собираем проект

g++ -std=c++17 -O2 main.cpp Menu.cpp BinaryTree.cpp -o tree_app.exe
if %ERRORLEVEL% neq 0 (
    echo Компиляция не удалась.
    pause
    exit /b %ERRORLEVEL%
)

echo Сборка прошла успешно. Чтение файлов...

rem Чтение команд из input.txt и вывод результата в output.txt
tree_app.exe < input.txt > output.txt

echo Готово. Результаты в output.txt
pause
