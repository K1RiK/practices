#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define IP "127.0.0.1"
#define PORT 6379

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

int main(int argc, char* argv[]) {
    system("chcp 1251");  // ��������� 1251

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSAStartup");
        exit(EXIT_FAILURE);
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);  // ������� �����
    if (sockfd == INVALID_SOCKET) {
        perror("socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  // �������� ���������
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(IP);  // ����� �������

    // ������������ � �������
    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        perror("connect");
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    char* message = argv[1];  // ��������� ��� ��������
    if (send(sockfd, message, strlen(message), 0) == SOCKET_ERROR) {  // ���������� ���������
        perror("send");
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // �������� ����� �� �������
    char buffer[4096];
    int bytesReceived = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        perror("recv");
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // �������� ����� �� �������
    printf("����� �� �������: %s\n", buffer);

    closesocket(sockfd);  // ��������� �����
    WSACleanup();

    return 0;
}
