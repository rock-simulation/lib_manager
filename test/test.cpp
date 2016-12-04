#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "LibManager.hpp"


TEST_CASE("existing library", "[loadLibrary]")
{
    lib_manager::LibManager libman;
    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);
}

TEST_CASE("unknown library", "[loadLibrary]")
{
    lib_manager::LibManager libman;
    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("unknown");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_ERR_NOT_ABLE_TO_LOAD);
}

TEST_CASE("twice", "[loadLibrary]")
{
    lib_manager::LibManager libman;
    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);
    error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_ERR_LIBNAME_EXISTS);
}

TEST_CASE("check library name", "[findLibrary]")
{
    lib_manager::LibManager libman;
    std::string libName = libman.findLibrary("abcd");
    REQUIRE(std::string("lib").compare(0, 3, libName));
    REQUIRE(std::string("abcd").compare(3, 7, libName));
}