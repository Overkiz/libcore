/*
 * PluginLoaderInterface.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_PLUGINLOADERINTERFACE_H_
#define OVERKIZ_PLUGINLOADERINTERFACE_H_

#include <string>

#include <kizbox/framework/core/PluginInterface.h>

namespace Overkiz
{
  /**
   * Define the Interface of a PluginLoader.
   *
   */
  class PluginLoaderInterface
  {
  public:

    /**
     * Empty virtual destructor
     *
     */
    virtual ~PluginLoaderInterface()
    {
    }

    /**
     * Load a new Plugin.
     *
     * @param pluginPath : the path to the Plugin File
     * @param pluginName : the name of the Plugin
     * @return the loaded plugin.
     */
    virtual PluginInterface *load(const std::string& pluginPath,
                                  const std::string& pluginName) = 0;

    /**
     * Unload a Plugin
     *
     * @param plugin : the plugin to unload.
     */
    virtual void unload(PluginInterface * plugin) = 0;
  };

}

#endif /* OVERKIZ_PLUGINLOADERINTERFACE_H_ */
