//represents one operation

#pragma once

#include "String.h"

#define FUNO FUN
#define NUMO 'n'
#define VARO 'v'

//precedence classes
#define PARP 0
#define FUNP 1
#define NOTP 2
#define MULP 3
#define ADDP 4
#define CMPP 5
#define EQP 6
#define ANDP 7
#define ORP 8

//1 + 2 * x
//1 2 x * +

//push 1
//push 2
//push x*8(%rbp)

//pop pop, mul, push

//helper macro to set ops PC and precedence in switch statement, o is the Ops char, t is index in the PC table, and p is precedence
#define SETOP(o, p) \
case o: \
return p;

typedef struct {
    char op;
    ulong val;
} Op;

Op Op_init(char op, ulong v) {
    Op ans;
    ans.op = op;
    ans.val = v;
    return ans;
}

Op Op_init0() {
    return Op_init('n', 0);
}

Op Op_init1(char op) {
    return Op_init(op, 0);
}

Op Op_initN(ulong n) {
    return Op_init('n', n);
}

uint Op_precedence(Op op) { //get the precedence of the operator
    switch (op.op) {
        SETOP('(', PARP);
        SETOP('!', NOTP);
        SETOP('*', MULP);
        SETOP('/', MULP);
        SETOP('%', MULP);
        SETOP('+', ADDP);
        SETOP('-', ADDP);
        SETOP('>', CMPP);
        SETOP('<', CMPP);
        SETOP(GEQ, CMPP);
        SETOP(LEQ, CMPP);
        SETOP(DEQ, EQP);
        SETOP(NEQ, EQP);
        SETOP('&', ANDP);
        SETOP('|', ORP);
    }
    return -(uint)1;
}

void Op_print(Op op) {
    printChar(op.op);
    printChar(' ');
    printUInt(op.val);
}

uint Op_arguments(Op op) { //number of arguments the op takes
    uint p = Op_precedence(op);
    return p == NOTP ? 1 : p > NOTP && p != -1 ? 2 : 0; 
}