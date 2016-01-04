

#include <kizbox/framework/core/Log.h>
#include <kizbox/framework/core/Poller.h>
#include <kizbox/framework/core/Shared.h>
#include <kizbox/framework/core/Terminal.h>
#include <kizbox/framework/core/Stream.h>
#include <kizbox/framework/core/Watcher.h>
#include <kizbox/framework/core/Process.h>

#include <getopt.h>
#include <iostream>


#ifndef UNUSED
  #define UNUSED(XXX) (void)(XXX)
#endif


class Serial: protected Overkiz::Stream::Delegate
{
public:

  Serial(const std::string & path) :
    _inputOutputStream(path), path(path)
  {
    _inputOutputStream.setDelegate(this);
  }

  void enable()
  {
    _inputOutputStream.wait(Overkiz::Stream::OPENED | Overkiz::Stream::INPUT_READY);
    _inputOutputStream.open();
    _inputOutputStream.setNoCTtyFlag();
    _inputOutputStream.setControlMode(CS8 | CLOCAL | CREAD);
    _inputOutputStream.setLocalMode(0);
    _inputOutputStream.setLine(0);
    _inputOutputStream.setControlCharacters(VTIME, 0);
    _inputOutputStream.setControlCharacters(VMIN, 1);
    _inputOutputStream.setInputMode(IGNPAR);
    _inputOutputStream.setInputSpeed(B115200);
    _inputOutputStream.setOutputMode(0);
    _inputOutputStream.setInputSpeed(B115200);
  }

  void disable()
  {
    _inputOutputStream.close();
  }

  void wait(int status, bool mask = true)
  {
    mask ? _inputOutputStream.wait(
      _inputOutputStream.getWaitStatus() | status) :
    _inputOutputStream.wait(status);
  }

  void stopWait(int status)
  {
    OVK_INFO("%s",__FUNCTION__);
    _inputOutputStream.wait(_inputOutputStream.getWaitStatus() & ~status);
  }


private:

  void ready(int status)
  {
    if(status & Overkiz::Stream::INPUT_READY)
    {
      readData(_inputOutputStream);
    }
    else if(status & Overkiz::Stream::OUTPUT_READY)
    {
      writeData(_inputOutputStream);
    }
    else if(status == 0)
    {
      /* FD is closed due to error */
      OVK_WARNING("Terminal closed due to an error. (ERR or HUP)");
      exit(1);
    }
    else
    {
      OVK_WARNING("Terminal manager unknown status: %02X ", status);
      exit(1);
    }
  }

  void readData(Overkiz::Stream::Input & input)
  {
    UNUSED(input);
    uint32_t bufferSize = 512;
    uint8_t buff[bufferSize];

    while(1)
    {
      size_t read = input.read(buff, bufferSize);

      if(read == 0)
        break;

      fwrite(buff,1,read,stdout);
    }

    wait(Overkiz::Stream::INPUT_READY);
  }

  void writeData(Overkiz::Stream::Output & output)
  {
    UNUSED(output);
    OVK_INFO("%s",__FUNCTION__);
  }

  Overkiz::Terminal::InputOutput _inputOutputStream;
  std::string path;

};


int main(int argc, char *argv[])
{
  UNUSED(argc);
  UNUSED(argv);
  //Use OVK_LOG_LVL= env var to see this log trace
  OVK_INFO("Starting useless application");
  std::string serialDev = "/dev/ttyUSB0";
  static const struct option long_options[] =
  {
    { "serial", required_argument, NULL, 's' },
    { "help", no_argument, NULL, 'h' },
    { NULL, no_argument, NULL, 0 }
  };
  int c;
  int option_index;

  while((c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1)
  {
    switch(c)
    {
      case 'h':
      {
        std::cout << "Usage: " << argv[0] << " \n";
        std::cout << "\nOptions:\n" <<
                  "  -h, --help           Show this help message and exit\n" <<
                  "  --serial             Set serial device default is : " <<  serialDev <<
                  std::endl;
        exit(EXIT_SUCCESS);
        break;
      }

      case 's':
      {
        serialDev = optarg;
        std::cout <<"set serial :" << serialDev << std::endl;
        break;
      }

      default:
        break;
    }
  }

  try
  {
    //Get main loop
    Overkiz::Shared::Pointer< Overkiz::Poller > & poller = Overkiz::Poller::get(false); //Using coroutine or not
    OVK_INFO(" read data from serial : %s", serialDev.c_str());
    Serial serial(serialDev);
    serial.disable();
    serial.enable();
    serial.wait(Overkiz::Stream::INPUT_READY, false);
    poller->loop();
  }
  catch(Overkiz::Exception & e)
  {
    OVK_ERROR("Overkiz::Exception: %s", e.getId());
  }
  catch(...)
  {
    OVK_ERROR("unhandled exception");
  }

  OVK_INFO("end");
}
