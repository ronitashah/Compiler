//stores Strings as an array of chars with a specified length

#pragma once

#include "basic.h"

#define RADIX (ULL)4321423 //radix for rolling hashes
#define MOD ((((ulong)27) << (ulong)59) + 1) //prime modulo for rolling hashes

//special characters to convert 2char operations
#define DEQ '~' //==
#define NEQ '`' //!=
#define GEQ '?' //>=
#define LEQ ';' //<=

//special characters to tokenize keywords
#define FUN '$'
#define IF '^'
#define ELSE '@'
#define WHILE '\''
#define RETURN '['
#define PRINT '"'


#define NUMBUFL 21
char NUMBUF[NUMBUFL];


typedef struct {
    char* chars; //array of chars
    uint length; //length of array
} String;

String String_init(char* chars, uint length) {
    String ans;
    ans.chars = chars;
    ans.length = length;
    return ans;
}

String String_initN() {
    return String_init(null, 0);
}

String String_initL(uint length) {
    return String_init((char*)malloc(length), length);
}

String String_init1(char* str) {
    return String_init(str, strLength(str));
}

String String_initI(ulong val) {
    memset(NUMBUF, 0, NUMBUFL);
    sprintf(NUMBUF, "%lu", val);
    return String_init1(NUMBUF);
}

String String_init2(char* from, char* to) { //initialize as a String from from to to
    String ans;
    ans.chars = from;
    ans.length = (uint)(to - from);
    return ans;
}

void String_clear(String str) {
    if (str.chars) {
        free(str.chars);
    }
}

String String_clone(String str) { //clone the input string and return a new one
    char* chars = (char*)malloc(str.length);
    memcpy(chars, str.chars, str.length);
    return String_init(chars, str.length);
}

String String_init0(char c) { //init a string that's just the char
    return String_clone(String_init(&c, 1));
}

String String_resize(String str, uint length) { //resize the string to length
    return String_init((char*)realloc(str.chars, length), length);
}

bool String_equals(String a, String b) {
    return a.length == b.length && !memcmp(a.chars, b.chars, a.length);
}

bool String_equals2(String str, char* string) {
    return String_equals(str, String_init1(string));
}

void String_print(String str) { //print the string
    printChars(str.chars, str.length);
}

void String_println(String str) {
    String_print(str);
    println();
}

String String_append(String str, char* chars) {
    uint length = strLength(chars);
    str = String_resize(str, str.length + length);
    memcpy(str.chars + str.length - length, chars, length);
    return str;
}

String String_append1(char* chars, String str) {
    uint length = strLength(chars);
    String ans = String_initL(str.length + length);
    memcpy(ans.chars, chars, length);
    memcpy(ans.chars + length, str.chars, str.length);
    String_clear(str);
    return ans;
}

String String_append2(char* a, String str, char* b) {
    return String_append1(a, String_append(str, b));
}

ulong String_hash(String str) { //hash the string using rolling hashes
    ulong ans = 0;
    for (uint x = 0; x < str.length; x++) {
        ans = (ulong)(((ULL)ans * RADIX + str.chars[x]) % MOD); //roll the hash
    }
    return ans;
}

String String_keyword(String str, char* keyword, char dest) { //in str replace all instances of keyword surrounded by not alphanum with dest
    uint kLength = strLength(keyword); //length of the keyword
    if (str.length < kLength) { //impossible for any instances
        return str;
    }
    uint y = 0; //index being written to
    for (uint x = 0; x < str.length; y++) { //loop through str
        char* chars = str.chars + x; //pointer to char being read
        if (x <= str.length - kLength) { //if chars has space to be an instance of keyword
            if ((x == 0 || !alphanum(chars[-1])) && (x == str.length - kLength || !alphanum(chars[kLength]))) { //characters before and after the keyword are not alphanum or non-existent
                bool good = true;
                for (uint z = 0; z < kLength; z++) {
                    if (chars[z] != keyword[z]) { //if its found not to be the keyword
                        good = false;
                        break;
                    }
                }
                if (good) { //if its the keyword
                    str.chars[y] = dest; //set the target to dest
                    x += kLength; //skip over the rest of the keyword
                    continue;
                }
            }
        }
        str.chars[y] = *chars; //copy from input to output since not keyword
        x++;
    }
    str.length = y; //fix string length
    return str;
}

String String_keywords(String str) { //replace all keywords in the string
    str = String_keyword(str, "if", IF);
    str = String_keyword(str, "else", ELSE);
    str = String_keyword(str, "while", WHILE);
    str = String_keyword(str, "fun", FUN);
    str = String_keyword(str, "return", RETURN);
    str = String_keyword(str, "print", PRINT);
    return str;
}

String String_delete(String str, char target) { //delete all instances of the target char from the string
    uint y = 0; //destination index
    for (uint x = 0; x < str.length; x++) { //iterator source through string
        if (str.chars[x] == target) { //if target found, skip
            continue;
        }
        str.chars[y] = str.chars[x]; //set dest to source
        y++;
    }
    str.length = y; //fix string length
    return str;
}

String String_deleteWhiteSpace(String str) { //delete all non newline whitespace from string
    str = String_delete(str, ' ');
    str = String_delete(str, '\t');
    str = String_delete(str, '\r');
    str = String_delete(str, '\v'); //verticle tab
    str = String_delete(str, '\f'); //cairrage return or smth
    return str;
}

String String_deleteComments(String str) {
    uint y = 0;
    bool good = true;
    for (uint x = 0; x < str.length; x++) {
        char c = str.chars[x];
        good = (good || c == '\n') && c != '#';
        if (good) {
            str.chars[y++] = c;
        }
    }
    str.length = y;
    return str;
}

String String_braceln(String str) { //add newlines before and after every { and }
    uint braces = 0; //num of braces
    for (uint x = 0; x < str.length; x++) {
        char c = str.chars[x];
        if (c == '{' || c == '}') {
            braces++;
        }
    }
    char* ans = (char*)malloc(str.length + 2 * braces); //new size is old size plus 2 for every brace
    uint y = 0; //destination index
    for (uint x = 0; x < str.length; x++) { //iterator source index through string
        char c = str.chars[x];
        if (c == '{' || c == '}') { //if brace
            //add newline, then brace, then newline to the destination
            ans[y] = '\n';
            ans[y + 1] = c;
            ans[y + 2] = '\n';
            y += 3;
            continue;
        }
        ans[y] = c; //set dest to src since not brace
        y++;
    }
    String_clear(str);
    return String_init(ans, str.length + 2 * braces);
}

String String_replace(String str, char* target, char dest) { //replace all instaces of target in str with dest
    uint tLength = strLength(target); //length of target
    uint y = 0; //destination index
    for (uint x = 0; x < str.length; y++) { //iterator through source indexes
        if (x + tLength > str.length || !prefix(str.chars + x, target)) { //if there's not space or the src is not an instance of target
            str.chars[y] = str.chars[x]; //set destintation index to src since not target
            x++;
            continue;
        }
        str.chars[y] = dest; //set destination index to dest since found target
        x += tLength; //skip over the rest of the target instance
    }
    str.length = y; //fix the string length
    return str;
}

String String_2CharOps(String str) { //replace all 2 char ops with the appropriate single char
    str = String_replace(str, "&&", '&');
    str = String_replace(str, "||", '|');
    str = String_replace(str, "==", DEQ);
    str = String_replace(str, "!=", NEQ);
    str = String_replace(str, ">=", GEQ);
    str = String_replace(str, "<=", LEQ);
    return str;
}

String String_compressln(String str) { //remove any empty lines, and make sure the string ends with a newline
    uint y = 0; //destination index
    for (uint x = 0; x < str.length;) { //iterate through source indexes
        for (;x < str.length && str.chars[x] != '\n'; x++, y++) { //copy everything in the line
            str.chars[y] = str.chars[x];
        }
        if (x == str.length) {
            break;
        }
        str.chars[y] = '\n'; //set the newline
        y++;
        for (;x < str.length && str.chars[x] == '\n'; x++); //skip over all superflous newlines
    }
    str.length = y; //fix string length
    if (str.length && *str.chars == '\n') { //if the first character is a newline, fix that
        memmove(str.chars, str.chars + 1, str.length - 1);
        str.length--;
    }
    if (str.length && str.chars[str.length - 1] != '\n') { //if the last character isn't a newline, fix that
        str = String_resize(str, str.length + 1);
        str.chars[str.length - 1] = '\n';
    }
    return str;
}

uint String_braceDepth(String str) { //maximum depth of braces in str
    uint depth = 0; //cur depth
    uint ans = 0;
    for (uint x = 0; x < str.length; x++) {
        char c = str.chars[x];
        if (c == '{') { //if { inc depth and maybe update ans
            depth++;
            if (depth > ans) {
                ans = depth;
            }
            continue;
        }
        if (c == '}') { //if } dec depth
            depth--;
        }
    }
    return ans;
}

uint String_count(String str, char target) { //count the instances of target in str
    uint ans = 0;
    for (uint x = 0; x < str.length; x++) {
        if (str.chars[x] == target) { //if target found inc count
            ans++;
        }
    }
    return ans;
}

ulong String_indexOf(String str, char c) { //get the index of the first c in str, else return -1
    for (uint x = 0; x < str.length; x++) {
        if (str.chars[x] == c) { //when c is found return index
            return x;
        }
    }
    return -(ulong)1; //c is not found, return -1
}

char String_last(String str) { //return the last char in str, or 0 if empty
    return str.length ? str.chars[str.length - 1] : 0;
}

char String_first(String str) { //return the first char in str, or 0 if empty
    return str.length ? *str.chars : 0;
}

String String_merge(String* strings, uint length) {
    uint tot = 0;
    for (uint x = 0; x < length; x++) {
        tot += strings[x].length;
    }
    String ans = String_initL(tot);
    uint cur = 0;
    for (uint x = 0; x < length; x++) {
        memcpy(ans.chars + cur, strings[x].chars, strings[x].length);
        cur += strings[x].length;
    }
    for (uint x = 0; x < length; x++) {
        String_clear(strings[x]);
    }
    free(strings);
    return ans;
}

String String_add(String a, String b) {
    a = String_resize(a, a.length + b.length);
    memcpy(a.chars + a.length - b.length, b.chars, b.length);
    String_clear(b);
    return a;   
}
