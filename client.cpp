
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib") 

using namespace std;


void inputArray() {
    int n;
    while (true) {
        cout << "Nhap so phan tu n (n <= 0 de thoat): ";
        cin >> n;

        
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cout << "Khoi tao WSA that bai: " << WSAGetLastError() << endl;
            return;
        }

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Tao socket that bai: " << WSAGetLastError() << endl;
            WSACleanup();
            return;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(100); // Port 100
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cout << "Ket noi that bai: " << WSAGetLastError() << endl;
            closesocket(clientSocket);
            WSACleanup();
            return;
        }

       
        send(clientSocket, (char*)&n, sizeof(n), 0);

        
        if (n <= 0) {
            break;
        }

        
        float* arr = new float[n];
        for (int i = 0; i < n; i++) {
            cout << "Nhap phan tu thu " << i + 1 << ": ";
            cin >> arr[i];
        }

        
        send(clientSocket, (char*)arr, n * sizeof(float), 0);

       
        delete[] arr;

        
        closesocket(clientSocket);
    }

   
    WSACleanup();
}

int main() {
    inputArray();
    return 0;
}
