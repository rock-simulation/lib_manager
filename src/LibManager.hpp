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
 * \file LibManager.hpp
 * \author Malte Langosz
 * \brief "LibManager" is the class which holds the pointers to the LibInterface
 *        of loaded classes.
 *
 */

#ifndef LIB_MANAGER_H
#define LIB_MANAGER_H

#ifdef _PRINT_HEADER_
#warning "LibManager.hpp"
#endif

#include "LibInterface.hpp"

#include <map>
#include <string>
#include <list>
#include <vector>
#include <class_loader/class_loader.h>

namespace lib_manager {

  struct libStruct {
    LibInterface *libInterface;
    boost::shared_ptr<LibInterface> shared_interface; 
    destroyLib *destroy;
    int useCount;
    bool wasUnloaded;
    std::string path;
  };

  struct LibInfo {
    std::string name;
    std::string path;
    int version;
    std::string src;
    std::string revision;
    int references;
  };

  class LibManager {

  public:
    enum ErrorNumber {
      LIBMGR_NO_ERROR             = 0,
      LIBMGR_ERR_NO_LIBRARY       = 1,
      LIBMGR_ERR_LIBNAME_EXISTS   = 2,
      LIBMGR_ERR_NOT_ABLE_TO_LOAD = 3,
      LIBMGR_ERR_LIB_IN_USE       = 4
    };

    std::string errMessage[5];

    LibManager();
    ~LibManager();
    
    static LibManager *getInstance();

    void addLibrary(LibInterface *_lib);
    ErrorNumber loadLibrary(const std::string &libPath,
                            void *config = NULL);

    LibInterface* acquireLibrary(const std::string &libName);
    template <typename T> T* acquireLibraryAs(const std::string &libName);
    LibInterface* getLibrary(const std::string &libName)
    { return acquireLibrary(libName); }
    template <typename T> T* getLibraryAs(const std::string &libName)
    { return acquireLibraryAs<T>(libName); }

    ErrorNumber releaseLibrary(const std::string &libName);

    ErrorNumber unloadLibrary(const std::string &libPath);
    void loadConfigFile(const std::string &config_file);
    void getAllLibraries(std::list<LibInterface*> *libList);
    void getAllLibraryNames(std::list<std::string> *libNameList) const;
    LibInfo getLibraryInfo(const std::string &libName) const;
    void dumpTo(const std::string &filename) const;
    void clearLibraries(void);

  private:
    /// The container in which information on all managed libraries is stored.
    std::map<std::string, libStruct*> libMap;
    static LibManager *instance;

    std::vector<class_loader::ClassLoader*> loaders;
        
  }; // class LibManager

  // template implementations
  template <typename T>
  T* LibManager::acquireLibraryAs(const std::string &libName) {
    T *lib = NULL;
    LibInterface *libInterface = acquireLibrary(libName);
    if(libInterface){
      lib = dynamic_cast<T*>(libInterface);
      if(!lib) {
        std::cerr << "!!!!!!!---- Got library " << libName  << "But cast failed" << std::endl;
        releaseLibrary(libName);
      }
    }else{
        std::cerr << "!!!!!!!---- Got NO library for " << libName  << std::endl;
    
    }
    return lib;
  }

} // namespace lib_manager

#endif /* LIB_MANAGER_H */
