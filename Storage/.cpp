#define StartDLL
#define StopDLL
#define StoargeApplicationProgrammingInterface
#define PrivateStoargeApplicationProgrammingInterface
#include ".h"
#include <ntstatus.h>
std::vector<std::tuple<uint8_t, HANDLE, std::unique_ptr<std::mutex>>> _Files;
std::vector<unsigned char>_Read(uint8_t ID, uint64_t Sector) {
	using namespace Storage;
	if (auto it = std::find_if(_Files.begin(), _Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != _Files.end()) {
		LARGE_INTEGER fileSize;
		GetFileSizeEx(std::get<1>(*it), &fileSize);
		if (Sector * 1024 < fileSize.QuadPart) {
			std::vector<unsigned char> buffer(1024, 0);
			OVERLAPPED overlapped = { 0 };
			uint64_t offset = Sector * 1024;
			overlapped.Offset = offset & 0xFFFFFFFF;
			overlapped.OffsetHigh = offset >> 32; 
			std::lock_guard<std::mutex> lock(*std::get<2>(*it));
			if (ReadFile(std::get<1>(*it), buffer.data(), 1024, nullptr, &overlapped))
				return buffer;
		}
	}
	return std::vector<unsigned char>();
}
bool _Write(uint8_t ID, uint64_t Sector, std::vector<unsigned char> buffer) {
	using namespace Storage;
	if (auto it = std::find_if(_Files.begin(), _Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != _Files.end() && buffer.size() == 1024) {
		OVERLAPPED overlapped = { 0 };
		uint64_t offset = Sector * 1024;
		overlapped.Offset = offset & 0xFFFFFFFF;
		overlapped.OffsetHigh = offset >> 32;
		std::lock_guard<std::mutex> lock(*std::get<2>(*it));
		if (WriteFile(std::get<1>(*it), buffer.data(), 1024, nullptr, &overlapped))
			return  FlushFileBuffers(std::get<1>(*it));
	}
	return false;
}
std::tuple<uint64_t, uint64_t> _Size(uint8_t ID) {
	using namespace Storage;
	if (auto it = std::find_if(_Files.begin(), _Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != _Files.end()) {
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
	System::Libraries::WaitForModules({ L"Firewall.dll" }, System::Stopped);
	using namespace Storage;
	for (auto& file : _Files)
		CloseHandle(std::get<1>(file));
	_Files.clear();
}
void Storage::Mount() {
	for (auto& server : International::Organization::Standardization::Servers) 
		if (server.Name == System::Name) {
			for (auto& storage : server.Storages)
			{
				if (storage.Space > 17592185978880)
					storage.Space = 17592185978880;
				if (auto it = std::find_if(_Files.begin(), _Files.end(), [&storage](auto& file) {return std::get<0>(file) == storage.ID; }); it != _Files.end()) {
					if (std::get<0>(*it) != storage.Space)
						std::get<0>(*it) = storage.Space;
					if (!storage.Space) {
						//begin move process
					}
					continue;
				}
				CREATEFILE2_EXTENDED_PARAMETERS extendedParams = { 0 };
				extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
				extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
				extendedParams.dwFileFlags = FILE_FLAG_WRITE_THROUGH;
				extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
				extendedParams.lpSecurityAttributes = nullptr;
				extendedParams.hTemplateFile = nullptr;
				_Files.push_back(std::make_tuple(storage.ID, CreateFile2(
					storage.Address.c_str(),
					GENERIC_READ | GENERIC_WRITE,
					0,
					OPEN_ALWAYS,
					&extendedParams
				), std::make_unique<std::mutex>()));
				if (auto size = _Size(storage.ID); !std::get<0>(size))
					_Write(storage.ID, 0, std::vector<unsigned char>(1024, 0));
			}
			break;
		}
	Status = System::Started;
}
void Main() {
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	Storage::Mount();
	Network::Add(Network::TCP, 1025, 1400, L"Storage.dll");
	//auto x=	Storage::Sector::Get();

	//for (unsigned char byte : x) {
	//	std::bitset<8> binary(byte); // convert byte to binary
	//	std::cout << binary << ' '; 
	//}
	//Storage::Sector::Delete(x);
}
std::vector<unsigned char> _CreateLocalSectorAddress(uint8_t id,uint64_t sector) {
	std::vector<unsigned char> buffer(16, 0);
	std::copy(System::Name.begin(), System::Name.end(), buffer.begin());
	buffer[7] = id;
	std::memcpy(&buffer[8], &sector, 8);
	return buffer;
}
std::vector<unsigned char>  Storage::Sector::New()
{
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	for (auto& server : International::Organization::Standardization::Servers)
		if (server.Name == System::Name) 
			for (auto& storage : server.Storages)
				if (storage.Space != 0)
					if (auto it = std::find_if(_Files.begin(), _Files.end(), [&storage](auto& file) {return std::get<0>(file) == storage.ID; }); it != _Files.end()) {
						uint64_t sector = 0;
						while (true)
						{
							std::vector< uint64_t> sectors;
							{
								auto buffer = _Read(storage.ID, sector);
								for (int i = 0; i < 128; i++)
								{
									uint64_t value = 0;
									std::memcpy(&value, buffer.data() + (i * 8), 8);
									if (value)
										sectors.push_back(value);
									else break;
								}
							}
							if (sectors.empty()) 
								if (auto _size = _Size(storage.ID); std::get<0>(_size) < std::get<1>(_size)) {
									uint16_t sector = (std::get<0>(_size) / 1024);
									_Write(storage.ID, sector, std::vector<unsigned char>(1024, 0));
									return _CreateLocalSectorAddress(storage.ID, sector);
								}
					
							if (auto ConfirmBufferEmpty = _Read(storage.ID, sectors.back());  std::all_of(ConfirmBufferEmpty.begin(), ConfirmBufferEmpty.end(), [](unsigned char c) { return c == 0; })) {
								uint64_t useSector = sectors.back();
								sectors.pop_back();
								auto buffer = std::vector<unsigned char>(1024, 0);
								std::memcpy(buffer.data(), sectors.data(), sectors.size() * 8);
								_Write(storage.ID, sector, buffer);
								return _CreateLocalSectorAddress(storage.ID, useSector);
							}
							sector = sectors.back();
						}
					}
     return std::vector<unsigned char>();
}
bool Storage::Sector::Delete(std::vector<unsigned char> sector)
{
	if (std::vector<unsigned char>(sector.begin(), sector.begin() + 7) != std::vector<unsigned char>(System::Name.begin(), System::Name.end()))
		return false;
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	for (auto& server : International::Organization::Standardization::Servers)
		if (server.Name == System::Name)
			for (auto& storage : server.Storages)
				if (storage.Space != 0)
					if (auto it = std::find_if(_Files.begin(), _Files.end(), [&storage](auto& file) {return std::get<0>(file) == storage.ID; }); it != _Files.end()) {
						uint64_t sectorValue = 0;
						std::memcpy(&sectorValue, sector.data() + 8, 8);
						std::vector<uint64_t> sectors;
						{
							auto buffer = _Read(storage.ID, sectorValue);
							for (int i = 0; i < 128; i++)
							{
								uint64_t value = 0;
								std::memcpy(&value, buffer.data() + (i * 8), 8);
								if (value)
									sectors.push_back(value);
								else break;
							}
						}
						if (sectors.empty())
							return false;
						sectors.push_back(0);
						auto buffer = std::vector<unsigned char>(1024, 0);
						std::memcpy(buffer.data(), sectors.data(), sectors.size() * 8);
						return _Write(storage.ID, sectorValue, buffer);
					}
	return false;
}
bool Storage::Sector::Write(std::vector<unsigned char> sector, std::vector<unsigned char> data) {
	if (data.size() != 1024|| std::vector<unsigned char>(sector.begin(), sector.begin() + 7) != std::vector<unsigned char>(System::Name.begin(), System::Name.end()))  return false;
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	for (auto& server : International::Organization::Standardization::Servers)
		if (server.Name == System::Name)
			for (auto& storage : server.Storages)
				if (storage.Space != 0)
					if (auto it = std::find_if(_Files.begin(), _Files.end(), [&sector](auto& file) {return std::get<0>(file) == sector[7]; }); it != _Files.end()) {
						uint64_t sectorValue = 0;
						std::memcpy(&sectorValue, sector.data() + 8, 8);
						return _Write(storage.ID, sectorValue, data);
					}
	return false;
}
std::vector<unsigned char> Storage::Sector::Read(std::vector<unsigned char> sector)
{
	if (std::vector<unsigned char>(sector.begin(), sector.begin() + 7) != std::vector<unsigned char>(System::Name.begin(), System::Name.end())) 
		return std::vector<unsigned char>();

	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	for (auto& server : International::Organization::Standardization::Servers)
		if (server.Name == System::Name)
			for (auto& storage : server.Storages)
				if (storage.Space != 0)
					if (auto it = std::find_if(_Files.begin(), _Files.end(), [&sector](auto& file) {return std::get<0>(file) == sector[7]; }); it != _Files.end()) {
						uint64_t sectorValue = 0;
						std::memcpy(&sectorValue, sector.data() + 8, 8);
						return _Read(storage.ID, sectorValue);
					}
	return std::vector<unsigned char>();
}



extern "C" _declspec(dllexport)void TCP(std::shared_ptr<SOCKET> socket, int port) {
	if (port == 1025) {


		// TODO: Add SSL check here later
		while (socket && *socket != INVALID_SOCKET) {
		
		}
	}
}