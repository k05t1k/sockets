#define WIN32_LEAN_AND_MEAN // Исключает из заголовочных файлов Windows некоторые малоиспользуемые компоненты, ускоряя тем самым компиляцию.

#include <iostream> // Включает стандартную библиотеку для ввода-вывода.
#include <Windows.h> // Включает основные заголовочные файлы для разработки приложений Windows.
#include <WinSock2.h> // Включает заголовочный файл для сокетов Windows.
#include <WS2tcpip.h> // Включает заголовочный файл для расширений TCP/IP для сокетов Windows.

int main() { // Точка входа в программу.

    WSADATA wsaData; // Структура, используемая для хранения информации о реализации сокетов Windows.
    ADDRINFO* addResult; // Указатель на структуру, содержащую информацию об адресе.
    ADDRINFO hints; // Структура, используемая для уточнения запроса к функции getaddrinfo.
    SOCKET ClientSocket = INVALID_SOCKET; // Сокет для подключения клиента. Инициализирован как недействительный.
    SOCKET ListentSocket = INVALID_SOCKET; // Сокет для прослушивания входящих подключений. Инициализирован как недействительный.

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
    hints.ai_flags = AI_PASSIVE; // Установка флага - использовать для сервера, прослушивающего входящие подключения.

    result = getaddrinfo(NULL, "666", &hints, &addResult); // Получение информации об адресе. Возвращает код результата.
    if (result != 0) { // Проверка на ошибку.
        std::cout << "Getaddrinfo wit error" << std::endl; // Вывод сообщения об ошибке.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    ListentSocket = socket(addResult->ai_family, addResult->ai_socktype, addResult->ai_protocol); // Создание сокета. Возвращает дескриптор сокета.
    if (ListentSocket == INVALID_SOCKET) { // Проверка на ошибку при создании.
        std::cout << "Socket creation with " << std::endl; // Вывод сообщения об ошибке.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        return 1; // Возврат кода ошибки.
    }

    result = bind(ListentSocket, addResult->ai_addr, (int)addResult->ai_addrlen); // Привязка сокета к адресу и порту. Возвращает код результата.
    if (result == SOCKET_ERROR) { // Проверка на ошибку.
        std::cout << "Binding connect failed" << std::endl; // Вывод сообщения об ошибке.
        closesocket(ListentSocket); // Закрытие сокета.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    result = listen(ListentSocket, SOMAXCONN); // Прослушивание входящих подключений. Возвращает код результата.
    if (result == SOCKET_ERROR) { // Проверка на ошибку.
        std::cout << "Listening socket failed" << std::endl; // Вывод сообщения об ошибке.
        closesocket(ListentSocket); // Закрытие сокета.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    ClientSocket = accept(ListentSocket, NULL, NULL); // Принятие входящего подключения. Возвращает дескриптор сокета.
    if (ClientSocket == INVALID_SOCKET) { // Проверка на ошибку при принятии.
        std::cout << "Accepting socket failed" << std::endl; // Вывод сообщения об ошибке.
        closesocket(ListentSocket); // Закрытие сокета.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }

    closesocket(ListentSocket); // Закрытие сокета, использованного для прослушивания.
    do { // Начало цикла обработки данных.
        ZeroMemory(recvBuffer, 512); // Обнуление памяти, выделенной для буфера получения данных.
        result = recv(ClientSocket, recvBuffer, 512, 0); // Получение данных от клиента. Возвращает код результата.
        if (result > 0) { // Проверка на успешное получение данных.
            std::cout << "Recieved " << result << "bytes" << std::endl; // Вывод количества полученных байт.
            std::cout << "Recieved data" << recvBuffer << std::endl; // Вывод полученных данных.

            for (int i = 0; i < 2; i++) { // Цикл отправки сообщений клиенту.
                result = send(ClientSocket, sendBuffer[i], (int)strlen(sendBuffer[i]), 0); // Отправка сообщения. Возвращает код результата.

                if (result == SOCKET_ERROR) { // Проверка на ошибку при отправке.
                    std::cout << "Failed to send data back" << std::endl; // Вывод сообщения об ошибке.
                    closesocket(ClientSocket); // Закрытие сокета.
                    freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
                    WSACleanup(); // Очистка библиотеки сокетов Windows.
                }
            }
        }
        else if (result == 0) // Проверка на закрытие соединения клиентом.
            std::cout << "Connection closing..." << std::endl; // Вывод сообщения о закрытии соединения.

        else { // Проверка на ошибку при получении данных.
            std::cout << "recv failed with error" << std::endl; // Вывод сообщения об ошибке.
            closesocket(ClientSocket); // Закрытие сокета.
            freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
            WSACleanup(); // Очистка библиотеки сокетов Windows.
            return 1; // Возврат кода ошибки.
        }
    } while (result > 0); // Условие продолжения цикла - успешное получение данных.

    result = shutdown(ClientSocket, SD_SEND); // Завершение отправки данных клиенту. Возвращает код результата.
    if (result == SOCKET_ERROR) { // Проверка на ошибку.
        closesocket(ClientSocket); // Закрытие сокета.
        freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
        WSACleanup(); // Очистка библиотеки сокетов Windows.
        return 1; // Возврат кода ошибки.
    }
    closesocket(ClientSocket); // Закрытие сокета.
    freeaddrinfo(addResult); // Освобождение памяти, выделенной для структуры addResult.
    WSACleanup(); // Очистка библиотеки сокетов Windows.
    return 0; // Возврат кода успешного завершения.
}
