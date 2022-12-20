// Server side program that sends
// a 'hi client' message
// to every client concurrently

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// PORT number
//#define PORT 4444

typedef struct node {
    char username[20];
    char password[20];
    int status;
	int wrongPasswordCount;
    struct node *next;
} node;

typedef struct data {
	int key;
	char msg[1000];
}data;

node *readFile() {
    FILE *fp;
    fp = fopen("account.txt", "r");
    if (fp == NULL) {
        printf("Error opening file \n");
        exit(1);
    }
    node *head = NULL;
    node *current = NULL;
    char username[20];
    char password[20];
    int status;
    while (!feof(fp)) {
        fscanf(fp, "%s %s %d\n", username, password, &status);
        //printf("%s %s %d \n", username , password, status);
        node *newNode = (node *) malloc(sizeof(node));
        strcpy(newNode->username, username);
        strcpy(newNode->password, password);
        newNode->status = status;
		newNode->wrongPasswordCount = 0;
        newNode->next = NULL;
        if (head == NULL) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }
    }
    fclose(fp);
    return head;
}


void printList(node *head) {
    node *current = head;
    while (current != NULL) {
        printf("%s %s %d \n", current->username, current->password, current->status);
        current = current->next;
    }
    printf("\n");
}


data splitClientResponse(char str[]){
    char msg[1000];
	char temp[1000];
	int key;
	temp[0]  = str[0];
	temp[1] = '\0';
	key = atoi(temp);
    strcpy(msg, str+1);
    printf("key: %d, msg: %s\n", key, msg);
	data d;
	d.key = key;
	strcpy(d.msg, msg);
	return d;
}


void saveToFile(node *head) {
	FILE *fp;
	fp = fopen("account.txt", "w+");
	if (fp == NULL) {
		printf("Error opening file \n");
		exit(1);
	}
	node *current = head;
	while (current != NULL) {
		fprintf(fp, "%s %s %d \n", current->username, current->password, current->status);
		current = current->next;
	}
	fclose(fp);
}
int main(int argc, char *argv[])
{
	int PORT = atoi(argv[1]);
    char buffer[1024];
    node *head = NULL;
    head = readFile();
    printList(head);    

    memset(buffer, '\0', sizeof(buffer));
	// Server socket id
	int sockfd, ret;

	// Server socket address structures
	struct sockaddr_in serverAddr;

	// Client socket id
	int clientSocket;

	// Client socket address structures
	struct sockaddr_in cliAddr;

	// Stores byte size of server socket address
	socklen_t addr_size;

	// Child process id
	pid_t childpid;

	// Creates a TCP socket id from IPV4 family
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// Error handling if socket id is not valid
	if (sockfd < 0) {
		printf("Error in connection.\n");
		exit(1);
	}

	printf("Server Socket is created.\n");

	// Initializing address structure with NULL
	memset(&serverAddr, '\0',
		sizeof(serverAddr));

	// Assign port number and IP address
	// to the socket created
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);

	// 127.0.0.1 is a loopback address
	serverAddr.sin_addr.s_addr
		= inet_addr("127.0.0.1");

	// Binding the socket id with
	// the socket structure
	ret = bind(sockfd,
			(struct sockaddr*)&serverAddr,
			sizeof(serverAddr));

	// Error handling
	if (ret < 0) {
		printf("Error in binding.\n");
		exit(1);
	}

	// Listening for connections (upto 10)
	if (listen(sockfd, 10) == 0) {
		printf("Listening...\n\n");
	}

	int cnt = 0;
	while (1) {

		// Accept clients and
		// store their information in cliAddr
		clientSocket = accept(
			sockfd, (struct sockaddr*)&cliAddr,
			&addr_size);

		// Error handling
		if (clientSocket < 0) {
			exit(1);
		}

		// Displaying information of
		// connected client
		printf("Connection accepted from %s:%d\n",
			inet_ntoa(cliAddr.sin_addr),
			ntohs(cliAddr.sin_port));

		// Print number of clients
		// connected till now
		printf("Clients connected: %d\n\n",
			++cnt);

		// Creates a child process
		if ((childpid = fork()) == 0) {

			// Closing the server socket id
			close(sockfd);
			node *current = NULL;
			while (1){
                recv(clientSocket, buffer, 1024, 0);
                if (strcmp(buffer, "exit") == 0) {
                    printf("Disconnected from %s:%d \n",
                        inet_ntoa(cliAddr.sin_addr),
                        ntohs(cliAddr.sin_port));
                    break;
                }
                else {
                    printf("Client: %s\n", buffer);
					data d = splitClientResponse(buffer);
					if (d.key == 0) {
						//check username
						current = head;
						int found = 0;
						while (current != NULL) {
							if (strcmp(current->username, d.msg) == 0) {
								found = 1;
								break;
							}
							current = current->next;
						}
						if (found == 1) {
							if (current->status == 0){
								send(clientSocket, "User has been blocked. Please try again later.", strlen("User has been blocked. Please try again later."), 0);
							}
							else {
								send(clientSocket, "Password: ", strlen("Password: "), 0);
							}
						}
						else {
							send(clientSocket, "Username not found. Please try again: ", strlen("Username not found. Please try again: "), 0);
						}
					}
					else if (d.key == 1) {
						//check password
						if (strcmp(current->password, d.msg) == 0) {
							if (current->status == 0) {
								send(clientSocket, "User has been blocked. Please try again later.", strlen("User has been blocked. Please try again later."), 0);
							}
							else {
								send(clientSocket, "Login successful.", strlen("Login successful."), 0);
							}
						}
						else {
							current->wrongPasswordCount++;
							if (current->wrongPasswordCount == 3) {
								current->status = 0;
								current->wrongPasswordCount = 0;
								send(clientSocket, "User has been blocked. Please try again later.", strlen("User has been blocked. Please try again later."), 0);
								
							}
							else {
								send(clientSocket, "Password is incorrect. Please try again: ", strlen("Password is incorrect. Please try again: "), 0);
							}
						}
					}

                    bzero(buffer, sizeof(buffer));
                }        
            }
		}
	}

	// Close the client socket id
	close(clientSocket);
	return 0;
}
