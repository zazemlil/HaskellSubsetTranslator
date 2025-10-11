#!/bin/bash

SRC_DIR="./src"
BUILD_DIR="./build"
INCLUDE_DIR="./include"

mkdir -p $BUILD_DIR
mkdir -p $INCLUDE_DIR

echo "📝 Generating parser..."
cd $SRC_DIR
bison -d -o ../$BUILD_DIR/Parser.cpp --defines=../$BUILD_DIR/Parser.hpp grammar.y
cd ..

echo "🔤 Generating lexer..."
cd $SRC_DIR
flex -o ../$BUILD_DIR/Scanner.cpp --header-file=../$BUILD_DIR/Scanner.hpp lexer.l
cd ..

cp $BUILD_DIR/Parser.hpp $INCLUDE_DIR/
cp $BUILD_DIR/Scanner.hpp $INCLUDE_DIR/

echo "⚙️ Compiling project..."
g++ -std=c++17 -I$INCLUDE_DIR -I$BUILD_DIR \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    -o $BUILD_DIR/main

x86_64-w64-mingw32-g++ -static -I$INCLUDE_DIR -I$BUILD_DIR \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    -o $BUILD_DIR/main.exe
