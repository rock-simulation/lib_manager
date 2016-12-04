#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "LibManager.hpp"

TEST_CASE("findLibrary", "[libmanager]")
{
	lib_manager::LibManager libman;
	std::string libName = libman.findLibrary("abcd");
	REQUIRE(std::string("lib").compare(0, 3, libName));
	REQUIRE(std::string("abcd").compare(3, 7, libName));
}

TEST_CASE("loadLibrary", "[libmanager]")
{
	lib_manager::LibManager libman;
	lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
	REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);
}