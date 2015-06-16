/*
 * PluginLoader.h
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#ifndef OVERKIZ_PLUGINLOADER_H_
#define OVERKIZ_PLUGINLOADER_H_

#include <kizbox/framework/core/PluginLoaderInterface.h>
#include <kizbox/framework/core/Exception.h>

namespace Overkiz
{

  /**
   * Implementation of PluginLoaderInterface using the dlopen/dlsym/dlclose system.
   *
   * @see PluginLoaderInterface
   */
  class PluginLoader: public PluginLoaderInterface
  {
  public:

    class Exception: public Overkiz::Exception
    {
    public:

      Exception(const std::string & path, const std::string & name,
                const char * message = NULL) :
        path(path), name(name)
      {
        if(message)
        {
          this->message = message;
        }
      }

      virtual ~Exception()
      {
      }

      const std::string & getPath() const
      {
        return path;
      }

      const std::string & getName() const
      {
        return name;
      }

      const char * getMessage() const
      {
        return message.c_str();
      }

      virtual const char *getId() const
      {
        return "com.overkiz.Framework.Core.PluginLoader.Error";
      }

    private:

      std::string path;
      std::string name;
      std::string message;

    };

    class OpenException: public Exception
    {
    public:

      OpenException(const std::string & path, const std::string & name,
                    const char * message = NULL) :
        Exception(path, name, message)
      {
      }

      virtual ~OpenException()
      {
      }

      virtual const char *getId() const
      {
        return "com.overkiz.Framework.Core.PluginLoader.OpenError";
      }

    };

    class CloseException: public Exception
    {
    public:

      CloseException(const std::string & path, const std::string & name,
                     const char * message = NULL) :
        Exception(path, name, message)
      {
      }

      virtual ~CloseException()
      {
      }

      virtual const char *getId() const
      {
        return "com.overkiz.Framework.Core.PluginLoader.CloseError";
      }

    };

    class SymbolException: public Exception
    {
    public:

      SymbolException(const std::string & path, const std::string & name,
                      const char * message = NULL) :
        Exception(path, name, message)
      {
      }

      virtual ~SymbolException()
      {
      }

      virtual const char *getId() const
      {
        return "com.overkiz.Framework.Core.PluginLoader.SymbolError";
      }

    };

    /**
     * Constructor.
     *
     * @return
     */
    PluginLoader();

    /**
     * Destructor.
     *
     * @return
     */
    virtual ~PluginLoader();

    /**
     * This method loads a Plugin.
     *
     * @param pluginPath : path to the Plugin File (*.so)
     * @param pluginName : name of the Plugin (used to find the Plugin Entry Point)
     * @return The Plugin the loaded plugin.
     */
    PluginInterface *load(const std::string& pluginPath,
                          const std::string& pluginName);

    /**
     * This method unloads a Plugin.
     *
     * @param plugin : the Plugin to unload.
     */
    void unload(PluginInterface * plugin);

  };

}

#endif /* OVERKIZ_PLUGINLOADER_H_ */
