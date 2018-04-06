#define DEFAULT_PORT 1212

#include "csapp.h"
#include "readcmd.h"
#include "time.h"

void echo(int connfd);

int main(int argc, char **argv)
{
    int esclavefd;
    char *maitre;
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <master>\n", argv[0]);
        exit(0);
    }
    maitre = argv[1];

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    esclavefd = Open_clientfd(maitre, DEFAULT_PORT);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("esclave connecté au maître\n"); 
    
    Rio_readinitb(&rio, esclavefd);
    while(1){
        echo(esclavefd);

    }
    Close(esclavefd);
}
