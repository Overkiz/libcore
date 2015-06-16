/*
 * SymLink.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_FILESYSTEM_SYMLINK_H_
#define OVERKIZ_FILESYSTEM_SYMLINK_H_

#include <unistd.h>

#include <kizbox/framework/core/Node.h>

namespace Overkiz
{

  namespace FileSystem
  {

    class SymLink: public Node
    {
    public:
      SymLink() :
        Node()
      {
      }

      SymLink(const Node & node) :
        Node(node)
      {
        *this = SymLink(this->getPath() + this->getName());
      }

      SymLink(const std::string & pathName) :
        Node(pathName)
      {
        char buffer[1024] = { '\0' };

        if(S_ISLNK(this->mode))
        {
          if(readlink((this->path + this->name).c_str(), buffer, 1024) > 0)
          {
            this->realPath.append(buffer);
          }
        }
      }

      Node follow()
      {
        if(this->realPath.find("/") == 0)
        {
          return (Node(this->realPath));
        }
        else
        {
          return (Node(this->path + this->realPath));
        }
      }

      virtual ~SymLink()
      {
      }

    private:

      std::string realPath;
    };

  }

}

#endif /* OVERKIZ_FILESYSTEM_SYMLINK_H_ */
