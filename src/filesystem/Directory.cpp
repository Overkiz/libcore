/*
 * Directory.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include "Directory.h"

namespace Overkiz
{

  namespace FileSystem
  {

    Directory::Iterator::Iterator(const std::string & path) :
      path(path)
    {
      this->directory = NULL;
    }

    Directory::Iterator::~Iterator()
    {
      if(this->directory)
      {
        closedir(this->directory);
      }

      this->directory = NULL;
    }

    bool Directory::Iterator::operator != (const Iterator & b)
    {
      bool returnValue = false;

      if(this->directory)
      {
        returnValue = true;
      }

      return (returnValue);
    }

    Directory::Iterator & Directory::Iterator::operator ++()
    {
      bool test = false;
      struct dirent * directoryInfo;

      if(this->directory)
      {
        directoryInfo = readdir(this->directory);

        if(directoryInfo)
        {
          test = true;
          this->node = Node(this->path + "/" + directoryInfo->d_name);
        }
      }

      if(!test)
      {
        closedir(this->directory);
        this->directory = NULL;
      }

      return (*this);
    }

    Directory::Iterator & Directory::Iterator::operator --()
    {
      bool test = false;
      struct dirent * directoryInfo;

      if(this->directory)
      {
        off_t pos = telldir(this->directory);

        if(pos >= 0)
        {
          seekdir(this->directory, (pos - 1));
          directoryInfo = readdir(this->directory);

          if(directoryInfo)
          {
            test = true;
            this->node = Node(this->path + "/" + directoryInfo->d_name);
          }
        }
      }

      if(!test)
      {
        closedir(this->directory);
        this->directory = NULL;
      }

      return (*this);
    }

    Node & Directory::Iterator::operator *()
    {
      return (this->node);
    }

    Node * Directory::Iterator::operator ->()
    {
      return (&this->node);
    }

    Directory::Directory() :
      Node()
    {
    }

    Directory::Directory(const std::string & startPath) :
      Node(startPath)
    {
    }

    Directory::Directory(const Node & node) :
      Node(node)
    {
    }

    Directory::~Directory()
    {
    }

    Directory::Iterator Directory::begin()
    {
      Iterator it(this->path + this->name + "/");

      if(this->isExisting())
      {
        struct dirent * directoryInfo;

        if((!this->name.empty()) && (!this->path.empty())
           && S_ISDIR(this->mode))
        {
          it.directory = opendir((this->path + this->name).c_str());

          if(it.directory)
          {
            directoryInfo = readdir(it.directory);

            if(directoryInfo)
            {
              it.node = Node((this->path + this->name + "/"),
                             directoryInfo->d_name);
            }
          }
          else
          {
            closedir(it.directory);
            it.directory = NULL;
          }
        }
      }
      else
      {
        throw DirectoryException();
      }

      return (it);
    }

    Directory::Iterator Directory::end()
    {
      Iterator it(this->path + this->name + "/");

      if(this->isExisting())
      {
        if(it.directory)
        {
          closedir(it.directory);
        }

        it.directory = NULL;
      }
      else
      {
        throw DirectoryException();
      }

      return (it);
    }

  }

}
