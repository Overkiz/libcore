/*
 * Directory.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <cstring>

#include <kizbox/framework/core/Node.h>

namespace Overkiz
{

  namespace FileSystem
  {

    Node::Node()
    {
      this->name = "";
      this->mode = 0;
    }

    Node::Node(const std::string & path)
    {
      std::string pathTemp = path;
      this->mode = 0;

      if(!pathTemp.empty())
      {
        if(pathTemp.find_last_of('/') == (pathTemp.size() - 1))
        {
          pathTemp.erase((pathTemp.size() - 1));
        }

        this->path = pathTemp.substr(0, pathTemp.find_last_of('/'));
        this->path.append("/");
        this->name = pathTemp.substr((pathTemp.find_last_of('/') + 1),
                                     (pathTemp.size() - pathTemp.find_last_of('/')));
        *this = Node(this->path, this->name);
      }
    }

    Node::Node(const std::string & path, const std::string & name)
    {
      struct stat fileInfo;
      this->name = name;
      this->path = path;
      this->mode = 0;

      if((!this->name.empty()) && (!this->path.empty()))
      {
        std::string fullName = this->path + "/" + this->name;
        errno = 0;

        if(lstat(fullName.c_str(), &fileInfo) != 0)
        {
          this->name = "";
          this->path = "";
          this->mode = 0;
        }
        else
        {
          this->mode = fileInfo.st_mode;
        }
      }
    }

    Node::Node(const Node & node)
    {
      this->name = node.getName();
      this->path = node.getPath();
      this->mode = node.getMode();
    }

    Node & Node::operator = (const Node & node)
    {
      this->name = node.getName();
      this->path = node.getPath();
      this->mode = node.getMode();
      return (*this);
    }

    Node::~Node()
    {
    }

    bool Node::isExisting()
    {
      return !name.empty() && !path.empty();
    }

    const std::string & Node::getName() const
    {
      return (name);
    }

    const std::string & Node::getPath() const
    {
      return (path);
    }

    std::string & Node::getName()
    {
      return (name);
    }

    std::string & Node::getPath()
    {
      return (path);
    }

    const mode_t & Node::getMode() const
    {
      return (mode);
    }

    mode_t & Node::getMode()
    {
      return (mode);
    }

  }

}
