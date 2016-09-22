#
# Framework
#
lib_LTLIBRARIES = libCore.la

# Core
libCore_la_SOURCES =  filesystem/Directory.cpp \
                      filesystem/Node.cpp \
                      filesystem/File.cpp \
                      filesystem/Inotify.cpp \
                      filesystem/UpdateFile.cpp \
                      misc/Base64.cpp \
                      misc/Daemon.cpp \
                      misc/Md5.cpp \
                      misc/Library.cpp \
                      misc/Pipe.cpp \
                      misc/PluginLoader.cpp \
                      misc/Terminal.cpp \
                      misc/UniversalUniqueIdentifier.cpp \
                      poll/Coroutine.cpp \
                      poll/Event.cpp \
                      poll/Poller.cpp \
                      poll/Signal.cpp \
                      poll/Task.cpp \
                      poll/Watcher.cpp \
                      time/Date.cpp \
                      time/Time.cpp \
                      time/Timer.cpp \
                      time/Twilight.cpp \
                      Errno.cpp \
                      Log.cpp \
                      Process.cpp \
                      Thread.cpp

# Use conditional AM since there is a bug in automake for subdir-objects with vars
# -> libCore_la_SOURCES +=  arch/$(arch)/Context.cpp
if ARCH_ARM
libCore_la_SOURCES +=  arch/arm/Context.cpp
endif
if ARCH_i386
libCore_la_SOURCES +=  arch/i386/Context.cpp
endif
if ARCH_x86_64
libCore_la_SOURCES +=  arch/x86_64/Context.cpp
endif

libCore_la_LIBADD = -lpthread -lrt -ldl

libCore_la_CXXFLAGS = -Werror -std=c++0x \
                      -I$(top_srcdir)/include/kizbox/framework/core \
                      -I$(top_srcdir)/include \
                      $(ARCH_CXXFLAGS)
libCore_la_LDFLAGS =  -version-info $(FRAMEWORK_LT_VERSION_INFO)
