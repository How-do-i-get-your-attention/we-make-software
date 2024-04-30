#define StartDLL
#define StopDLL
#define NetworkApplicationProgrammingInterface
#include ".h"
std::vector<std::tuple<Network::Protocol, int, std::shared_ptr<SOCKET>, std::shared_ptr<SOCKET>, std::wstring>> Connections;
std::mutex Mutex;
void Remove(Network::Protocol protocol, int port)
{
	std::lock_guard<std::mutex> lock(Mutex);
	if (auto it = std::find_if(Connections.begin(), Connections.end(), [protocol, port](std::tuple<Network::Protocol, int, std::shared_ptr<SOCKET>, std::shared_ptr<SOCKET>, std::wstring>& connection) {return std::get<0>(connection) == protocol && std::get<1>(connection) == port; }); it != Connections.end()) {
		System::Libraries::Wait(std::get<4>(*it), System::Stopped);
		if (*std::get<2>(*it) != INVALID_SOCKET) {
			closesocket(*std::get<2>(*it));
			std::get<2>(*it).reset();
		}
		if (*std::get<3>(*it) != INVALID_SOCKET) {
			closesocket(*std::get<3>(*it));
			std::get<3>(*it).reset();
		}
		Firewall::Remove(protocol == Network::UDP ? Firewall::UDP : Firewall::TCP, port);
		Connections.erase(it);
		Task.End();
	}
}
typedef void (*UDPFunction)(std::shared_ptr<SOCKET>, int, std::shared_ptr<sockaddr_storage>, std::shared_ptr<std::vector<unsigned char>>);
typedef void (*TCPFunction)(std::shared_ptr<SOCKET>, int);
void handleUDP(std::shared_ptr<SOCKET> sock, int port, std::wstring name, UDPFunction uDPFunction) {
	int BufferSize;
	{
		socklen_t optlen = sizeof(BufferSize);
		getsockopt(*sock, SOL_SOCKET, SO_RCVBUF, (char*)&BufferSize, &optlen);
	}
	while (System::Libraries::Wait(name, System::Started)) {
		std::shared_ptr<sockaddr_storage> clientaddr = std::make_shared<sockaddr_storage>();
		socklen_t addrlen = sizeof(*clientaddr);
		std::shared_ptr<std::vector<unsigned char>> buffer = std::make_shared<std::vector<unsigned char>>(BufferSize);
		if (int bufferReceived = recvfrom(*sock.get(), reinterpret_cast<char*>(buffer->data()), BufferSize, 0, reinterpret_cast<struct sockaddr*>(clientaddr.get()), &addrlen); bufferReceived > 1) {
			buffer->resize(bufferReceived);
			std::thread(uDPFunction, sock, port, clientaddr, buffer).detach();
		}
	}
}
void handleTCP(std::shared_ptr<SOCKET> sock, int port, std::wstring name, TCPFunction tCPFunction) {
	while (System::Libraries::Wait(name, System::Started)) {
		if (auto ClientSocket = std::shared_ptr<SOCKET>(new SOCKET(accept(*sock, NULL, NULL)), [](SOCKET* ptr) {
			if (*ptr != INVALID_SOCKET)
				closesocket(*ptr);
			delete ptr;
			}); *ClientSocket != INVALID_SOCKET)
			std::thread(tCPFunction, ClientSocket, port).detach();

	}
}
void Reader(std::shared_ptr<SOCKET> sock, Network::Protocol protocol, int port, std::wstring name) {

	switch (protocol)
	{
	case Network::UDP:
		if (UDPFunction uDPFunction = (UDPFunction)GetProcAddress(System::Libraries::Get(name), "UDP"); uDPFunction != nullptr)
			handleUDP(sock, port, name, uDPFunction);
		break;
	case Network::TCP:
		if (TCPFunction tCPFunction = (TCPFunction)GetProcAddress(System::Libraries::Get(name), "TCP"); tCPFunction != nullptr)
			handleTCP(sock, port, name, tCPFunction);
		break;
	}
	Remove(protocol, port);
}
void Network::Add(Network::Protocol protocol, int port, int bufferSize, std::wstring name)
{
	if (auto it = std::find_if(Connections.begin(), Connections.end(), [protocol, port, name](std::tuple<Network::Protocol, int, std::shared_ptr<SOCKET>, std::shared_ptr<SOCKET>, std::wstring>& connection) {return std::get<0>(connection) == protocol && std::get<1>(connection) == port && std::get<4>(connection) == name; }); it == Connections.end()) {

		Task.Start();
		Firewall::Add(protocol == UDP ? Firewall::UDP : Firewall::TCP, port);
		std::shared_ptr<SOCKET> IneternetProtocolVersion4(new SOCKET(INVALID_SOCKET));
		std::shared_ptr<SOCKET> IneternetProtocolVersion6(new SOCKET(INVALID_SOCKET));

		if (protocol == UDP) {
			*IneternetProtocolVersion4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			*IneternetProtocolVersion6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		}
		else
		{
			*IneternetProtocolVersion4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			*IneternetProtocolVersion6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
			int enable = 1;
			setsockopt(*IneternetProtocolVersion4, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(int));
			setsockopt(*IneternetProtocolVersion6, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(int));
		}
		u_long mode = 1;
		ioctlsocket(*IneternetProtocolVersion4, FIONBIO, &mode);
		ioctlsocket(*IneternetProtocolVersion6, FIONBIO, &mode);
		struct timeval timeout;
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		setsockopt(*IneternetProtocolVersion4, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		setsockopt(*IneternetProtocolVersion4, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
		setsockopt(*IneternetProtocolVersion6, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		setsockopt(*IneternetProtocolVersion6, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
		setsockopt(*IneternetProtocolVersion4, SOL_SOCKET, SO_RCVBUF, (const char*)&bufferSize, sizeof(int));
		setsockopt(*IneternetProtocolVersion4, SOL_SOCKET, SO_SNDBUF, (const char*)&bufferSize, sizeof(int));
		setsockopt(*IneternetProtocolVersion6, SOL_SOCKET, SO_RCVBUF, (const char*)&bufferSize, sizeof(int));
		setsockopt(*IneternetProtocolVersion6, SOL_SOCKET, SO_SNDBUF, (const char*)&bufferSize, sizeof(int));
		struct sockaddr_in servaddr4;
		servaddr4.sin_family = AF_INET;
		servaddr4.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr4.sin_port = htons(port);
		struct sockaddr_in6 servaddr6;
		servaddr6.sin6_family = AF_INET6;
		servaddr6.sin6_addr = in6addr_any;
		servaddr6.sin6_port = htons(port);
		Connections.push_back(std::make_tuple(protocol, port, IneternetProtocolVersion4, IneternetProtocolVersion6, name));
		bind(*IneternetProtocolVersion4, (struct sockaddr*)&servaddr4, sizeof(servaddr4));
		bind(*IneternetProtocolVersion6, (struct sockaddr*)&servaddr6, sizeof(servaddr6));
		if (auto it = std::find_if(Connections.begin(), Connections.end(), [protocol, port, name](std::tuple<Network::Protocol, int, std::shared_ptr<SOCKET>, std::shared_ptr<SOCKET>, std::wstring>& connection) {return std::get<0>(connection) == protocol && std::get<1>(connection) == port && std::get<4>(connection) == name; }); it != Connections.end()) {
			if (protocol != Network::UDP) {
				listen(*std::get<2>(*it), SOMAXCONN);
				listen(*std::get<3>(*it), SOMAXCONN);
			}
			std::thread(Reader, std::ref(std::get<2>(*it)), protocol, port, name).detach();
			std::thread(Reader, std::ref(std::get<3>(*it)), protocol, port, name).detach();
		}
	}
}
void Main() {
	System::Libraries::WaitForModule(L"Firewall.dll");
	System::Libraries::Wait(L"Firewall.dll", System::Started);
}
void Exit() {
	while (!Connections.empty()) {
		auto connection = Connections.front();
		System::Libraries::Wait(std::get<4>(connection), System::Stopped);
		std::thread(Remove, std::get<0>(connection), std::get<1>(connection)).detach();
	}
}