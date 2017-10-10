/*
 * Thread.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <csignal>
#include <unistd.h>
#include <climits>
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
    address = nullptr;
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
    if(pthread_attr_init(&attributes) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
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

    if(pthread_attr_getscope(&attributes, (int *)(&scope)) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    return scope;
  }

  void Thread::Configuration::setScopeProperty(Scope scope)
  {
    if(pthread_attr_setscope(&attributes, scope) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  void Thread::Configuration::getStack(Stack& stack)
  {
    if(pthread_attr_getstack(&attributes, &stack.address, &stack.size) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_attr_getguardsize(&attributes, &stack.guardSize) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  void Thread::Configuration::setStack(Stack& stack)
  {
    if(pthread_attr_setstack(&attributes, stack.address, stack.size) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_attr_setguardsize(&attributes, stack.guardSize) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  bool Thread::Configuration::isJoindedToParent()
  {
    int detachstate;

    if(pthread_attr_getdetachstate(&attributes, &detachstate) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    return detachstate == PTHREAD_CREATE_JOINABLE ? true : false;
  }

  void Thread::Configuration::joinToParent(bool join)
  {
    int detachstate =
      join == true ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED;

    if(pthread_attr_setdetachstate(&attributes, detachstate)  != 0)
    {
      throw Overkiz::Errno::Exception();
    }
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
    if(pthread_mutexattr_init(&attributes) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Lock::Configuration::~Configuration()
  {
    pthread_mutexattr_destroy(&attributes);
  }

  Thread::Lock::Type Thread::Lock::Configuration::getType()
  {
    Type type;

    if(pthread_mutexattr_gettype(&attributes, (int *)(&type)) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    return type;
  }

  void Thread::Lock::Configuration::setType(Type type)
  {
    if(pthread_mutexattr_settype(&attributes, type) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Scope Thread::Lock::Configuration::getScope()
  {
    int scope;

    if(pthread_mutexattr_getpshared(&attributes, &scope) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

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

    if(pthread_mutexattr_setpshared(&attributes, newScope) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Lock::Lock()
  {
    if(pthread_mutex_init(&lock, nullptr) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Lock::Lock(Configuration& config)
  {
    if(pthread_mutex_init(&lock, &config.attributes) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Lock::~Lock()
  {
    pthread_mutex_destroy(&lock);
  }

  void Thread::Lock::acquire()
  {
    if(pthread_mutex_lock(&lock) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  bool Thread::Lock::tryToAcquire()
  {
    int ret = pthread_mutex_trylock(&lock);
    return ret != 0 ? false : true;
  }

  void Thread::Lock::release()
  {
    if(pthread_mutex_unlock(&lock) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::ReadWriteLock::Configuration::Configuration()
  {
    if(pthread_rwlockattr_init(&attributes) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::ReadWriteLock::Configuration::~Configuration()
  {
    pthread_rwlockattr_destroy(&attributes);
  }

  Thread::Scope Thread::ReadWriteLock::Configuration::getScope()
  {
    int scope;

    if(pthread_rwlockattr_getpshared(&attributes, &scope) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    return scope == PTHREAD_PROCESS_SHARED ? SCOPE_SYSTEM : SCOPE_PROCESS;
  }

  void Thread::ReadWriteLock::Configuration::setScope(Scope scope)
  {
    int newScope = scope == SCOPE_SYSTEM ?
                   PTHREAD_PROCESS_SHARED :
                   PTHREAD_PROCESS_PRIVATE;

    if(pthread_rwlockattr_setpshared(&attributes, newScope) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::ReadWriteLock::Policy Thread::ReadWriteLock::Configuration::getPolicy()
  {
    Policy policy;

    if(pthread_rwlockattr_getkind_np(&attributes, (int *)(&policy)) != 0)
    {
      throw Overkiz::Errno::Exception();
    }

    return policy;
  }

  void Thread::ReadWriteLock::Configuration::setPolicy(Policy policy)
  {
    if(pthread_rwlockattr_setkind_np(&attributes, policy) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::ReadWriteLock::ReadWriteLock()
  {
    if(pthread_rwlock_init(&lock, nullptr) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::ReadWriteLock::ReadWriteLock(Configuration& config)
  {
    if(pthread_rwlock_init(&lock, &config.attributes) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::ReadWriteLock::~ReadWriteLock()
  {
    pthread_rwlock_destroy(&lock);
  }

  void Thread::ReadWriteLock::acquireReadAccess()
  {
    if(pthread_rwlock_rdlock(&lock) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  bool Thread::ReadWriteLock::tryToAcquireReadAccess()
  {
    return pthread_rwlock_tryrdlock(&lock);
  }

  void Thread::ReadWriteLock::acquireWriteAccess()
  {
    if(pthread_rwlock_wrlock(&lock) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  bool Thread::ReadWriteLock::tryToAcquireWriteAccess()
  {
    return pthread_rwlock_trywrlock(&lock);
  }

  void Thread::ReadWriteLock::release()
  {
    if(pthread_rwlock_unlock(&lock) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Signal::Configuration::Configuration()
  {
    if(pthread_condattr_init(&attributes) != 0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Signal::Configuration::~Configuration()
  {
    pthread_condattr_destroy(&attributes);
  }

  Thread::Scope Thread::Signal::Configuration::getScope()
  {
    int scope;

    if(pthread_condattr_getpshared(&attributes, &scope) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    return scope == PTHREAD_PROCESS_SHARED ? SCOPE_SYSTEM : SCOPE_PROCESS;
  }

  void Thread::Signal::Configuration::setScope(Scope scope)
  {
    int newScope = scope == SCOPE_SYSTEM ?
                   PTHREAD_PROCESS_SHARED :
                   PTHREAD_PROCESS_PRIVATE;

    if(pthread_condattr_setpshared(&attributes, newScope) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Signal::Signal()
  {
    if(pthread_cond_init(&condition, nullptr) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_mutex_init(&lock, nullptr) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Signal::Signal(Configuration& config)
  {
    if(pthread_cond_init(&condition, &config.attributes) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_mutex_init(&lock, nullptr) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Signal::Signal(Lock::Configuration& lockConfig)
  {
    if(pthread_cond_init(&condition, nullptr) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_mutex_init(&lock, &lockConfig.attributes) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Signal::Signal(Configuration& config, Lock::Configuration& lockConfig)
  {
    if(pthread_cond_init(&condition, &config.attributes) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_mutex_init(&lock, &lockConfig.attributes) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  Thread::Signal::~Signal()
  {
    pthread_cond_destroy(&condition);
    pthread_mutex_destroy(&lock);
  }

  void Thread::Signal::broadcast()
  {
    if(pthread_mutex_lock(&lock) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_cond_broadcast(&condition) !=0)
    {
      pthread_mutex_unlock(&lock);
      throw Overkiz::Errno::Exception();
    }

    if(pthread_mutex_unlock(&lock) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  void Thread::Signal::send()
  {
    if(pthread_mutex_lock(&lock) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

    if(pthread_cond_signal(&condition) !=0)
    {
      pthread_mutex_unlock(&lock);
      throw Overkiz::Errno::Exception();
    }

    if(pthread_mutex_unlock(&lock) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
  }

  void Thread::Signal::wait()
  {
    if(pthread_cond_wait(&condition, &lock) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
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
    delegate = nullptr;
  }

  Shared::Pointer<Thread>& Thread::self()
  {
    if(key->empty())
    {
      key = Shared::Pointer<Thread>::create();
    }

    return *key;
  }

  void Thread::checkChildren() throw(Overkiz::Errno::Exception)
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
        {
          if(pthread_create(& tr->id, &tr->config.attributes, startCallback, tr) != 0)
          {
            (*it)->lock.release();
            throw Overkiz::Errno::Exception();
          }

          (*it)->status = STATUS_STARTING;
          (*it)->lock.release();
          it++;
        }
        break;

        case STATUS_STOPPED:
        {
          void *ret;

          if(int err = pthread_join(tr->id, &ret) != 0)
          {
            if(err != ESRCH)
            {
              (*it)->lock.release();
              throw Overkiz::Errno::Exception();
            }
          }

          tr->status = STATUS_IDLE;

          if(tr->delegate)
          {
            tr->delegate->release(tr);
          }

          (*it)->lock.release();
          it = children.erase(it);
        }
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

  void Thread::addChild(Shared::Pointer<Thread>& child) throw(Overkiz::Errno::Exception)
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

    bool check = false;

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

    for(auto i = current->children.begin();
        i != current->children.end(); i++)
    {
      (*i)->lock.acquire();

      switch((*i)->status)
      {
        case STATUS_IDLE:
          (*i)->status = STATUS_STOPPED;
          break;

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

    if(pthread_detach(child->id) !=0)
    {
      child->lock.release();
      throw Overkiz::Errno::Exception();
    }

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

    if(pthread_join(child->id, &ret) !=0)
    {
      throw Overkiz::Errno::Exception();
    }

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

    if(pthread_kill(id, sig) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
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

    if(pthread_cancel(this->id) !=0)
    {
      throw Overkiz::Errno::Exception();
    }
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
          break;

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
