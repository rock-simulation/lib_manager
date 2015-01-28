/*
 *  Copyright 2011 - 2015 DFKI GmbH Robotics Innovation Center
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
 * \file LibInterface.cpp
 * \author Malte Langosz
 * \brief "LibInterface" is an interface to dynamically load libraries
 *
 */

#include "LibManager.hpp"

#ifdef WIN32
#  include <windows.h>
#  define LibHandle HINSTANCE
#else
#  include <dlfcn.h>
#  define LibHandle void*
#endif

#include <cstdio>
#include <stdlib.h>

using namespace std;

namespace lib_manager {
   
LibManager *LibManager::instance=0;

LibManager* LibManager::getInstance(){
    if(!instance){
        new LibManager();
    }
    return instance;
}

#if 0
#if defined(_LIBCPP_VERSION)
  // clang's libc++
  static struct LibInfo stdlibInfo = { "libc++", "", _LIBCPP_VERSION,
                                       "", "", 0 };
#elif defined(__GLIBCPP__) || defined(__GLIBCXX__)
  // GNU libstdc++
  static struct LibInfo stdlibInfo = { "libstdc++", "",
                                       (__GNUC__*100+__GNUC_MINOR__),"","",0};
#else
#  warning Unknown standard C Library!
  static struct LibInfo stdlibInfo = { "unknown stdlib", "", 0, "", "", 0 };
#endif

  using namespace std;


  // forward declarations
  static LibHandle intern_loadLib(const string &libPath);
  template <typename T>
  static T getFunc(LibHandle libHandle, const string &name);

#endif

  LibManager::LibManager() {
      if(instance){
          std::cerr << "Warning new LibManager created without singleton access" << std::endl;
      }
      LibManager::instance = this;
#if 0
    errMessage[0] = "no error";
    errMessage[1] = "no library with given name loaded";
    errMessage[2] = "library name already exists";
    errMessage[3] = "not able to load library";  
    errMessage[4] = "library is still in use";  
#endif
  }

  LibManager::~LibManager() {
      std::cerr << "Delete libManager" << std::endl;
#if 0
    std::map<std::string, libStruct>::iterator it;
    clearLibraries();

    if(libMap.size() > 0) {
      for(it = libMap.begin(); it != libMap.end(); ++it) {
        fprintf(stderr, "LibManager: [%s] not deleted correctly! "
                "%d references remain.\n"
                "      NOTE: The semantics of the LibManager has changed. To correctly\n"
                "            dispose of a library acquired by a call to getLibrary(libName)\n"
                "            you should now call releaseLibrary(libName) instead\n"
                "            of unloadLibrary(libName).\n",
                it->first.c_str(), it->second.useCount);
      }
    } else {
      fprintf(stderr, "LibManager: successfully deleted all libraries!\n");
    }
    fprintf(stderr, "Delete lib_manager\n");
#endif
  }

  /**
   * Empties the libMap field and deletes all items (libStructs) from memory.
   */
  void LibManager::clearLibraries() {
#if 0
    std::map<std::string, libStruct>::iterator it;
    bool finished = false;

    while(!finished) {
      finished = true;
      for(it = libMap.begin(); it != libMap.end(); ++it) {
        if(it->second.useCount == 0) {
          fprintf(stderr, "LibManager: delete [%s] !\n", it->first.c_str() );
          if(it->second.destroy) {
            it->second.destroy(it->second.libInterface);
          }
          libMap.erase(it);
          finished = false;
          break;
        }
      }
    }
#endif
  }

  /**
   * Registers a new library. An associated libStruct is created and all other
   * registered libraries are informed about the new library by calling the
   * newLibLoaded() method of their interface.
   * 
   * @param _lib A pointer to any class implementing the LibInterface.
   */
  void LibManager::addLibrary(LibInterface *_lib) {
    if(!_lib) {
      return;
    }
    libStruct *newLib = new libStruct();
    std::string name = _lib->getLibName();
    loaders.push_back(new class_loader::ClassLoader(name,false)); 

    std::cout << "Has loaded: "  <<__FUNCTION__ << _lib->getLibName() << std::endl;
    
    newLib->destroy = 0;
    newLib->libInterface = _lib;
    newLib->useCount = 1;
    newLib->wasUnloaded = false;
    _lib->createModuleInfo();
    
    if(libMap.find(name) == libMap.end()) {
      std::cerr << "######### Adding: " << newLib->libInterface->getLibName() << " <-> " << name << std::endl;
      libMap[name] = newLib;
    }
    
/*
    if(libMap.find(name) == libMap.end()) {
      libMap[name] = newLib;

      // notify all Libs of newly loaded lib
      for(map<string, libStruct>::iterator it = libMap.begin();
          it != libMap.end(); ++it) {
        // not notify the new lib about itself
        if(it->first != name)
          it->second.libInterface->newLibLoaded(name);
      }
    }
    */
  }

  /**
   * Loads a library from a specified path from the hard drive. This function
   * can be used to load a library (or plugin) through the LibManager at runtime
   * rather than adding an already-existing instance with the addLibrary method.
   * @param libPath The path to the library.
   * @param config 
   * @return 
   */
  LibManager::ErrorNumber LibManager::loadLibrary(const std::string &libPath, 
                                                  void *config) {
    const char *prefix = "lib";
    const char *env2 = "MARS_LIBRARY_PATH";
#ifdef WIN32
    const char *suffix = ".dll";
    const char sep = ';';
    const char *env = "PATH";
#elif __APPLE__
    const char *suffix = ".dylib";
    const char sep = ':';
    const char *env = "DYLD_LIBRARY_PATH";
#else
    const char *suffix = ".so";
    const char sep = ':';
    const char *env = "LD_LIBRARY_PATH";
#endif
    fprintf(stderr, "lib_manager: load plugin: %s\n", libPath.c_str());

    //try to locate the library somewhere by checking at various path positions
    FILE *testFile = fopen(libPath.c_str(), "r");
    std::string filepath;
    if(!testFile) {
      filepath = prefix;
      filepath.append(libPath);
      filepath.append(suffix);
      char* lib_path = getenv(env);
      char* lib_path2 = getenv(env2);
      if(lib_path || lib_path2) {
        // try to first find library
        std::string lib_path_s(lib_path);
        if(lib_path2) {
          lib_path_s.append(":");
          lib_path_s.append(lib_path2);
        }
        size_t next_path_pos = 0;
        size_t actual_path_pos = 0;
        while(next_path_pos != std::string::npos) {
          next_path_pos = lib_path_s.find(sep, actual_path_pos);
          std::string actual_path = lib_path_s.substr(actual_path_pos, (next_path_pos != std::string::npos) ? next_path_pos - actual_path_pos : lib_path_s.size() - actual_path_pos);
          actual_path.append("/");
          std::string actual_lib_path = actual_path;
          actual_lib_path.append(filepath);
          testFile = fopen(actual_lib_path.c_str(), "r");
          if(testFile) {
            fclose(testFile);
            filepath = actual_lib_path;
            fprintf(stderr, "lib_manager: found plugin at: %s\n",
                    filepath.c_str());
            break;
          }
          actual_path_pos = next_path_pos + 1;
        }
      }
    }
    else {
      filepath = libPath;
      fclose(testFile);
    }


    printf("Loading lib: %s\n",filepath.c_str());
    class_loader::ClassLoader *loader = new class_loader::ClassLoader(filepath,false);
    loaders.push_back(loader); 


    //LibHandle pl = intern_loadLib(filepath);
#if 0
    if(pl) {
      newLib.destroy = getFunc<destroyLib*>(pl, "destroy_c");
      if(newLib.destroy) {
        if(!config) {
          createLib *tmp_con = getFunc<createLib*>(pl, "create_c");
          if(tmp_con)
            newLib.libInterface = tmp_con(this);
        } else {
          createLib2 *tmp_con2 = getFunc<createLib2*>(pl, "config_create_c");
          if(tmp_con2)
            newLib.libInterface = tmp_con2(this, config);
        }
      }
    }
#endif
    loader->loadLibrary();
    std::vector<std::string> lib_names = loader->getAvailableClasses<lib_manager::LibInterface>();
    std::cerr << "Availible classes (" << lib_names.size() << "):" << std::endl;
    for(int i=0;i<lib_names.size();i++){
        std::cerr << "-- " <<lib_names[i] << std::endl;
    
    libStruct *newLib = new libStruct();
    newLib->destroy = 0;
    newLib->libInterface = 0;
    newLib->useCount = 0;
    newLib->wasUnloaded = false;
    newLib->path = filepath;

    
    //newLib.libInterface = loader->createInstance<lib_manager::LibInterface>(lib_names[0]).get();  
    newLib->shared_interface = loader->createInstance<lib_manager::LibInterface>(lib_names[i]);  
    newLib->libInterface = newLib->shared_interface.get();
//    newLib.shared_interface->setLibManager(this);

    if(!newLib->libInterface){
      return LIBMGR_ERR_NOT_ABLE_TO_LOAD;
    }
    /*
    if(lib_names.size() != 1){
        std::cerr << "Unable to load lib, its ambigious or nonexistend" << std::endl;
      return LIBMGR_ERR_NOT_ABLE_TO_LOAD;
    }
    */
    std::string name = newLib->libInterface->getLibName();
/*
    if(libMap.find(name) != libMap.end()) {
      newLib.destroy(newLib.libInterface);
      return LIBMGR_ERR_LIBNAME_EXISTS;
    }
    
*/
    std::cerr << "#########(2) Adding: " << newLib->libInterface->getLibName() << " <-> " << name << std::endl;
    libMap[name] = newLib;
#if 1
    // notify all Libs of newly loaded lib
    for(map<string, libStruct*>::iterator it = libMap.begin();
        it != libMap.end(); ++it) {
      // not notify the new lib about itself
      if(it->first != name)
        it->second->libInterface->newLibLoaded(name);
    }
#endif
    }
    std::cerr << "Ende: " << __FUNCTION__ << std::endl; 
    return LIBMGR_NO_ERROR;
  }

  /**
   * Returns a pointer to the libStruct associated with the requested library.
   * @param libName The name of the requested library.
   * @return A libStruct* if library is registered, otherwise 0.
   */
  LibInterface* LibManager::acquireLibrary(const string &libName) {
      std::cerr << __FUNCTION__ << std::endl;
    if(libMap.find(libName) == libMap.end()) {
      fprintf(stderr, "LibManager: could not find \"%s\"\n", libName.c_str());
      return 0;
    }
    libStruct *theLib = (libMap[libName]);
    if(!theLib->libInterface){
        fprintf(stderr, "LibManager: could not get instance for \"%s\"\n", libName.c_str());
        return 0;
    }
    if(theLib->libInterface->getLibName() != libName){
        std::cerr << "Got library with name " <<  theLib->libInterface->getLibName() <<" for " << libName << std::endl;
        exit(-1);
    }
    theLib->useCount++;
    return theLib->libInterface;
  }

  /**
   * Releases a previously acquired library
   * @param libName
   * @return 
   */
  LibManager::ErrorNumber LibManager::releaseLibrary(const string &libName) {
      std::cerr << __FUNCTION__ << std::endl;
#if 0
    if(libMap.find(libName) == libMap.end()) {
      return LIBMGR_ERR_NO_LIBRARY;
    }
    libStruct *theLib = &(libMap[libName]);
    theLib->useCount--;
    if(theLib->useCount <= 0 && theLib->wasUnloaded) {
      unloadLibrary(libName);
    }
#endif
    return LIBMGR_NO_ERROR;
  }

  /**
   * This method is not to be directly called. Use releaseLibrary() instead.
   */
  LibManager::ErrorNumber LibManager::unloadLibrary(const string &libName) {
      std::cerr << __FUNCTION__ << std::endl;
#if 0
    if(libMap.find(libName) == libMap.end()) {
      return LIBMGR_ERR_NO_LIBRARY;
    }

    libStruct *theLib = &(libMap[libName]);
    theLib->wasUnloaded = true;
    if(theLib->useCount <= 0) {
      fprintf(stderr, "LibManager: unload delete [%s]\n", libName.c_str());
      if(theLib->destroy) {
        theLib->destroy(theLib->libInterface);
      }
      libMap.erase(libName);
      return LIBMGR_NO_ERROR;
    }
#endif
    return LIBMGR_ERR_LIB_IN_USE;
  }

  /**
   * Loads all libraries specified in a configuration file. The configuration file
   * has to be a text file that contains one library path per line; each can have
   * a maximum length of 255 characters.
   * @param config_file
   */
  void LibManager::loadConfigFile(const std::string &config_file) {
      std::cerr << __FUNCTION__ << std::endl;
    char plugin_chars[255];
    std::string plugin_path;
    FILE *plugin_config;

    plugin_config = fopen(config_file.c_str() , "r");
    if(!plugin_config) {
      fprintf(stderr, "LibManager::loadConfigFile: file \"%s\" not found.\n",
              config_file.c_str());
      return;
    }

    //check every input line for a library to load and if one is found, try to load it
    while(fgets(plugin_chars, 255, plugin_config)) {
      plugin_path = plugin_chars;
      // strip whitespaces from start and end of line
      size_t pos1 = plugin_path.find_first_not_of(" \t\n\r");
      size_t pos2 = plugin_path.find_last_not_of(" \t\n\r");
      if(pos1 == string::npos || pos2 == string::npos) {
        continue;
      }
      plugin_path = plugin_path.substr(pos1, pos2 - pos1 + 1);
      // ignore lines that start with #
      if(plugin_path[0] != '#') {
        loadLibrary(plugin_path);
      }
    }
    fclose(plugin_config);
  }

  /**
   * Accepts a pointer to a list of type LibInterface and appends pointers to 
   * all registered libraries.
   * @param libList
   */
  void LibManager::getAllLibraries(std::list<LibInterface*> *libList) {
      std::cerr << __FUNCTION__ << std::endl;
    std::map<std::string, libStruct*>::iterator it;
    for(it = libMap.begin(); it != libMap.end(); ++it) {
      it->second->useCount++;
      libList->push_back(it->second->libInterface);
    }
  }
    
  /**
   * Accepts a pointer to a string list and appends the names of the registered libraries.
   * @param libNameList A pointer to a list of strings.
   */
  void LibManager::getAllLibraryNames(std::list<std::string> *libNameList) const {
      std::cerr << __FUNCTION__ << std::endl;
    std::map<std::string, libStruct*>::const_iterator it;
    for(it = libMap.begin(); it != libMap.end(); ++it) {
      libNameList->push_back(it->first);
    }
  }

  /**
   * Accepts a constant string and returns a LibInfo struct.
   * @param libName
   * @return 
   */
  LibInfo LibManager::getLibraryInfo(const std::string &libName) const {
      std::cerr << __FUNCTION__ << std::endl;
    LibInfo info;
    std::map<std::string, libStruct*>::const_iterator it;
    it = libMap.find(libName);
    if(it != libMap.end()) {
      ModuleInfo modInfo = it->second->libInterface->getModuleInfo();
      info.name = libName;
      info.path = it->second->path;
      info.version = it->second->libInterface->getLibVersion();
      info.src = modInfo.src;
      info.revision = modInfo.revision;
      info.references = it->second->useCount;
    }
    return info;
  }

  /**
   * Writes the names, source and revision of all registered libraries to the specified file.
   * @param filepath The path of the file where the names should be dumped.
   */
  void LibManager::dumpTo(const std::string &filepath) const {
      std::cerr << __FUNCTION__ << std::endl;
#if 0
    std::list<std::string> libNames;
    std::list<std::string>::const_iterator libNamesIt;
    getAllLibraryNames(&libNames);
    FILE *file = fopen(filepath.c_str(), "w");

    fprintf(file, "  <modules>\n");
    for(libNamesIt = libNames.begin();
        libNamesIt != libNames.end(); ++libNamesIt) {
      lib_manager::LibInfo info = getLibraryInfo(*libNamesIt);

      fprintf(file,
              "    <module>\n"
              "      <name>%s</name>\n"
              "      <src>%s</src>\n"
              "      <revision>%s</revision>\n"
              "    </module>\n",
              info.name.c_str(), info.src.c_str(), info.revision.c_str());
    }
    fprintf(file,
            "    <module>\n"
            "      <name>%s</name>\n"
            "      <src>%s</src>\n"
            "      <version>%d</version>\n"
            "      <revision>%s</revision>\n"
            "    </module>\n",
            stdlibInfo.name.c_str(), stdlibInfo.src.c_str(),
            stdlibInfo.version, stdlibInfo.revision.c_str());
    fprintf(file, "  </modules>\n");
    fclose(file);
#endif
  }

  ////////////////////
  // Helper Functions
  ////////////////////

  static std::string getErrorStr() {
      std::cerr << __FUNCTION__ << std::endl;
#if 0
    string errorMsg;
#ifdef WIN32
    LPTSTR lpErrorText = NULL;
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_ALLOCATE_BUFFER,
                    0, GetLastError(), 0,
                    (LPTSTR)&lpErrorText, MAX_PATH, 0);
    errorMsg = lpErrorText;
    ::LocalFree(lpErrorText);
#else
    errorMsg = dlerror();
#endif
    return errorMsg;
#endif
  }

  static LibHandle intern_loadLib(const std::string &libPath) {
      std::cerr << __FUNCTION__ << std::endl;
#if 0
    LibHandle libHandle;
#ifdef WIN32
    libHandle = LoadLibrary(libPath.c_str());
#else
    libHandle = dlopen(libPath.c_str(), RTLD_LAZY);
#endif
    if(!libHandle) {
      string errorMsg = getErrorStr();
      fprintf(stderr, "ERROR: lib_manager cannot load library:\n       %s\n",
              errorMsg.c_str());
    }
    return libHandle;
#endif
  }

  template <typename T>
  static T getFunc(LibHandle libHandle, const std::string &name) {
      std::cerr << __FUNCTION__ << std::endl;
#if 0
    T func = NULL;
#ifdef WIN32
    func = reinterpret_cast<T>(GetProcAddress(libHandle, name.c_str()));
#else
    func = reinterpret_cast<T>(dlsym(libHandle, name.c_str()));
#endif
    if(!func) {
      string err = getErrorStr();
      fprintf(stderr, 
              "ERROR: lib_manager cannot load library symbol \"%s\"\n"
              "       %s\n", name.c_str(), err.c_str());
    }
    return func;
#endif
  }

} // end of namespace lib_manager
