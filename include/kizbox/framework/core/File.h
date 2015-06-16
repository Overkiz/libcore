/*
 * File.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_FILE_H_
#define OVERKIZ_FILE_H_

#include <string>

#include <kizbox/framework/core/Stream.h>

namespace Overkiz
{

  /*!
   * \brief File implementation using the Stream interface
   *
   * File namespace defines a common interface named Base Stream that is derived into two final clases Input and Output.
   *
   */
  namespace File
  {

    /*!
     * \class Base File.h "kizbox/framework/core/File.h"
     * \brief Class implementing a common base object File Stream
     *
     * \note Base is a common class thus it should not be instantiated.
     * \note Use final classes such as Input or Output instead.
     *
     */
    class Base: virtual public Overkiz::Stream::Base
    {
    public:

      /*!
       * \brief Constructor with path and fd
       *
       * If the file descriptor is given and different from 0, the stream is already opened.
       *
       * \note File Stream is synchronous: the delegate is never called.
       */
      Base();

      /*!
       * \brief Empty virtual destructor
       *
       * Does nothing.
       *
       */
      virtual ~Base();

      /*!
       * \brief Open the stream
       *
       * \note File Stream is synchronous: the delegate is never called.
       *
       * \sa close
       */
      virtual void open();

      /*!
       * \brief Close the stream
       *
       * \note File Stream is synchronous: the delegate is never called.
       *
       * \sa open
       */
      virtual void close();

      /*!
       * \brief Get the stream status
       *
       * \return The stream status
       *
       * \sa wait getWaitStatus
       */
      virtual int getStatus() const;

      /*!
       * \brief Get the stream wait status
       *
       * \return The wait stream status (= status)
       *
       * \note File Stream is synchronous: the wait status is the same as status
       *
       */
      virtual int getWaitStatus() const;

      /*!
       * \brief Set the delegate
       *
       * \note File Stream is synchronous: the delegate is never called.
       *
       * \param [in] delegate The stream delegate (never used)
       */
      void setDelegate(Overkiz::Stream::Delegate * delegate);

      /*!
       * \brief Wait the stream for status
       *
       * \note File Stream is synchronous: the delegate is never called.
       *
       * \param [in] status The wait status
       *
       * \return The current stream status
       *
       * \sa getWaitStatus getStatus
       */
      virtual int wait(int status);

      /*!
       * \brief Get the file path
       *
       * \return Return the file path
       *
       * \sa setPath
       */
      const std::string & getPath() const;

      /*!
       * \brief Set the file path
       *
       * \param [in] path The file path
       *
       * \sa getPath
       */
      void setPath(const std::string & path);

      /*!
       * \brief Set the flags
       *
       * \param [in] enable Enable flags
       * \param [in] flag The mask of flags
       *
       * \sa setFlag
       */
      void setFlag(bool enable, int flags);

      /*!
       * \brief Set the cursor on a position.
       *
       * \param [in] new position of cursor.
       *
       * \sa seek
       */
      off_t seek(long offset);

    protected:

      int status; //!< Status
      Overkiz::Stream::Delegate * delegate; //!< Delegate
      std::string path; //!< Path
      int fd; //!< File descriptor
      int flags; //!< Flags

    };

    /*!
     * \class Input File.h kizbox/framework/core/File.h
     * \brief Class implementing an Input File
     *
     */
    class Input: virtual public Overkiz::File::Base,
      virtual public Overkiz::Stream::Input
    {
    public:

      /*!
       * \brief Constructor with path and fd
       *
       * If the file descriptor is given and different from 0, the stream is already opened.
       *
       * \note File Stream is synchronous: the delegate is never called.
       */
      Input();

      /*!
       * \brief Constructor with path and fd
       *
       * If the file descriptor is given and different from 0, the stream is already opened.
       *
       * \note File Stream is synchronous: the delegate is never called.
       *
       * \param [in] path The path to the file
       * \param [in] fd The file descriptor (if opened)
       */
      Input(const std::string & path, int fd = -1);

      /*!
       * \brief Empty virtual destructor
       *
       */
      virtual ~Input();

      /*!
       * \copydoc Overkiz::File::Base::open
       *
       */
      void open();

      /*!
       * \copydoc Overkiz::Stream::Input::read
       *
       * Reads and consumes count bytes from the stream into buffer
       *
       * \param [out] buffer The buffer where to copy read bytes
       * \param [in] count The number of byte to copy into the buffer
       *
       * \return Return The number of bytes read
       */
      size_t read(void * buffer, size_t count);

    };

    /*!
     * \class Output File.h kizbox/framework/core/File.h
     * \brief Class implementing an Output File
     *
     */
    class Output: virtual public Overkiz::File::Base,
      virtual public Overkiz::Stream::Output
    {
    public:

      /*!
       * \brief Constructor with path and fd
       *
       * If the file descriptor is given and different from 0, the stream is already opened.
       *
       * \note File Stream is synchronous: the delegate is never called.
       *
       */
      Output();

      /*!
       * \brief Constructor with path and fd
       *
       * If the file descriptor is given and different from 0, the stream is already opened.
       *
       * \note File Stream is synchronous: the delegate is never called.
       *
       * \param [in] path The path to the file
       * \param [in] fd The file descriptor (if opened)
       */
      Output(const std::string & path, int fd = -1);

      /*!
       * \brief Empty virtual destructor
       *
       */
      virtual ~Output();

      /*!
       * \copydoc Overkiz::File::Base::open
       *
       */
      void open();

      /*!
       * \copydoc Overkiz::Stream::Input::read
       *
       * Write count bytes from buffer into the stream
       *
       * \param [in] buffer The buffer where to copy read bytes
       * \param [in] count The number of bytes to copy into the stream
       * \param [in] more More data has to been written into the stream
       *
       * \return Return the number of bytes wrote
       */
      size_t write(const void * buffer, size_t count, bool more = false);

    };

  }

}

#endif /* OVERKIZ_FILE_H_ */
