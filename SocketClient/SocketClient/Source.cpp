#define WIN32_LEAN_AND_MEAN // Исключает из заголовочных файлов Windows некоторые малоиспользуемые компоненты, ускоряя тем самым компиляцию.

#include <iostream> // Включает стандартную библиотеку для ввода-вывода.
#include <Windows.h> // Включает основные заголовочные файлы для разработки приложений Windows.
#include <WinSock2.h> // Включает заголовочный файл для сокетов Windows.
#include <WS2tcpip.h> // Включает заголовочный файл для расширений TCP/IP для сокетов Windows.

using namespace std; // Использует пространство имен std, содержащее стандартные библиотеки.

int main() { // Точка входа в программу.

    WSADATA wsaData; // Структура, используемая для хранения информации о реализации сокетов Windows.
    ADDRINFO* addResult; // Указатель на структуру, содержащую информацию об адресе.
    ADDRINFO hints; // Структура, используемая для уточнения запроса к функции getaddrinfo.
    SOCKET ConnectSocket = INVALID_SOCKET; // Сокет для подключения к серверу. Инициализирован как недействительный.
    const char* sendBuffer[2] = { "\n\nHello from Client\n", "\n\nGoodbye from Server!\n" }; // Буфер для отправки данных. Содержит два сообщения.
    char recvBuffer[512]; // Буфер для получения данных. Размер - 512 байт.

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация библиотеки сокетов Windows. Возвращает код результата.

    if (result != 0) { // Проверка на ошибку при инициализации.
        std::cout << "WSAStartup failes result" << std::endl; // Вывод сообщения об ошибке.
        return 1; // Возврат кода ошибки.
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуление памяти, выделенной для структуры hints.
    hints.ai_family = AF_INET; // Установка семейства адресов - IPv4.
    hints.ai_socktype = SOCK_STREAM; // Установка типа сокета - потоковый.
    hints.ai_protocol = IPPROTO_TCP; // Установка протокола - TCP.

    result = getaddrinfo("localhost", "666", &hints, &addResult); // Получение информации об адресе. Возвращает код результата.
    if (result != 0) { // Проверка на ошибку.
        std::cout << "Getaddrinfo wit error" << std::endl; // Вывод сообщения об ошибке.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    ConnectSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol); // Создание сокета. Возвращает дескриптор сокета.
    if (ConnectSocket == INVALID_SOCKET) { // Проверка на ошибку при создании.
        std::cout << "Socket creation with " << std::endl; // Вывод сообщения об ошибке.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    result = connect(ConnectSocket, addResult->ai_addr, (int)addResult->ai_addrlen); // Подключение к серверу. Возвращает код результата.
    if (result == SOCKET_ERROR) { // Проверка на ошибку.
        std::cout << "Unable to connect to server" << std::endl; // Вывод сообщения об ошибке.
        closesocket(ConnectSocket); // Закрытие сокета.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    for (int i = 0; i < 2; i++) { // Цикл отправки сообщений серверу.
        result = send(ConnectSocket, sendBuffer[i], (int)strlen(sendBuffer[i]), MSG_DONTROUTE); // Отправка сообщения. Возвращает код результата.
        if (result == SOCKET_ERROR) { // Проверка на ошибку при отправке.
            std::cout << "Send failed with error" << std::endl; // Вывод сообщения об ошибке.
            closesocket(ConnectSocket); // Закрытие сокета.
            freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
            WSACleanup(); // Очистка библиотеки сокетов Windows.
            return 1; // Возврат кода ошибки.
        }
    }

    std::cout << "Data sent succsesfully" << result << std::endl; // Вывод сообщения об успешной отправке данных.

    result = shutdown(ConnectSocket, SD_SEND); // Завершение отправки данных серверу. Возвращает код результата.
    if (result == SOCKET_ERROR) { // Проверка на ошибку.
        cout << "shutdown error <3" << endl; // Вывод сообщения об ошибке.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    do { // Начало цикла обработки данных, полученных от сервера.
        ZeroMemory(recvBuffer, 512); // Обнуление памяти, выделенной для буфера получения данных.
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Получение данных от сервера. Возвращает код результата.
        if (result > 0) { // Проверка на успешное получение данных.
            cout << "Received " << result << "bytes" << endl; // Вывод количества полученных байт.
            cout << "Received data " << recvBuffer << endl; // Вывод полученных данных.

        }
        else if (result == 0) // Проверка на закрытие соединения сервером.
            cout << "Connection closed" << endl; // Вывод сообщения о закрытии соединения.

        else { // Проверка на ошибку при получении данных.
            cout << "Received failed " << endl; // Вывод сообщения об ошибке.
            closesocket(ConnectSocket); // Закрытие сокета.

        }
    } while (result > 0); // Условие продолжения цикла - успешное получение данных.

    result = shutdown(ConnectSocket, SD_SEND); // Завершение отправки данных серверу. Возвращает код результата.
    if (result == SOCKET_ERROR) { // Проверка на ошибку.
        cout << "shutdown  error " << endl; // Вывод сообщения об ошибке.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    closesocket(ConnectSocket); // Закрытие сокета.
    freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
    WSACleanup(); // Очистка библиотеки сокетов Windows.
    return 0; // Возврат кода успешного завершения.
}
