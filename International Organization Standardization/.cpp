#define StartDLL
#define InternationaOrganizationStandardizationlApplicationProgrammingInterface
#include "../Storage/.h"


std::vector<International::Organization::Standardization::Language::Data>::iterator International::Organization::Standardization::Language::Get(std::string Alpha) {
	for (auto it = International::Organization::Standardization::Languages.begin(); it != International::Organization::Standardization::Languages.end(); ++it) {
		if (it->Alpha == Alpha)
			return it;
	}
	return International::Organization::Standardization::Languages.end();
}
std::vector<International::Organization::Standardization::Regulation::Data>::iterator International::Organization::Standardization::Regulation::Get(std::wstring Name) {
	for (auto it = International::Organization::Standardization::Regulations.begin(); it != International::Organization::Standardization::Regulations.end(); ++it) {
		if (it->Name == Name)
			return it;
	}
	return International::Organization::Standardization::Regulations.end();
}
void International::Organization::Standardization::Regulation::Add(std::vector<International::Organization::Standardization::Regulation::Data>::iterator it, std::wstring Name, std::wstring Law, std::any Value) {
	for (auto& law : it->Laws) {
		if (std::get<0>(law) == Name && std::get<1>(law) == Law) {
			std::get<2>(law) = Value;
			return;
		}
	}
	it->Laws.push_back(std::make_tuple(Name, Law, Value));
}
std::any International::Organization::Standardization::CountryOrCountryAndState::Get(std::vector<International::Organization::Standardization::CountryOrCountryAndState::Data>::iterator it, std::wstring Name, std::wstring Law, std::any standard) {
	for (auto& reglation : it->Regulations)
		for (auto& law : reglation->Laws)
			if (std::get<0>(law) == Name && std::get<1>(law) == Law)
				return std::get<2>(law);
	return standard;
}
std::vector<International::Organization::Standardization::Company::Data>::iterator International::Organization::Standardization::Company::Get(std::wstring Name) {
	for (auto it = International::Organization::Standardization::Companies.begin(); it != International::Organization::Standardization::Companies.end(); ++it) {
		if (it->Name == Name)
			return it;
	}
	return International::Organization::Standardization::Companies.end();
}
std::vector<International::Organization::Standardization::Server::Data>::iterator International::Organization::Standardization::Server::Get(std::string Name) {
	for (auto it = International::Organization::Standardization::Servers.begin(); it != International::Organization::Standardization::Servers.end(); ++it) {
		if (it->Name == Name)
			return it;
	}
	return International::Organization::Standardization::Servers.end();
}
void Main() {
	using namespace International::Organization::Standardization;
	// Server name always needs to be in lowercase
	// Server name can't be more than 255 in length
	// Max size of bin file is 17592185978880
	// Files should be indexed starting from 0
	Servers = {
		{ "b873ee7",{212,227,201,38},{32, 1, 8, 216, 24, 1, 128, 56, 0, 0, 0, 0, 0, 0, 0, 1},{
			{L"D:\\Quality Time.bin",0,3000000000000}
		}}
	};
	Companies = {
		{ L"123456789", L"dk.how-do-i-get-your-attention.com", L"Address A",{ Server::Get("b873ee7")} },
	};
	// Languages iso code always needs to be in lowercase
	Languages = {
		{ "da" },
		{ "en" }
	};
	Regulations = {
		{L"Danish legislation"},
		{L"General Data Protection Regulation"}
	};
	// Country Or Country And States iso code always needs to be in lowercase
	CountriesOrCountryAndStates = {
		{ "dk",Company::Get(L"dk.how-do-i-get-your-attention.com"), Language::Get("da") , {Regulation::Get(L"Danish legislation"), Regulation::Get(L"General Data Protection Regulation")}}
	};
	System::Libraries::WaitForModules({ L"Storage.dll" }, System::Started);
	Storage::Mount();
}
