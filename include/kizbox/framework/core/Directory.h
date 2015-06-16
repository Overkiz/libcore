/*
 * Directory.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_FILESYSTEM_DIRECTORY_H_
#define OVERKIZ_FILESYSTEM_DIRECTORY_H_

#include <string>

#include <kizbox/framework/core/Node.h>

namespace Overkiz
{

  namespace FileSystem
  {

    class DirectoryException: public Overkiz::Exception
    {
    public:

      DirectoryException()
      {
      }
      ;
      virtual ~DirectoryException()
      {
      }
      ;

      const char *getId() const
      {
        return ("com.overkiz.Kizbox.Framework.Core.FileSystem.Directory.Error.NotExisting");
      }
    };

    class Directory: public Node
    {
    public:

      class Iterator
      {
      public:

        Iterator(const std::string & path = NULL);
        virtual ~Iterator();

        bool operator != (const Iterator & b);
        Iterator & operator ++();
        Iterator & operator --();

        Node & operator *();
        Node * operator ->();

      private:

        Node node;
        DIR * directory;
        const std::string path;

        friend class Directory;
      };

      Directory();
      Directory(const Node & node);

      Directory(const std::string & startPath);
      virtual ~Directory();

      Iterator begin();
      Iterator end();

    private:
      friend class Iterator;
    };

  }

}
#endif /* OVERKIZ_FILESYSTEM_DIRECTORY_H_ */
