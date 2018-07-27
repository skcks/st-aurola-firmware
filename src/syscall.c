#include "syscall.h"


void __wrap_SystemInit() 
{
    __real_SystemInit();
    /*rt-thread os入口 */
    entry();
    
    
}