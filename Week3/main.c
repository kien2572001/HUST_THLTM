#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


typedef struct Account {
    char name[50];
    char password[50];
    char homepage[100];
    int status;
    int isSignedIn; // 0 = false, 1 = true
} Account;

typedef struct node {
    Account account;
    struct node *next;
} node;


void printMenu();
node *readFile(FILE *f);
void printList(node *head);
void saveToFile(node *head);

//Function 1
void registerAccount(node *head);
//Function 2
void activeAccount(node *head);
//Function 3
void signinAccount(node *head);
//Function 4
void findAccountStatus(node *head);
//Function 5
void changePassword(node *head);
//Function 6
void singoutAccount(node *currentAccount);
//Function 7 8
void changeHomepage(node *head, int function);

int main() {
    int choice = 0;
    node *head = NULL;
    //Openfile
    FILE *fptr = fopen("nguoidung.txt", "r");
    if (fptr == NULL) {
        printf("Open file failed\n");
        exit(1);
    }
    //Read file
    head = readFile(fptr);
    fclose(fptr);
    //printList(head);

    do {
        //printList(head);
        printMenu();
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                registerAccount(head);
                break;
            case 2:
                activeAccount(head);
                break;
            case 3:
                signinAccount(head);
                break;
            case 4:
                findAccountStatus(head);
                break;
            case 5:
                changePassword(head);
                break;
            case 6:
                singoutAccount(head);
                break;
            case 7:
                changeHomepage(head, 1);
                break;
            case 8:
                changeHomepage(head, 2);
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }while(choice == 1 || choice == 2 || choice == 3 || choice == 4 || choice == 5 || choice == 6 || choice == 7 || choice == 8);
    return 0;
}

void printMenu(){
    printf("\nUSER MANAGER PROGRAM\n");
    printf("-------------------------------------\n");
    printf("1. Register\n");
    printf("2. Active\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change password\n");
    printf("6. Sign out\n");
    printf("7. Homepage with domain name\n");
    printf("8. Homepage with IP address\n");
    printf("Your choice (1-8, other to quit): ");
}

node *readFile(FILE *f){
    node *head = NULL;
    node *current = NULL;
    node *newNode = NULL;
    char name[50];
    char password[50];
    char homepage[100];
    int status;
    while (fscanf(f, "%s %s %d %s", name, password, &status,homepage) != EOF) {
        newNode = (node *) malloc(sizeof(node));
        strcpy(newNode->account.name, name);
        strcpy(newNode->account.password, password);
        newNode->account.status = status;
        strcpy(newNode->account.homepage, homepage);
        newNode->account.isSignedIn = 0;
        newNode->next = NULL;
        if (head == NULL) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }
    }
    return head;
}

void saveToFile(node *head){
    FILE *fptr = fopen("nguoidung.txt", "w");
    node *current = head;
    while (current != NULL) {
        fprintf(fptr, "%s %s %d %s\n", current->account.name, current->account.password, current->account.status,current->account.homepage);
        current = current->next;
    }
    fclose(fptr);
}

void printList(node *head){
    node *p = head;
    printf("List of accounts:\n");
    while (p != NULL) {
        printf("%s %s %d %s\n", p->account.name, p->account.password, p->account.status,p->account.homepage);
        p = p->next;
    }
    printf("\n");
}

void registerAccount(node *head){
    node *p = head;
    char name[50];
    char password[50];
    int status;
    printf("Username: ");
    scanf("%s", name);
    while (p != NULL) {
        if (strcmp(p->account.name, name) == 0) {
            printf("Account existed\n");
            return;
        }
        p = p->next;
    }
    printf("Password: ");
    scanf("%s", password);
    status = 2;
    char homepage[100];
    printf("Homepage: ");
    scanf("%s", homepage);

    //check length
    if (strlen(homepage) <5 || strlen(homepage) > 100) {
        printf("Invalid homepage\n");
        return;
    }
    
    node *newNode = (node *) malloc(sizeof(node));
    strcpy(newNode->account.name, name);
    strcpy(newNode->account.password, password);
    strcpy(newNode->account.homepage, homepage);
    newNode->account.status = status;
    newNode->account.isSignedIn = 0;
    newNode->next = NULL;
    if (head == NULL) {
        head = newNode;
    } else {
        p = head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = newNode;
    }
    saveToFile(head);
    printf("Successfully registration. Activation required\n");
}

void activeAccount(node *head){
    node *p = head;
    char name[50];
    char password[50];
    int status;
    printf("Username: ");
    scanf("%s", name);
    printf("Password: ");
    scanf("%s", password);

    node *temp = NULL;
    while (p != NULL) {
        if (strcmp(p->account.name, name) == 0) {
            if (strcmp(p->account.password, password) == 0) {
                temp = p;
            } else {
                printf("Password is incorrect\n");
                return;
            }
        }
        p = p->next;
    }

    if (temp == NULL) {
        printf("Account not existed\n");
        return;
    }

    if (temp->account.status == 1) {
        printf("Account is activated\n");
        return;
    }
    int count = 0;
    char code[50];
    do {
        printf("Code: ");
        scanf("%s", code);
        if (strcmp(code, "20194598") == 0) {
            temp->account.status = 1;
            saveToFile(head);
            printf("Successfully activated\n");
            return;
        }
        else{
            count++;
            printf("Activation code is incorrect\n");
        }
    } while (count < 4);
    printf("Account is blocked\n");
    temp->account.status = 0;
    saveToFile(head);
    return;
}

void signinAccount(node *head){
    node *p = head;
    char name[50];
    char password[50];
    int status;
    printf("Username: ");
    scanf("%s", name);
    node *temp = NULL;
    while (p != NULL) {
        if (strcmp(p->account.name, name) == 0) {
            temp = p;
        }
        p = p->next;
    }
    if (temp == NULL) {
        printf("Cannot find account\n");
        return ;
    }
    if (temp->account.status == 0) {
        printf("Account is blocked\n");
        return ;
    }
    else if (temp->account.status == 2) {
        printf("Account is not activated\n");
        return ;
    }
    if (temp->account.isSignedIn == 1) {
        printf("Account is signed in\n");
        return ;
    }
    int count =0 ;
    do {
        printf("Password: ");
        scanf("%s", password);
        if (strcmp(password, temp->account.password) == 0) {
            printf("Hello %s\n", temp->account.name);
            temp->account.isSignedIn = 1;
            return ;
        }
        else{
            count++;
            printf("Password is incorrect\n");
        }
    } while (count < 4);
    printf("Account is blocked\n");
    temp->account.status = 0;
    saveToFile(head);
    return ;
}    
    

void findAccountStatus(node *head){
    node *p = head;
    char name[50];
    printf("Username: ");
    scanf("%s", name);
    while (p != NULL) {
        if (strcmp(p->account.name, name) == 0) {
            if (p->account.status == 0) {
                printf("Account is blocked\n");
                return;
            }
            else if (p->account.status == 1) {
                printf("Account is activate\n");
                return;
            }
            else if (p->account.status == 2) {
                printf("Account is not activate\n");
                return;
            }
        }
        p = p->next;
    }
    printf("Cannot find account\n");
}

void changePassword(node *head){

    printf("Username: ");
    char name[50];
    scanf("%s", name);
    node *p = head;
    node *temp = NULL;
    while (p != NULL) {
        if (strcmp(p->account.name, name) == 0) {
            temp = p;
        }
        p = p->next;
    }
    if (temp == NULL) {
        printf("Cannot find account\n");
        return;
    }

    if (temp->account.status == 0) {
        printf("Account is blocked\n");
        return;
    }
    else if (temp->account.status == 2) {
        printf("Account is not activated\n");
        return;
    }

    if (temp->account.isSignedIn == 0) {
        printf("Account is not signed in\n");
        return;
    }

    char password[50];
    printf("Password: ");
    scanf("%s", password);
    if (strcmp(password, temp->account.password) == 0) {
        printf("New password: ");
        scanf("%s", password);
        strcpy(temp->account.password, password);
        printf("Password is changed\n");
    }
    else{
        printf("Current password is incorrect. Please try again\n");
    }
    saveToFile(head);
}

void singoutAccount(node *head){
    node *p = head;
    char name[50];
    printf("Username: ");
    scanf("%s", name);
    node *temp = NULL;
    while (p != NULL) {
        if (strcmp(p->account.name, name) == 0) {
            temp = p;
        }
        p = p->next;
    }
    if (temp == NULL) {
        printf("Cannot find account\n");
        return;
    }
    if (temp->account.status == 0) {
        printf("Account is blocked\n");
        return;
    }
    else if (temp->account.status == 2) {
        printf("Account is not activated\n");
        return;
    }
    if (temp->account.isSignedIn == 0) {
        printf("Account is not signed in\n");
        return;
    }
    temp->account.isSignedIn = 0;
    printf("Goodbye %s\n", temp->account.name);
    saveToFile(head);
    return;
}

void changeHomepage(node *head, int function){
    node *p = head;
    printf("Username: ");
    char name[50];
    scanf("%s", name);
    node *temp = NULL;
    while (p != NULL) {
        if (strcmp(p->account.name, name) == 0) {
            temp = p;
        }
        p = p->next;
    }
    if (temp == NULL) {
        printf("Cannot find account\n");
        return;
    }
    if (temp->account.status == 0) {
        printf("Account is blocked\n");
        return;
    }
    else if (temp->account.status == 2) {
        printf("Account is not activated\n");
        return;
    }
    if (temp->account.isSignedIn == 0) {
        printf("Account is not signed in\n");
        return;
    }

    int type  = function;
    char link[100];
    strcpy(link, temp->account.homepage);
    //printf("type: %d link: %s\n", type, link);

    struct hostent *host_info;
    struct in_addr *address;

    char input[20];
    strcpy(input, link);
    //printf("Will do a reverse DNS query on: %s\n", input);

    if (input[0] >= '0' && input[0] <= '9') {
       if (type==2) {
        printf("Wrong parameter\n");
        return ;
       }
    } else {
        if (type==1) {
        printf("Wrong parameter\n");
        return ;
        }
    }
    printf("Homepage: %s\n", link);
    if (type == 2) {
        host_info = gethostbyname(input);
        if (host_info == NULL) {
            printf("Not found information\n");
            return ;
        }
        else{
            address = (struct in_addr *) (host_info->h_addr);
            printf("Offical IP: %s\n", inet_ntoa(*address));
            int i = 0;
            // printf("Alias IP:\n");
            // while (host_info->h_aliases[i] != NULL) {
            //     printf("%s\n", host_info->h_aliases[i]);
            //     i++;
            // }
        }
    } else if (type == 1) {
        struct sockaddr_in sa;
        socklen_t len;
        int err;
        char hostname[NI_MAXHOST];

        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = inet_addr(input);
        len = sizeof(sa);

        err = getnameinfo((struct sockaddr *) &sa, len, hostname, sizeof(hostname), NULL, 0, 0);
        if (err != 0) {
            printf("Not found information\n");
            return ;
        } else {
            printf("Offical name: %s\n", hostname);
        }

    }
}