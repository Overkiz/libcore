#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Poller.h>
#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Signal.h>

//Catching signal using poller and signalfd
class SignalHandler : protected Overkiz::Signal::Handler
{
public:

  SignalHandler()
  {
    Overkiz::Signal::Manager::add(SIGINT, this);
    Overkiz::Signal::Manager::add(SIGTERM, this);
    Overkiz::Signal::Manager::add(SIGABRT, this);
  }

  virtual ~SignalHandler()
  {
  }

  void handle(const Overkiz::Signal& signal)
  {
    printf("Signal %d caught.\n", signal.info.ssi_signo);

    if(signal.info.ssi_signo == SIGINT)
    {
      OVK_NOTICE("Signal caught: %d", signal.info.ssi_signo);
    }
    else
    {
      OVK_CRITICAL("Signal caught:%d", signal.info.ssi_signo);
    }

    exit(signal.info.ssi_signo);
  }
};

int main(int argc, char *argv[])
{
  //Use OVK_LOG_LVL= env var to see this log trace
  OVK_INFO("Starting useless application.");
  //Get main loop
  Overkiz::Shared::Pointer< Overkiz::Poller > & poller = Overkiz::Poller::get(false); //Using coroutine or not
  SignalHandler sig;
  poller->loop();
}

