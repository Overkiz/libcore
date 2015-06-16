/*
 * Event.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_EVENT_H_
#define OVERKIZ_EVENT_H_

#include <map>

#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Thread.h>

namespace Overkiz
{

  /**
   * Class used to manage events.
   */
  class Event: public Task
  {
  public:

    /**
     * Constructor.
     *
     * @return
     */
    Event();

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~Event();

    /**
     * Send this event.
     */
    void send();

    /**
     * Receive event method.
     * Must be implemented by the inherited class.
     *
     * @param numberOfEvents
     */
    virtual void receive(uint64_t numberOfEvents) = 0;

  private:

    void entry();

    void save();

    void restore();

    uint64_t count;

    /**
     * Class managing all the declared events of a thread.
     */
    class Manager: public Watcher
    {
    private:

      Manager();

      virtual ~Manager();

      static Shared::Pointer<Manager>& get();

      void send(Event *evt);

      void writeEvent(Event *evt = nullptr);

      void remove(Event *evt);

      void process(uint32_t evts);

      static Thread::Key<Manager> manager;

      std::map<Event*, uint64_t> wevents;

      friend class Event;
      template<typename T> friend class Shared::Pointer;
    };

  };

}

#endif /* OVERKIZ_EVENT_H_ */
