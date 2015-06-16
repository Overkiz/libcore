/*
 * Errno.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_ERRNO_H_
#define OVERKIZ_ERRNO_H_

#include <errno.h>

#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  namespace Errno
  {

    /*!
     * \interface Base Errno.h kizbox/framework/core/Errno.h
     * \brief Exception implementing the libc errno error
     *
     */
    class Exception: public Overkiz::Exception
    {
    public:

      /*!
       * \brief Constructor with errno error code
       *
       * \param [in] errno The errno error code
       */
      Exception(int error = errno);

      /*!
       * \brief Virtual destructor
       *
       */
      virtual ~Exception();

      /*!
       * \brief Get the errno error code
       *
       * \return Return the errno error code
       */
      int getErrno() const;

      /*!
       * \copydoc Overkiz::Exception::getId
       *
       */
      virtual const char * getId() const;

      /*!
       * \brief Get the errno string error code
       *
       * \return Return the errno string error code
       */
      static const char * getString(const int error = errno);

    protected:

      const int code; //!< The errno error code
    };

  }

}

#endif /* OVERKIZ_ERRNO_H_ */
