#define StartDLL
#include ".h"

void Main() {
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	std::cout << "Datacenter OK" << std::endl;
}