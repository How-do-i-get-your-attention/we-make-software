#pragma once
#include "../Network/.h"
#ifdef StoargeApplicationProgrammingInterface
#define StoargeApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "Storage.lib")
#define StoargeApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace Storage {
#ifdef  PrivateStoargeApplicationProgrammingInterface
	StoargeApplicationProgrammingInterface void Mount();
#endif 
	namespace Sector {
		StoargeApplicationProgrammingInterface std::vector<unsigned char> New();
		StoargeApplicationProgrammingInterface bool Delete(std::vector<unsigned char>);
		StoargeApplicationProgrammingInterface bool Write(std::vector<unsigned char> sector, std::vector<unsigned char> data);
		StoargeApplicationProgrammingInterface  std::vector<unsigned char> Read(std::vector<unsigned char> sector);
	}
}