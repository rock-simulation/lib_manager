#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "LibManager.hpp"
#include "LibInterface.hpp"


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

TEST_CASE("get library that has not been loaded", "[getLibrary]")
{
    lib_manager::LibManager libman;

    lib_manager::LibInterface* lib = libman.getLibrary("abcd");
    REQUIRE(lib == NULL);
}

TEST_CASE("get existing library", "[getLibrary]")
{
    lib_manager::LibManager libman;

    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    lib_manager::LibInterface* lib = libman.getLibrary("abcd");
    REQUIRE(lib != NULL);
    REQUIRE(lib->getLibVersion() == 0);
    REQUIRE(lib->getLibName() == "abcd");

    error = libman.releaseLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);
}

TEST_CASE("get library info", "[getLibraryInfo]")
{
    lib_manager::LibManager libman;

    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    lib_manager::LibInfo info = libman.getLibraryInfo("abcd");
    REQUIRE(info.name == "abcd");
    REQUIRE(info.version == 0);
}

TEST_CASE("get once, release twice", "[releaseLibrary]")
{
    lib_manager::LibManager libman;

    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    lib_manager::LibInterface* lib = libman.getLibrary("abcd");

    error = libman.releaseLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    error = libman.releaseLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);
}

TEST_CASE("get twice, release twice", "[getLibrary]")
{
    lib_manager::LibManager libman;

    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    lib_manager::LibInterface* lib1 = libman.getLibrary("abcd");
    lib_manager::LibInterface* lib2 = libman.getLibrary("abcd");
    REQUIRE(lib1 == lib2);

    error = libman.releaseLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    error = libman.releaseLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);
}

TEST_CASE("release unknown library", "[releaseLibrary]")
{
    lib_manager::LibManager libman;

    lib_manager::LibManager::ErrorNumber error = libman.releaseLibrary("unknown");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_ERR_NO_LIBRARY);
}

TEST_CASE("get all names without loaded libraries", "[getAllLibraryNames]")
{
    lib_manager::LibManager libman;

    std::list<std::string> libNameList;
    libman.getAllLibraryNames(&libNameList);
    REQUIRE(libNameList.size() == 0);
}

TEST_CASE("get all names with one library loaded", "[getAllLibraryNames]")
{
    lib_manager::LibManager libman;

    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    std::list<std::string> libNameList;
    libman.getAllLibraryNames(&libNameList);
    REQUIRE(libNameList.size() == 1);
}

TEST_CASE("clear one library", "[clearLibraries]")
{
    lib_manager::LibManager libman;

    lib_manager::LibManager::ErrorNumber error = libman.loadLibrary("abcd");
    REQUIRE(error == lib_manager::LibManager::LIBMGR_NO_ERROR);

    libman.clearLibraries();

    std::list<std::string> libNameList;
    libman.getAllLibraryNames(&libNameList);
    REQUIRE(libNameList.size() == 0);
}