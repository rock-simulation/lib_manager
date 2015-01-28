/*
 *  Copyright 2011 - 2015, DFKI GmbH Robotics Innovation Center
 *
 *  This file is part of the MARS simulation framework.
 *
 *  MARS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3
 *  of the License, or (at your option) any later version.
 *
 *  MARS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with MARS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file LibInterface.hpp
 * \author Malte Langosz
 * \brief "LibInterface" is an interface to dynamically load libraries
 *
 */

#ifndef LIB_INTERFACE_H
#define LIB_INTERFACE_H

#ifdef _PRINT_HEADER_
#warning "LibInterface.hpp"
#endif

#include <string>
#include <class_loader/class_loader.h>
/* We had to jump through some hoops to get the git version information
 * into the libInterface:
 * 1) If we set it directly in the constructor the linker seems to mess things
 *    up (has only a single code section of the ctor?).
 * 2) So we set the info in the create_c.
 * 3) create_c must therefore be a friend function of the LibInterface.
 * 4) create_c must therefore be forward declared.
 * 5) The LibInterface and LibManager must therefore be forward declared.
 * NOTE: because of step 2) the git info only becomes available after the
 *       classes constructor!
 */

namespace lib_manager {
  class LibInterface;
  class LibManager;
}

extern "C" lib_manager::LibInterface* create_c(lib_manager::LibManager *theManager);

#ifdef GIT_INFO
#define EXPAND_STRING(x) #x
#define MACRO_TO_STRING(x) EXPAND_STRING(x)
#define GIT_INFO_REV_STR MACRO_TO_STRING(GIT_INFO_REV)
#define GIT_INFO_SRC_STR GIT_INFO_SRC
#else
#define GIT_INFO_REV_STR "<no git info>"
#define GIT_INFO_SRC_STR "<unknown>"
#endif

#define DESTROY_LIB(theClass)                                 \
  extern "C" void *destroy_c(lib_manager::LibInterface *sp) { \
    delete (dynamic_cast<theClass*>(sp));                     \
    return 0;                                                 \
  }

#define CREATE_LIB(theClass)                                            \
    CLASS_LOADER_REGISTER_CLASS(theClass, lib_manager::LibInterface);      \
  extern "C" lib_manager::LibInterface* create_c(lib_manager::LibManager *theManager) { \
    theClass *instance = new theClass(theManager);                      \
    instance->createModuleInfo();                                       \
    return dynamic_cast<lib_manager::LibInterface*>(instance);          \
  }

#define CREATE_LIB_CONFIG(theClass, configType)                         \
  extern "C" lib_manager::LibInterface* config_create_c(lib_manager::LibManager *theManager, configType *config) { \
    configType *_config = dynamic_cast<configType*>(config);            \
    if(_config == NULL) return 0;                                       \
    theClass *instance = new theClass(theManager, _config);             \
    instance->createModuleInfo();                                       \
    return dynamic_cast<lib_manager::LibInterface*>(instance);          \
  }

#define CREATE_MODULE_INFO()                    \
  void createModuleInfo() {                     \
    moduleInfo.name = getLibName();             \
    moduleInfo.src = GIT_INFO_SRC_STR;          \
    moduleInfo.revision = GIT_INFO_REV_STR;     \
  }


namespace lib_manager {

  struct ModuleInfo {
    std::string name;
    std::string src;
    std::string revision;
  };

  /**
   * The interface to load libraries dynamically
   *
   */
  class LibInterface {
  public:
    LibInterface(LibManager *theManager=0);

    virtual ~LibInterface(void) {}
    virtual int getLibVersion() const = 0;
    virtual const std::string getLibName() const = 0;
    ModuleInfo getModuleInfo() const
    { return moduleInfo; }
    virtual void newLibLoaded(const std::string &libName) {}
    virtual void createModuleInfo(void) {}
    virtual void setLibManager(LibManager *manager){libManager = manager;};
  protected:
    LibManager *libManager;
    ModuleInfo moduleInfo;
  };

  typedef void *destroyLib(LibInterface *sp);
  typedef LibInterface* createLib(LibManager *theManager);
  typedef LibInterface* createLib2(LibManager *theManager, void *configuration);

} // end of namespace lib_manager


#endif  /* LIB_INTERFACE_H */
