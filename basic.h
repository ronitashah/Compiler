//basic macros and functions template

#pragma once

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define PAREN0 (
#define PAREN1 )

#define CONCAT0(X, Y) X ## Y
#define CONCAT(X, Y) CONCAT0(X, Y)

//redifine regular integer type names for portability

#define int __int32_t
#define uint __uint32_t
#define long __int64_t
#define ulong __uint64_t
#define LL __int128_t
#define ULL __uint128_t

#define null NULL

#define MAXINT (((uint)1) << 31)

//various print functions

void println() {
    printf("%s", "\n");
}

void printInt(long x) {
    printf("%ld", x);
}

void printIntln(long x) {
    printInt(x);
    println();
}

void printUInt(ulong x) {
    printf("%lu", x);
}

void printUIntln(ulong x) {
    printUInt(x);
    println();
}

void printStr(char* str) {
    printf("%s", str);
}

void printStrln(char* str) {
    printStr(str);
    println();
}

void printChars(char* chars, uint length) {
    for (uint x = 0; x < length; x++) {
        printf("%c", chars[x]);
    }
}

void printCharsln(char* chars, uint length) {
    printChars(chars, length);
    println();
}

void printChar(char c) {
    printf("%c", c);
}

void printCharln(char c) {
    printChar(c);
    println();
}

bool whiteSpace(char c) { //is c whitespace
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

bool lowercase(char c) { //is c lowercase
    return 'a' <= c && c <= 'z';
}

bool uppercase(char c) { //is c uppercase
    return 'A' <= c && c <= 'Z';
}

bool letter(char c) { //is c a letter
    return lowercase(c) || uppercase(c);
}

bool digit(char c) { //is c a digit
    return '0' <= c && c <= '9';
}

bool alphanum(char c) { //is c alphanumeric
    return letter(c) || digit(c);
}

bool prefix(char* a, char* b) { //is b a prefix of a
    for (uint x = 0; b[x]; x++) {
        if (a[x] != b[x]) {
            return false;
        }
    }
    return true;
}

uint strLength(char* str) { //length of null terminated string
    uint length = 0;
    for (;str[length];length++);
    return length;
}
