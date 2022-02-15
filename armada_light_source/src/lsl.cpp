#include <lsl.h>
#include <string.h>
#include <vm.h>
LSL::LSL()
{

}

void LSL::push(void* pArgs, VM* pVM){

    pVM->push(*(static_cast<uint32_t*>(pArgs)));
    pVM->CP++;
    //if(VM::CP < STACK_SIZE) VM::stack[VM::SP++]=data;
}

void LSL::pop(void* pArgs, VM* pVM)
{
   uint32_t var_index;
   var_index = *(static_cast<uint32_t*>(pArgs));
    if(pVM->CP > 0) pVM->SP--;
    //   if(VM::CP > 0) return VM::stack[VM::SP--];
    //  else return 0;
}

void LSL::done(void* pArgs, VM* pVM)
{

}

void LSL::minus(void* pArgs, VM* pVM)
{
    uint32_t a,b,result;
    b = pVM->pop();
    a = pVM->pop();
    result = a-b;
   // VM::CP++;
    pVM->push(result);
}

void LSL::division(void* pArgs, VM* pVM)
{
    uint32_t a,b,result;
    b = pVM->pop();
    a = pVM->pop();
    if (b==0) return;
    result = a/b;
    pVM->push(result);
}

void LSL::jump(void* pArgs, VM* pVM)
{
    uint32_t jump_address;
    jump_address = *(static_cast<uint32_t*>(pArgs));
    pVM->CP = jump_address;
}

void LSL::jump_if_equally(void*  pArgs, VM* pVM)
{
    uint32_t jump_address,compare_result;
    if (pVM->SP == 0) return;
  //  compare_result = VM::pop();
    if(pVM->STATUS_REGISTER == EQUALLY)
    {
        jump_address = *(static_cast<uint32_t*>(pArgs));
        pVM->CP = jump_address;
    }
}

void LSL::jump_if_more(void *pArgs, VM* pVM)
{
    uint32_t jump_address,compare_result;
    if (pVM->SP == 0) return;
  //  compare_result = VM::pop();
    if(pVM->STATUS_REGISTER == MORE)
    {
        jump_address = *(static_cast<uint32_t*>(pArgs));
        pVM->CP = jump_address;
    }
}

void LSL::jump_if_less(void *pArgs, VM* pVM)
{
    uint32_t jump_address,compare_result;
    if (pVM->SP == 0) return;
  //  compare_result = VM::pop();
    if(pVM->STATUS_REGISTER == LESS)
    {
        jump_address = *(static_cast<uint32_t*>(pArgs));
        pVM->CP = jump_address;
    }
}



//extern  std::map <int, tVariableDescription> VM::VariableDescriptionMap;
void LSL::init_var(void* pArgs, VM* pVM)
{
 VM_VariableTypes var_type;
 var_type = *(static_cast<VM_VariableTypes*>(pArgs));
 switch (var_type) {
 case VM_INTEGER:
 {
     uint32_t* pointer;
     pointer = (static_cast<uint32_t*>(pArgs));
     pointer++;
     uint32_t var_index = * pointer;
     pointer++;
     uint32_t var_value = *pointer;
     tVariableDescription var_def;
     var_def.var_type = var_type;
     var_def.var_pointer =  new uint32_t(var_value);

    pVM->VariableDescriptionMap[var_index] = var_def;
    pVM->CP+=3;
  }
     break;
 case VM_CHAR:
    {
     uint32_t* pointer;
     pointer = (static_cast<uint32_t*>(pArgs));
     pointer++;
     uint32_t var_index = * pointer;
     pointer++;
     uint32_t var_value = *pointer;
     char var_char;
    var_char = (static_cast<char>(var_value));
    tVariableDescription var_def;
    var_def.var_type = var_type;
    var_def.var_pointer =  new char(var_char);
    pVM->VariableDescriptionMap[var_index] = var_def;
    pVM->CP+=3;

    }

     break;
 case VM_STRING:
    {
     std::string str_tmp;
     uint32_t* pointer;
     pointer = (static_cast<uint32_t*>(pArgs));
     pointer++;
     uint32_t var_index = * pointer++;
     char var_char;
     do {
     uint32_t var_value = *pointer;

    var_char = (static_cast<char>(var_value));
    if ( var_char!='\0')
        {
            str_tmp +=  var_char;
            pointer++;
        }
        pVM->CP++;
     } while (var_char!='\0');
     tVariableDescription var_def;
     var_def.var_type = var_type;
     var_def.var_pointer =  new std::string(str_tmp);
     pVM->VariableDescriptionMap[var_index] = var_def;
     pVM->CP++;
    }


     break;
 default:
     break;
 }
}

void LSL::load_sys_var(void* pArgs, VM* pVM)
{
    uint32_t sys_var_index;
    uint32_t* pointer;
    pointer = (static_cast<uint32_t*>(pArgs));
    sys_var_index = * pointer;
    if( VM::SystemVariableDescriptionMap.find(sys_var_index)!= VM::SystemVariableDescriptionMap.end())
    {

    if( VM::SystemVariableDescriptionMap.at(sys_var_index).var_type == VM_INTEGER)
   //  pointer =static_cast<uint32_t*>(VM::SystemVariableDescriptionMap.at(sys_var_index).var_pointer);
 //    value = *(static_cast<uint32_t*>(VM::SystemVariableDescriptionMap[sys_var_index].var_pointer));
     pVM->push(*(static_cast<uint32_t*>(VM::SystemVariableDescriptionMap.at(sys_var_index).var_pointer)));
    }
    pVM->CP++;

}

void LSL::store_sys_var(void* pArgs, VM* pVM)
{
    uint32_t sys_var_index;
    uint32_t* pointer;
    pointer = (static_cast<uint32_t*>(pArgs));
    sys_var_index = * pointer;
    if( VM::SystemVariableDescriptionMap.find(sys_var_index)!= VM::SystemVariableDescriptionMap.end())
    {

       *(static_cast<uint32_t*>(VM::SystemVariableDescriptionMap.at(sys_var_index).var_pointer)) = pVM->pop();
    }
    pVM->CP++;
}

void LSL::compare(void* pArgs, VM* pVM)
{
    tCompareResult result;
    uint32_t a,b;
    if(pVM->SP <2) return;
    b = pVM->pop();
    a = pVM->pop();
    if (a==b) result = EQUALLY;
    else if (a>b) result = MORE;
    else if (a<b) result = LESS;
    //VM::push(result);
    pVM->STATUS_REGISTER = result;
}



void LSL::sum(void* pArgs, VM* pVM){
 uint32_t a,b,result;
 b = pVM->pop();
 a = pVM->pop();
 result = a+b;
// VM::CP++;
 pVM->push(result);
  //result =  static_cast<tArgs*>(pArgs)->a + static_cast<tArgs*>(pArgs)->b;


}


void LSL::multiply(void* pArgs, VM* pVM)
{
    uint32_t a,b,result;
    b = pVM->pop();
    a = pVM->pop();
    result = a*b;
   // VM::CP++;
    pVM->push(result);
}

void LSL::call(void* pArgs, VM* pVM)
{
    uint32_t call_address;
    call_address = *(static_cast<uint32_t*>(pArgs));
    pVM->push(pVM->CP+1);
    pVM->CP = call_address;
}

void LSL::ret(void* pArgs, VM* pVM)
{
    uint32_t call_address;
    call_address = pVM->pop();
    pVM->CP = call_address;
}



void LSL::nop(void* pArgs, VM* pVM)
{

}
