#ifndef LSL_H
#define LSL_H
//#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <stdint.h>
#include<functional>
//#include "vm.h"

class VM;

//typedef void(*tpFuncToCall)(void*);
using tpFuncToCall = void(*)(void*, VM*);

#pragma pack(push, 1)
struct FuncDef{
    tpFuncToCall funcAddress;

};
#pragma pack(pop)
using tFuncDef = FuncDef ;


#pragma pack(push, 1)
struct Args{
    uint32_t a;
    uint32_t b;
};
#pragma pack(pop)
using  tArgs = Args;

enum optoCode {
    NOP=0,
    PUSH,
    POP,
    SUM,
    DONE,
    MINUS,
    DIVISION,
    JUMP,
    INIT_VAR,
    LOAD_SYS_VAR,
    STORE_SYS_VAR,
    COMPARE,
    JUMP_IF_EQUALLY,
    JUMP_IF_MORE,
    JUMP_IF_LESS,
    MULTIPLY,
    CALL,
    RET

};

using tOptpCode = optoCode;

enum compareResult{
    EQUALLY,
    MORE,
    LESS
};
using tCompareResult = compareResult;

class LSL
{
public:
    LSL();
    static void sum(void*,VM* pVM);
    static void nop(void*,VM* pVM);
    static void push(void*, VM* pVM);
    static void pop(void*,VM* pVM);
    static void done(void*,VM* pVM);
    static void minus(void*,VM* pVM);
    static void division(void*,VM* pVM);
    static void jump(void*,VM* pVM);
    static void init_var(void*,VM* pVM);
    static void load_sys_var(void*,VM* pVM);
    static void store_sys_var(void*,VM* pVM);
    static void compare(void*,VM* pVM);
    static void jump_if_equally(void*,VM* pVM);
    static void jump_if_more(void*,VM* pVM);
    static void jump_if_less(void*,VM* pVM);
    static void multiply(void*,VM* pVM);
    static void call(void*,VM* pVM);
    static void ret(void*,VM* pVM);

};

#endif // LSL_H
