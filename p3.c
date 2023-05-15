#include "parser.h"
#include "compiler.h"
#include "optimizer.h"

void printStarting() { //starter assembly code
    puts("\t.data");
    puts("format: .byte '%', 'l', 'u', '\n', 0");
    puts("\t.text");
    puts("print:");
    puts("\tpush %rbp");
    puts("\tmov %rsp, %rbp");
    puts("\tand $0xFFFFFFFFFFFFFFF0, %rsp");
    puts("\tmov %rdi, %rsi");
    puts("\tlea format(%rip),%rdi");
    puts("\txor %rax, %rax");
    puts("\t.extern printf");
    puts("\tcall printf");
    puts("\tmov %rbp, %rsp");
    puts("\tpop %rbp");
    puts("\tretq");
    puts(".global main");
    puts("main:");
    puts("\tpush %rbp");
    puts("\tpush %rbx");
    puts("\tpush %r12");
    puts("\tpush %r13");
    puts("\tpush %r14");
    puts("\tpush %r15");
    puts("\tcall ._main");
    puts("\tpop %r15");
    puts("\tpop %r14");
    puts("\tpop %r13");
    puts("\tpop %r12");
    puts("\tpop %rbx");
    puts("\tpop %rbp");
    puts("\tretq");
    puts("");
    puts("");
    puts("");
}

String readInput() { //reads from stdin to a string
    uint length = 1;
    uint size = 0;
    char* input = (char*)malloc(1);
    char c;
    for (;read(STDIN_FILENO, &c, 1) > 0;) {
        if (size == length) {
            length <<= 1;
            input = (char*)realloc(input, length);
        }
        input[size++] = c;
    } 
    return String_init(input, size);
}

int main() {
    printStarting();
    String input = readInput();
    Code code = parseCode(input);
    optCode(code);
    String assembly = compileCode(code);
    Code_clear(code);
    String_println(assembly);
    String_clear(assembly);
}
