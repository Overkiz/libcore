/*
 * Daemon.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_MISC_DAEMON_H_
#define OVERKIZ_MISC_DAEMON_H_

#include <string>
#include <set>

#include <kizbox/framework/core/Node.h>

namespace Overkiz
{

  class Daemon
  {

  public:

    class Listener
    {
    public:
      Listener()
      {
      }
      virtual ~Listener()
      {
      }

      virtual void willFork() = 0;

      virtual void forked() = 0;
    };


    Daemon();

    virtual ~Daemon();

    void run(bool fork);

    void checkPidFile();

  protected:

    std::set<Listener *> eventListeners;

  private:

    void removePidFile();

    void updatePidFile();

    void makePidFile(int pid);

    int lock;
    std::string pidfile;

  };

}
#endif /* OVERKIZ_MISC_DAEMON_H_ */
