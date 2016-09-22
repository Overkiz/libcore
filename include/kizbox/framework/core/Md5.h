/*
 * Md5.h
 *
 *      Copyright (C) 2016 Overkiz SA.
 */

#ifndef OVERKIZ_MD5_H_
#define OVERKIZ_MD5_H_

#include <stdio.h>
#include <stdint.h>

#define MD5_OUPUT_SIZE 16


namespace Overkiz
{
  class Md5
  {
  public:

    typedef struct
    {
      uint32_t total[2];          /*!< number of bytes processed  */
      uint32_t state[4];          /*!< intermediate digest state  */
      unsigned char buffer[64];   /*!< data block being processed */
    } md5_context;

    Md5();
    ~Md5();
    void update(const unsigned char *input, size_t ilen);
    void finish(unsigned char output[16]);
    void md5(const unsigned char *input, size_t ilen, unsigned char output[MD5_OUPUT_SIZE]);

  private:
    md5_context ctx;
  };
}

#endif /* OVERKIZ_MD5_H_ */
