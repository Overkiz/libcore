/*
 * Context.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include "Context.h"

namespace Overkiz
{

  namespace Architecture
  {

    namespace Context
    {

      static void execute(Launcher *launcher)
      {
        if(launcher)
        {
          launcher->launch();
        }
      }

      void *init(void *stk, Launcher *launcher)
      {
        unsigned long *stack = (unsigned long *)((unsigned long) stk & ~(15UL));
        stack[0] = (unsigned long) execute; //execute func pointer storage
        stack[-1] = (unsigned long) execute; //old eip
        stack[-2] = (unsigned long) 0; //old esp
        stack[-3] = (unsigned long) 0; //old ebp
        stack[-4] = (unsigned long) 0; //old ebx
        stack[-5] = (unsigned long) 0; //old esi
        stack[-6] = (unsigned long) 0; //old edi
        stack[-7] = (unsigned long) 0; //old cw
        stack[-8] = (unsigned long) 0; //padding
        stack[-9] = (unsigned long) 0; //esp[2]
        stack[-10] = (unsigned long) 0; //esp[1]
        stack[-11] = (unsigned long) 0; //esp[0]
        stack[-12] = (unsigned long) launcher; //first arg
        return stack;
      }

      void resume(void *stk)
      {
        asm("movl %0, %%eax" : : "m"(stk));
        asm("movl %esp, %edx");
        asm("addl $4, %edx");
        asm("cmpl %esp, %ebp");
        asm("jne 1f");
        asm("addl $4, %edx");
        asm("1:");
        asm("movl %edx, -36(%eax)");
        asm("movl (%esp), %edx");
        asm("movl %edx, -44(%eax)");
        asm("movl -8(%eax), %ecx");
        asm("movl %esp, -8(%eax)");
        asm("movl 4(%esp), %edx");
        asm("movl %edx, -40(%eax)");
        asm("movl -4(%eax), %edx");
        asm("movl $2f, -4(%eax)");
        CFI(asm(".cfi_remember_state"));
        asm("movl %ebp, -12(%eax)");
        asm("movl %ebx, -16(%eax)");
        asm("movl %esi, -20(%eax)");
        asm("movl %edi, -24(%eax)");
        asm("fnstcw  -28(%eax)");
        asm("movl $959, -32(%eax)");
        asm("fldcw -32(%eax)");
        asm("subl $48, %eax");
        asm("movl %eax, %esp");
        CFI(asm(".cfi_def_cfa_register esp"));
        CFI(asm(".cfi_adjust_cfa_offset 4"));
        CFI(asm(".cfi_rel_offset esp, 12"));
        asm("call *%edx");
        asm("movl 48(%esp), %edx");
        asm("movl %esp, %ecx");
        asm("2:");
        asm("movl %esp, %eax");
        asm("addl $48, %eax");
        asm("fldcw -28(%eax)");
        asm("movl -24(%eax), %edi");
        asm("movl -20(%eax), %esi");
        asm("movl -16(%eax), %ebx");
        asm("movl -12(%eax), %ebp");
        asm("movl -8(%eax), %esp");
        CFI(asm(".cfi_restore_state"));
        asm("movl %edx, -4(%eax)");
        asm("movl %ecx, -8(%eax)");
      }

      void yield(void *stk)
      {
        asm("movl %0, %%eax" : : "m"(stk));
        asm("movl %eax, %edx");
        asm("subl $48, %edx");
        asm("movl -4(%eax), %eax");
        CFI(asm(".cfi_remember_state"));
        asm("subl $20, %esp");
        CFI(asm(".cfi_def_cfa_register esp"));
        CFI(asm(".cfi_adjust_cfa_offset 20"));
        asm("movl %ebp, 16(%esp)");
        asm("movl %ebx, 12(%esp)");
        asm("movl %esi, 8(%esp)");
        asm("movl %edi, 4(%esp)");
        asm("fnstcw  (%esp)");
        asm("movl %esp, %ecx");
        CFI(asm(".cfi_def_cfa_register ecx"));
        asm("movl %edx, %esp");
        asm("movl $1f, %edx");
        asm("jmp *%eax");
        asm("1:");
        asm("movl %ecx, %esp");
        CFI(asm(".cfi_def_cfa_register esp"));
        asm("fldcw (%esp)");
        asm("movl 4(%esp), %edi");
        asm("movl 8(%esp), %esi");
        asm("movl 12(%esp), %ebx");
        asm("movl 16(%esp), %ebp");
        asm("addl $20, %esp");
        CFI(asm(".cfi_adjust_cfa_offset -20"));
        CFI(asm(".cfi_restore_state"));
      }

    }

  }

}
