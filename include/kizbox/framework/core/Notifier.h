/*
 * Notifier.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_NOTIFIER_H_
#define OVERKIZ_NOTIFIER_H_

#include <list>

namespace Overkiz
{

  /*!
   * \class Notifier Notifier.h kizbox/framework/core/Notifier.h
   * \brief Class implementing a notifier
   *
   */
  template<class Notification>
  class Notifier
  {
  public:

    /*!
     * \brief Virtual destructor
     *
     */
    virtual ~Notifier()
    {
      notifiedObjects.clear();
    }

    /*!
     * \brief Add to notified object list
     *
     * \param [in] notifiedObject The notified object to add
     */
    void add(Notification * notifiedObject)
    {
      printf("%s:%i\n", __FUNCTION__, __LINE__);
      notifiedObjects.push_back(notifiedObject);
      notifiedObjects.unique();
    }

    /*!
     * \brief Remove from notified object list
     *
     * \param [in] notifiedObject The notified object to remove
     */
    void remove(Notification * notifiedObject)
    {
      printf("%s:%i\n", __FUNCTION__, __LINE__);
      notifiedObjects.remove(notifiedObject);
    }

  protected:

    std::list<Notification *> notifiedObjects; //!< The list of object to notify

  };

}

#endif /* OVERKIZ_NOTIFIER_H_ */
