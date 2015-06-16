/*
 * UniversalUniqueIdentifier.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_UNIVERSALUNIQUEIDENTIFIER_H_
#define OVERKIZ_UNIVERSALUNIQUEIDENTIFIER_H_

#include <string>
#include <stdint.h>
#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  class UniversalUniqueIdentifier
  {
  public:

    typedef enum
    {
      DEFAULT_CASE = 0x0, //!< The UUID string is in the system default case (lower)
      LOWER_CASE = 0x1, //!< The UUID string is in lower case
      UPPER_CASE, //!< The UUID string is in upper case
    } Case;

    class MalformedException: public Overkiz::Exception
    {
    public:

      MalformedException()
      {
      }

      virtual ~MalformedException()
      {
      }

      const char * getId() const
      {
        return "com.overkiz.Framework.Core.UUID.MalformedException";
      }
    };

    UniversalUniqueIdentifier();

    UniversalUniqueIdentifier(const char * uuid);

    UniversalUniqueIdentifier(const std::string & uuid);

    UniversalUniqueIdentifier(const Overkiz::UniversalUniqueIdentifier & src);

    virtual ~UniversalUniqueIdentifier();

    void generate();

    void clear();

    void copy(const Overkiz::UniversalUniqueIdentifier & src);

    Overkiz::UniversalUniqueIdentifier & operator = (
      const Overkiz::UniversalUniqueIdentifier & src)
    {
      copy(src);
      return *this;
    }

    int compare(const Overkiz::UniversalUniqueIdentifier & uuid) const;

    int compare(const std::string & uuid) const;

    bool operator == (const Overkiz::UniversalUniqueIdentifier & uuid) const
    {
      return (compare(uuid) == 0);
    }

    bool operator == (const std::string & uuid) const
    {
      return (compare(uuid) == 0);
    }

    bool operator != (const Overkiz::UniversalUniqueIdentifier & uuid) const
    {
      return (compare(uuid) != 0);
    }

    bool operator != (const std::string & uuid) const
    {
      return (compare(uuid) != 0);
    }

    bool isNil() const;

    void build(const char * uuid)
    throw(Overkiz::UniversalUniqueIdentifier::MalformedException);

    void build(const std::string & uuid)
    throw(Overkiz::UniversalUniqueIdentifier::MalformedException);

    void get(char * cstring,
             Overkiz::UniversalUniqueIdentifier::Case stringCase =
               Overkiz::UniversalUniqueIdentifier::DEFAULT_CASE) const;

    void get(std::string & string,
             Overkiz::UniversalUniqueIdentifier::Case stringCase =
               Overkiz::UniversalUniqueIdentifier::DEFAULT_CASE) const;

    std::string getString(Overkiz::UniversalUniqueIdentifier::Case stringCase =
                            Overkiz::UniversalUniqueIdentifier::DEFAULT_CASE) const;

    static Overkiz::UniversalUniqueIdentifier get();

    static const Overkiz::UniversalUniqueIdentifier NIL; //!< Constant object for nil uuid @em "00000000-0000-0000-0000-000000000000"
    static const std::string NIL_STRING; //!< Constant string for nil uuid @em "00000000-0000-0000-0000-000000000000"

  private:

    uint8_t identifier[16];

  };

}

#endif /* OVERKIZ_UNIVERSALUNIQUEIDENTIFIER_H_ */
