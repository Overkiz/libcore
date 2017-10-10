/*
 * CRC.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_CRC_H_
#define OVERKIZ_CRC_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

namespace Overkiz
{

  /**
   * Class used to manage events.
   */
  template<typename T>
  class CRC
  {
  public:

    /**
     * Constructor.
     *
     * @return
     */
    CRC(T poly, T initRemainder = 0, T finalXor = 0, bool rflData = false,
        bool rflRemainder = false)
    {
      initial = initRemainder;
      final = finalXor;
      remainder = initRemainder;
      polynomial = poly;
      reflectData = rflData;
      reflectRemainder = rflRemainder;
      init();
    }

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~CRC()
    {
    }

    void feed(const void *data, size_t size)
    {
      uint8_t *buf = (uint8_t *) data;
      uint8_t tmp;

      for(size_t i = 0; i < size; ++i)
      {
        tmp = buf[i];

        if(reflectData)
          tmp = reflect(tmp);

        tmp ^= (remainder >> ((sizeof(T) - 1) * 8));
        remainder = table[tmp] ^ (remainder << 8);
      }
    }

    T operator *()
    {
      T crc = remainder;

      if(reflectRemainder)
        crc = reflect(remainder);

      return (crc ^ final);
    }

    void reset()
    {
      remainder = initial;
    }

  private:

    void init()
    {
      T tmp;

      for(int i = 0; i < 256; ++i)
      {
        tmp = i << ((sizeof(T) - 1) * 8);

        for(int j = 8; j > 0; --j)
        {
          if(tmp & (1 << ((sizeof(T) * 8) - 1)))
            tmp = (tmp << 1) ^ polynomial;
          else
            tmp <<= 1;
        }

        table[i] = tmp;
      }
    }

    template<typename V>
    V reflect(V val)
    {
      V res = 0;
      int nBits = sizeof(V) * 8;

      for(int i = 0; i < nBits; ++i)
      {
        if(val & 0x1)
          res |= 1 << (nBits - 1 - i);

        val >>= 1;
      }

      return res;
    }

    T polynomial;
    T remainder;
    T initial;
    T final;
    bool reflectData;
    bool reflectRemainder;
    T table[256];
  };

}

#endif /* OVERKIZ_CRC_H_ */
