/*
 * Subject.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_SUBJECT_H_
#define OVERKIZ_SUBJECT_H_

#include <map>
#include <list>
#include <stdio.h>

#include <kizbox/framework/core/Shared.h>

namespace Overkiz
{
  /**
   * Class implements Observer design pattern : an object, called the subject, maintains a list of its dependents, called observers,
   * and notifies them automatically of any state changes.
   */

  template<typename Key, typename Observer> class SubjectMap;

  /**
   * Note about templates:
   *  - Create a simple Subject: Subject<Observer>.
   *
   *  - Create a SubjectMap: SubjectMap<Key, Observer>
   *    - The related subject must be: Subject<Observer, Key>
   */

  template< typename Observer, typename Key = Observer>
  class Subject
  {

  public:

    /**
     * Constructor.
     *
     * @return
     */
    Subject()
    {
    }

    /**
     * Copy Constructor.
     *
     * @return
     */
    Subject(const Subject& obs)
    {
      observers = obs.observers;
    }

    /**
     * operator=.
     *
     * @return
     */
    Subject & operator= (const Subject& obs)
    {
      observers = obs.observers;
      return (*this);
    }

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~Subject()
    {
    }

    /**
     * Method used to register observers.
     *
     * @param time
     * @return
     */
    void add(const Shared::Pointer<Observer>& observer)
    {
      if(observer.empty())
        return;

      observers.push_back(observer);
    }

    /**
     * Method used to unregister observers.
     *
     * @param observer
     * @return
     */
    void remove(const Shared::Pointer<Observer>& observer)
    {
      if(observer.empty())
        return;

      for(auto it = observers.begin(); it != observers.end();)
      {
        if(observer == *it)
        {
          it = observers.erase(it);
        }
        else
        {
          it++;
        }
      }
    }

  protected:

    /**
     * Method used to notify all observers
     *
     * @param subject
     * @param params
     * @return
     */
    template< typename ...P >
    void notify(P&&... params)
    {
      for(auto it = this->observers.begin(); it != this->observers.end(); it++)
      {
        (*it)->notified(this, std::forward<P>(params)...);
      }
    }

  private:

    std::list< Shared::Pointer<Observer> > observers;

    friend class SubjectMap<Key, Observer>;
  };

  template<typename Key, typename Observer>
  class SubjectMap
  {
  public:

    /**
     * Constructor.
     *
     * @return
     */
    SubjectMap()
    {
    }

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~SubjectMap()
    {
    }

    /**
     * Method used to register observers.
     *
     * @param Key
     * @param Observer
     * @return
     */
    void add(Key key, const Shared::Pointer<Observer> & observer)
    {
      std::pair<Key, Shared::Pointer<Subject<Observer, Key> > > entry;
      entry.first  = key;
      entry.second = Shared::Pointer<Subject<Observer, Key> >::create();
      auto subject = _subjects.find(entry.first);

      if(subject == _subjects.end())
      {
        entry.second->add(observer);
        _subjects.insert(entry);
      }
      else
      {
        _subjects[entry.first]->add(observer);
      }
    }

    /**
     * Method used to unregister observers.
     *
     * @param Key
     * @param Observer
     * @return
     */
    void remove(uint64_t key, const Shared::Pointer<Observer> & observer)
    {
      auto subject = _subjects.find(key);

      if(subject != _subjects.end())
      {
        _subjects[key]->remove(observer);

        if(_subjects[key]->observers.empty())
        {
          _subjects.erase(subject);
        }
      }
    }

    /**
     * Method used to notify all observers
     *
     * @param Key
     * @param params
     * @return
     */
    template< typename ...P >
    void notify(Key key, P&&... params)
    {
      for(auto it = _subjects.begin(); it != this->_subjects.end(); ++it)
      {
        if(it->first == key)
        {
          _subjects[key]->notify(std::forward<P>(params)...);
        }
      }
    }

  private:
    std::map<Key, Shared::Pointer<Subject<Observer, Key> > > _subjects;
  };
}

#endif /* OVERKIZ_OBSERVABLE_H_ */
