
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <atomic> // Them header atomic
#pragma comment(lib, "Ws2_32.lib") // Lien ket voi thu vien Winsock

using namespace std;

// Bien toan cuc de kiem tra thoat chuong trinh
atomic<bool> shouldExit{false};

// Ham nhan du lieu tu client
void receiveData(SOCKET serverSocket) {
    cout << "Server dang lang nghe tren port 100..." << endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Chap nhan ket noi that bai: " << WSAGetLastError() << endl;
            shouldExit = true;
            return;
        }

        // Nhan gia tri n
        int n;
        int bytesReceived = recv(clientSocket, (char*)&n, sizeof(n), 0);
        if (bytesReceived <= 0) {
            cout << "Nhan gia tri n that bai." << endl;
            closesocket(clientSocket);
            continue;
        }

        // Truong hop 1: n < 0 thi thoat
        if (n < 0) {
            cout << "Nhan duoc n = " << n << ". Thoat..." << endl;
            closesocket(clientSocket);
            shouldExit = true;
            break;
        }

        // Truong hop 2: Nhan mang n so thuc va hien thi
        float* arr = new float[n];
        bytesReceived = recv(clientSocket, (char*)arr, n * sizeof(float), 0);
        if (bytesReceived <= 0) {
            cout << "Nhan mang that bai." << endl;
            delete[] arr;
            closesocket(clientSocket);
            continue;
        }

        cout << "Nhan duoc mang voi " << n << " phan tu: ";
        for (int i = 0; i < n; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;

        // Giai phong bo nho
        delete[] arr;

        // Dong socket client
        closesocket(clientSocket);
    }
}

// Ham timer de goi receiveData
VOID CALLBACK TimerProc(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {
    SOCKET* serverSocket = (SOCKET*)lpParameter;
    receiveData(*serverSocket);
}

int main() {
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Khoi tao WSA that bai: " << WSAGetLastError() << endl;
        return 1;
    }

    
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Tao socket that bai: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(100); 
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind that bai: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Lang nghe that bai: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    
    HANDLE hTimer = NULL;
    CreateTimerQueueTimer(&hTimer, NULL, TimerProc, &serverSocket, 0, 0, WT_EXECUTEONLYONCE);

    
    cout << "Server da khoi dong. Cho ket noi..." << endl;
    while (!shouldExit) {
        Sleep(100); 
    }

    
    closesocket(serverSocket);
    WSACleanup();

    
    DeleteTimerQueueTimer(NULL, hTimer, NULL);
    cout << "Chuong trinh server ket thuc." << endl;
    return 0;
}
