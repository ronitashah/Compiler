//one statement in code

#pragma once

#include "Expr.h"

#define JUMPS 'j'
#define IFS 'i'
#define VARS VARO
#define FUNS FUN
#define RETURNS RETURN
#define PRINTS PRINT
#define LABELS 'l'


typedef struct {
    char state; //type of statement
    ulong val; //value used in statement
    Expr expr; //expression used in statement
    String comment; //comment of statement
    uint tabs; //number of tabs before statement
} State;

State State_init(char state, ulong val, Expr expr, String comment, uint tabs) {
    State ans;
    ans.state = state;
    ans.val = val;
    ans.expr = expr;
    ans.comment = comment;
    ans.tabs = tabs;
    return ans;
}

State State_init2(char state, ulong val, String comment, uint tabs) {
    return State_init(state, val, Expr_initN(), comment, tabs);
}

State State_init2NC(char state, ulong val, uint tabs) {
    return State_init2(state, val, String_initN(), tabs);
}

State State_init1(char state, String comment, uint tabs) {
    return State_init2(state, 0, comment, tabs);
}

State State_init1NC(char state, uint tabs) {
    return State_init1(state, String_initN(), tabs);
}

State State_initN() {
    return State_init1NC(0, 0);
}

void State_clear(State state) {
    Expr_clear(state.expr);
    String_clear(state.comment);
}

void State_print(State state) {
    printChar(state.state);
    printChar('\t');
    printUInt(state.val);
    printChar('\t');
    printUInt(state.tabs);
    printChar('\t');
    Expr_println(state.expr);
}
