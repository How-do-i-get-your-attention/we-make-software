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
	StoargeApplicationProgrammingInterface void Unmount();
}