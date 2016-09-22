/*
 * UpdateFile.h
 *
 *      Copyright (C) 2016 Overkiz SA.
 */

#ifndef OVERKIZ_UPDATE_FILE_H_
#define OVERKIZ_UPDATE_FILE_H_


namespace Overkiz
{

  namespace FileSystem
  {

    class UpdateFile
    {
    public:
      static int update(const char* file, const char* data, const size_t size);

      static int md5(const char* file,unsigned char output[16]);
      static bool compare(const char* file1, const char* file2);
      static int write(const char* path, const char* data, const size_t size);
    };

  }

}

#endif /* OVERKIZ_UPDATE_FILE_H_ */
