//function as an array of statements, along with locals and params count and name

#pragma once

#include "State.h"

typedef struct {
    String name; //name of the function
    uint params; //number of parameters
    uint locals; //number of locals
    State* states; //array of statements
    uint length; //length of array
} Fun;

Fun Fun_init(String name, uint params, uint locals, State* states, uint length) {
    Fun ans;
    ans.name = name;
    ans.params = params;
    ans.locals = locals;
    ans.states = states;
    ans.length = length;
    return ans;
}

Fun Fun_init1(String name) {
    return Fun_init(name, 0, 0, null, 0);
}

void Fun_clear(Fun fun) {
    String_clear(fun.name);
    for (uint x = 0; x < fun.length; x++) {
        State_clear(fun.states[x]);
    }
    free(fun.states);
}

void Fun_print(Fun fun) {
    String_println(fun.name);
    printUIntln(fun.params);
    printUIntln(fun.locals);
    println();
    for (uint x = 0; x < fun.length; x++) {
        State_print(fun.states[x]);
    }
    println();
}
