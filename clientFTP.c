#define DEFAULT_PORT 2121
#define TAILLE_MAX_FICHIER 1024

#include "csapp.h"
#include "readcmd.h"

int main(int argc, char **argv)
{
    int clientfd;
    char *host, buf[TAILLE_MAX_FICHIER];
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
		int test;
		if ((test = Rio_readnb(&rio, buf, TAILLE_MAX_FICHIER)) > 0) {
			printf("%d",test);
			/***** MODIFIER A LA FIN DU PROJET ******/
			nomFichier = "client/test.txt";	
			//strcat(nomFichier,l->seq[0][1]);
			/****************************************/
			fd = open(nomFichier,O_WRONLY | O_CREAT);

			write(fd,buf,TAILLE_MAX_FICHIER);

        } else { // the server has prematurely closed the connection 
            break;
        }
        printf("ftp> ");
    }
    Close(clientfd);
    exit(0);
}
