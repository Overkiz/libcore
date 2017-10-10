/*
 * PluginLoader.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include <typeinfo>
#include <dlfcn.h>

#include <kizbox/framework/core/Plugin.h>
#include "PluginLoader.h"

namespace Overkiz
{

  PluginLoader::PluginLoader()
  {
  }

  PluginLoader::~PluginLoader()
  {
  }

  PluginInterface *PluginLoader::load(const std::string& pluginPath,
                                      const std::string& pluginName)
  {
    void *libHandler = dlopen(pluginPath.c_str(), RTLD_LAZY);

    if(libHandler == nullptr)
    {
      Overkiz::PluginLoader::OpenException e(pluginPath, pluginName,
                                             dlerror());
      throw e;
    }

    std::string getPluginMethodName;
    getPluginMethodName = "get" + pluginName + "Plugin";
    PluginGetter pluginGetter = (PluginGetter) dlsym(libHandler,
                                getPluginMethodName.c_str());

    if(pluginGetter == nullptr)
    {
      Overkiz::PluginLoader::SymbolException e(pluginPath, pluginName,
          dlerror());
      dlclose(libHandler);
      throw e;
    }

    Plugin *newPlugin = pluginGetter();
    newPlugin->libraryHandler = libHandler;

    try
    {
      newPlugin->init();
    }
    catch(...)
    {
      dlclose(libHandler);
    }

    return newPlugin;
  }

  void PluginLoader::unload(PluginInterface *plugin)
  {
    Plugin * oldPlugin = nullptr;

    try
    {
      oldPlugin = dynamic_cast<Plugin *>(plugin);
    }
    catch(const std::bad_cast & e)
    {
      return;
    }

    try
    {
      oldPlugin->cleanup();
      (void) dlclose(oldPlugin->libraryHandler);
    }
    catch(...)
    {
      (void) dlclose(oldPlugin->libraryHandler);
    }
  }

}
