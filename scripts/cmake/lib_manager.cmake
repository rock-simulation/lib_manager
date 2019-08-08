cmake_policy(PUSH)
cmake_minimum_required(VERSION 2.6)

function(define_module_info)
  execute_process(COMMAND git rev-parse
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE under_git_control
    ERROR_QUIET)
  if(NOT under_git_control)
    execute_process(COMMAND git rev-parse HEAD 
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE git_hash
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND git diff-index --quiet HEAD 
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      RESULT_VARIABLE git_has_local_changes
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND git remote -v
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE git_src
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "	(([^ ]|[:/])*) \\(fetch\\).*" "\\1" git_src ${git_src})
    if(git_has_local_changes)
      #message(WARNING "git repository has local uncommitted changes!")
      set(git_revision "${git_hash}_localChanges")
    else(git_has_local_changes)
      set(git_revision ${git_hash})
    endif(git_has_local_changes)
    if(WIN32)
      add_definitions("-DGIT_INFO" "-DGIT_INFO_REV=${git_revision}" "-DGIT_INFO_SRC=\"'${git_src}'\"")
    else(WIN32)
      add_definitions("-DGIT_INFO" "-DGIT_INFO_REV=${git_revision}" "-DGIT_INFO_SRC=\"${git_src}\"")
    endif(WIN32)
  endif(NOT under_git_control)
endfunction(define_module_info)


macro(mars_defaults)
  message("*------------* The use of the mars_defaults() macro is deprecated! Use lib_defaults() instead.")
  lib_defaults()
endmacro(mars_defaults)

macro(lib_defaults)
  if(WIN32)
    # this fixes the error 998 from the LibManager
    set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-auto-import")
    set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--enable-auto-import")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
  else(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif(WIN32)
  
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  endif()

endmacro(lib_defaults)


macro(setup_qt)
  # Widgets finds its own dependencies (QtGui and QtCore).

  if($ENV{USE_QT5})
    set(QT_OPTION OFF)
  else()
    set(QT_OPTION ON)
  endif()

  option(PREFERE_QT4 "set to OFF to allow build against QT5" ${QT_OPTION})
  if (PREFERE_QT4)
    find_package(Qt4)
    if (Qt4_FOUND)
       set(USE_QT4 1)
    else ()
      find_package(Qt5Widgets REQUIRED)
    endif()
  else()
    find_package(Qt5Widgets)
  endif()

  if (Qt5Widgets_FOUND)
    set(USE_QT5 1)

    # The Qt5Widgets_INCLUDES also includes the include directories for
    # dependencies QtCore and QtGui
    include_directories(${Qt5Widgets_INCLUDES})
    # We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
    add_definitions(${Qt5Widgets_DEFINITIONS})

    # Executables fail to build with Qt 5 in the default configuration
    # without -fPIE. We add that here.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    add_definitions("-DUSE_QT5")
  else ()
    if (Qt4_NOTFOUND)
      find_package(Qt4 REQUIRED)
    endif()
    include(${QT_USE_FILE})
    include_directories(${QT_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})
    link_directories(${QT_LIBRARY_DIR})
    add_definitions("-D_REENTRANT -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED")
    remove_definitions(-DQT_DLL)
  endif ()
endmacro(setup_qt)


cmake_policy(POP)

