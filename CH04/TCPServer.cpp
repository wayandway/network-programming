#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE 512

void err_quit(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFE|FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

void err_display(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFE|FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

int main(int argc, char *argv[])
{
    int retval;

    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa)!=0)
       return 1;

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock == INVALID_SOCKET) err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = hton(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
    if(retval == SOCKET_ERROR) err_quit("bind()");

    retval = listen(listen_sock, SOMAXCONN);
    if(retval == SOCKET_ERROR) err_quit("listen()");

    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE+1];

    while(1) {
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if(client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        printf("\n[TCP 서버] 클라이언트 접속 : IP = %s, PORT = %d\n", inet_ntoa(Clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        while(1) {
            retval = recv(client_sock, buf, BUFSIZE, 0);
            if(retval == SOCKET_ERROR) {
                err_display("recv()");
                break;
            }
            else if(retval == 0)
                break;

            buf[retval] = '\0';
            printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

            retval = send(client_sock, buf, retval, 0);
            if(retval == SOCKET_ERROR) {
                err_display("send()");
                break;
            }
        }

        closesocket(client_sock);
        printf("[TCP 서버] 클라이언트 종료 : IP = %s, PORT = %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    }
    closesocket(listen_sock);

    WSACleanup();
    return 0;
}