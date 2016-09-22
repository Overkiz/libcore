#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Poller.h>
#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Inotify.h>

class Test : public Overkiz::InotifyInstance
{
public:

  Test(const std::string & path) : Overkiz::InotifyInstance(path, IN_MODIFY | IN_ACCESS | IN_ATTRIB)
  {
    printf("Start inotify %s\n", path.c_str());
    start();
  }

  virtual ~Test()
  {
  }

  void modified(const uint32_t mask)
  {
    printf("File modified %u!!\n", mask);
  }

};

int main(int argc, char *argv[])
{
  //Use OVK_LOG_LVL= env var to see this log trace
  OVK_INFO("Starting useless application.");
  //Get main loop
  Overkiz::Shared::Pointer< Overkiz::Poller > & poller = Overkiz::Poller::get(false); //Using coroutine or not
  const std::string path(argv[1]);
  Test notif(path);
  poller->loop();
}

