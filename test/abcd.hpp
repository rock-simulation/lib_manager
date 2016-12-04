#include "LibInterface.hpp"


class LibClassA : public lib_manager::LibInterface
{
public:
	LibClassA(lib_manager::LibManager* theManager);
	virtual ~LibClassA();
    virtual int getLibVersion() const;
    virtual const std::string getLibName() const;
    virtual void createModuleInfo();
protected:
	std::string libName;
	int libVersion;
};