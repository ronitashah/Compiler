//compile the Code bytecodes to assembly

#pragma once

#include "parser.h"

//macros for combining strings together/doing common operations
#define CLNS(X) String_clone(X)
#define INITS(X) CLNS(String_init1(X))
#define INITS0(X) String_init0(X)
#define TABS(X) String_append1("\n", String_append1(tabs, X))
#define TINITS(X) TABS(INITS(X))
#define ADDS(X, Y) String_add(X, Y)
#define ADDS3(X, Y, Z) ADDS(X, ADDS(Y, Z))
#define ADDS4(X, Y, Z, W) ADDS3(ADDS(X, Y), Z, W)
#define ADDS5(X, Y, Z, W, Q) ADDS3(ADDS3(X, Y, Z), W, Q)
#define ADDS6(X, Y, Z, W, Q, R) ADDS5(X, Y, Z, W, ADDS(Q, R))
#define INTS(X) CLNS(String_initI(X))
#define NLS(X) ADDS(X, INITS("\n"))
#define POP2 ADDS(TINITS("pop %rdx"), TINITS("pop %rax"))
#define CMP2 ADDS(POP2, TINITS("cmp %rdx, %rax"))
#define BOOL2 ADDS5(POP2, TINITS("test %rax, %rax"), TINITS("setnz %al"), TINITS("test %rdx, %rdx"), TINITS("setnz %dl"))
#define ANDR TINITS("and $1, %eax")
#define PUSHR TINITS("push %rax")
#define APR ADDS(ANDR, PUSHR)


char* getTabs(uint t) { //make a string with t tabs
    char* ans = (char*)malloc(t + 1);
    memset(ans, '\t', t);
    ans[t] = 0;
    return ans;
}

uint getIndex(uint index, Fun fun) { //get the offset of a local variable givne its index in the function
    if (index >= fun.params) {
        index += 2;
    }
    index = fun.locals + 1 - index;
    return index * 8;
}

String compileOp(Op op, char* tabs, Fun fun, Code code) { //compile an operation to assembly
    String ans = String_initN();
    switch (op.op) {
    case FUNO:; //funciton call
        Fun call = code.funs[op.val];
        ans = ADDS6(TINITS("call ._"), CLNS(call.name), TINITS("add $"), INTS(call.params * 8), INITS(", %rsp"), PUSHR);
        break;
    case NUMO: //load number
        ans = ADDS4(TINITS("mov $"), INTS(op.val), INITS(", %rax"), TINITS("push %rax"));
        break;
    case VARO: //load variable
        ans = ADDS3(TINITS("push "), INTS(getIndex(op.val, fun)), INITS("(%rbp)"));
        break;
    case '!':
        ans = ADDS4(TINITS("pop %rax"), TINITS("test %rax, %rax"), TINITS("setz %al"), APR);
        break;
    case '+':
        ans = ADDS3(POP2, TINITS("add %rdx, %rax"), PUSHR);
        break;
    case '-':
        ans = ADDS3(POP2, TINITS("sub %rdx, %rax"), PUSHR);
        break;
    case '*':
        ans = ADDS3(POP2, TINITS("imul %rdx, %rax"), PUSHR);
        break;  
    case '/':
        ans = ADDS5(TINITS("pop %rdi"), TINITS("pop %rax"), TINITS("xor %rdx, %rdx"), TINITS("div %rdi"), PUSHR);
        break;   
    case '%':
        ans = ADDS5(TINITS("pop %rdi"), TINITS("pop %rax"), TINITS("xor %rdx, %rdx"), TINITS("div %rdi"), TINITS("push %rdx"));
        break;
    case '>':
        ans = ADDS3(CMP2, TINITS("seta %al"), APR);
        break;
    case '<':
        ans = ADDS3(CMP2, TINITS("setb %al"), APR);
        break;
    case GEQ:
        ans = ADDS3(CMP2, TINITS("setae %al"), APR);
        break;
    case LEQ:
        ans = ADDS3(CMP2, TINITS("setbe %al"), APR);
        break;
    case DEQ:
        ans = ADDS3(CMP2, TINITS("sete %al"), APR);
        break;
    case NEQ:
        ans = ADDS3(CMP2, TINITS("setne %al"), APR);
        break;
    case '&':
        ans = ADDS3(BOOL2, TINITS("and %rdx, %rax"), APR);
        break;
    case '|':
        ans = ADDS3(BOOL2, TINITS("or %rdx, %rax"), APR);
        break;
    }
    return ADDS3(TINITS("#"), INITS0(op.op), ans); //add comment of the type of expression
}

String compileExpr(Expr expr, char* tabs, Fun fun, Code code) { //compile the expression into assembly
    String* strings = (String*)malloc(expr.length * sizeof(String));
    for (uint x = 0; x < expr.length; x++) {
        strings[x] = compileOp(expr.ops[x], tabs, fun, code); //just compile each operation
    }
    return String_merge(strings, expr.length);
}

String compileState(State state, Fun fun, Code code) { //compile the statement into assembly
    char* tabs = getTabs(state.tabs); //tabs of the statement
    String* strings = (String*)malloc(5 * sizeof(String));
    if (state.comment.chars) {
        *strings = NLS(ADDS(TINITS("#"), CLNS(state.comment))); //comment of what the statment was in the given code
    }
    else {
        *strings = INITS("");
    }
    strings[1] = compileExpr(state.expr, tabs, fun, code);
    strings[2] = ADDS(TINITS("#"), INITS0(state.state)); //comment for the type of statement
    switch (state.state) {
    case RETURNS: //return
        strings[3] = ADDS6(TINITS("pop %rax"), TINITS("add $"), INTS((fun.locals - fun.params) * 8), INITS(", %rsp"), TINITS("pop %rbp"), TINITS("retq"));
        break;
    case PRINTS: //print
        strings[3] = ADDS(TINITS("pop %rdi"), TINITS("call print"));
        break;
    case JUMPS: //jump
        strings[3] = ADDS(TINITS("jmp ."), INTS(state.val));
        break;
    case IFS: //if
        strings[3] = ADDS4(TINITS("pop %rax"), TINITS("test %rax, %rax"), TINITS("jz ."), INTS(state.val));
        break;
    case LABELS: //label
        strings[3] = ADDS3(TINITS("."), INTS(state.val), INITS(":"));
        break;
    case VARS: //set variable
        strings[3] = ADDS4(TINITS("pop %rax"), TINITS("mov %rax, "), INTS(getIndex(state.val, fun)), INITS("(%rbp)"));
        break;
    case FUNS: //call general funciton
        strings[3] = TINITS("pop %rax");
        break;
    case 'q': //tail end recursion
        strings[3] = ADDS5(TINITS("pop %rax"), TINITS("mov %rax, "), INTS(getIndex(state.val, fun)), INITS("(%rbp)"), TINITS("jmp 0b"));
        break;
    }
    strings[4] = INITS("\n");
    free(tabs);
    return String_merge(strings, 5);
}   

String compileFun(Fun fun, Code code) { //compile the fun into asembly
    String* strings = (String*)malloc((fun.length + 2) * sizeof(String));
    *strings = ADDS3(INITS("._"), CLNS(fun.name), INITS(":\n")); //label of the function name
    strings[1] = ADDS4(INITS("\tpush %rbp\n\tsub $"), INTS((fun.locals - fun.params) * 8), INITS(", %rsp\n\tmov %rsp, %rbp\n"), INITS("0:\n")); //store rbp, make space for locals, set rbp, define 0 label
    for (uint x = 0; x < fun.length; x++) {
        strings[x + 2] = compileState(fun.states[x], fun, code);
    }
    return String_merge(strings, fun.length + 2);
}

String compileCode(Code code) { //compile the Code into assembly
    String* strings = (String*)malloc(code.length * sizeof(String));
    for (uint x = 0; x < code.length; x++) {
        strings[x] = compileFun(code.funs[x], code);
    }
    return String_merge(strings, code.length);
}
