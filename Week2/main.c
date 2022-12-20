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


int main(int argc, char *argv[])
{
    int type  = atoi(argv[1]);
    char *link = argv[2];
    //printf("type: %d link: %s\n", type, link);

    struct hostent *host_info;
    struct in_addr *address;

    char input[20];
    strcpy(input, link);
    //printf("Will do a reverse DNS query on: %s\n", input);

    if (input[0] >= '0' && input[0] <= '9') {
       if (type==2) {
        printf("Wrong parameter\n");
        return 0;
       }
    } else {
        if (type==1) {
        printf("Wrong parameter\n");
        return 0;
        }
    }

    if (type == 2) {
        host_info = gethostbyname(input);
        if (host_info == NULL) {
            printf("Not found information\n");
            return 1;
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
            return 1;
        } else {
            printf("Offical name: %s\n", hostname);
        }

    }

    return 0;
}