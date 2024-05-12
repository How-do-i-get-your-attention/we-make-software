#pragma once
#include "../System/.h"
#ifdef InternationaOrganizationStandardizationlApplicationProgrammingInterface
#define InternationaOrganizationStandardizationlApplicationProgrammingInterface _declspec(dllexport)  
#else
#pragma comment(lib, "International Organization Standardization.lib")
#define InternationaOrganizationStandardizationlApplicationProgrammingInterface  _declspec(dllimport)  
#endif
namespace  International{
	namespace  Organization {
		namespace Standardization
		{
			InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<std::string> Domains;
			namespace Server {
				namespace  Storage {
					struct Data
					{
						std::wstring Address;
						uint8_t ID;
						uint64_t Space;
					};
				}
				struct Data
				{
					std::string Name;
					std::vector<unsigned char> IntenetProtocolAddressVersion4, IntenetProtocolAddressVersion6;
					std::vector<Storage::Data> Storages;
				};
				InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<Server::Data>::iterator Get(std::string);
			}
			InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<Server::Data> Servers;
			namespace Company {
				struct Data
				{
					std::wstring RegisteringNumber,Name, Address;
					std::vector< std::vector<Server::Data>::iterator> Servers;
				};
				InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<Company::Data>::iterator Get(std::wstring);
			}
			InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<Company::Data> Companies;
			namespace Language {
				struct Data
				{
					std::string Alpha;
				};
				InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<International::Organization::Standardization::Language::Data>::iterator Get(std::string);
			}
			InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<Language::Data> Languages;
			namespace Regulation {
				struct Data
				{
					std::wstring Name;
					std::vector<std::tuple<std::wstring,std::wstring,std::any>> Laws;
				};
				InternationaOrganizationStandardizationlApplicationProgrammingInterface void Add(std::vector<Data>::iterator, std::wstring, std::wstring, std::any);
				InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<Data>::iterator Get (std::wstring);
			}
			InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<Regulation::Data> Regulations;
			namespace CountryOrCountryAndState {
				struct Data
				{
					std::string Alpha;
					std::vector<Company::Data>::iterator Company;
					std::vector<Language::Data>::iterator Language;
					std::vector<std::vector<Regulation::Data>::iterator> Regulations;
				};
				InternationaOrganizationStandardizationlApplicationProgrammingInterface std::any Get(std::vector<CountryOrCountryAndState::Data>::iterator, std::wstring, std::wstring,std::any);
			}
			InternationaOrganizationStandardizationlApplicationProgrammingInterface std::vector<CountryOrCountryAndState::Data> CountriesOrCountryAndStates;
		}
	}
}