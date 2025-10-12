#!/bin/bash

SRC_DIR="./src"
BUILD_DIR="./build"

rm -rf $BUILD_DIR
mkdir -p "$BUILD_DIR"/{linux,windows}

echo "📝 Generating parser..."
cd $BUILD_DIR
bison -d ../$SRC_DIR/grammar.y
echo "🔤 Generating lexer..."
flex ../$SRC_DIR/lexer.l
cd ..


echo "⚙️ Compiling project..."
g++ -std=c++17 -I$SRC_DIR \
    $SRC_DIR/main.cpp \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    -o $BUILD_DIR/linux/main

x86_64-w64-mingw32-g++ -static -I$SRC_DIR \
    $SRC_DIR/main.cpp \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    -o $BUILD_DIR/windows/main.exe
