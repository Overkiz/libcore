/*
 * Exception.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_EXCEPTION_H_
#define OVERKIZ_EXCEPTION_H_

namespace Overkiz
{
  /**
   * Exception Interface.
   * An implementation of this interface must be used when throwing Overkiz Execption.
   *
   * The identication on the exception is based on the exception Id
   * return by the getId function.
   *
   * This Id is a const char *.
   */
  class Exception
  {
  public:

    /**
     * Empty virtual destructor
     *
     */
    virtual ~Exception()
    {
    }

    /**
     * Fuction which returns the exception Id.
     *
     * @return the execption Id
     */
    virtual const char *getId() const = 0;

  };

}

#endif /* OVERKIZ_EXCEPTION_H_ */
