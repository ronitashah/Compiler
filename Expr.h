//an expression as a postfix list of operators

#pragma once

#include "Op.h"

typedef struct {
    Op* ops; //array of operations
    uint length; //length of array
} Expr;

Expr Expr_init(Op* ops, uint length) {
    Expr expr;
    expr.ops = ops;
    expr.length = length;
    return expr;
}

Expr Expr_initL(uint length) {
    return Expr_init((Op*)malloc(length * sizeof(Op)), length);
}

Expr Expr_init1(Op op) {
    Op* ans = (Op*)malloc(sizeof(Op));
    *ans = op;
    return Expr_init(ans, 1);
}

Expr Expr_init0() {
    return Expr_init1(Op_init0());
}

Expr Expr_initN() {
    return Expr_initL(0);
}

void Expr_clear(Expr expr) {
    if (expr.ops) {
        free(expr.ops);
    }
} 

void Expr_print(Expr expr) {
    for (uint x = 0; x < expr.length; x++) {
        printChar('(');
        Op_print(expr.ops[x]);
        printChar(')');
        printChar(' ');
    }
}

void Expr_println(Expr expr) {
    Expr_print(expr);
    println();
}
