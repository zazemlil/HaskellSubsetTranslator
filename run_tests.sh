#!/bin/bash

# Путь к исполняемому файлу транслятора (поменяй, если он в другой папке)
TRANSLATOR="./build/linux/main"

# Цвета для красивого вывода в консоль
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "========================================"
echo " Запуск тестирования транслятора Haskell"
echo "========================================"

# Флаг общего успеха
ALL_PASSED=1

# 1. Запуск позитивных тестов
echo -e "\n--- Позитивные тесты (ожидается код 0) ---"
for file in tests/positive/*.hs; do
    # Запускаем транслятор (вывод перенаправляем в /dev/null, чтобы не мусорить в консоли)
    $TRANSLATOR "$file" > /dev/null 2>&1
    EXIT_CODE=$?
    
    if [ $EXIT_CODE -eq 0 ]; then
        echo -e "[${GREEN}PASS${NC}] $file"
    else
        echo -e "[${RED}FAIL${NC}] $file (Exit code: $EXIT_CODE)"
        ALL_PASSED=0
    fi
done

# 2. Запуск негативных тестов
echo -e "\n--- Негативные тесты (ожидается ненулевой код ошибки) ---"
for file in tests/negative/*.hs; do
    $TRANSLATOR "$file" > /dev/null 2>&1
    EXIT_CODE=$?
    
    if [ $EXIT_CODE -ne 0 ]; then
        echo -e "[${GREEN}PASS${NC}] $file (Caught error, exit code: $EXIT_CODE)"
    else
        echo -e "[${RED}FAIL${NC}] $file (Translator accepted invalid code!)"
        ALL_PASSED=0
    fi
done

echo "========================================"
if [ $ALL_PASSED -eq 1 ]; then
    echo -e "${GREEN}Все тесты успешно пройдены!${NC}"
    exit 0
else
    echo -e "${RED}Некоторые тесты завершились с ошибкой.${NC}"
    exit 1
fi