#pragma once
#include "../Storage/.h"
#ifdef DatacenterApplicationProgrammingInterface
#define DatacenterApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "Datacenter.lib")
#define DatacenterApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace Datacenter {
	DatacenterApplicationProgrammingInterface void Mount();
	DatacenterApplicationProgrammingInterface std::vector<std::tuple<std::wstring, std::wstring, std::any,std::any>> Funtions;
	DatacenterApplicationProgrammingInterface std::any Distribute(std::vector<International::Organization::Standardization::CountryOrCountryAndState::Data>::iterator, std::wstring, std::wstring);
	DatacenterApplicationProgrammingInterface std::any Distribute(std::vector<International::Organization::Standardization::Server::Data>::iterator, std::wstring, std::wstring);
}