/*
 * Thread.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include "Thread.h"
#include <kizbox/framework/core/Errno.h>

namespace Overkiz
{

  Thread::Scheduler::Scheduler(Policy newPolicy, Parameters& newParameters)
  {
    policy = newPolicy;
    parameters = newParameters;
    inheritance = true;
  }

  Thread::Scheduler::Scheduler()
  {
    policy = POLICY_OTHER;
    inheritance = true;
  }

  Thread::Scheduler::~Scheduler()
  {
  }

  Thread::Scheduler::Policy Thread::Scheduler::getPolicy()
  {
    return policy;
  }

  void Thread::Scheduler::setPolicy(Policy newPolicy)
  {
    policy = newPolicy;
  }

  Thread::Scheduler::Parameters& Thread::Scheduler::getParameters()
  {
    return parameters;
  }

  void Thread::Scheduler::setParameters(Parameters& newParameters)
  {
    parameters = newParameters;
  }

  bool Thread::Scheduler::isInheritedFromParent()
  {
    return inheritance;
  }

  void Thread::Scheduler::inheritsFromParent(bool inherits)
  {
    inheritance = inherits;
  }

  Thread::Stack::Stack()
  {
    address = NULL;
    size = 0;
    guardSize = 0;
  }

  Thread::Stack::Stack(void *addr, size_t newSize)
  {
    address = addr;
    size = newSize;
    guardSize = 0;
  }

  Thread::Stack::~Stack()
  {
  }

  size_t Thread::Stack::getStackSize()
  {
    return size;
  }

  void *Thread::Stack::getStackAddress()
  {
    return address;
  }

  void Thread::Stack::setStack(void *addr, size_t newSize)
  {
    address = addr;
    size = newSize;
  }

  size_t Thread::Stack::getGuardSize()
  {
    return guardSize;
  }

  void Thread::Stack::setGuardSize(size_t newSize)
  {
    guardSize = newSize;
  }

  Thread::Configuration::Configuration()
  {
    pthread_attr_init(&attributes);
  }

  Thread::Configuration::~Configuration()
  {
    pthread_attr_destroy(&attributes);
  }

  void Thread::Configuration::getSchedulingProperties(Scheduler& sched)
  {
    int inheritsched;

    if(pthread_attr_getinheritsched(&attributes, &inheritsched) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    sched.inheritance = inheritsched;

    if(pthread_attr_getschedpolicy(&attributes, (int *)(&sched.policy)) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_attr_getschedparam(&attributes, &sched.parameters) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  void Thread::Configuration::setSchedulingProperties(Scheduler& sched)
  {
    if(pthread_attr_setinheritsched(&attributes, sched.isInheritedFromParent()) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_attr_setschedpolicy(&attributes, sched.getPolicy()) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_attr_setschedparam(&attributes, &sched.getParameters()) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Scope Thread::Configuration::getScopeProperty()
  {
    Scope scope;
    pthread_attr_getscope(&attributes, (int *)(&scope));
    return scope;
  }

  void Thread::Configuration::setScopeProperty(Scope scope)
  {
    pthread_attr_setscope(&attributes, scope);
  }

  void Thread::Configuration::getStack(Stack& stack)
  {
    pthread_attr_getstack(&attributes, &stack.address, &stack.size);
    pthread_attr_getguardsize(&attributes, &stack.guardSize);
  }

  void Thread::Configuration::setStack(Stack& stack)
  {
    pthread_attr_setstack(&attributes, stack.address, stack.size);
    pthread_attr_setguardsize(&attributes, stack.guardSize);
  }

  bool Thread::Configuration::isJoindedToParent()
  {
    int detachstate;
    pthread_attr_getdetachstate(&attributes, &detachstate);
    return detachstate == PTHREAD_CREATE_JOINABLE ? true : false;
  }

  void Thread::Configuration::joinToParent(bool join)
  {
    int detachstate =
      join == true ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED;
    pthread_attr_setdetachstate(&attributes, detachstate);
  }


  size_t Thread::Configuration::getStackSize()
  {
    size_t stacksize=0;

    if(pthread_attr_getstacksize(&attributes, &stacksize)!= 0)
    {
      throw Overkiz::Errno::Exception();
    }

    return stacksize;
  }


  size_t Thread::Configuration::setStackSize(size_t stacksize)
  {
    #ifndef PTHREAD_STACK_MIN
#define  PTHREAD_STACK_MIN 16384
    #endif

    if(stacksize < PTHREAD_STACK_MIN)
    {
      stacksize = PTHREAD_STACK_MIN;
    }

    int pgsize = getpagesize();

    if(stacksize % pgsize)
    {
      stacksize = ((stacksize / pgsize) + 1) * pgsize;
    }

    if(pthread_attr_setstacksize(&attributes, stacksize)!= 0)
    {
      throw Overkiz::Errno::Exception();
    }

    return stacksize;
  }

  Thread::Lock::Configuration::Configuration()
  {
    pthread_mutexattr_init(&attributes);
  }

  Thread::Lock::Configuration::~Configuration()
  {
    pthread_mutexattr_destroy(&attributes);
  }

  Thread::Lock::Type Thread::Lock::Configuration::getType()
  {
    Type type;
    pthread_mutexattr_gettype(&attributes, (int *)(&type));
    return type;
  }

  void Thread::Lock::Configuration::setType(Type type)
  {
    pthread_mutexattr_settype(&attributes, type);
  }

  Thread::Scope Thread::Lock::Configuration::getScope()
  {
    int scope;
    pthread_mutexattr_getpshared(&attributes, &scope);
    return scope == PTHREAD_PROCESS_SHARED ? SCOPE_SYSTEM : SCOPE_PROCESS;
  }

  void Thread::Lock::Configuration::setScope(Scope scope)
  {
    int newScope;

    switch(scope)
    {
      case SCOPE_SYSTEM:
        newScope = PTHREAD_PROCESS_SHARED;
        break;

      default:
      case SCOPE_PROCESS:
        newScope = PTHREAD_PROCESS_PRIVATE;
        break;
    }

    pthread_mutexattr_setpshared(&attributes, newScope);
  }

  Thread::Lock::Lock()
  {
    pthread_mutex_init(&lock, NULL);
  }

  Thread::Lock::Lock(Configuration& config)
  {
    pthread_mutex_init(&lock, &config.attributes);
  }

  Thread::Lock::~Lock()
  {
    pthread_mutex_destroy(&lock);
  }

  void Thread::Lock::acquire()
  {
    pthread_mutex_lock(&lock);
  }

  bool Thread::Lock::tryToAcquire()
  {
    int ret = pthread_mutex_trylock(&lock);
    return ret != 0 ? false : true;
  }

  void Thread::Lock::release()
  {
    pthread_mutex_unlock(&lock);
  }

  Thread::ReadWriteLock::Configuration::Configuration()
  {
    pthread_rwlockattr_init(&attributes);
  }

  Thread::ReadWriteLock::Configuration::~Configuration()
  {
    pthread_rwlockattr_destroy(&attributes);
  }

  Thread::Scope Thread::ReadWriteLock::Configuration::getScope()
  {
    int scope;
    pthread_rwlockattr_getpshared(&attributes, &scope);
    return scope == PTHREAD_PROCESS_SHARED ? SCOPE_SYSTEM : SCOPE_PROCESS;
  }

  void Thread::ReadWriteLock::Configuration::setScope(Scope scope)
  {
    int newScope = scope == SCOPE_SYSTEM ?
                   PTHREAD_PROCESS_SHARED :
                   PTHREAD_PROCESS_PRIVATE;
    pthread_rwlockattr_setpshared(&attributes, newScope);
  }

  Thread::ReadWriteLock::Policy Thread::ReadWriteLock::Configuration::getPolicy()
  {
    Policy policy;
    pthread_rwlockattr_getkind_np(&attributes, (int *)(&policy));
    return policy;
  }

  void Thread::ReadWriteLock::Configuration::setPolicy(Policy policy)
  {
    pthread_rwlockattr_setkind_np(&attributes, policy);
  }

  Thread::ReadWriteLock::ReadWriteLock()
  {
    pthread_rwlock_init(&lock, NULL);
  }

  Thread::ReadWriteLock::ReadWriteLock(Configuration& config)
  {
    pthread_rwlock_init(&lock, &config.attributes);
  }

  Thread::ReadWriteLock::~ReadWriteLock()
  {
    pthread_rwlock_destroy(&lock);
  }

  void Thread::ReadWriteLock::acquireReadAccess()
  {
    pthread_rwlock_rdlock(&lock);
  }

  bool Thread::ReadWriteLock::tryToAcquireReadAccess()
  {
    return pthread_rwlock_tryrdlock(&lock);
  }

  void Thread::ReadWriteLock::acquireWriteAccess()
  {
    pthread_rwlock_wrlock(&lock);
  }

  bool Thread::ReadWriteLock::tryToAcquireWriteAccess()
  {
    return pthread_rwlock_trywrlock(&lock);
  }

  void Thread::ReadWriteLock::release()
  {
    pthread_rwlock_unlock(&lock);
  }

  Thread::Signal::Configuration::Configuration()
  {
    pthread_condattr_init(&attributes);
  }

  Thread::Signal::Configuration::~Configuration()
  {
    pthread_condattr_destroy(&attributes);
  }

  Thread::Scope Thread::Signal::Configuration::getScope()
  {
    int scope;
    pthread_condattr_getpshared(&attributes, &scope);
    return scope == PTHREAD_PROCESS_SHARED ? SCOPE_SYSTEM : SCOPE_PROCESS;
  }

  void Thread::Signal::Configuration::setScope(Scope scope)
  {
    int newScope = scope == SCOPE_SYSTEM ?
                   PTHREAD_PROCESS_SHARED :
                   PTHREAD_PROCESS_PRIVATE;
    pthread_condattr_setpshared(&attributes, newScope);
  }

  Thread::Signal::Signal()
  {
    pthread_cond_init(&condition, NULL);
    pthread_mutex_init(&lock, NULL);
  }

  Thread::Signal::Signal(Configuration& config)
  {
    pthread_cond_init(&condition, &config.attributes);
    pthread_mutex_init(&lock, NULL);
  }

  Thread::Signal::Signal(Lock::Configuration& lockConfig)
  {
    pthread_cond_init(&condition, NULL);
    pthread_mutex_init(&lock, &lockConfig.attributes);
  }

  Thread::Signal::Signal(Configuration& config, Lock::Configuration& lockConfig)
  {
    pthread_cond_init(&condition, &config.attributes);
    pthread_mutex_init(&lock, &lockConfig.attributes);
  }

  Thread::Signal::~Signal()
  {
    pthread_cond_destroy(&condition);
    pthread_mutex_destroy(&lock);
  }

  void Thread::Signal::broadcast()
  {
    pthread_mutex_lock(&lock);
    pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&lock);
  }

  void Thread::Signal::send()
  {
    pthread_mutex_lock(&lock);
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&lock);
  }

  void Thread::Signal::wait()
  {
    pthread_cond_wait(&condition, &lock);
  }

  bool Thread::Signal::wait(Timeout& timeout)
  {
    return pthread_cond_timedwait(&condition, &lock, &timeout);
  }

  Thread::Thread()
  {
    id = pthread_self();
    status = STATUS_IDLE;
    delegate = nullptr;
  }

  Thread::Thread(Configuration& newConfig) :
    Thread()
  {
    config = newConfig;
  }

  Thread::Thread(Delegate *newDelegate) :
    Thread()
  {
    delegate = newDelegate;
  }

  Thread::Thread(Configuration& newConfig, Delegate *newDelegate) :
    Thread()
  {
    config = newConfig;
    delegate = newDelegate;
  }

  Thread::~Thread()
  {
    delegate = NULL;
  }

  Shared::Pointer<Thread>& Thread::self()
  {
    if(key->empty())
    {
      key = Shared::Pointer<Thread>::create();
    }

    return *key;
  }

  void Thread::checkChildren()
  {
    auto it = children.begin();

    while(it != children.end())
    {
      (*it)->lock.acquire();
      Shared::Pointer<Thread> & t = (*it);
      Thread * tr = &(*t);

      switch((*it)->status)
      {
        case STATUS_IDLE:
          pthread_create(& tr->id, &config.attributes, startCallback,
                         tr);
          (*it)->status = STATUS_STARTING;
          (*it)->lock.release();
          it++;
          break;

        case STATUS_STOPPED:
          void *ret;
          pthread_join(tr->id, &ret);
          tr->status = STATUS_IDLE;

          if(tr->delegate)
          {
            tr->delegate->release(tr);
          }

          (*it)->lock.release();
          it = children.erase(it);
          break;

        case STATUS_RUNNING:
        case STATUS_STARTING:
        case STATUS_STOPPING:
        default:
          (*it)->lock.release();
          it++;
          break;
      }
    }
  }

  void Thread::addChild(Shared::Pointer<Thread>& child)
  {
    Shared::Pointer<Thread> current = self();
    child->lock.acquire();

    if(child->parent != current)
    {
      if(!child->parent.empty())
      {
        child->lock.release();
        throw;
      }

      current->children.push_back(child);
      child->parent = current;
    }

    child->lock.release();
    current->checkChildren();
  }

  void Thread::removeChild(Shared::Pointer<Thread>& child)
  {
    Shared::Pointer<Thread> current = self();
    child->lock.acquire();

    if(child->parent != current)
    {
      child->lock.release();
      throw;
    }

    bool check;

    switch(child->status)
    {
      case STATUS_STARTING:
      case STATUS_RUNNING:
        child->cancel();
        break;

      case STATUS_STOPPING:
        break;

      case STATUS_IDLE:
        child->status = STATUS_STOPPED;
        break;

      case STATUS_STOPPED:
        check = true;
        break;

      default:
        break;
    }

    child->lock.release();

    if(check == true)
    {
      current->checkChildren();
    }
  }

  void Thread::cancelChildren()
  {
    Shared::Pointer<Thread> current = self();

    for(ThreadIterator i = current->children.begin();
        i != current->children.end(); i++)
    {
      (*i)->lock.acquire();

      switch((*i)->status)
      {
        case STATUS_IDLE:
          (*i)->status = STATUS_STOPPED;

        case STATUS_RUNNING:
        case STATUS_STARTING:
          (*i)->cancel();
          break;

        case STATUS_STOPPING:
        case STATUS_STOPPED:
        default:
          break;
      }

      (*i)->lock.release();
    }
  }

  void Thread::detach(Shared::Pointer<Thread>& child)
  {
    Shared::Pointer<Thread> current = self();
    child->lock.acquire();

    if(child->parent != current)
    {
      child->lock.release();
      throw;
    }

    auto it = current->children.begin();

    while(it != current->children.end())
    {
      if(*it == child)
      {
        it = current->children.erase(it);
      }
      else
      {
        it++;
      }
    }

    pthread_detach(child->id);
    child->parent = Shared::Pointer<Thread>();
    child->lock.release();
  }

  void Thread::exit(long ret)
  {
    pthread_exit((void *) ret);
  }

  long Thread::join(Shared::Pointer<Thread>& child)
  {
    Shared::Pointer<Thread> current = self();
    child->lock.acquire();

    if(current != child->parent)
    {
      child->lock.release();
      throw;
    }

    void *ret;
    child->lock.release();
    pthread_join(child->id, &ret);
    child->lock.acquire();
    auto it = current->children.begin();

    while(it != current->children.end())
    {
      if(*it == child)
      {
        it = current->children.erase(it);
      }
      else
      {
        it++;
      }
    }

    child->parent = Shared::Pointer<Thread>();
    child->lock.release();
    return (long) ret;
  }

  void Thread::kill(long sig)
  {
    if(parent != self())
    {
      throw;
    }

    pthread_kill(id, sig);
  }

  void Thread::setDelegate(Delegate *newDelegate)
  {
    delegate = newDelegate;
  }

  long Thread::start()
  {
    Shared::Pointer<Thread> cur = *key;
    void *result = startCallback(this);
    *key = cur;
    return (long) result;
  }

  int Thread::run()
  {
    if(this == & (*self()))
    {
      while(true)
      {
        Signal signal;
        Timeout timeout;
        timeout.tv_sec = 1;
        timeout.tv_nsec = 0;
        signal.wait(timeout);
      }
    }

    return 0;
  }

  void Thread::cancel()
  {
    if(self() != this->parent)
    {
      throw;
    }

    pthread_cancel(this->id);
  }

  void Thread::started(Thread *child)
  {
  }

  void Thread::stopped(Thread *child)
  {
  }

  int Thread::init()
  {
    return 0;
  }

  void Thread::cleanup()
  {
  }

  void Thread::stoppedCallback(void *arg)
  {
    Thread *thread = static_cast<Thread *>(arg);
    thread->status = STATUS_STOPPED;

    if(!thread->parent.empty())
    {
      thread->parent->stopped(thread);
    }
  }

  void Thread::cleanupCallback(void *arg)
  {
    Thread *thread = static_cast<Thread *>(arg);
    auto it = thread->children.begin();

    while(it != thread->children.end())
    {
      (*it)->lock.acquire();

      switch((*it)->status)
      {
        case STATUS_IDLE:
          (*it)->status = STATUS_STOPPED;
          break;

        case STATUS_RUNNING:
        case STATUS_STARTING:
          (*it)->cancel();

        case STATUS_STOPPING:
        case STATUS_STOPPED:
          join((*it));
          break;

        default:
          break;
      }

      (*it)->lock.release();
      it = thread->children.erase(it);
    }

    thread->cleanup();
    *key = Shared::Pointer<Thread>();
  }

  void *Thread::startCallback(void *arg)
  {
    Thread *thread = static_cast<Thread *>(arg);
    long ret;
    thread->lock.acquire();
    *key = Shared::Pointer<Thread>();
    pthread_cleanup_push(stoppedCallback, thread);
    ret = thread->init();

    if(ret == 0)
    {
      pthread_cleanup_push(cleanupCallback, thread);
      thread->status = STATUS_RUNNING;

      if(!thread->parent.empty())
      {
        thread->parent->started(thread);
      }

      thread->lock.release();
      ret = thread->run();
      thread->lock.acquire();
      thread->status = STATUS_STOPPING;
      pthread_cleanup_pop(1);
    }

    pthread_cleanup_pop(1);
    thread->lock.release();
    return (void *) ret;
  }

  int Thread::getSchedulingMaxPriority(Scheduler::Policy policy)
  {
    return sched_get_priority_max((int)policy);
  }

  int Thread::getSchedulingMinPriority(Scheduler::Policy policy)
  {
    return sched_get_priority_min((int)policy);
  }

  void Thread::getSchedulingProperties(Scheduler& sched)
  {
    Scheduler::Parameters param;
    Scheduler::Policy policy;

    if(pthread_getschedparam(id, (int*)&policy,&param) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    sched.setPolicy(policy);
    sched.setParameters(param);
  }

  void Thread::setSchedulingProperties(Scheduler& sched)
  {
    if(pthread_setschedparam(id, (int)(sched.getPolicy()),&sched.getParameters()) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Configuration Thread::getConfiguration()
  {
    return config;
  }

  void Thread::setConfiguration(Configuration& newConfig)
  {
    config = newConfig;
  }

  Thread::Key<Thread> Thread::key;

}
