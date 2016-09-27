/*
 * UniversalUniqueIdentifier.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "UniversalUniqueIdentifier.h"
#include "Errno.h"

namespace Overkiz
{

  UniversalUniqueIdentifier::UniversalUniqueIdentifier()
  {
    clear();
  }

  UniversalUniqueIdentifier::UniversalUniqueIdentifier(const char * uuid)
  {
    build(uuid);
  }

  UniversalUniqueIdentifier::UniversalUniqueIdentifier(const std::string & uuid)
  {
    build(uuid.c_str());
  }

  UniversalUniqueIdentifier::UniversalUniqueIdentifier(
    const Overkiz::UniversalUniqueIdentifier & src)
  {
    copy(src);
  }

  UniversalUniqueIdentifier::~UniversalUniqueIdentifier()
  {
    clear();
  }

  void UniversalUniqueIdentifier::generate()
  {
    int fd = open("/dev/urandom", O_RDONLY);

    if(fd < 0)
      throw Overkiz::Errno::Exception();

    ssize_t ret = ::read(fd, identifier, sizeof(identifier));

    if(ret != sizeof(identifier))
    {
      throw Overkiz::Errno::Exception();
    }

    identifier[8] &= 0x3F;
    identifier[8] |= 0x80;
    identifier[6] &= 0x0F;
    identifier[6] |= 0x40;
    ::close(fd);
  }

  void UniversalUniqueIdentifier::clear()
  {
    memset(identifier, 0, sizeof(identifier));
  }

  void UniversalUniqueIdentifier::copy(
    const Overkiz::UniversalUniqueIdentifier & src)
  {
    memcpy(identifier, src.identifier, sizeof(identifier));
  }

  int UniversalUniqueIdentifier::compare(
    const Overkiz::UniversalUniqueIdentifier & uuid) const
  {
    return memcmp(identifier, uuid.identifier, sizeof(identifier));
  }

  int UniversalUniqueIdentifier::compare(const std::string & uuid) const
  {
    Overkiz::UniversalUniqueIdentifier tmp(uuid);
    return compare(tmp);
  }

  bool UniversalUniqueIdentifier::isNil() const
  {
    uint8_t nil[16];
    memset(nil, 0, sizeof(nil));
    return memcmp(identifier, nil, sizeof(identifier)) == 0 ? true : false;
  }
  void UniversalUniqueIdentifier::build(const char * uuid)
  throw(Overkiz::UniversalUniqueIdentifier::MalformedException)
  {
    uint8_t id[16];
    int j = 0;
    memset(id, 0, sizeof(id));

    for(int i = 0; i < 36; i++)
    {
      if(i == 8 || i == 13 || i == 18 || i == 23)
      {
        if(uuid[i] != '-')
        {
          MalformedException e;
          throw e;
        }
      }
      else
      {
        uint8_t val;

        if(uuid[i] >= '0' && uuid[i] <= '9')
        {
          val = uuid[i] - '0';
        }
        else if(uuid[i] >= 'a' && uuid[i] <= 'f')
        {
          val = 10 + uuid[i] - 'a';
        }
        else if(uuid[i] >= 'A' && uuid[i] <= 'F')
        {
          val = 10 + uuid[i] - 'A';
        }
        else
        {
          MalformedException e;
          throw e;
        }

        id[j / 2] |= (val << (4 - (4 * (j % 2))));
        ++j;
      }
    }

    memcpy(identifier, id, sizeof(identifier));
  }

  void UniversalUniqueIdentifier::build(const std::string & uuid)
  throw(Overkiz::UniversalUniqueIdentifier::MalformedException)
  {
    build(uuid.c_str());
  }

  void UniversalUniqueIdentifier::get(char * cstring,
                                      Overkiz::UniversalUniqueIdentifier::Case stringCase) const
  {
    int j = 0;

    for(int i = 0; i < 16; ++i)
    {
      for(int k = 0; k < 2; ++k)
      {
        uint8_t val = (identifier[i] >> (4 - (k * 4))) & 0x0F;

        if(val < 10)
        {
          cstring[j++] = '0' + val;
        }
        else
        {
          val -= 10;

          switch(stringCase)
          {
            case UPPER_CASE:
              cstring[j++] = 'A' + val;
              break;

            case LOWER_CASE:
            default:
              cstring[j++] = 'a' + val;
              break;
          }
        }
      }

      if(i == 3 || i == 5 || i == 7 || i == 9)
      {
        cstring[j++] = '-';
      }
    }

    cstring[j] = '\0';
  }

  void UniversalUniqueIdentifier::get(std::string & string,
                                      Overkiz::UniversalUniqueIdentifier::Case stringCase) const
  {
    char cstring[37];
    get(cstring, stringCase);
    string = cstring;
  }

  std::string UniversalUniqueIdentifier::getString(
    Overkiz::UniversalUniqueIdentifier::Case stringCase) const
  {
    char cstring[37];
    get(cstring, stringCase);
    return std::string(cstring);
  }

  Overkiz::UniversalUniqueIdentifier UniversalUniqueIdentifier::get()
  {
    Overkiz::UniversalUniqueIdentifier uuid;
    uuid.generate();
    return uuid;
  }

  const UniversalUniqueIdentifier UniversalUniqueIdentifier::NIL;
  const std::string UniversalUniqueIdentifier::NIL_STRING =
    "00000000-0000-0000-0000-000000000000";

}
