// Client Side program to test
// the TCP server that returns
// a 'hi client' message

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// serverPort number
//#define serverPort 4444

//Logic for client
//0 = username
//1 = password

int main(int argc, char *argv[])
{
	//serverPort
	char *serverIp = argv[1];
	int serverPort = atoi(argv[2]);

    //Setup for code
    int count = 0;
    int step = 0; //0 = username, 1 = password

	// Socket id
	int clientSocket, ret;

	// Client socket structure
	struct sockaddr_in cliAddr;

	// char array to store incoming message
	char buffer[1024];

	// Creating socket id
	clientSocket = socket(AF_INET,
						SOCK_STREAM, 0);

	if (clientSocket < 0) {
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	// Initializing socket structure with NULL
	memset(&cliAddr, '\0', sizeof(cliAddr));

	// Initializing buffer array with NULL
	memset(buffer, '\0', sizeof(buffer));

	// Assigning serverPort number and IP address

    struct sockaddr_in serverAddr;
    memset(&serverAddr, '\0', sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);

	// 127.0.0.1 is Loopback IP
	serverAddr.sin_addr.s_addr
		= inet_addr(serverIp);

	// connect() to connect to the server
	ret = connect(clientSocket,
				(struct sockaddr*)&serverAddr,
				sizeof(serverAddr));

	if (ret < 0) {
		printf("Error in connection.\n");
		exit(1);
	}

	printf("Connected to Server.\n");

	while (1) {
        //printf("Enter a message to send to server: ");
        //fgets(buffer, 1024, stdin);
		switch (step) {
			case 0:
				printf("Username: ");
				fgets(buffer, 1024, stdin);
				break;
			case 1:
				//printf("Password: ");
				fgets(buffer, 1024, stdin);
				break;

			case 2:
				printf("Waiting for next action\n");
				pause();
			default:
				break;
		}
        //clear enter key from buffer
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        //printf("Testing: %s\n", buffer);

		//strcat
		char temp[1024];
		if (step == 0) {
			strcpy(temp,"0");
			strcat(temp, buffer);
		}
		else if (step == 1) {
			strcpy(temp,"1");
			strcat(temp, buffer);
		}
		strcpy(buffer, temp);
        // Sending message to server
        send(clientSocket, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            close(clientSocket);
            printf("Disconnected from server.\n");
            break;
        }
        // Receiving message from server
        if (recv(clientSocket, buffer, 1024, 0) < 0) {
            printf("Error in receiving data.\n");
        }
        else {
			if (strcmp(buffer, "Password: ") == 0 || strcmp(buffer, "Password is incorrect. Please try again: ") == 0) {
				printf("%s", buffer);
				step = 1;
			}
			else if (strcmp(buffer, "Login successful.") == 0) {
				printf("%s\n", buffer);
				step = 2;
			}
			else {
				printf("%s\n", buffer);
				step = 0;
			}
            
            memset(buffer, '\0', sizeof(buffer));
        }
	}

	return 0;
}
