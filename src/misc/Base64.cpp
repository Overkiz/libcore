/*
 * Coroutine.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <iostream>

#include <kizbox/framework/core/Base64.h>

namespace Overkiz
{
  char Base64::encodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/";

  char Base64::decodeTable[] = { 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
                                 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
                                 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 62, 65, 65, 65, 63,
                                 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 65, 65, 65, 64, 65, 65, 65, 0,
                                 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                                 21, 22, 23, 24, 25, 65, 65, 65, 65, 65, 65, 26, 27, 28, 29, 30, 31, 32,
                                 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                                 51, 65, 65, 65, 65, 65
                               };

  void Base64::encode(const uint8_t * src, int32_t size, std::string& dest)
  {
    dest.clear();

    //If empty blob, return a base64 string empty.
    if(size == 0 || src == nullptr)
    {
      return;
    }

    int32_t i = 0;

    while(i < size)
    {
      uint8_t tmp;
      tmp = (src[i] >> 2);
      dest.append(1, encodeTable[tmp]);
      tmp = ((src[i] << 4) & 0x3F);

      if(++i == size)
      {
        dest.append(1, encodeTable[tmp]);
        dest.append(2, '=');
      }
      else
      {
        tmp |= (src[i] >> 4);
        dest.append(1, encodeTable[tmp]);
        tmp = ((src[i] << 2) & 0x3F);

        if(++i == size)
        {
          dest.append(1, encodeTable[tmp]);
          dest.append(1, '=');
        }
        else
        {
          tmp |= (src[i] >> 6);
          dest.append(1, encodeTable[tmp]);
          tmp = (src[i] & 0x3F);
          dest.append(1, encodeTable[tmp]);
          ++i;
        }
      }
    }

    if(!dest.size() % 4)
    {
      dest.append(4 - (dest.size() % 4), '=');
    }
  }

  void Base64::encode(const std::vector<uint8_t> & src, std::string & dest)
  {
    dest.clear();

    //If empty blob, return a base64 string empty.
    if(src.empty())
    {
      return;
    }

    std::vector<uint8_t>::const_iterator i = src.begin();

    while(i != src.end())
    {
      uint8_t tmp;
      tmp = (*i >> 2);
      dest.append(1, encodeTable[tmp]);
      tmp = ((*i << 4) & 0x3F);

      if(++i == src.end())
      {
        dest.append(1, encodeTable[tmp]);
        dest.append(2, '=');
      }
      else
      {
        tmp |= (*i >> 4);
        dest.append(1, encodeTable[tmp]);
        tmp = ((*i << 2) & 0x3F);

        if(++i == src.end())
        {
          dest.append(1, encodeTable[tmp]);
          dest.append(1, '=');
        }
        else
        {
          tmp |= (*i >> 6);
          dest.append(1, encodeTable[tmp]);
          tmp = (*i & 0x3F);
          dest.append(1, encodeTable[tmp]);
          ++i;
        }
      }
    }

    if(!dest.size() % 4)
    {
      dest.append(4 - (dest.size() % 4), '=');
    }
  }

  int32_t Base64::decode(const std::string& src, uint8_t * dest, int32_t size)
  {
    std::string::const_iterator i = src.begin();
    int32_t index = 0;

    while(true)
    {
      if(i == src.end())
      {
        break;
      }

      uint8_t tmp[4] = { 64, 64, 64, 64 };

      for(int idx = 0; idx < 4; idx++)
      {
        tmp[idx] = decodeTable[uint8_t (*i)];
        ++i;

        if(tmp[idx] >= 65)
        {
          throw Base64::BadReformating();
        }

        if(i == src.end())
        {
          break; //loop for, because it's the end of the input string.
        }
      }

      if(tmp[0] == 64)
      {
        break;
      }

      tmp[0] <<= 2;

      if(tmp[1] == 64)
      {
        if((index + 1) > size)
        {
          throw OutOfBound();
        }

        dest[index++] = tmp[0];
        break;
      }
      else
      {
        tmp[0] |= tmp[1] >> 4;

        if((index + 1) > size)
        {
          throw OutOfBound();
        }

        dest[index++] = tmp[0];
      }

      if(tmp[2] == 64)
      {
        if(tmp[3] != 64)
        {
          throw Base64::BadEndOfString();
        }
        else
        {
          break;
        }
      }
      else
      {
        tmp[1] = (tmp[1] << 4) | (tmp[2] >> 2);

        if((index + 1) > size)
        {
          throw OutOfBound();
        }

        dest[index++] = tmp[1];

        if(tmp[3] != 64)
        {
          tmp[2] = (tmp[2] << 6) | tmp[3];

          if((index + 1) > size)
          {
            throw OutOfBound();
          }

          dest[index++] = tmp[2];
        }
      }
    }

    return index;
  }

  void Base64::decode(const std::string& src, std::vector<uint8_t>& dest)
  {
    dest.clear();
    std::string::const_iterator i = src.begin();

    while(true)
    {
      if(i == src.end())
      {
        break;
      }

      uint8_t tmp[4] = { 64, 64, 64, 64 };

      for(int idx = 0; idx < 4; idx++)
      {
        tmp[idx] = decodeTable[uint8_t (*i)];
        ++i;

        if(tmp[idx] >= 65)
        {
          throw Base64::BadReformating();
        }

        if(i == src.end())
        {
          break; //loop for, because it's the end of the input string.
        }
      }

      if(tmp[0] == 64)
      {
        break;
      }

      tmp[0] <<= 2;

      if(tmp[1] == 64)
      {
        dest.push_back(tmp[0]);
        break;
      }
      else
      {
        tmp[0] |= tmp[1] >> 4;
        dest.push_back(tmp[0]);
      }

      if(tmp[2] == 64)
      {
        if(tmp[3] != 64)
        {
          throw Base64::BadEndOfString();
        }
        else
        {
          break;
        }
      }
      else
      {
        tmp[1] = (tmp[1] << 4) | (tmp[2] >> 2);
        dest.push_back(tmp[1]);

        if(tmp[3] != 64)
        {
          tmp[2] = (tmp[2] << 6) | tmp[3];
          dest.push_back(tmp[2]);
        }
      }
    }
  }

}

