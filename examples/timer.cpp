#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Poller.h>
#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Timer.h>

//Timer implementation using poller and timerfd
class Timer : public Overkiz::Timer::Monotonic
{
public:

  Timer()
  {
  }

  virtual ~Timer()
  {
  }

  void expired(const Overkiz::Time::Monotonic& time)
  {
    printf("Timer expired.\n");
    exit(EXIT_SUCCESS);
  }

};

int main(int argc, char *argv[])
{
  //Use OVK_LOG_LVL= env var to see this log trace
  OVK_INFO("Starting useless application.");
  //Get main loop
  Overkiz::Shared::Pointer< Overkiz::Poller > & poller = Overkiz::Poller::get(false); //Using coroutine or not
  Timer timer;
  timer.setTime(Overkiz::Time::Elapsed{2,0}, true);
  timer.start();
  poller->loop();
}

