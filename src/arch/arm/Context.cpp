/*
 * Directory.cpp
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
        stack[0] = (unsigned long) execute; //execute
        //        stack[-1] = (unsigned long)0;//esp
        stack[-2] = (unsigned long) execute; //eip
        stack[-3] = (unsigned long) launcher; //launcher
        return stack;
      }

      __attribute__((naked)) void resume(void *stk)
      {
        asm("sub r1, r0, #12");
        asm("ldmfd r1, {r0, r2, r3}");
        CFI(asm(".cfi_remember_state"));
        asm("sub r1, r1, #4");
        asm("stmfd r1!, {r4-r11, sp, lr}");
        asm(".save {r4-r11, sp, lr}");
        asm("mov sp, r1");
        CFI(asm(".cfi_def_cfa sp, 40"));
        CFI(asm(".cfi_offset lr, -4"));
        CFI(asm(".cfi_offset sp, -8"));
        CFI(asm(".cfi_offset r11, -12"));
        CFI(asm(".cfi_offset r10, -16"));
        CFI(asm(".cfi_offset r9, -20"));
        CFI(asm(".cfi_offset r8, -24"));
        CFI(asm(".cfi_offset r7, -28"));
        CFI(asm(".cfi_offset r6, -32"));
        CFI(asm(".cfi_offset r5, -36"));
        CFI(asm(".cfi_offset r4, -40"));
        asm("ldr r4, =yield_ret");
        asm("str r4, [r1, #48]");
        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        asm("stmfd sp!, {s16-s31}");
        asm(".save {s16-s31}");
        CFI(asm(".cfi_adjust_cfa_offset 64"));
        CFI(asm(".cfi_offset s31, -44"));
        CFI(asm(".cfi_offset s30, -48"));
        CFI(asm(".cfi_offset s29, -52"));
        CFI(asm(".cfi_offset s28, -56"));
        CFI(asm(".cfi_offset s27, -60"));
        CFI(asm(".cfi_offset s26, -64"));
        CFI(asm(".cfi_offset s25, -68"));
        CFI(asm(".cfi_offset s24, -72"));
        CFI(asm(".cfi_offset s23, -76"));
        CFI(asm(".cfi_offset s22, -80"));
        CFI(asm(".cfi_offset s21, -84"));
        CFI(asm(".cfi_offset s20, -88"));
        CFI(asm(".cfi_offset s19, -92"));
        CFI(asm(".cfi_offset s18, -96"));
        CFI(asm(".cfi_offset s17, -100"));
        CFI(asm(".cfi_offset s16, -104"));
        #endif
        asm("blx r2");
        asm("mov r3, sp");
        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        asm("ldr r2, [sp, #120]");
        #else
        asm("ldr r2, [sp, #56]");
        #endif
        asm("yield_ret:");
        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        asm("ldmfd sp!, {s16-s31}");
        CFI(asm(".cfi_restore s16"));
        CFI(asm(".cfi_restore s17"));
        CFI(asm(".cfi_restore s18"));
        CFI(asm(".cfi_restore s19"));
        CFI(asm(".cfi_restore s20"));
        CFI(asm(".cfi_restore s21"));
        CFI(asm(".cfi_restore s22"));
        CFI(asm(".cfi_restore s23"));
        CFI(asm(".cfi_restore s24"));
        CFI(asm(".cfi_restore s25"));
        CFI(asm(".cfi_restore s26"));
        CFI(asm(".cfi_restore s27"));
        CFI(asm(".cfi_restore s28"));
        CFI(asm(".cfi_restore s29"));
        CFI(asm(".cfi_restore s30"));
        CFI(asm(".cfi_restore s31"));
        #endif
        asm("mov r1, sp");
        asm("ldmfd r1!, {r4-r11, sp, lr}");
        CFI(asm(".cfi_restore r4"));
        CFI(asm(".cfi_restore r5"));
        CFI(asm(".cfi_restore r6"));
        CFI(asm(".cfi_restore r7"));
        CFI(asm(".cfi_restore r8"));
        CFI(asm(".cfi_restore r9"));
        CFI(asm(".cfi_restore r10"));
        CFI(asm(".cfi_restore r11"));
        CFI(asm(".cfi_restore sp"));
        CFI(asm(".cfi_restore lr"));
        asm("add r1, r1, #16");
        CFI(asm(".cfi_restore_state"));
        asm("stmfd r1, {r2, r3}");
        asm("bx lr");
      }

      __attribute__((naked)) void yield(void *stk)
      {
        asm("stmfd sp!, {r4-r11, lr}");
        asm(".save {r4-r11, lr}");
        CFI(asm(".cfi_def_cfa_offset 36"));
        CFI(asm(".cfi_offset lr, -4"));
        CFI(asm(".cfi_offset r11, -8"));
        CFI(asm(".cfi_offset r10, -12"));
        CFI(asm(".cfi_offset r9, -16"));
        CFI(asm(".cfi_offset r8, -20"));
        CFI(asm(".cfi_offset r7, -24"));
        CFI(asm(".cfi_offset r6, -28"));
        CFI(asm(".cfi_offset r5, -32"));
        CFI(asm(".cfi_offset r4, -36"));
        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        asm("stmfd sp!, {s16-s31}");
        asm(".save {s16-s31}");
        CFI(asm(".cfi_adjust_cfa_offset 64"));
        CFI(asm(".cfi_offset s31, -40"));
        CFI(asm(".cfi_offset s30, -44"));
        CFI(asm(".cfi_offset s29, -48"));
        CFI(asm(".cfi_offset s28, -52"));
        CFI(asm(".cfi_offset s27, -56"));
        CFI(asm(".cfi_offset s26, -60"));
        CFI(asm(".cfi_offset s25, -64"));
        CFI(asm(".cfi_offset s24, -68"));
        CFI(asm(".cfi_offset s23, -72"));
        CFI(asm(".cfi_offset s22, -76"));
        CFI(asm(".cfi_offset s21, -80"));
        CFI(asm(".cfi_offset s20, -84"));
        CFI(asm(".cfi_offset s19, -88"));
        CFI(asm(".cfi_offset s18, -92"));
        CFI(asm(".cfi_offset s17, -96"));
        CFI(asm(".cfi_offset s16, -100"));
        #endif
        asm("ldr r2, =resume_ret");
        asm("ldr r1, [r0, #-8]");
        CFI(asm(".cfi_remember_state"));
        asm("mov r3, sp");
        CFI(asm(".cfi_def_cfa_register r3"));
        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        asm("sub sp, r0, #120");
        #else
        asm("sub sp, r0, #56");
        #endif
        asm("bx r1");
        asm("resume_ret:");
        asm("mov sp, r3");
        CFI(asm(".cfi_restore_state"));
        #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
        asm("ldmfd sp!, {s16-s31}");
        CFI(asm(".cfi_restore s16"));
        CFI(asm(".cfi_restore s17"));
        CFI(asm(".cfi_restore s18"));
        CFI(asm(".cfi_restore s19"));
        CFI(asm(".cfi_restore s20"));
        CFI(asm(".cfi_restore s21"));
        CFI(asm(".cfi_restore s22"));
        CFI(asm(".cfi_restore s23"));
        CFI(asm(".cfi_restore s24"));
        CFI(asm(".cfi_restore s25"));
        CFI(asm(".cfi_restore s26"));
        CFI(asm(".cfi_restore s27"));
        CFI(asm(".cfi_restore s28"));
        CFI(asm(".cfi_restore s29"));
        CFI(asm(".cfi_restore s30"));
        CFI(asm(".cfi_restore s31"));
        #endif
        asm("ldmfd sp!, {r4-r11, lr}");
        CFI(asm(".cfi_restore r4"));
        CFI(asm(".cfi_restore r5"));
        CFI(asm(".cfi_restore r6"));
        CFI(asm(".cfi_restore r7"));
        CFI(asm(".cfi_restore r8"));
        CFI(asm(".cfi_restore r9"));
        CFI(asm(".cfi_restore r10"));
        CFI(asm(".cfi_restore r11"));
        CFI(asm(".cfi_restore lr"));
        asm("bx lr");
      }

    }

  }

}
