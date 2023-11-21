#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#include "mutex.h"
#include "dbms.h"

#pragma comment(lib, "ws2_32.lib")

#define IP "127.0.0.1"
#define PORT 6379
#define MAX_CONNECTIONS 3

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

// ��������� ��� �������� ������ �������
typedef struct {
    SOCKET sockfd;
    sockaddr_in addr;
    mutex_t* mutex;
} ClientData;

// ������� ��������� �������� �������
DWORD WINAPI handleClient(LPVOID arg) {
    ClientData* client = (ClientData*)arg;
    char buffer[1024];  // ������ � ������� -> �md
    char output[4096];  // ����� �������
    int bytesRead;

    // ����� ������ �� �������
    while ((bytesRead = recv(client->sockfd, buffer, sizeof(buffer), 0)) > 0) {
        // ��������� ������ � ������������ ������
        printf("������������ ����� ������� \"%s\"\n", buffer);

        mutex_lock(client->mutex);
        _main(output, buffer);
        printf("OUT:%s\n", output);
        mutex_unlock(client->mutex);

        printf("��������� ������ �������.\n");

        // �������� ������ �������
        send(client->sockfd, output, sizeof(output), 0);
    }

    // �������� ���������� � ��������
    closesocket(client->sockfd);
    free(client);
    return 0;
}

int main(void) {
    system("chcp 1251");  // ��������� Cyrillic1251

    SOCKET sockfd, newsockfd;
    sockaddr_in serv_addr, cli_addr;
    int clilen;
    HANDLE thread;

    // �������� ������
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("������ ��� �������� ������");
        exit(1);
    }

    // ���������� ��������� ������ �������
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = htons(PORT);

    // �������� ������ � ������ �������
    if (bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("������ ��� �������� ������ � ������");
        exit(1);
    }

    // �������� �����������
    listen(sockfd, MAX_CONNECTIONS);
    printf("������ ������� � ������� �����������...\n");

    // ��������� ����������� ��������
    while (1) {
        clilen = sizeof(cli_addr);

        // �������� �����������
        newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen);
        if (newsockfd < 0) {
            printf("������ ��� �������� �����������");
            exit(1);
        }

        // �������� ��������� � ������� �������
        ClientData* client = calloc(1, sizeof(ClientData));
        client->sockfd = newsockfd;
        client->addr = cli_addr;
        client->mutex = mutex_init(sockfd);

        // �������� ������ ������ ��� ��������� �������
        thread = CreateThread(NULL, 0, handleClient, (LPVOID)client, 0, NULL);
        if (thread == NULL) {
            printf("������ ��� �������� ������");
            free(client);
            exit(1);
        }
    }

    // �������� ������ �������
    closesocket(sockfd);
    WSACleanup();

    return 0;
}
