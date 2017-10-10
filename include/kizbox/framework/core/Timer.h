/*
 * Timer.h
 *
 * Class defining a Real Timer : a timer based on the real time clock.
 * This type of timer is sensible to system time changes.
 *
 * Class defining a Monotonic Timer : a timer based on the monotonic time clock.
 * This type of timer is sensible to system time changes.
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_TIMER_H_
#define OVERKIZ_TIMER_H_

#include <list>
#include <sys/timerfd.h>
#include <time.h>

#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Thread.h>
#include <kizbox/framework/core/Time.h>
#include <kizbox/framework/core/Event.h>

namespace Overkiz
{
  namespace Timer
  {

    /**
     * Timer type.
     * ABSOLUTE mean the timer will expire at the given time.
     * RELATIVE mean the timer will expire in the given time.
     */
    typedef enum
    {
      TYPE_ABSOLUTE = 0, //!< TYPE_ABSOLUTE
      TYPE_RELATIVE = 1, //!< TYPE_RELATIVE
    } Type;

    class Real: public Task
    {
    public:

      class Manager: public Watcher
      {

      public:

      private:

        Manager();

        virtual ~Manager();

        void start(Real *timer);

        void stop(Real *timer);

        static Shared::Pointer<Manager>& get();

        void process(uint32_t evts);

        static Thread::Key<Manager> manager;

        void reschedule();

        void check();

        std::list<Real *> timers;
        bool reliable;

        friend class Real;
        template<typename T> friend class Shared::Pointer;
      };

      Real(bool relative = false);

      Real(const Time::Real& time, bool relative = false);

      virtual ~Real();

      void setTime(const Time::Real& time, bool relative = false);

      const Time::Real& getTime(bool& relative) const;

      const Time::Real& getTime() const;

      void start();

      void stop();

      bool isRunning();

      virtual void enable();

      virtual void disable();

      void pause();

      void resume();

    protected:

      virtual void expired(const Time::Real& time) = 0;

      virtual void reschedule(const Time::Real& time);

      virtual void save();

      virtual void restore();

    private:

      void entry();

      uint8_t relative : 1;
      uint8_t cancelled : 1;
      Time::Real time;
      Time::Real offset;
      Shared::Pointer<Manager> manager;
    };

    class Monotonic: public Task
    {
    public:
      class Manager: public Watcher
      {
      public:

      private:

        Manager();

        virtual ~Manager();

        void start(Monotonic *timer);

        void stop(Monotonic *timer);

        static Shared::Pointer<Manager>& get();

        void process(uint32_t evts);

        static Thread::Key<Manager> manager;

        void reschedule();

        std::list<Monotonic *> timers;

        friend class Monotonic;
        template<typename T> friend class Shared::Pointer;
      };

      Monotonic(bool relative = false);

      Monotonic(const Time::Monotonic& time, bool relative = false);

      virtual ~Monotonic();

      void setTime(const Time::Monotonic& time, bool relative = false);

      const Time::Monotonic& getTime(bool& relative) const;

      const Time::Monotonic& getTime() const;

      void start();

      void stop();

      virtual void enable();

      virtual void disable();

      void pause();

      void resume();

      bool isRunning();

    protected:

      virtual void expired(const Time::Monotonic& time) = 0;

      virtual void save();

      virtual void restore();

    private:

      void entry();

      uint8_t relative : 1;
      Time::Monotonic time;
      Time::Monotonic offset;
      Shared::Pointer<Manager> manager;
    };

    namespace Concrete
    {
      class Monotonic: public Overkiz::Timer::Monotonic
      {
      public:

        class Delegate
        {
        public:

          virtual ~Delegate();

          virtual void expired(Timer::Monotonic *timer,
                               const Time::Monotonic& time) = 0;
        };

        Monotonic(bool relative = false);

        Monotonic(const Time::Monotonic& time, bool relative = false);

        virtual ~Monotonic();

        void setDelegate(Delegate * delegate);

        void clearDelegate();

      private:

        void expired(const Time::Monotonic& time);

        Delegate * delegate;
      };

      class Real: public Overkiz::Timer::Real
      {
      public:

        class Delegate
        {
        public:

          virtual ~Delegate();

          virtual void expired(Timer::Real *timer, const Time::Real& time) = 0;

          virtual void reschedule(Timer::Real *timer, const Time::Real& time);
        };

        Real(bool relative = false);

        Real(const Time::Real& time, bool relative = false);

        virtual ~Real();

        void setDelegate(Delegate * delegate);

        void clearDelegate();

      private:

        void expired(const Time::Real& time);

        void reschedule(const Time::Real& time);

        Delegate * delegate;
      };

    }

  }

}

#endif /* OVERKIZ_TIMER_H_ */
