#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Poller.h>
#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Event.h>

//Timer implementation using poller and timerfd
class Event : public Overkiz::Event
{
public:

  Event()
  {
  }

  virtual ~Event()
  {
  }

  void receive(uint64_t numberOfEvents)
  {
    printf("Event received. %lu\n", numberOfEvents);

    if(numberOfEvents < 2)
    {
      send();
      send();
    }
  }

};

int main(int argc, char *argv[])
{
  //Use OVK_LOG_LVL= env var to see this log trace
  OVK_INFO("Starting useless application.");
  //Get main loop
  Overkiz::Shared::Pointer< Overkiz::Poller > & poller = Overkiz::Poller::get(false); //Using coroutine or not
  Event evt;
  evt.send();
  poller->loop();
}

