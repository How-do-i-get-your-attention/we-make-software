#define StopDLL
#define StoargeApplicationProgrammingInterface
#include ".h"
#include <ntstatus.h>


std::vector<unsigned char> Storage::Read(uint8_t ID, uint64_t Sector) {
	if (Status != System::Started)
		return std::vector<unsigned char>();
	using namespace Storage;
	if (auto it = std::find_if(Files.begin(), Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != Files.end()) {
		LARGE_INTEGER fileSize;
		GetFileSizeEx(std::get<1>(*it), &fileSize);
		if (Sector * 512 > fileSize.QuadPart) {
			std::vector<unsigned char> buffer(512, 0);
			OVERLAPPED overlapped = { 0 };
			uint64_t offset = Sector * 512;
			overlapped.Offset = offset & 0xFFFFFFFF;
			overlapped.OffsetHigh = offset >> 32; 
			if (!ReadFile(std::get<1>(*it), buffer.data(), 512, nullptr, &overlapped)||!GetOverlappedResult(std::get<1>(*it), &overlapped, nullptr, TRUE))
				return std::vector<unsigned char>();
			return buffer;
		}
	}
	return std::vector<unsigned char>();
}
bool Storage::Write(uint8_t ID, uint64_t Sector, std::vector<unsigned char> buffer) {
	if (Status != System::Started)
		return false;
	using namespace Storage;
	if (auto it = std::find_if(Files.begin(), Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != Files.end()&&buffer.size()<=512) {
		if (buffer.size() < 512) 
			buffer.resize(512, 0);
		OVERLAPPED overlapped = { 0 };
		uint64_t offset = Sector * 512;
		overlapped.Offset = offset & 0xFFFFFFFF; // Lower 32 bits
		overlapped.OffsetHigh = offset >> 32; // Upper 32 bits
		if (!WriteFile(std::get<1>(*it), buffer.data(), 512, nullptr, &overlapped)||!GetOverlappedResult(std::get<1>(*it), &overlapped, nullptr, TRUE))
			return false;
		return true;
	}
	return false;
}
std::tuple<int,int> Storage::Size(uint8_t ID) {
	if (Status != System::Started)
		return std::make_tuple(0, 0);
	using namespace Storage;
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	if (auto it = std::find_if(Files.begin(), Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != Files.end()) {
		LARGE_INTEGER Used;
		GetFileSizeEx(std::get<1>(*it), &Used);
		for (auto& server : International::Organization::Standardization::Servers)
			if (server.Name == System::Name) 
				for (auto& storage : server.Storages)
					if (storage.ID == ID)
						return std::make_tuple(Used.QuadPart, storage.Space);
	}
	return std::make_tuple(0, 0);
}
void Exit() {
	using namespace Storage;
	for (auto& file : Files) 
		CloseHandle(std::get<1>(file));
	Files.clear();
}
void Storage::Mount() {
	for (auto& server : International::Organization::Standardization::Servers) 
		if (server.Name == System::Name) {
			for (auto& storage : server.Storages)
			{
				if (auto it = std::find_if(Files.begin(), Files.end(), [&storage](auto& file) {return std::get<0>(file) == storage.ID; }); it != Files.end()|| storage.Space> 17592185978880) 	continue;
				Files.push_back(std::make_tuple(storage.ID, CreateFile2(
					storage.Address.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					0,
					OPEN_ALWAYS,
					nullptr
				)));
			}
			break;
		}
}
