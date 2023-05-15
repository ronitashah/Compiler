//an array of strings, where each string represents a different line

#pragma once

#include "String.h"

typedef struct {
    String* lines; //array of strings
    uint length; //length of array
} Lines;

Lines Lines_init2(String* lines, uint length) {
    Lines ans;
    ans.lines = lines;
    ans.length = length;
    return ans;
}

Lines Lines_initL(uint length) {
    return Lines_init2((String*)malloc(length * sizeof(String)), length);
}

Lines Lines_init(String str) { //initialize by splitting a string by its newlines
    uint length = String_count(str, '\n'); //final length
    String* lines = (String*)malloc(sizeof(String) * length);
    uint last = 0;
    uint y = 0;
    for (uint x = 0; x < str.length; x++) {
        if (str.chars[x] != '\n') { //newlines aren't storeed directly in Lines
            continue;
        }
        lines[y] = String_init(str.chars + last, x - last); // set the line in the array
        y++;
        last = x + 1;
    }
    return Lines_init2(lines, length);
}

Lines Lines_initS(String* strs, uint length) {
    Lines ans = Lines_initL(length);
    memcpy(ans.lines, strs, length * sizeof(String));
    return ans;
}

Lines Lines_initS1(String a) {
    Lines ans = Lines_initL(1);
    *ans.lines = a;
    return ans;
}

void Lines_clear(Lines lines) {
    free(lines.lines);
}

void Lines_clear2(Lines lines) {
    for (uint x = 0; x < lines.length; x++) {
        String_clear(lines.lines[x]);
    }
    Lines_clear(lines);
}

uint Lines_length(Lines lines) { //total number of characters in Lines
    uint ans = 0;
    for (uint x = 0; x < lines.length; x++) {
        ans += lines.lines[x].length;
    }
    return ans;
}

void Lines_print(Lines lines) { //print the Lines
    for (uint x = 0; x < lines.length; x++) {
        String_print(lines.lines[x]);
        println();
    }
}

Lines Lines_fixElse(Lines lines) { //remove the { and } direclty before and after elses
    uint y = 0;
    for (uint x = 0; x < lines.length;) {
        if (lines.lines[x].chars[0] == ELSE) {
            lines.lines[y - 1] = lines.lines[x];
            x += 2; //skip the { and } lines surrounding the else
            continue;
        }
        lines.lines[y] = lines.lines[x];
        x++;
        y++;
    }
    lines.length = y;
    return lines;
}

uint Lines_maxLength(Lines lines) { //longest length of a line
    uint ans = 0;
    for (uint x = 0; x < lines.length; x++) {
        uint length = lines.lines[x].length;
        if (length > ans) {
            ans = length;
        }
    }
    return ans;
}

uint Lines_count(Lines lines, char c) {
    uint ans = 0;
    for (uint x = 0; x < lines.length; x++) {
        ans += String_count(lines.lines[x], c);
    }
    return ans;
}
