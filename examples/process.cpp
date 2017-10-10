#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Poller.h>
#include <kizbox/framework/core/Shared.h>

#include <kizbox/framework/core/Process.h>


class Term : public Overkiz::Process::OnChildTerminated
{
public:
  Term() {}
  virtual ~Term() {}

  void terminated(int status)
  {
    printf("Parent get notified by jesus terminates with %i\n", WEXITSTATUS(status));
  }
};


int main(int argc, char *argv[])
{
  //Use OVK_LOG_LVL= env var to see this log trace
  OVK_INFO("Starting useless application.");
  //Get main loop
  Overkiz::Shared::Pointer< Overkiz::Poller > & poller = Overkiz::Poller::get(false); //Using coroutine or not

  if(Overkiz::Process::fork(Overkiz::Shared::Pointer<Term>::create()))
  {
    printf("Here Jesus %i\n", getpid());
    char *newargv[] = { "/usr/bin/false", "hello", "world", NULL };
    char *newenviron[] = { NULL };
    Overkiz::Process::exec(newargv[0], newargv, newenviron);
  }
  else
  {
    printf("Here god %i\n", getpid());
    printf("Do nothing, wait for jesus to be crusified\n");
  }

  poller->loop();
}

