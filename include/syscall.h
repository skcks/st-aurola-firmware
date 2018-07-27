#ifndef __SYSCALL_H
#define __SYSCALL_H



extern void entry();
extern void __real_SystemInit();

#ifdef __cplusplus
extern "C" {
#endif 

void __wrap_SystemInit();

#ifdef __cplusplus
}
#endif

#endif