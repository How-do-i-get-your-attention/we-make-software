#define StartDLL
#define InternationaOrganizationStandardizationlApplicationProgrammingInterface
#include ".h"


std::vector<International::Organization::Standardization::Language::Data>::iterator International::Organization::Standardization::Language::Get(std::wstring Alpha) {
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
std::vector<International::Organization::Standardization::Server::Data>::iterator International::Organization::Standardization::Server::Get(std::wstring Name) {
	for (auto it = International::Organization::Standardization::Servers.begin(); it != International::Organization::Standardization::Servers.end(); ++it) {
		if (it->Name == Name)
			return it;
	}
	return International::Organization::Standardization::Servers.end();
}
void Main() {
	using namespace International::Organization::Standardization;
	CountriesOrCountryAndStates.clear();
	Companies.clear();
	Regulations.clear();
	Languages.clear();
	Servers.clear();
	Servers = {
		{L"b873ee7",{212,227,201,38},{32, 1, 8, 216, 24, 1, 128, 56, 0, 0, 0, 0, 0, 0, 0, 1}}
	};
	Companies = {
		{ L"123456789", L"dk.how-do-i-get-your-attention.com", L"Address A",{ Server::Get(L"b873ee7")} },
	};
	Languages = {
		{ L"da" },
		{ L"en" }
	};
	Regulations = {
		{L"Danish legislation"},
		{L"General Data Protection Regulation"}
	};
	CountriesOrCountryAndStates = {
		{L"dk",Company::Get(L"dk.how-do-i-get-your-attention.com"), Language::Get(L"da") , {Regulation::Get(L"Danish legislation"), Regulation::Get(L"General Data Protection Regulation")}}
	};
}