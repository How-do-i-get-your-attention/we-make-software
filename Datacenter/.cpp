#define StartDLL
#define DatacenterApplicationProgrammingInterface
#include ".h"


extern "C" _declspec(dllexport)void TCP(std::shared_ptr<SOCKET> ClientSocket , int Port) {

}
std::once_flag RunOnceMountFlag;
void RunOnceMount() {
	std::cout << "RunOnceMount" << std::endl;
}
void Datacenter::Mount() {


	std::cout << "Mount" << std::endl;
	std::call_once(RunOnceMountFlag, RunOnceMount);
}
void Main() {
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	Datacenter::Mount();
}
std::any Datacenter::Distribute(std::vector<International::Organization::Standardization::CountryOrCountryAndState::Data>::iterator, std::wstring, std::wstring) {

	return false;
}
std::any Datacenter::Distribute(std::vector<International::Organization::Standardization::Server::Data>::iterator, std::wstring, std::wstring) {

	return false;
}