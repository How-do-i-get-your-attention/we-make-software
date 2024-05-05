#define StartDLL
#define StopDLL
#define StoargeApplicationProgrammingInterface
#include ".h"
#include <ntstatus.h>

std::vector<std::tuple<uint8_t, HANDLE, std::unique_ptr<std::mutex>>> _Files;
std::vector<unsigned char>_Read(uint8_t ID, uint64_t Sector) {
	using namespace Storage;
	if (auto it = std::find_if(_Files.begin(), _Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != _Files.end()) {
		LARGE_INTEGER fileSize;
		GetFileSizeEx(std::get<1>(*it), &fileSize);
		if (Sector * 512 < fileSize.QuadPart) {
			std::vector<unsigned char> buffer(512, 0);
			OVERLAPPED overlapped = { 0 };
			uint64_t offset = Sector * 512;
			overlapped.Offset = offset & 0xFFFFFFFF;
			overlapped.OffsetHigh = offset >> 32; 
			std::lock_guard<std::mutex> lock(*std::get<2>(*it));
			if (!ReadFile(std::get<1>(*it), buffer.data(), 512, nullptr, &overlapped))
				return std::vector<unsigned char>();
			return buffer;
		}
	}

	return std::vector<unsigned char>();
}
bool _Write(uint8_t ID, uint64_t Sector, std::vector<unsigned char> buffer) {
	std::cout << "Mai0 \n";
	using namespace Storage;
	if (auto it = std::find_if(_Files.begin(), _Files.end(), [&ID](auto& file) {return std::get<0>(file) == ID; }); it != _Files.end() && buffer.size() == 512) {
		OVERLAPPED overlapped = { 0 };
		uint64_t offset = Sector * 512;
		overlapped.Offset = offset & 0xFFFFFFFF;
		overlapped.OffsetHigh = offset >> 32;
		std::lock_guard<std::mutex> lock(*std::get<2>(*it));
		if (!WriteFile(std::get<1>(*it), buffer.data(), 512, nullptr, &overlapped))
			return false;
		FlushFileBuffers(std::get<1>(*it));
		return true;
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
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					OPEN_ALWAYS,
					&extendedParams
				), std::make_unique<std::mutex>()));
				auto size = _Size(storage.ID);
				if (!std::get<0>(size)) {
					_Write(storage.ID, 0, std::vector<unsigned char>(512, 0));// Sector system
					_Write(storage.ID, 1, std::vector<unsigned char>(512, 0));// Master
				}
			}
			break;
		}
	Status = System::Started;
}
void Main() {
	System::Libraries::WaitForModules({ L"International Organization Standardization.dll" }, System::Started);
	Storage::Mount();
	Sleep(5000);
	auto x=	Storage::Sector::Get();

	for (unsigned char byte : x) {
		std::bitset<8> binary(byte); // convert byte to binary
		std::cout << binary << ' '; 
	}
	Storage::Sector::Delete(x);
}
std::vector<unsigned char> _CreateLocalSectorAddress(uint8_t id,uint64_t sector) {
	std::vector<unsigned char> buffer(16, 0);
	std::copy(System::Name.begin(), System::Name.end(), buffer.begin());
	buffer[7] = id;
	std::memcpy(&buffer[8], &sector, 8);
	return buffer;
}
std::vector<unsigned char>  Storage::Sector::Get()
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
								for (int i = 0; i < 64; i++)
								{
									uint64_t value = 0;
									std::memcpy(&value, buffer.data() + (i * 8), 8);
									if (value)
										sectors.push_back(value);
									else break;
								}
							}
							if (sectors.empty()) {
								auto _size = _Size(storage.ID);
								if (std::get<0>(_size) < std::get<1>(_size)) {
									uint16_t sector = (std::get<0>(_size) /512);
									_Write(storage.ID, sector, std::vector<unsigned char>(512, 0));
									return _CreateLocalSectorAddress(storage.ID, sector);
								}
							}
							auto ConfirmBufferEmpty = _Read(storage.ID, sectors.back());
							if (std::all_of(ConfirmBufferEmpty.begin(), ConfirmBufferEmpty.end(), [](unsigned char c) { return c == 0; })) {
								uint64_t useSector = sectors.back();
								sectors.pop_back();
								auto buffer = std::vector<unsigned char>(512, 0);
								std::memcpy(buffer.data(), sectors.data(), sectors.size() * 8);
								_Write(storage.ID, sector, buffer);
								return _CreateLocalSectorAddress(storage.ID, useSector);
							}
							sector = sectors.back();
						}
					}
     return std::vector<unsigned char>();
}

void Storage::Sector::Delete(std::vector<unsigned char> buffer) {
	if (buffer.size() == 16)
	{
		if (std::string(buffer.begin(), buffer.begin() + 7) != System::Name) return;
		uint8_t bufferid = buffer[7];
		uint64_t buffersector = 0;
		std::memcpy(&buffersector, &buffer[8], 8);
		if (auto it = std::find_if(_Files.begin(), _Files.end(), [&bufferid](auto& file) {return std::get<0>(file) == bufferid; }); it != _Files.end()) {
			uint64_t sector = 0;
			while (true)
			{
				std::vector<uint64_t> sectors;
				{
					auto buffer = _Read(bufferid, sector);
					for (int i = 0; i < 64; i++)
					{
						uint64_t value = 0;
						std::memcpy(&value, buffer.data() + (i * 8), 8);
						if (value)
							sectors.push_back(value);
						else break;
					}
				}
				if(sectors.size()==64)
					sector = sectors.back();
				else
				{
					auto buffer = std::vector<unsigned char>(512, 0);
					_Write(bufferid, buffersector, buffer);
					sectors.push_back(buffersector);
					std::memcpy(buffer.data(), sectors.data(), sectors.size() * 8);
					_Write(bufferid, sector, buffer);
					break;
				}
			}
		}
	}
}
