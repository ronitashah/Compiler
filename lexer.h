#pragma once

#include "Map.h"
#include "Code.h"
#include "Fun.h"
#include "Lines.h"

String StrFix(String str) { //fix the input string to tokenize keywords and 2char operators, delete superflous whitespace, and add newlines around braces
    str = String_keywords(str);
    str = String_deleteWhiteSpace(str);
    str = String_deleteComments(str);
    str = String_braceln(str);
    str = String_compressln(str);
    str = String_2CharOps(str);
    return str;
}

Lines* getBlocks(Lines lines, uint length) { //convert the lines into length many blocks, where the first block is all statements in the global scope and each other block is a function
    Lines* ans = (Lines*)calloc(length, sizeof(Lines));
    uint cur = -(uint)1; //last function entered
    //just get the length of each block first
    for (uint x = 0; x < lines.length; x++) {
        if (*lines.lines[x].chars == FUN) { //if the line begins a fun
            cur++; //inc the index of the last fun entered
        }
        ans[cur].length++;
    }
    for (uint x = 0; x < length; x++) { //allocate each block
        ans[x].lines = (String*)malloc(sizeof(String) * (ans[x].length + 1)); //length + 1 because all blocks will have a return 0 line as their last statement
        ans[x].length = 0; //reset length as it will be used to get the index when setting the lines
    }
    cur = -(uint)1;
    //now actually set the lines in the blocks
    for (uint x = 0; x < lines.length; x++) {
        String str = lines.lines[x];
        if (*str.chars == FUN) { //enter a function
            cur++;
        }
        ans[cur].lines[ans[cur].length++] = str;
    }
    for (uint x = 0; x < length; x++) {
        //for every function block, move the final } forward and add a return 0
        ans[x].lines[ans[x].length] = ans[x].lines[ans[x].length - 1];
        ans[x].lines[ans[x].length - 1] = String_init0(RETURN);
        ans[x].length++;
    }
    return ans;
}

void clearBlocks(Lines* blocks, uint length) { //clear the array of blocks
    for (uint x = 0; x < length; x++) {
        String_clear(blocks[x].lines[blocks[x].length - 2]); //clear the function block's return 0 added to the end before the final }
    }
    for (uint x = 0; x < length; x++) {
        Lines_clear(blocks[x]); //clear each block
    }
    free(blocks); //clear the array of blocks
}

Fun* getFunArr(Lines* blocks, uint length) { //get a map of all functions given the function blocks
    Fun* ans = (Fun*)malloc(length * sizeof(Fun));
    for (uint x = 0; x < length; x++) {
        String str = *blocks[x].lines; //first line of the function block -- the function declaration
        str.chars++; //skip the FUN first char in the lin
        str.length = String_indexOf(str, '(');
        ans[x] = Fun_init1(String_clone(str));
    }
    return ans;
}

Map getFunMap(Code code) {
    Map ans = Map_init();
    for (uint x = 0; x < code.length; x++) {
        ans = Map_put(ans, String_hash(code.funs[x].name), x);
    }
    return ans;
}

Map getParamMap(Lines block) { //get a map of all the params for the given funtion block
    String str = *block.lines; //first line of the function block -- the function declaration
    str.chars += String_indexOf(str, '(') + 1; //skip to the parameter list after the first (
    uint end = String_indexOf(str, ')'); //end of parameter list, the first )
    if (end == 0) { //if the parameter list is empty, no params
        return Map_init();
    }
    str.chars[end] = ','; //set the ) to a sentinel ,
    Map ans = Map_init();
    uint last = 0; //last index after ,
    for (uint x = 0; x <= end; x++) {
        if (str.chars[x] != ',') { //iterate until ,
            continue;
        }
        ans = Map_put(ans, String_hash(String_init(str.chars + last, x - last)), ans.size); //add substring between current and last comma to map
        last = x + 1; //update last
    }
    str.chars[end] = ')'; //set the sentinel , back to a )
    return ans;
}

Map getVarMap(Lines block, Map params) { //get a map of all locals(inc params) in a funcion block given a map of globals and params
    for (uint x = 0; x < block.length; x++) {
        String str = block.lines[x]; //current line
        uint index = String_indexOf(str, '='); //index of equals
        if (index == -(uint)1) { //no =, so not an assignment, so no local dec possible
            continue;
        }
        str.length = index;
        ulong hash = String_hash(str);
        if (Map_contains(params, hash)) { //if the var being set is already defined in either map, skip
            continue;
        }
        params = Map_put(params, hash, params.size); //add the var to the map of locals
    }
    return params;
}
