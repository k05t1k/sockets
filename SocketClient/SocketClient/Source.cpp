#define WIN32_LEAN_AND_MEAN // ��������� �� ������������ ������ Windows ��������� ���������������� ����������, ������� ��� ����� ����������.

#include <iostream> // �������� ����������� ���������� ��� �����-������.
#include <Windows.h> // �������� �������� ������������ ����� ��� ���������� ���������� Windows.
#include <WinSock2.h> // �������� ������������ ���� ��� ������� Windows.
#include <WS2tcpip.h> // �������� ������������ ���� ��� ���������� TCP/IP ��� ������� Windows.

using namespace std; // ���������� ������������ ���� std, ���������� ����������� ����������.

int main() { // ����� ����� � ���������.

    WSADATA wsaData; // ���������, ������������ ��� �������� ���������� � ���������� ������� Windows.
    ADDRINFO* addResult; // ��������� �� ���������, ���������� ���������� �� ������.
    ADDRINFO hints; // ���������, ������������ ��� ��������� ������� � ������� getaddrinfo.
    SOCKET ConnectSocket = INVALID_SOCKET; // ����� ��� ����������� � �������. ��������������� ��� ����������������.
    const char* sendBuffer[2] = { "\n\nHello from Client\n", "\n\nGoodbye from Server!\n" }; // ����� ��� �������� ������. �������� ��� ���������.
    char recvBuffer[512]; // ����� ��� ��������� ������. ������ - 512 ����.

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // ������������� ���������� ������� Windows. ���������� ��� ����������.

    if (result != 0) { // �������� �� ������ ��� �������������.
        std::cout << "WSAStartup failes result" << std::endl; // ����� ��������� �� ������.
        return 1; // ������� ���� ������.
    }

    ZeroMemory(&hints, sizeof(hints)); // ��������� ������, ���������� ��� ��������� hints.
    hints.ai_family = AF_INET; // ��������� ��������� ������� - IPv4.
    hints.ai_socktype = SOCK_STREAM; // ��������� ���� ������ - ���������.
    hints.ai_protocol = IPPROTO_TCP; // ��������� ��������� - TCP.

    result = getaddrinfo("localhost", "666", &hints, &addResult); // ��������� ���������� �� ������. ���������� ��� ����������.
    if (result != 0) { // �������� �� ������.
        std::cout << "Getaddrinfo wit error" << std::endl; // ����� ��������� �� ������.
        WSACleanup(); // ������� ���������� ������� Windows.
        return 1; // ������� ���� ������.
    }

    ConnectSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol); // �������� ������. ���������� ���������� ������.
    if (ConnectSocket == INVALID_SOCKET) { // �������� �� ������ ��� ��������.
        std::cout << "Socket creation with " << std::endl; // ����� ��������� �� ������.
        WSACleanup(); // ������� ���������� ������� Windows.
        return 1; // ������� ���� ������.
    }

    result = connect(ConnectSocket, addResult->ai_addr, (int)addResult->ai_addrlen); // ����������� � �������. ���������� ��� ����������.
    if (result == SOCKET_ERROR) { // �������� �� ������.
        std::cout << "Unable to connect to server" << std::endl; // ����� ��������� �� ������.
        closesocket(ConnectSocket); // �������� ������.
        freeaddrinfo(addResult); // ������������ ������, ���������� ��� ��������� addResult.
        WSACleanup(); // ������� ���������� ������� Windows.
        return 1; // ������� ���� ������.
    }

    for (int i = 0; i < 2; i++) { // ���� �������� ��������� �������.
        result = send(ConnectSocket, sendBuffer[i], (int)strlen(sendBuffer[i]), MSG_DONTROUTE); // �������� ���������. ���������� ��� ����������.
        if (result == SOCKET_ERROR) { // �������� �� ������ ��� ��������.
            std::cout << "Send failed with error" << std::endl; // ����� ��������� �� ������.
            closesocket(ConnectSocket); // �������� ������.
            freeaddrinfo(addResult); // ������������ ������, ���������� ��� ��������� addResult.
            WSACleanup(); // ������� ���������� ������� Windows.
            return 1; // ������� ���� ������.
        }
    }

    std::cout << "Data sent succsesfully" << result << std::endl; // ����� ��������� �� �������� �������� ������.

    result = shutdown(ConnectSocket, SD_SEND); // ���������� �������� ������ �������. ���������� ��� ����������.
    if (result == SOCKET_ERROR) { // �������� �� ������.
        cout << "shutdown error <3" << endl; // ����� ��������� �� ������.
        freeaddrinfo(addResult); // ������������ ������, ���������� ��� ��������� addResult.
        WSACleanup(); // ������� ���������� ������� Windows.
        return 1; // ������� ���� ������.
    }

    do { // ������ ����� ��������� ������, ���������� �� �������.
        ZeroMemory(recvBuffer, 512); // ��������� ������, ���������� ��� ������ ��������� ������.
        result = recv(ConnectSocket, recvBuffer, 512, 0); // ��������� ������ �� �������. ���������� ��� ����������.
        if (result > 0) { // �������� �� �������� ��������� ������.
            cout << "Received " << result << "bytes" << endl; // ����� ���������� ���������� ����.
            cout << "Received data " << recvBuffer << endl; // ����� ���������� ������.

        }
        else if (result == 0) // �������� �� �������� ���������� ��������.
            cout << "Connection closed" << endl; // ����� ��������� � �������� ����������.

        else { // �������� �� ������ ��� ��������� ������.
            cout << "Received failed " << endl; // ����� ��������� �� ������.
            closesocket(ConnectSocket); // �������� ������.

        }
    } while (result > 0); // ������� ����������� ����� - �������� ��������� ������.

    result = shutdown(ConnectSocket, SD_SEND); // ���������� �������� ������ �������. ���������� ��� ����������.
    if (result == SOCKET_ERROR) { // �������� �� ������.
        cout << "shutdown  error " << endl; // ����� ��������� �� ������.
        freeaddrinfo(addResult); // ������������ ������, ���������� ��� ��������� addResult.
        WSACleanup(); // ������� ���������� ������� Windows.
        return 1; // ������� ���� ������.
    }

    closesocket(ConnectSocket); // �������� ������.
    freeaddrinfo(addResult); // ������������ ������, ���������� ��� ��������� addResult.
    WSACleanup(); // ������� ���������� ������� Windows.
    return 0; // ������� ���� ��������� ����������.
}
