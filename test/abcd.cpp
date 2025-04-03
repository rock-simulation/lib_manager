#include "abcd.hpp"


LibClassA::LibClassA(lib_manager::LibManager* theManager) :
    lib_manager::LibInterface(theManager), libName("abcd"),
    libVersion(0)
{
}

LibClassA::~LibClassA()
{
}

int LibClassA::getLibVersion() const
{
    return libVersion;
}

const std::string LibClassA::getLibName() const
{
    return libName;
}

void LibClassA::createModuleInfo()
{
}

DESTROY_LIB(LibClassA);
CREATE_LIB(LibClassA);