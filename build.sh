#!/bin/bash

SRC_DIR="./src"
BUILD_DIR="./build"
INCLUDE_DIR="./include"

rm -rf $BUILD_DIR
rm -rf $INCLUDE_DIR
mkdir -p "$BUILD_DIR"/{linux,windows}
mkdir -p $INCLUDE_DIR

echo "📝 Generating parser..."
cd $BUILD_DIR
bison -d ../$SRC_DIR/grammar.y
echo "🔤 Generating lexer..."
flex ../$SRC_DIR/lexer.l
cd ..

cp $BUILD_DIR/Parser.hpp $INCLUDE_DIR/
cp $BUILD_DIR/Scanner.hpp $INCLUDE_DIR/

echo "⚙️ Compiling project..."
g++ -std=c++17 -I$INCLUDE_DIR -I$SRC_DIR \
    $SRC_DIR/main.cpp \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    -o $BUILD_DIR/linux/main

x86_64-w64-mingw32-g++ -static -I$INCLUDE_DIR -I$SRC_DIR \
    $SRC_DIR/main.cpp \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    -o $BUILD_DIR/windows/main.exe
