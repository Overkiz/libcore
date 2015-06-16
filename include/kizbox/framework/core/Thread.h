/*
 * Thread.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_THREAD_H_
#define OVERKIZ_THREAD_H_

#include <pthread.h>
#include <vector>
#include <stdio.h>

#include <kizbox/framework/core/Shared.h>

namespace Overkiz
{

  /**
   * Class used to manage a thread.
   */
  class Thread
  {
  public:

    /**
     * Thread timeout definition.
     */
    typedef struct timespec Timeout;

    /**
     * Define ClockId.
     */
    typedef clockid_t ClockId;

    class Configuration;

    /**
     * Class used to define a thread scheduler.
     */
    class Scheduler
    {
    public:
      /**
       * Define the different scheduling policies
       */
      typedef enum
      {
        POLICY_OTHER = SCHED_OTHER, //!< POLICY_OTHER
        POLICY_FIFO = SCHED_FIFO, //!< POLICY_FIFO
        POLICY_RR = SCHED_RR
                    //!< POLICY_RR
      } Policy;

      /**
       * Define the scheduling parameters.
       * Depends on the choosen policy.
       *
       * @see pthread
       */
      typedef struct sched_param Parameters;

      /**
       * Constructor.
       * The scheduler inherits his properties from his parent
       * (inheritance = true).
       *
       * @return
       */
      Scheduler();

      /**
       * Constructor whith scecified policies and associated parameters.
       * The scheduler does not inherit his properties from his parent
       * (inheritance = false).
       *
       * @param policy : the policy to use
       * @param parameters : the parameters associated to the policy.
       * @return
       */
      Scheduler(Policy policy, Parameters& parameters);

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~Scheduler();

      /**
       * Get policy
       *
       * @return the policy
       */
      Policy getPolicy();

      /**
       * Set policy
       *
       * @param policy : the new policy
       */
      void setPolicy(Policy policy);

      /**
       * Get the policy parameters.
       *
       * @return the policy parameters
       */
      Parameters& getParameters();

      /**
       * Set the policy parameters.
       *
       * @param params : the new params.
       */
      void setParameters(Parameters& params);

      /**
       * Get inheritance property.
       *
       * @return true if the scheduler inherits from his parent,
       * else return false.
       */
      bool isInheritedFromParent();

      /**
       * Set inheritance property.
       *
       * @param inherits : true if you want the scheduler to inherit from his parent,
       * else false
       */
      void inheritsFromParent(bool inherits);

    private:
      Policy policy;
      Parameters parameters;
      bool inheritance;

      friend class Configuration;

    };

    /**
     * Define the scope for any thread elem (thread, lock, read/write lock, condition).
     *
     * SCOPE_SYSTEM means the thread elem is shared upon all thread inside any processes.
     * SCOPE_PROCESS means the thread elem is shared upon all thread inside this process.
     */
    typedef enum
    {
      SCOPE_SYSTEM = PTHREAD_SCOPE_SYSTEM,  //!< SCOPE_SYSTEM
      SCOPE_PROCESS = PTHREAD_SCOPE_PROCESS,  //!< SCOPE_PROCESS
    } Scope;

    /**
     * Used to define a thread stack.
     */
    class Stack
    {
    public:

      /**
       * Constructor of an empty stack.
       *
       * @return
       */
      Stack();

      /**
       * Constructor of a stack from given address and size.
       *
       * @param addr : the start address of the stack
       * @param size : the stack size.
       * @return
       */
      Stack(void *addr, size_t size);

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~Stack();

      /**
       * Get stack size.
       *
       * @return
       */
      size_t getStackSize();

      /**
       * Get the start address of the stack.
       *
       * @return start address of the stack.
       */
      void *getStackAddress();

      /**
       * Set a new stack address and size.
       *
       * @param address : the new address.
       * @param size : the new size.
       */
      void setStack(void *address, size_t size);

      /**
       * Get the size a the guard area.
       *
       * @return
       */
      size_t getGuardSize();

      /**
       * Set a guard area to control thread stack overflow.
       * Cannot be set if address is not NULL
       * (ie thread stack not dynamically allocated).
       *
       * @param size : the size of the guard area
       */
      void setGuardSize(size_t size);

    private:
      size_t size;
      void *address;
      size_t guardSize;

      friend class Configuration;
    };

    /**
     * Class used to specify thread configuration.
     */
    class Configuration
    {
    public:

      /**
       * Constructor.
       *
       * @return
       */
      Configuration();

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~Configuration();

      /**
       * Get the scheduling inheritance attribute.
       *
       * @return SCHEDULING_INHERITANCE_ACTIVATED if thread inherits scheduling properties
       * from his parent, else return SCHEDULING_INHERITANCE_DEACTIVATED
       */
      void getSchedulingProperties(Scheduler& sched);

      /**
       * Set the scheduling inheritance attribute.
       *
       * @param attr : SCHEDULING_INHERITANCE_ACTIVATED if thread inherits scheduling properties
       * from his parent, else return SCHEDULING_INHERITANCE_DEACTIVATED
       */
      void setSchedulingProperties(Scheduler& sched);

      /**
       * Get scope property.
       *
       * @return the scope property
       *
       * @see Scope
       */
      Scope getScopeProperty();

      /**
       * Set scope property.
       *
       * @param scope
       */
      void setScopeProperty(Scope scope);

      /**
       * Get the stack properties.
       *
       * @param stack : used to store the result.
       */
      void getStack(Stack& stack);

      /**
       * Set the stack properties
       *
       * @param stack : the new properties.
       */
      void setStack(Stack& stack);

      /**
       * Test if a thread is joined to parent thread.
       *
       * @return
       */
      bool isJoindedToParent();

      /**
       * Set the join property of the thread.
       *
       * @param join
       */
      void joinToParent(bool join);

    private:
      pthread_attr_t attributes;

      friend class Thread;
    };

    /**
     * Class template used to define an object which is unique to
     * any thread launched by a process.
     * Replace T by the type of data you want to store.
     * T must implement void cleanup () method.
     * This method is called when a key is destroyed.
     */
    template<class T>
    class Key
    {
    public:
      /**
       * Constructor.
       *
       * @return
       */
      Key()
      {
        pthread_key_create(&key, destroy);
        Shared::Pointer<T> *value = new Shared::Pointer<T>();
        pthread_setspecific(key, value);
      }

      /**
       * Constructor with new key object.
       *
       * @param t : the new key object.
       * @return
       */
      Key(const Shared::Pointer<T>& val)
      {
        pthread_key_create(&key, destroy);
        Shared::Pointer<T> *value = new Shared::Pointer<T> (val);
        pthread_setspecific(key, value);
      }

      /**
       * Destructor
       *
       * @return
       */
      virtual ~Key()
      {
        pthread_key_delete(key);
      }

      Shared::Pointer<T>& operator *()
      {
        Shared::Pointer<T> *val =
          static_cast<Shared::Pointer<T> *>(pthread_getspecific(key));

        if(val == NULL)
        {
          val = new Shared::Pointer<T>();
          pthread_setspecific(key, val);
        }

        return *val;
      }

      const Shared::Pointer<T>& operator *() const
      {
        Shared::Pointer<T> *val =
          static_cast<Shared::Pointer<T> *>(pthread_getspecific(key));

        if(val == NULL)
        {
          val = new Shared::Pointer<T>();
          pthread_setspecific(key, val);
        }

        return *val;
      }

      Shared::Pointer<T> *operator ->()
      {
        Shared::Pointer<T> *val =
          static_cast<Shared::Pointer<T> *>(pthread_getspecific(key));

        if(val == NULL)
        {
          val = new Shared::Pointer<T>();
          pthread_setspecific(key, val);
        }

        return val;
      }

      const Shared::Pointer<T> *operator ->() const
      {
        Shared::Pointer<T> *val =
          static_cast<Shared::Pointer<T> *>(pthread_getspecific(key));

        if(val == NULL)
        {
          val = new Shared::Pointer<T>();
          pthread_setspecific(key, val);
        }

        return val;
      }

      Key& operator = (const Shared::Pointer<T>& val)
      {
        Shared::Pointer<T> *value =
          static_cast<Shared::Pointer<T> *>(pthread_getspecific(key));

        if(value == NULL)
        {
          value = new Shared::Pointer<T> (val);
          pthread_setspecific(key, value);
        }
        else
        {
          *value = val;
        }

        return *this;
      }

    private:
      pthread_key_t key;

      static void destroy(void *ptr)
      {
        Shared::Pointer<T> *val = static_cast<Shared::Pointer<T> *>(ptr);

        if(val)
        {
          delete val;
        }
      }
    };

    class Signal;

    /**
     * Used to manage simple locks.
     */
    class Lock
    {
    public:

      /**
       * Define the type of lock.
       *
       * @see pthread
       */
      typedef enum
      {
        TIMED_TYPE = PTHREAD_MUTEX_TIMED_NP,          //!< TIMED_TYPE
        RECURSIVE_TYPE = PTHREAD_MUTEX_RECURSIVE_NP,  //!< RECURSIVE_TYPE
        ERRORCHECK_TYPE = PTHREAD_MUTEX_ERRORCHECK_NP,  //!< ERRORCHECK_TYPE
        ADAPTIVE_TYPE = PTHREAD_MUTEX_ADAPTIVE_NP,    //!< ADAPTIVE_TYPE
      } Type;

      /**
       * Class used to configure the mutex.
       */
      class Configuration
      {
      public:
        /**
         * Constructor.
         *
         * @return
         */
        Configuration();

        /**
         * Destructor.
         *
         * @return
         */
        virtual ~Configuration();

        /**
         * Type getter.
         *
         * @return
         */
        Type getType();

        /**
         * Type setter.
         *
         * @param type
         */
        void setType(Type type);

        /**
         * Scope getter.
         *
         * @return the scope of the mutex.
         */
        Scope getScope();

        /**
         * Scope setter.
         *
         * @param scope : the new scope of mutex.
         */
        void setScope(Scope scope);

      private:
        pthread_mutexattr_t attributes;

        friend class Lock;
        friend class Signal;
      };

      /**
       * Constructor with default mutex configuration.
       *
       * @return
       */
      Lock();

      /**
       * Constructor with given configuration.
       *
       * @param config
       * @return
       */
      Lock(Configuration& config);

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~Lock();

      /**
       * Method used to acquire this mutex lock.
       *
       */
      void acquire();

      /**
       * Method used to try to acquire this mutex lock.
       *
       * @return true if it succeed else false.
       */
      bool tryToAcquire();

      /**
       * Method used to release this mutex lock.
       *
       */
      void release();
    private:
      pthread_mutex_t lock;
    };

    /**
     * Used to manage read-write locks.
     */
    class ReadWriteLock
    {
    public:

      /**
       * Define the type of read/write lock policy.
       *
       * @see pthread
       */
      typedef enum
      {
        POLICY_PREFER_READER = PTHREAD_RWLOCK_PREFER_READER_NP,
        POLICY_PREFER_WRITER = PTHREAD_RWLOCK_PREFER_WRITER_NP,
        POLICY_PREFER_NONRECURSIVE_WRITER = PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
        POLICY_DEFAULT = PTHREAD_RWLOCK_DEFAULT_NP,
      } Policy;

      class Configuration
      {
      public:
        /**
         * Constructor.
         *
         * @return
         */
        Configuration();

        /**
         * Destructor.
         *
         * @return
         */
        virtual ~Configuration();

        /**
         * Scope getter.
         *
         * @return the scope of the mutex.
         */
        Scope getScope();

        /**
         * Scope setter.
         *
         * @param scope : the new scope of mutex.
         */
        void setScope(Scope scope);

        /**
         * Policy getter.
         *
         * @return
         */
        Policy getPolicy();

        /**
         * Policy setter.
         *
         * @param policy
         */
        void setPolicy(Policy policy);

      private:
        pthread_rwlockattr_t attributes;

        friend class ReadWriteLock;
      };

      /**
       * Constructor.
       *
       * @return
       */
      ReadWriteLock();

      /**
       * Constructor with specified config.
       *
       * @param config
       * @return
       */
      ReadWriteLock(Configuration& config);

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~ReadWriteLock();

      /**
       * Method used to acquire a read lock.
       */
      void acquireReadAccess();

      /**
       * Method used to try to acquire a read lock.
       *
       * @return true if it succeed else false.
       */
      bool tryToAcquireReadAccess();

      /**
       * Method used to acquire a write lock.
       */
      void acquireWriteAccess();

      /**
       * Method used to try to acquire a write lock.
       *
       * @return true if it succeed else false.
       */
      bool tryToAcquireWriteAccess();

      /**
       * Method used to release a lock.
       */
      void release();
    private:
      pthread_rwlock_t lock;
    };

    /**
     * Class used to manage signal between threads.
     *
     */
    class Signal
    {
    public:

      class Configuration
      {
      public:
        /**
         * Constructor.
         *
         * @return
         */
        Configuration();

        /**
         * Destructor.
         *
         * @return
         */
        virtual ~Configuration();

        /**
         * Scope getter.
         *
         * @return the scope of the mutex.
         */
        Scope getScope();

        /**
         * Scope setter.
         *
         * @param scope : the new scope of mutex.
         */
        void setScope(Scope scope);

      private:
        pthread_condattr_t attributes;

        friend class Signal;
      };

      /**
       * Constructor.
       *
       * @return
       */
      Signal();

      /**
       * Constructor with specified config.
       *
       * @param config
       * @return
       */
      Signal(Configuration& config);

      /**
       * Constructor with specified lock config.
       *
       * @param lockConfig
       * @return
       */
      Signal(Lock::Configuration& lockConfig);

      /**
       * Constructor with config and lock config both specified.
       *
       * @param config
       * @param lockConfig
       * @return
       */
      Signal(Configuration& config, Lock::Configuration& lockConfig);

      /**
       * Destructor.
       *
       * @return
       */
      virtual ~Signal();

      /**
       * Send this signal in broadcast.
       */
      void broadcast();

      /**
       * Send this signal.
       */
      void send();

      /**
       * Wait for a signal to be emmited.
       */
      void wait();

      /**
       * Wait a signal for a given time.
       *
       * @param timeout : the max time to wait.
       * @return true if signal has been received before the timeout expired
       * else false
       */
      bool wait(Timeout& timeout);

    private:
      pthread_mutex_t lock;
      pthread_cond_t condition;
    };

    class Delegate
    {
    public:
      virtual void release(Thread *thread) = 0;
    private:

    };

    /**
     * Get current thread.
     *
     * @return
     */
    static Shared::Pointer<Thread>& self();

    /**
     * Add a child thread to this thread.
     * If this thread is currently running,
     * the new child is automatically launched.
     *
     * @param child : the new child to add.
     */
    static void addChild(Shared::Pointer<Thread>& child);

    /**
     * Remove a child thread from this thread.
     * If the child thread is running,
     * it is stopped before the removal takes place.
     *
     * @param child : the child to remove.
     */
    static void removeChild(Shared::Pointer<Thread>& child);

    /**
     * Function used to cancel all running children.
     */
    static void cancelChildren();

    /**
     * Remove a child from the thread children.
     * The child continue to run but is no longer attached to his parent.
     *
     * @param child : the child to detach.
     */
    static void detach(Shared::Pointer<Thread>& child);

    /**
     * Exit from the running thread.
     *
     * @param ret
     */
    static void exit(long ret);

    /**
     *
     * @param child
     * @return
     */
    static long join(Shared::Pointer<Thread>& child);

    /**
     * Send a signal to this thread.
     * This thread must be one of your direct child.
     *
     * @param sig
     */
    void kill(long sig);

    /**
     * Set a new delegate to handle thread end.
     *
     * @param delegate : the new delegate.
     */
    void setDelegate(Delegate *delegate);

    /**
     * Execute this thread inside the current one.
     * Used to start a process execution.
     *
     * @return
     */
    long start();

  protected:
    /**
     * Run function called by startCallback.
     * Represent the 'int main (void)' function of the thread.
     * Must be implemented by class derived from thread.
     *
     * @return
     */
    virtual int run();

    /**
     *
     * @param child
     */
    virtual void cancel();

    /**
     * Called by the child thread when it has started.
     *
     * @param child
     */
    virtual void started(Thread *child);

    /**
     * Called by the child thread when it has stopped.
     *
     * @param child
     */
    virtual void stopped(Thread *child);

    /**
     * Init method called before run method is called.
     * Any initialization of the thread can takes place here.
     */
    virtual int init();

    /**
     * Cleanup function called when a thread stops
     * (when run function returns or when cancel or exit is called).
     */
    virtual void cleanup();

    /**
     * Function used to check children status and launch the appropriate actions.
     *
     * STATUS_IDLE : the child is launched.
     * STATUS_STARTING : nothing is done.
     * STATUS_RUNNING : nothing is done.
     * STATUS_STOPPING : nothing is done.
     * STATUS_STOPPED : the child is joined, cleanup and released,
     *  the child entry is removed from the children vector.
     *
     */
    void checkChildren();

  private:

    /**
     * Constructor with default configuration.
     *
     * @return
     */
    Thread();

    /**
     *
     * @param config
     * @return
     */
    Thread(Configuration& config);

    /**
     * Constructor with given delegate.
     *
     * @param delegate
     * @return
     */
    Thread(Delegate *delegate);

    /**
     *
     * @param config
     * @param delegate
     * @return
     */
    Thread(Configuration& config, Delegate *delegate);

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~Thread();

    /**
     * Define the thread status.
     */
    typedef enum
    {
      STATUS_IDLE = 0,    //!< STATUS_IDLE
      STATUS_STARTING = 1,    //!< STATUS_STARTING
      STATUS_RUNNING = 2, //!< STATUS_RUNNING
      STATUS_STOPPING = 3, //!< STATUS_STOPPING
      STATUS_STOPPED = 4, //!< STATUS_STOPPED
    } Status;

    typedef std::vector<Shared::Pointer<Thread> > ThreadVector;
    typedef ThreadVector::iterator ThreadIterator;

    static Key<Thread> key;

    pthread_t id;
    Configuration config;
    Status status;

    ThreadVector children;
    Lock lock;

    Shared::Pointer<Thread> parent;

    Delegate *delegate;

    static void cleanupCallback(void *arg);
    static void stoppedCallback(void *arg);
    static void *startCallback(void *arg);

    template<typename T> friend class Shared::Pointer;

  };

}

#endif /* OVERKIZ_THREAD_H_ */
