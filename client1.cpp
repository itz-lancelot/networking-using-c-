#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMsg(SOCKET s) {
    cout << "Enter your chat name: ";
    string name;
    getline(cin, name);
    string message;

    while (1) {
        getline(cin, message);
        string msg = name + ": " + message;
        int bytesSent = send(s, msg.c_str(), msg.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            cout << "Error sending message" << endl;
            break;
        }

        if (message == "quit") {
            cout << "Quitting the application" << endl;
            break;
        }
    }
    closesocket(s);
    WSACleanup();
}

void ReceiveMsg(SOCKET s) {
    char buffer[4096];
    while (1) {
        int recvLength = recv(s, buffer, sizeof(buffer), 0);
        if (recvLength <= 0) {
            cout << "Disconnected from the server" << endl;
            break;
        }
        string message(buffer, recvLength);
        cout << message << endl; // Print received message
    }
    closesocket(s);
    WSACleanup();
}

int main() {
    if (!Initialize()) {
        cout << "Winsock initialization failed" << endl;
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        cout << "Invalid socket created" << endl;
        return 1;
    }

    int port = 12345;
    string serverAddress = "127.0.0.1";
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

    if (connect(s, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Unable to connect to the server: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }

    cout << "Successfully connected to the server" << endl;

    thread senderThread(SendMsg, s);
    thread receiverThread(ReceiveMsg, s);

    senderThread.join();
    receiverThread.join();

    return 0;
}
