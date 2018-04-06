#include "csapp.h"

#define MAX_NAME_LEN 256
#define NPROC 2
#define DEFAULT_PORT 2121

void handler(int sig){
    kill(-2,SIGKILL);
    kill(getpid(),SIGKILL);
}

void echo(int connfd);

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    Signal(SIGINT,handler);

    // Création des processus fils qui vont gérer les connexions
    pid_t pid;
    for(int i = 0; i < NPROC; i++){
        if((pid = Fork()) == 0){
            break;
        }
    }

    if (pid == 0){
        while (1) {
        
        }
    } else {
        int listenfd, connfd;
        socklen_t clientlen;
        struct sockaddr_in clientaddr;
        char client_ip_string[INET_ADDRSTRLEN];
        char client_hostname[MAX_NAME_LEN];
        

        clientlen = (socklen_t)sizeof(clientaddr);
        listenfd = Open_listenfd(DEFAULT_PORT);

        while(1){
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            Close(listenfd);
            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                        client_hostname, MAX_NAME_LEN, 0, 0, 0);
            
            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                      INET_ADDRSTRLEN);
            
            printf("server connected to %s (%s)\n", client_hostname,
                   client_ip_string);

            echo(connfd);
            Close(connfd);
            exit(0);
        }
    }


    exit(0);
}
