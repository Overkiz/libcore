/*
 * Context.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <stdio.h>
#include <string.h>

#ifndef ARCHCONTEXT_H_
#define ARCHCONTEXT_H_

namespace Overkiz
{

  namespace Architecture
  {

    namespace Context
    {

#ifdef __GCC_HAVE_DWARF2_CFI_ASM
  #define CFI(str) str
#else
  #define CFI(str)
#endif

      class Launcher
      {
      protected:

        Launcher()
        {
        }

        virtual ~Launcher()
        {
        }

      public:

        virtual void launch() = 0;

      };

      void *init(void *stack, Launcher *launcher);

      void resume(void *stk);

      void yield(void *stk);
    }

  }

}

#endif /* ARCHCONTEXT_H_ */
