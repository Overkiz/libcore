/*
 * Coroutine.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_COROUTINE_H_
#define OVERKIZ_COROUTINE_H_

#include <stddef.h>

#include <kizbox/framework/core/Thread.h>
#include <kizbox/framework/core/Context.h>
#include <kizbox/framework/core/Shared.h>

namespace Overkiz
{

  /**
   * A coroutine is a light thread which is launched, paused and resumed by users.
   * There is no kernel or user space scheduler.
   */
  class Coroutine: public Architecture::Context::Launcher
  {
  public:

    /**
     * Status of the coroutine
     */
    typedef enum
    {
      STOPPED, //!< STOPPED
      RUNNING, //!< RUNNING
      PAUSED, //!< PAUSED
      WAITING, //!< WAITING
    } Status;

    /**
     * Resume a given coroutine.
     *
     * @param coro : the coroutine to resume.
     */
    static void resume(Shared::Pointer<Coroutine>& coro);

    /**
     * Resume a given coroutine.
     *
     * @param coro : the coroutine to resume.
     */
    template<typename T>
    static void resume(Shared::Pointer<T>& coro)
    {
      Shared::Pointer<Coroutine> coroutine = coro;
      resume(coroutine);
    }

    /**
     * Pause the running coroutine.
     */
    static void yield();

    /**
     * Entry point is called when the coroutine is launched.
     */
    virtual void entry()
    {
    }

    /**
     * Save point is called before pausing the coroutine.
     */
    virtual void save()
    {
    }

    /**
     * Restore point is called before resuming a coroutine.
     */
    virtual void restore()
    {
    }

    /**
     * Get the coroutine status.
     *
     * @return the coroutine status.
     */
    Status status() const;

    /**
     * Get the coroutine stack size.
     *
     * @return the coroutine stack size.
     */
    size_t stackSize();

    /**
     * Get the running coroutine.
     *
     * @return a shared pointer to the current coroutine.
     */
    static Shared::Pointer<Coroutine>& self();

  protected:

    /**
     * Constructor.
     *
     * @param stackSize : size of the coroutine stack.
     * @return a new coroutine.
     */
    Coroutine(size_t stackSize);

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~Coroutine();

    /**
     * The coroutine caller.
     * Can be another coroutine or the main thread.
     */
    Shared::Pointer<Coroutine> caller;

  private:

    Coroutine();

    void launch();

    struct
    {
      void *base;
      void *top;
    } stack;

    size_t size;
    Status state;
    #ifdef VALGRIND
    int valgrind;
    #endif
    static Thread::Key<Coroutine> current;

    template<typename T> friend class Overkiz::Shared::Pointer;
  };

}

#endif /* OVERKIZ_COROUTINE_H_ */
