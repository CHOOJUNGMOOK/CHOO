#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")    //윈도우즈 소켓 프로그래밍용 라이브러리    

#define _CRT_SECURE_NO_WARNINGS
#define LINE		1024
#define MAXCLIENT	5
#define	SERV_PORT	10080

int maxfdp1;
int chatuser;	// 채팅 참가자 수
unsigned int client_s[MAXCLIENT];
char *escape = "/quit";	// 종료 명령




						/* i번째 유저 계정삭제 */
void disconCli(int i)
{
	closesocket(client_s[i]);	// i번째 소켓 닫음
	if (i == chatuser - 1) {
		client_s[i] = client_s[chatuser - 1];
		chatuser--;		// 총유저수 줄임
		printf("%d users connected\n", chatuser);
	}
}

/* 현재 채팅 참가자수를 검색 */
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

	SOCKET serversocket, clientsocket, clilen;		// 소켓 변수
	struct sockaddr_in clientd_addr, server_addr;	// 소켓 주소
	int	iError;

	WSADATA wsa;		// 윈도우즈 소켓선언
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {		// 소켓 초기화
		exit(1);
	}

	if ((serversocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {		// 소켓 생성
		printf("fail make socket\n");
		exit(0);
	}

	port = SERV_PORT;
	ZeroMemory(&server_addr, sizeof(server_addr));		// 초기화
	server_addr.sin_family = AF_INET;			// 소켓 옵션 지정
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if (bind(serversocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("fail bind\n");
		exit(0);
	}

	listen(serversocket, SOMAXCONN);	// 연결 대기, SOMAXCONN : 최대 연결수
	maxfdp1 = serversocket + 1;			// 최대소켓 + 1

	while (1)
	{
		FD_ZERO(&read_fds);		// 파일 디스크립터 초기화
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
			// 파일디스크립터에 읽을 데이터가 있으면
			addrlen = sizeof(clientd_addr); // 주소길이 지정
											// 소켓 연결
			clilen = sizeof(clientd_addr);
			clientsocket = accept(serversocket, (struct sockaddr *)&clientd_addr, &addrlen);

			if (clientsocket == -1) {
				printf("fail accept\n");
				exit(0);
			}

			client_s[chatuser] = clientsocket;
			chatuser++;
						
		}

		// 브로드 캐스팅
		for (i = 0; i < chatuser; i++) {		// 접속해 있는 유저수만큼
			memset(readline, '\0', LINE);
			if (FD_ISSET(client_s[i], &read_fds)) {		// 파일 디스크립터 버퍼 확인
				if ((n = recv(client_s[i], readline, LINE, 0)) <= 0) {
					disconCli(i);
					continue;
				}

				if (readline[0] == '4') {		// FIXME : 사용자 삭제를 위한 키워드 처리 수정
					disconCli(i); // 유저 계정 삭제
					continue;
				}

				readline[n] = '\0';
				for (j = 0; j < chatuser; j++) {	// 브로드캐스팅

					if (readline[0] != '0' &&readline[0] != '1' &&readline[0] != '2' &&readline[0] != '3' && readline[0] != '4')
					{ 
						printf("%s is connectd\n", readline);

						printf("chat user : %d \n", chatuser);

						char *start = "[Notice] Welcome ";		// 접속알림
						
						send(client_s[j], start, LINE, 0);
						send(client_s[j], readline, LINE, 0);
					}

					if (readline[0] == '1')
					{
						int temp = 0;
						recv(client_s[j], readline, LINE, 0);
		//				printf("유저아이딘 받았다 %s\n", readline);
						strcpy(userID, readline);
		//				printf("이게안되\n", readline);


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

