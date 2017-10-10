/*
 * Event.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <algorithm>
#include <string>
#include <cstring>

#include "Errno.h"

#define ERRNO_ID_PREFIX     "com.kizbox.Framework.Core.Errno."
#define ERRNO_MAX_SIZE 512

namespace Overkiz
{

  namespace Errno
  {
    Exception::Exception(int error) :
      code(error)
    {
    }

    Exception::~Exception()
    {
    }

    int Exception::getErrno() const
    {
      return errno;
    }

    const char * Exception::getId() const
    {
      return Exception::getString(code);
    }

    const char * Exception::getString(const int error)
    {
      static char buffer[ERRNO_MAX_SIZE] = {0}; /* Thread safe function scope */
      sprintf(buffer, ERRNO_ID_PREFIX "%s", strerror(error));

      for(char * b = buffer + strlen(ERRNO_ID_PREFIX); *b != '\0'; ++b)
      {
        if(!isalpha(*b))
        {
          *b = '-';
          *(b+1) = toupper(*(b+1));
        }
      }

      return buffer;
    }

  }

}
