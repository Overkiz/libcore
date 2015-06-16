/*
 * Plugin.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_PLUGIN_H_
#define OVERKIZ_PLUGIN_H_

#include <stdlib.h>

#include <kizbox/framework/core/PluginInterface.h>

namespace Overkiz
{

  /**
   * MACRO used to define the plugin entry point which is a function called
   * getXXPlugin (XX is replaced by the Plugin name) and returning a pointer
   * to a Plugin Object.
   *
   * This MACRO must be used in every plugin library.
   */

#define PLUGIN(__name)                \
  extern "C" Overkiz::Plugin *get##__name##Plugin (void)   \
  {                                     \
    return ((Overkiz::Plugin *)(&__name));       \
  }

  /**
   * @see PluginLoader
   */
  class PluginLoader;

  /**
   * Class used to define a Plugin.
   */
  class Plugin: public PluginInterface
  {
  public:
    /**
     * Constructor.
     *
     * @return
     */
    Plugin()
    {
      libraryHandler = NULL;
    }

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~Plugin()
    {
    }

    /**
     * Init function which is called when the plugin is loaded.
     * This implementation does nothing.
     * If your plugin needs some initiliazation you should overload this method.
     *
     */
    virtual void init()
    {
    }

    /**
     * Cleanup function which is called when the plugin is unloaded.
     * This implementation does nothing.
     * If your plugin needs some cleanup you should overload this method.
     *
     */
    virtual void cleanup()
    {
    }

  private:
    void *libraryHandler;

    friend class PluginLoader;

  };

  typedef Plugin * (*PluginGetter)(void);

}

#endif /* PLUGIN_H_ */
