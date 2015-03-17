lib_manager {#lib_manager}
===========

## Overview

The lib\_manager library provides functionality to dynamically manage libraries in MARS. Note that  plugins in MARS have to be loaded and are handled as libraries.

To be used with a [LibManager](@ref lib_manager::LibManager), all libraries (and thus, plugins) have to implement the interface [LibInterface](@ref lib_manager::LibInterface) specified in this library.


## Loading Libraries

For the general use case you should only create one instance of the
`LibManager` within your project. All libraries that are loaded by this
instance will have access to it, to e. g. load further libraries.

    lib_manager::LibManager *theManager = new lib_manager::LibManager();
    theManager->loadConfigFile("path to config file");
    theManager->loadLibrary("cfg_manager");

The config file should simply include a list of libraries to be loaded. You
can either give the absolute or relative path of the file or specify only the
library name like "cfg_manager". If you only specify the name, the LibManager
will search for "libcfg_manager.dylib" (respectivly ".dll" or ".so") within the
"DYLD_LIBRARY_PATH" ("PATH" or "LD_LIBRARY_PATH"). The '#' character is used to
add comments to the config file. The same library definition (using the name or
the path) is used for the `loadLibrary()` method.


## Getting Pointer to Loaded Libraries

You can ask for an instance of a library by specifying the class and the
library name. If the `LibManager` finds the library name it returns a
dynamic_cast on the `LibInterface` pointer.

    cfg_manager::CFGManagerInterface *cfg;
    cfg = theManager->getLibraryAs<cfg_manager::CFGManagerInterface>("cfg_manager");
    if(!cfg) { /* no library "cfg_manager" found */ }

## Implementing a Library

## Links

[LibManager](@ref lib_manager::LibManager) <br/>
[LibInterface](@ref lib_manager::LibInterface) <br/>
[cfg_manager](@ref cfg_manager)