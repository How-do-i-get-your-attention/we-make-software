#pragma once
#include "../Network/.h"
#ifdef StoargeApplicationProgrammingInterface
#define StoargeApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "Storage.lib")
#define StoargeApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace Storage {
	StoargeApplicationProgrammingInterface void Mount();
	namespace Sector {
		StoargeApplicationProgrammingInterface std::vector<unsigned char> Get();
		StoargeApplicationProgrammingInterface void Delete(std::vector<unsigned char>);
	}
}