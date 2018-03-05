#define DEFAULT_PORT 2121

/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include "readcmd.h"

int main(int argc, char **argv)
{
    int clientfd;
    char *host, buf[MAXLINE];
    rio_t rio;
	int fd;
	char *nomFichier;
	struct cmdline *l;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, DEFAULT_PORT);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);
    
    printf("ftp> ");
    while (Fgets(buf, MAXLINE, stdin) != NULL) {

		l = readcmd(buf);
		// Vérification de la validité de la commande
		if(l->err){
			continue;
		}      
		
		// Envoi de la commande
		Rio_writen(clientfd, buf, strlen(buf));
		// Récupération de la réponse
        if (Rio_readlineb(&rio, buf, MAXLINE) > 0) {
			/***** MODIFIER A LA FIN DU PROJET ******/
			nomFichier = "client/test.txt";	
			//strcat(nomFichier,l->seq[0][1]);
			/****************************************/
			fd = open(nomFichier,O_WRONLY);
			
			write(fd,buf,1024);

        } else { // the server has prematurely closed the connection 
            break;
        }
        printf("ftp> ");
    }
    Close(clientfd);
    exit(0);
}
