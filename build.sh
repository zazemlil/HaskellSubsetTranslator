#!/bin/bash

SRC_DIR="./src"
BUILD_DIR="./build"

rm -rf $BUILD_DIR
mkdir -p "$BUILD_DIR"/{linux,windows}

echo "📝 Generating parser..."
cd $BUILD_DIR
bison -d -Wcounterexamples ../$SRC_DIR/grammar.y
echo "🔤 Generating lexer..."
flex ../$SRC_DIR/lexer.l
cd ..


echo "⚙️ Compiling project..."
g++ -w -std=c++17 -I$SRC_DIR \
    $SRC_DIR/main.cpp \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    $SRC_DIR/cBigNumber/Cbignum.cpp \
    $SRC_DIR/cBigNumber/Cbignumf.cpp \
    $SRC_DIR/cBigNumber/Cbignums.cpp \
    $SRC_DIR/cBigNumber/Ctty.cpp \
    $SRC_DIR/cBigNumber/Exarray.cpp \
    $SRC_DIR/cBigNumber/Prime.cpp \
    -o $BUILD_DIR/linux/main

x86_64-w64-mingw32-g++ -w -static -I$SRC_DIR \
    $SRC_DIR/main.cpp \
    $BUILD_DIR/Parser.cpp \
    $BUILD_DIR/Scanner.cpp \
    $SRC_DIR/cBigNumber/Cbignum.cpp \
    $SRC_DIR/cBigNumber/Cbignumf.cpp \
    $SRC_DIR/cBigNumber/Cbignums.cpp \
    $SRC_DIR/cBigNumber/Ctty.cpp \
    $SRC_DIR/cBigNumber/Exarray.cpp \
    $SRC_DIR/cBigNumber/Prime.cpp \
    -o $BUILD_DIR/windows/main.exe
