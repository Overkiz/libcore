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
        stack[0] = (unsigned long) execute; //execute storage
        stack[-1] = (unsigned long) launcher; //arg storage
        stack[-2] = (unsigned long) execute; //old rip
        stack[-3] = (unsigned long) 0; //old rsp
        stack[-4] = (unsigned long) 0; //old rbp
        stack[-5] = (unsigned long) 0; //old rbx
        stack[-6] = (unsigned long) 0; //old r12
        stack[-7] = (unsigned long) 0; //old r13
        stack[-8] = (unsigned long) 0; //old r14
        stack[-9] = (unsigned long) 0; //old r15
        stack[-10] = (unsigned long) 0; //old cw
        stack[-11] = (unsigned long) 0; //rsp[1]
        stack[-12] = (unsigned long) 0; //rsp[0]
        return stack;
      }

      void resume(void *stk)
      {
        asm("movq (%rsp), %rdx");
        asm("movq %rdx, -96(%rdi)");
        asm("movq -24(%rdi), %rcx");
        asm("movq %rsp, %rax");
        asm("addq $8, %rax");
        asm("cmpq %rsp, %rbp");
        asm("jne 1f");
        asm("addq $8, %rax");
        asm("1:");
        asm("movq %rax, -24(%rdi)");
        asm("movq 8(%rsp), %rdx");
        asm("movq %rdx, -88(%rdi)");
        asm("movq -16(%rdi), %rdx");
        asm("leaq 2f(%rip), %rax");
        asm("movq %rax, -16(%rdi)");
        CFI(asm(".cfi_remember_state"));
        asm("movq %rbp, -32(%rdi)");
        asm("movq %rbx, -40(%rdi)");
        asm("movq %r12, -48(%rdi)");
        asm("movq %r13, -56(%rdi)");
        asm("movq %r14, -64(%rdi)");
        asm("movq %r15, -72(%rdi)");
        asm("fnstcw  -80(%rdi)");
        asm("movq $959, -8(%rsp)");
        asm("fldcw -8(%rsp)");
        asm("subq $96, %rdi");
        asm("movq %rdi, %rsp");
        CFI(asm(".cfi_def_cfa_register rsp"));
        CFI(asm(".cfi_rel_offset rsp, 72"));
        asm("movq 88(%rdi), %rdi");
        asm("call *%rdx");
        asm("movq 96(%rsp), %rdx");
        asm("movq %rsp, %rcx");
        asm("2:");
        asm("movq %rsp, %rdi");
        asm("addq $96, %rdi");
        asm("fldcw -80(%rdi)");
        asm("movq -72(%rdi), %r15");
        asm("movq -64(%rdi), %r14");
        asm("movq -56(%rdi), %r13");
        asm("movq -48(%rdi), %r12");
        asm("movq -40(%rdi), %rbx");
        asm("movq -32(%rdi), %rbp");
        asm("movq -24(%rdi), %rax");
        asm("subq $16, %rax");
        asm("cmpq %rax, %rbp");
        asm("je 3f");
        asm("addq $8, %rax");
        asm("3:");
        asm("movq %rax, %rsp");
        CFI(asm(".cfi_restore_state"));
        asm("movq %rdx, -16(%rdi)");
        asm("movq %rcx, -24(%rdi)");
      }

      void yield(void *stk)
      {
        asm("movq %rbp, -8(%rsp)");
        asm("movq %rbx, -16(%rsp)");
        asm("movq %r12, -24(%rsp)");
        asm("movq %r13, -32(%rsp)");
        asm("movq %r14, -40(%rsp)");
        asm("movq %r15, -48(%rsp)");
        asm("fnstcw  -56(%rsp)");
        asm("movq %rsp, %rcx");
        asm("movq %rdi, %rdx");
        asm("subq $96, %rdx");
        CFI(asm(".cfi_remember_state"));
        CFI(asm(".cfi_def_cfa_register rcx"));
        asm("movq %rdx, %rsp");
        asm("leaq 1f(%rip), %rdx");
        asm("movq -16(%rdi), %rdi");
        asm("jmp *%rdi");
        asm("1:");
        asm("movq %rcx, %rsp");
        CFI(asm(".cfi_restore_state"));
        asm("fldcw -56(%rsp)");
        asm("movq -48(%rsp), %r15");
        asm("movq -40(%rsp), %r14");
        asm("movq -32(%rsp), %r13");
        asm("movq -24(%rsp), %r12");
        asm("movq -16(%rsp), %rbx");
        CFI(asm(".cfi_restore rbx"));
        asm("movq -8(%rsp), %rbp");
      }

    }

  }

}
