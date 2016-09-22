/*
 * Daemon.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_MISC_DAEMON_H_
#define OVERKIZ_MISC_DAEMON_H_

#include <string>

#include <kizbox/framework/core/Node.h>

namespace Overkiz
{

  class Daemon
  {

  public:

    Daemon();

    virtual ~Daemon();

    void run(bool fork);

    void checkPidFile();

  private:

    void removePidFile();

    void updatePidFile();

    void makePidFile(int pid);

    int lock;
    std::string pidfile;


  };

}
#endif /* OVERKIZ_MISC_DAEMON_H_ */
