#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Poller.h>
#include <kizbox/framework/core/Shared.h>

int main(int argc, char *argv[])
{
  //Use OVK_LOG_LVL= env var to see this log trace
  OVK_INFO("Starting useless application.");
  //Get main loop
  Overkiz::Shared::Pointer< Overkiz::Poller > & poller = Overkiz::Poller::get(false); //Using coroutine or not
  poller->loop();
}

