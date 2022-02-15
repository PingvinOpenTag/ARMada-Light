#include <string.h>
#include <vm.h>


VM::VM(){


}

extern uint32_t bytecod[];

void VM::exec_optocod(tOptpCode optocod)
{

    pF=LSL_funcList[optocod].funcAddress;
    pF(&bytecod[VM::CP], this);
}

void VM::push(uint32_t data)
{
    if(VM::CP < STACK_SIZE) VM::stack[VM::SP++]=data;
}

uint32_t VM::pop(void)
{
    if(VM::CP > 0) return VM::stack[--VM::SP];
    else return 0;
}

void VM::removeAllVariables()
{
    std::map<int, tVariableDescription>::const_iterator it;
    for(it = VM::VariableDescriptionMap.begin();it !=  VM::VariableDescriptionMap.end(); ++it )
    {
       // std::cout << it->first << " : " << it->second.var_type << ":" << it->second.var_pointer << std::endl; //Вывод ключей и значений
        switch (it->second.var_type) {
        case VM_INTEGER:
            {
                uint32_t* int_pointer;
                int_pointer = static_cast<uint32_t*>(it->second.var_pointer);
  //              std::cout << "pointer: " << int_pointer << std::endl;
 //               std::cout << "value: " << (*int_pointer) << std::endl;

                delete int_pointer;
            }
            break;
        case VM_CHAR:
        {
            uint32_t* char_pointer;
            char_pointer = static_cast<uint32_t*>(it->second.var_pointer);
       //     std::cout << "pointer: " << char_pointer << std::endl;
      //      std::cout << "value: " << static_cast<char>(*char_pointer) << std::endl;

            delete char_pointer;

        }
            break;
        case VM_STRING:
        {
            uint32_t* string_pointer;
            std::string str_tmp;
            string_pointer = static_cast<uint32_t*>(it->second.var_pointer);
            str_tmp = *(static_cast<std::string*>(it->second.var_pointer));
     //       std::cout << "pointer: " << string_pointer << std::endl;
    //        std::cout << "value: " << str_tmp << std::endl;

            delete string_pointer;

        }
            break;



        default:
            break;
        }

        //delete (it->second.var_pointer);
    }
    VM::VariableDescriptionMap.clear();
}


tFuncDef const VM::LSL_funcList[]={{LSL::nop},
                                   {LSL::push},
                                   {LSL::pop},
                                   {LSL::sum},
                                   {LSL::done},
                                   {LSL::minus},
                                   {LSL::division},
                                   {LSL::jump},
                                   {LSL::init_var},
                                   {LSL::load_sys_var},
                                   {LSL::store_sys_var},
                                   {LSL::compare},
                                   {LSL::jump_if_equally},
                                   {LSL::jump_if_more},
                                   {LSL::jump_if_less},
                                   {LSL::multiply},
                                   {LSL::call},
                                   {LSL::ret}
                                  };
//uint32_t VM::stack[STACK_SIZE];

uint32_t damage = 5;

// uint32_t VM::CP = 0;//указатель на текущую позицию в байт коде
// uint32_t VM::SP = 0;//указатель на вершину стека
// uint32_t VM::STATUS_REGISTER; //регистр статуса операции сравнения
 //std::map <int, tVariableDescription> VM::VariableDescriptionMap;
 //const std::map <int, tVariableDescription> VM::SystemVariableDescriptionMap = {0,{VM_INTEGER,static_cast<void*>(&damage)}};

const MyMap VM::SystemVariableDescriptionMap;

 MyMap::MyMap(){

   (*this)[1]={VM_INTEGER,static_cast<void*>(&damage)};
     //  (*this)[1]=var_dskrpt;

    // insert(std::make_pair(0, {{VM_INTEGER},{static_cast<void*>(&damage)}}));
 //    insert(make_pair(3, 4));
//     insert(make_pair(5, 6));

 }
