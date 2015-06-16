/*
 * Base64.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_BASE64_H_
#define OVERKIZ_BASE64_H_

#include <string>
#include <vector>

#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  class Base64
  {
  public:

    class BadEndOfString: public Overkiz::Exception
    {
    public:
      BadEndOfString()
      {
      }

      virtual ~BadEndOfString()
      {
      }

      virtual const char *getId() const
      {
        return "com.overkiz.framework.core.Base64.BadEndOfString";
      }
    };

    class BadReformating: public Overkiz::Exception
    {
    public:
      BadReformating()
      {
      }

      virtual ~BadReformating()
      {
      }

      virtual const char *getId() const
      {
        return "com.overkiz.framework.core.Base64.BadReformating";
      }
    };

    class OutOfBound: public Overkiz::Exception
    {
    public:
      OutOfBound()
      {
      }

      virtual ~OutOfBound()
      {
      }

      virtual const char *getId() const
      {
        return "com.overkiz.framework.core.Base64.OutOfBound";
      }
    };

    static void encode(const uint8_t * src, int32_t size, std::string& dest);

    static void encode(const std::vector<uint8_t>& src, std::string& dest);

    static int32_t decode(const std::string& src, uint8_t * dest, int32_t size);

    static void decode(const std::string& src, std::vector<uint8_t>& dest);

  private:
    static char encodeTable[];
    static char decodeTable[];
  };
}

#endif /* BASE64_H_ */
