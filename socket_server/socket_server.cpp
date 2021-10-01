// socket_server.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

//#pragma comment(lib, "Ws2_32.lib")
//
//#include <WinSock2.h>
//#include <iostream>
//
//using namespace std;
//
//int main()
//{
//    int r;
//    WSAData wsaData;
//    WORD DLLVSERION;
//    DLLVSERION = MAKEWORD(2, 1);//Winsocket-DLL 版本
//
//    //用 WSAStartup 開始 Winsocket-DLL
//    r = WSAStartup(DLLVSERION, &wsaData);
//
//    //宣告 socket 位址資訊(不同的通訊,有不同的位址資訊,所以會有不同的資料結構存放這些位址資訊)
//    SOCKADDR_IN addr;
//    int addrlen = sizeof(addr);
//
//    //建立 socket
//    SOCKET sListen; //listening for an incoming connection
//    SOCKET sConnect; //operating if a connection was found
//
//    //AF_INET：表示建立的 socket 屬於 internet family
//    //SOCK_STREAM：表示建立的 socket 是 connection-oriented socket 
//    sConnect = socket(AF_INET, SOCK_STREAM, NULL);
//
//    //設定位址資訊的資料
//    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//    //addr.sin_addr.s_addr = inet_pton("127.0.0.1");
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(1234);
//
//    //設定 Listen
//    sListen = socket(AF_INET, SOCK_STREAM, NULL);
//    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
//    listen(sListen, SOMAXCONN);//SOMAXCONN: listening without any limit
//
//    //等待連線
//    SOCKADDR_IN clinetAddr;
//    while (true)
//    {
//        cout << "waiting..." << endl;
//
//        if (sConnect = accept(sListen, (SOCKADDR*)&clinetAddr, &addrlen))
//        {
//            cout << "a connection was found" << endl;
//            printf("server: got connection from %s\n", inet_ntoa(addr.sin_addr));
//
//            //傳送訊息給 client 端
//            char* sendbuf = "sending data test";
//            send(sConnect, sendbuf, (int)strlen(sendbuf), 0);
//
//        }
//    }
//
//    //getchar();
//}


#include <stdio.h>
#include <Winsock2.h> //Socket的函数调用　
#include <windows.h>
#include <iostream>
#include <string>
#include <cstring>

using std::cout;
using std::cin;
using std::endl;
using std::string;

#define BUF_SIZE 1024  //  缓冲区大小

#pragma comment (lib, "ws2_32")     // 使用WINSOCK2.H时，则需要库文件WS2_32.LIB

DWORD WINAPI Rcv(LPVOID lpParam)
{
    SOCKET sClient = *(SOCKET*)lpParam;
    int retVal;
    char bufRecv[BUF_SIZE];
    memset(bufRecv, 0, sizeof(bufRecv));
   /* char bufSend[BUF_SIZE];
    memset(bufSend, 0, sizeof(bufSend));
    string tmp_string = "This will be converted to char*";
    char* c_string_copy = new char[tmp_string.length() + 1];
    memmove(c_string_copy, tmp_string.c_str(), tmp_string.length());
    bufSend[BUF_SIZE] = *c_string_copy;*/
    while (1)
    {
        retVal = recv(sClient, bufRecv, BUF_SIZE, 0);
        if (retVal == SOCKET_ERROR) {
            printf("recive faild!\n");
            break;
        }
        else {
            printf("Client:%s\n", bufRecv);
            //retVal = send(sClient, bufSend, strlen(bufSend) + sizeof(char), 0);
            //if (retVal == SOCKET_ERROR) {
            //    printf("send faild!\n");
            //    //break;
            //}
        }
    }
    return 0;
}

DWORD WINAPI Snd(LPVOID lpParam)
{
    SOCKET sClient = *(SOCKET*)lpParam;
    int retVal;
    char bufSend[BUF_SIZE];
    memset(bufSend, 0, sizeof(bufSend));
    while (1)
    {
        gets_s(bufSend);
        retVal = send(sClient, bufSend, strlen(bufSend) + sizeof(char), 0);
        if (retVal == SOCKET_ERROR) {
            printf("send faild!\n");
            break;
        }
    }
    return 0;
}


int main(int argc, char* argv[])
{
    // 初始化套接字动态库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("winsock load faild!\n");
        return 1;
    }

    //  创建服务段套接字
    SOCKET sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sServer == INVALID_SOCKET) {
        printf("socket faild!\n");
        WSACleanup();
        return -1;
    }

    //  服务端地址
    sockaddr_in addrServ;

    addrServ.sin_family = AF_INET;
    addrServ.sin_port = htons(9999);
    /*addrServ.sin_addr.s_addr = htonl(INADDR_ANY);*/
    addrServ.sin_addr.s_addr = inet_addr("192.168.0.6");

    //  绑定套接字
    if (bind(sServer, (const struct sockaddr*)&addrServ, sizeof(addrServ)) == SOCKET_ERROR) {
        printf("bind faild!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    printf("Server is On IP:[%s],port:[%d]\n", inet_ntoa(addrServ.sin_addr), ntohs(addrServ.sin_port));

    //  监听套接字  数字表示最多能监听客户个数
    if (listen(sServer, 5) == SOCKET_ERROR) {
        printf("listen faild!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }

    SOCKET sClient; //  客户端套接字

    sockaddr_in addrClient;
    int addrClientLen = sizeof(addrClient);


    sClient = accept(sServer, (sockaddr FAR*) & addrClient, &addrClientLen);
    if (sClient == INVALID_SOCKET) {
        printf("accept faild!\n");
        closesocket(sServer);
        WSACleanup();
        return -1;
    }
    printf("accepted client IP:[%s],port:[%d]\n", inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));

    HANDLE hThread1, hThread2;
    DWORD dwThreadId1, dwThreadId2;

    hThread1 = ::CreateThread(NULL, NULL, Snd, (LPVOID*)&sClient, 0, &dwThreadId1);
    hThread2 = ::CreateThread(NULL, NULL, Rcv, (LPVOID*)&sClient, 0, &dwThreadId2);

    ::WaitForSingleObject(hThread1, INFINITE);
    ::WaitForSingleObject(hThread2, INFINITE);
    ::CloseHandle(hThread1);
    ::CloseHandle(hThread2);

    closesocket(sClient);
    WSACleanup(); // 资源释放

    return 0;
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
