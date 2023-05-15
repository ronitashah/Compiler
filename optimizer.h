//optimize the bytecode

#include "parser.h"

ulong evalOp2(Op op, ulong a, ulong b) { //evaluate the binary operator on a and b
    switch (op.op) {
    case '*':
        return a * b;
    case '/':
        return a / b;
    case '%':
        return a % b;
    case '+':
        return a + b;
    case '-':
        return a - b;
    case '>':
        return a > b;
    case '<':
        return a < b;
    case GEQ:
        return a >= b;
    case LEQ:
        return a <= b;
    case DEQ:
        return a == b;
    case NEQ:
        return a != b;
    case '&':
        return a && b;
    case '|':
        return a || b;
    }
    return 0;
}

ulong evalOp1(Op op, ulong x) { //evaluate the unary operator on a and b
    switch (op.op) {
    case '!':
        return !x;
    }
    return 0;
}

Expr optExpr(Expr expr) { //optimize the expression by precalculating as much as possible
    uint y = 0;
    for (uint x = 0; x < expr.length; x++) {
        Op op = expr.ops[x];
        uint args = Op_arguments(op);
        if (!args) {
            expr.ops[y++] = op;
            continue;
        }
        Op op1 = expr.ops[y - 1];
        if (op1.op != 'n') {
            expr.ops[y++] = op;
            continue;
        }
        if (args == 1) {
            expr.ops[y - 1] = Op_initN(evalOp1(op, op1.val)); //precalculate unary operators
            continue;
        } 
        Op op0 = expr.ops[y - 2];
        if (op0.op != 'n') {
            expr.ops[y++] = op;
            continue;
        }
        y--;
        expr.ops[y - 1] = Op_initN(evalOp2(op, op0.val, op1.val));  //precalculate binary operators
    }
    expr.length = y;
    expr.ops = (Op*)realloc(expr.ops, y * sizeof(Op));
    return expr;
}

State optState(State state, Fun fun, uint funi) { //optimize a statement using tail end recursion
    state.expr = optExpr(state.expr);
    if (state.state != RETURNS) {
        return state;
    }
    Expr expr = state.expr;
    Op* ops = expr.ops;
    Op op = ops[expr.length - 1];
    if (fun.locals - fun.params || op.op != FUNO) {
        return state;
    }
    if (fun.params != 2 || op.val != funi) {
        return state;
    }
    if (expr.length != 5 || ops[0].op != 'v' || ops[0].val != 0 || ops[1].op != 'n' || ops[2].op != '-' || ops[3].op != 'v' || ops[3].val != 1) {
        return state;
    }
    //meets the requirements for tail end recursion
    state.expr.length = 3;
    state.state = 'q';
    state.val = ops[0].val;
    return state;
}

void optFun(Fun fun, uint funi) {
    for (uint x = 0; x < fun.length; x++) {
        fun.states[x] = optState(fun.states[x], fun, funi);
    }
}

void optCode(Code code) {
    for (uint x = 0; x < code.length; x++) {
        optFun(code.funs[x], x);
    }
}