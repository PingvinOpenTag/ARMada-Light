#ifndef VM_H
#define VM_H

#include "lsl.h"
#include <map>    //подключили библиотеку для работы с map

#define STACK_SIZE 1024
//typedef void(*tpFuncToCall)(void*);

typedef enum  variable_types{
    VM_INTEGER,
    VM_CHAR,
    VM_STRING

} VM_VariableTypes;

struct VariableDescription{
     VM_VariableTypes var_type;
     void* var_pointer;
};
typedef struct VariableDescription tVariableDescription;

class MyMap : public std::map<int, tVariableDescription>
{
public:
    MyMap();

        //either

        //or
  //      (*this)[1] = 2;
 //       (*this)[3] = 4;
 //       (*this)[5] = 6;

};// const static my_map;

class VM
{
public:
     std::map <int, tVariableDescription> VariableDescriptionMap;
    static const MyMap SystemVariableDescriptionMap;
    static const tFuncDef LSL_funcList[] ;
    VM();
    tpFuncToCall pF;


    uint32_t CP;//указатель на текущую позицию в байт коде
    uint32_t SP;//указатель на вершину стека
    uint32_t stack[1024];
    uint32_t STATUS_REGISTER;

    void exec_optocod(tOptpCode);
    void push(uint32_t data);
    uint32_t pop(void);
    void removeAllVariables(void);

};






#endif // VM_H
