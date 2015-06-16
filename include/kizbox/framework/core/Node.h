/*
 * Node.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKZ_FILESYSTEM_NODE_H_
#define OVERKZ_FILESYSTEM_NODE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <errno.h>

#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace FileSystem
  {

    class Node
    {
    public:
      Node();

      Node(const Node & node);

      Node(const std::string & name);

      Node(const std::string & path, const std::string & name);

      virtual ~Node();

      Node & operator = (const Node & b);

      bool isExisting();

      const std::string & getName() const;

      const std::string & getPath() const;

      const mode_t & getMode() const;

      std::string & getName();

      std::string & getPath();

      mode_t & getMode();

    protected:
      std::string path;
      std::string name;
      mode_t mode;
    };

  }

}

#endif /* OVERKZ_FILESYSTEM_NODE_H_ */
