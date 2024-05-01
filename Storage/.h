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
	StoargeApplicationProgrammingInterface std::vector<std::tuple<uint8_t, HANDLE>> Files;
	StoargeApplicationProgrammingInterface std::vector<unsigned char> Read(uint8_t ID, uint64_t Sector);
	StoargeApplicationProgrammingInterface bool Write(uint8_t ID, uint64_t Sector, std::vector<unsigned char> buffer);
	StoargeApplicationProgrammingInterface std::tuple<int, int> Size(uint8_t ID);
	//StoargeApplicationProgrammingInterface void Transfer(uint8_t ID, std::string ServerName, uint8_t ServerStorageID);
	struct StoargeApplicationProgrammingInterface Master {


	};
	namespace Sector {
		 
	}
}