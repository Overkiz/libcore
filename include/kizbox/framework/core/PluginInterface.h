/*
 * PluginInterface.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_PLUGININTERFACE_H_
#define OVERKIZ_PLUGININTERFACE_H_

namespace Overkiz
{

  /**
   * Interface defining a Plugin Object.
   *
   */
  class PluginInterface
  {
  public:

    /**
     * Init function called by the PluginLoader when it loads a Plugin.
     * The Plugin Implementation must implement this method.
     *
     */
    virtual ~PluginInterface()
    {
    }

    /**
     * Init function called by the PluginLoader when it loads a Plugin.
     * The Plugin Implementation must implement this method.
     *
     */
    virtual void init() = 0;

    /**
     * Cleanup function called by the PluginLoader when it unloads a Plugin.
     * The Plugin Implementation must implement this method.
     *
     */
    virtual void cleanup() = 0;
  };

}

#endif /* OVERKIZ_PLUGININTERFACE_H_ */
