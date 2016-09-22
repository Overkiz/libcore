/*
 * UpdateFile.cpp
 *
 *      Copyright (C) 2016 Overkiz SA.
 */

#include "config.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Errno.h>
#include <kizbox/framework/core/Md5.h>

#include "UpdateFile.h"


namespace Overkiz
{

  namespace FileSystem
  {

#define FAST_COMPARE_MAX_SIZE (2*1024)
#define COMPARE_MAX_SIZE (10*1024)


    int UpdateFile::md5(const char* file,unsigned char output[16])
    {
      int fd = ::open(file, O_RDONLY);

      if(fd < 0)
      {
        return -1;
      }

      Overkiz::Md5 md5;
      char buffer[1024];
      ssize_t r;

      while((r=::read(fd,buffer,sizeof(buffer)))  > 0)
      {
        md5.update((const unsigned char*)buffer, r);
      }

      ::close(fd);
      md5.finish(output);
      return 0;
    }

    bool UpdateFile::compare(const char* file1, const char* file2)
    {
      struct stat sb1;
      struct stat sb2;
      bool ret = false;

      if(stat(file1, &sb1) < 0)
      {
        return ret;
      }

      if(stat(file2, &sb2) < 0)
      {
        return ret;
      }

      if(sb1.st_size!= sb2.st_size)
      {
        return ret;
      }

      //fast compare
      if(sb1.st_size < FAST_COMPARE_MAX_SIZE)
      {
        int fd1 = ::open(file1, O_RDONLY);
        int fd2 = ::open(file2, O_RDONLY);

        if(fd1 < 0 || fd2 < 0)
        {
          if(fd1) ::close(fd1);

          if(fd2) ::close(fd2);

          OVK_DEBUG("%s > open error : (%s)", __FUNCTION__, strerror(errno));
          return ret;
        }

        char *buffer1 = new char[sb1.st_size];
        char *buffer2 = new char[sb2.st_size];
        ssize_t r1 = read(fd1, buffer1, sb1.st_size);
        ssize_t r2 = read(fd2, buffer2, sb2.st_size);
        ::close(fd1);
        ::close(fd2);

        if((r1 == r2) && (memcmp(buffer1,buffer2,sb1.st_size) == 0))
        {
          ret=true;
        }

        delete [] buffer1;
        delete [] buffer2;
      }
      else
      {
        uint8_t md5_1[MD5_OUPUT_SIZE];
        uint8_t md5_2[MD5_OUPUT_SIZE];

        if(md5(file1,md5_1) <0)
        {
          return ret;
        }

        if(md5(file2,md5_2) <0)
        {
          return ret;
        }

        if(memcmp(md5_1,md5_2, sizeof(md5_1)) == 0)
        {
          ret = true;
        }
      }

      return ret;
    }

    int UpdateFile::write(const char* path, const char* data, const size_t size)
    {
      int fd = ::open(path, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

      if(fd < 0)
      {
        OVK_ERROR("%s > open error : (%s)", __FUNCTION__, strerror(errno));
        return -1;
      }

      ssize_t w = ::write(fd, data, size);

      if(w < 0 || w != (int)size)
      {
        OVK_ERROR("%s > write error : (%s)", __FUNCTION__, strerror(errno));
        ::close(fd);
        return -1;
      }

      if(::close(fd) != 0)
      {
        return -1;
      }

      return w;
    }

    int UpdateFile::update(const char* file, const char* data, const size_t size)
    {
      int ret=-1;

      if(file == NULL)
      {
        return ret;
      }

      struct stat sb;

      if((stat(file, &sb) < 0) || (sb.st_size != (int)size) || (sb.st_size > COMPARE_MAX_SIZE))
      {
        // update or new file;
        ret= write(file,data,size);

        if(ret>0)
        {
          ::sync();
        }
      }
      else
      {
        char name[] = "/tmp/update-file-XXXXXX";
        int fd =mkstemp(name);

        if(fd < 0)
        {
          OVK_ERROR("%s > can't create temporary file  : (%s)", __FUNCTION__, strerror(errno));
          return ret;
        }

        ::close(fd);
        write(name,data,size);

        if(compare(name,file) == false)
        {
          // update file
          ret= write(file,data,size);

          if(ret>0)
          {
            ::sync();
          }
        }
        else
        {
          // nothing to do
          ret = 0;
        }

        ::remove(name);
      }

      return ret;
    }

  }

}
