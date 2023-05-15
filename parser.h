//parse the string to bytecodes

#pragma once

#include "lexer.h"

Expr parseExpr(String expr, Op* out, Op* stack, Map funs, Map vars) { //parse the expression to an Expr
    Op* ogout = out;
    *stack++ = Op_init1('(');
    for (uint x = 0; x < expr.length; x++) {
        char c = expr.chars[x];
        if (digit(c)) { //number
            ulong val = 0;
            for (;x < expr.length && digit(expr.chars[x]); x++) {
                val = val * 10 + expr.chars[x] - '0';
            }
            x--;
            *out++ = Op_init(NUMO, val);
            goto NOTPOP;
        }
        if (letter(c)) { //token
            uint y = x;
            x++;
            for (;x < expr.length && alphanum(expr.chars[x]); x++);
            ulong hash = String_hash(String_init(expr.chars + y, x - y));
            if (x == expr.length || expr.chars[x] != '(') { //variable
                x--;
                ulong index = Map_get(vars, hash);
                *out++ = Op_init(VARO, index);
                goto NOTPOP;
            }
            //function
            x--;
            ulong index = Map_get(funs, hash);
            *stack++ = Op_init(FUNO, index);
            continue;
        }
        if (c == ',') { //parameter
            for (;stack[-1].op != '('; stack--) {
                *out++ = stack[-1];
            }
            continue;
        }
        if (c == ')') { //end parenthesis
            for (;stack[-1].op != '('; stack--) {
                *out++ = stack[-1];
            }
            stack--;
            if (stack[-1].op == FUNO) {
                *out++ = *--stack;
            }
            goto NOTPOP;
        }
        { //regular operator
            Op op = Op_init1(c);
            uint prec = Op_precedence(op);
            for (;; stack--) {
                uint p = Op_precedence(stack[-1]);
                if (p <= NOTP || p > prec) {
                    break;
                }
                *out++ = stack[-1];
            }
            *stack++ = op;
            continue;
        }
        NOTPOP: //pop all nots off of the stack
        for (;stack[-1].op == '!'; stack--) {
            *out++ = stack[-1];
        }
    }
    for (;stack[-1].op != '('; stack--) {
        *out++ = stack[-1];
    }
    uint length = out - ogout;
    Op* ans = (Op*)malloc(length * sizeof(Op));
    memcpy(ans, ogout, length * sizeof(Op));
    return Expr_init(ans, length);
}

Fun parseFun(Lines block, Fun fun, State* out, Map funs, Map vars, uint* labelStack, Op* exprSpace, Op* exprStack) { //parse the Lines into a function
    uint tab = 1;
    uint label = *labelStack;
    State* ogout = out;
    for (uint x = 0; x < block.length; x++) {
        String line = String_clone(block.lines[x]);
        char c = *line.chars;
        switch (c) {
        case '{': //open brace
            tab++;
            String_clear(line);
            continue;
        case RETURN: //return
            if (line.length == 1) {
                *out++ = State_init(RETURNS, 0, Expr_init0(), line, tab);
                continue;
            }
            *out++ = State_init(RETURNS, 0, parseExpr(String_init(line.chars + 1, line.length - 1), exprSpace, exprStack, funs, vars), line, tab);
            continue;
        case PRINT: //print
            *out++ = State_init(PRINTS, 0, parseExpr(String_init(line.chars + 1, line.length - 1), exprSpace, exprStack, funs, vars), line, tab);
            continue;
        case IF: //if
            *out++ = State_init(IFS, label, parseExpr(String_init(line.chars + 1, line.length - 1), exprSpace, exprStack, funs, vars), line, tab);
            *labelStack++ = label++;
            continue;
        case WHILE: //while
            *out++ = State_init2NC(LABELS, label, tab);
            *labelStack++ = label++ ^ MAXINT;
            *out++ = State_init(IFS, label++, parseExpr(String_init(line.chars + 1, line.length - 1), exprSpace, exprStack, funs, vars), line, tab);
            continue;
        case ELSE: //else
            *out++ = State_init2NC(JUMPS, label, tab);
            *out++ = State_init2(LABELS, *--labelStack, line, tab - 1);
            *labelStack++ = label++;
            continue;
        case '}': //end brace
            String_clear(line);
            uint prevLabel = *--labelStack;
            if (prevLabel & MAXINT) {
                prevLabel ^= MAXINT;
                *out++ = State_init2NC(JUMPS, prevLabel++, tab);
            }
            *out++ = State_init2NC(LABELS, prevLabel, --tab);
            continue;
        }
        uint index = String_indexOf(line, '=');
        if (index != -(uint)1) { //assignment
            *out++ = State_init(VARS, Map_get(vars, String_hash(String_init(line.chars, index))), parseExpr(String_init(line.chars + index + 1, line.length - index - 1), exprSpace, exprStack, funs, vars), line, tab);
            continue;
        }
        //expression
        *out++ = State_init(FUNS, 0, parseExpr(line, exprSpace, exprStack, funs, vars), line, tab);
    }
    *labelStack = label;
    fun.length = out - ogout;
    fun.states = (State*)malloc(fun.length * sizeof(State));
    memcpy(fun.states, ogout, fun.length * sizeof(State));
    return fun;
}

Code parseCode(String str) { //parse the string into a Code
    str = StrFix(str);
    uint maxBraceDepth = String_braceDepth(str);
    Code code = Code_init(null, String_count(str, FUN));
    Lines lines = Lines_init(str);
    lines = Lines_fixElse(lines);
    Lines* blocks = getBlocks(lines, code.length);
    code.funs = getFunArr(blocks, code.length);
    Map funMap = getFunMap(code);
    Map* varMaps = (Map*)malloc(code.length * sizeof(Map));
    for (uint x = 0; x < code.length; x++) { //get param and local maps for each function
        varMaps[x] = getParamMap(blocks[x]);
        code.funs[x].params = varMaps[x].size;
        varMaps[x] = getVarMap(blocks[x], varMaps[x]);
        code.funs[x].locals = varMaps[x].size;
    }
    uint* labelStack = (uint*)malloc(maxBraceDepth * sizeof(uint));
    *labelStack = 0;
    uint maxLineLength = 0;
    uint maxBlockLength = 0;
    for (uint x = 0; x < code.length; x++) {
        uint length = Lines_maxLength(blocks[x]);
        if (length > maxLineLength) {
            maxLineLength = length;
        }
        length = blocks[x].length + Lines_count(blocks[x], WHILE);
        if (length > maxBlockLength) {
            maxBlockLength = length;
        }
    }
    //buffer space used for parsing
    Op* exprSpace = (Op*)malloc(maxLineLength * sizeof(Op));
    Op* exprStack = (Op*)malloc(maxLineLength * sizeof(Op));
    State* stateSpace = (State*)malloc(maxBlockLength * sizeof(State));
    for (uint x = 0; x < code.length; x++) { //parse each function
        code.funs[x] = parseFun(Lines_init2(blocks[x].lines + 2, blocks[x].length - 3), code.funs[x], stateSpace, funMap, varMaps[x], labelStack, exprSpace, exprStack);
    }
    //free everything
    free(stateSpace);
    free(exprStack);
    free(exprSpace);
    free(labelStack);
    for (uint x = 0; x < code.length; x++) {
        Map_clear(varMaps[x]);
    }
    free(varMaps);
    Map_clear(funMap);
    clearBlocks(blocks, code.length);
    Lines_clear(lines);
    String_clear(str);
    return code;
}
