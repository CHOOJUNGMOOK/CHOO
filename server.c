#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")    //�������� ���� ���α׷��ֿ� ���̺귯��    

#define _CRT_SECURE_NO_WARNINGS
#define LINE		1024
#define MAXCLIENT	5
#define	SERV_PORT	10080

int maxfdp1;
int chatuser;	// ä�� ������ ��
unsigned int client_s[MAXCLIENT];
char *escape = "/quit";	// ���� ���




						/* i��° ���� �������� */
void disconCli(int i)
{
	closesocket(client_s[i]);	// i��° ���� ����
	if (i == chatuser - 1) {
		client_s[i] = client_s[chatuser - 1];
		chatuser--;		// �������� ����
		printf("%d users connected\n", chatuser);
	}
}

/* ���� ä�� �����ڼ��� �˻� */
int maxuser(int user)
{
	unsigned int max = user;
	int i;

	for (i = 0; i < chatuser; i++) {
		if (max < client_s[i])
			max = client_s[i];
	}

	return max;
}

int main()
{
	char *userID = malloc(sizeof(char) * 20);
	char *filename = malloc(sizeof(char) * 20);


	char readline[LINE];	// read buffer
	int	i, j, n, addrlen;
	unsigned short	port;
	fd_set read_fds;

	SOCKET serversocket, clientsocket, clilen;		// ���� ����
	struct sockaddr_in clientd_addr, server_addr;	// ���� �ּ�
	int	iError;

	WSADATA wsa;		// �������� ���ϼ���
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {		// ���� �ʱ�ȭ
		exit(1);
	}

	if ((serversocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {		// ���� ����
		printf("fail make socket\n");
		exit(0);
	}

	port = SERV_PORT;
	ZeroMemory(&server_addr, sizeof(server_addr));		// �ʱ�ȭ
	server_addr.sin_family = AF_INET;			// ���� �ɼ� ����
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if (bind(serversocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("fail bind\n");
		exit(0);
	}

	listen(serversocket, SOMAXCONN);	// ���� ���, SOMAXCONN : �ִ� �����
	maxfdp1 = serversocket + 1;			// �ִ���� + 1

	while (1)
	{
		FD_ZERO(&read_fds);		// ���� ��ũ���� �ʱ�ȭ
		FD_SET(serversocket, &read_fds);
		for (i = 0; i < chatuser; i++) {
			FD_SET(client_s[i], &read_fds);
		}
		maxfdp1 = maxuser(serversocket) + 1;

		if (select(maxfdp1, &read_fds, (fd_set *)0, (fd_set *)0, NULL) < 0) {
			iError = WSAGetLastError();
			printf("fail select function\n");
			exit(0);
		}

		if (FD_ISSET(serversocket, &read_fds)) {
			// ���ϵ�ũ���Ϳ� ���� �����Ͱ� ������
			addrlen = sizeof(clientd_addr); // �ּұ��� ����
											// ���� ����
			clilen = sizeof(clientd_addr);
			clientsocket = accept(serversocket, (struct sockaddr *)&clientd_addr, &addrlen);

			if (clientsocket == -1) {
				printf("fail accept\n");
				exit(0);
			}

			client_s[chatuser] = clientsocket;
			chatuser++;
						
		}

		// ��ε� ĳ����
		for (i = 0; i < chatuser; i++) {		// ������ �ִ� ��������ŭ
			memset(readline, '\0', LINE);
			if (FD_ISSET(client_s[i], &read_fds)) {		// ���� ��ũ���� ���� Ȯ��
				if ((n = recv(client_s[i], readline, LINE, 0)) <= 0) {
					disconCli(i);
					continue;
				}

				if (readline[0] == '4') {		// FIXME : ����� ������ ���� Ű���� ó�� ����
					disconCli(i); // ���� ���� ����
					continue;
				}

				readline[n] = '\0';
				for (j = 0; j < chatuser; j++) {	// ��ε�ĳ����

					if (readline[0] != '0' &&readline[0] != '1' &&readline[0] != '2' &&readline[0] != '3' && readline[0] != '4')
					{ 
						printf("%s is connectd\n", readline);

						printf("chat user : %d \n", chatuser);

						char *start = "[Notice] Welcome ";		// ���Ӿ˸�
						
						send(client_s[j], start, LINE, 0);
						send(client_s[j], readline, LINE, 0);
					}

					if (readline[0] == '1')
					{
						int temp = 0;
						recv(client_s[j], readline, LINE, 0);
		//				printf("�������̵� �޾Ҵ� %s\n", readline);
						strcpy(userID, readline);
		//				printf("�̰Ծȵ�\n", readline);


						recv(client_s[i], readline, LINE, 0);
						strcpy(filename, readline);

						printf("The global file list is as follows:\n");
						printf("%s/%s\n", userID, filename);

						temp++;
					}

					if (readline[0] == '2')
					{
						recv(client_s[i], readline, LINE, 0);
						printf("%s\n", readline);
					}

					if (readline[0] == '3')
					{
						recv(client_s[i], readline, LINE, 0);
						printf("%s\n", readline);
					}				

				}


			}
		}
	}

	free(userID);
	free(filename);
	closesocket(serversocket);
	WSACleanup();
	return 0;
}

