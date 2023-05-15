//a program as a list of functions

#pragma once

#include "Fun.h"

typedef struct {
    Fun* funs; //array of functions
    uint length; //length
} Code;

Code Code_init(Fun* funs, uint length) {
    Code ans;
    ans.funs = funs;
    ans.length = length;
    return ans;
}

Code Code_initL(uint length) {
    return Code_init((Fun*)malloc(length * sizeof(Fun)), length);
}

void Code_clear(Code code) {
    for (uint x = 0; x < code.length; x++) {
        Fun_clear(code.funs[x]);
    }
    free(code.funs);
}

void Code_print(Code code) {
    for (uint x = 0; x < code.length; x++) {
        Fun_print(code.funs[x]);
        println();
    }
}
