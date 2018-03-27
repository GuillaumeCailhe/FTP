#define DEFAULT_PORT 2121
#define TAILLE_MAX_FICHIER 4096

#include "csapp.h"
#include "readcmd.h"
#include "time.h"



int main(int argc, char **argv)
{
    int clientfd;
    char *host, buf[TAILLE_MAX_FICHIER];
    rio_t rio;
	int fd;
	char nomFichier[50], nomDossier[20];
	struct cmdline *l;
	int taille, taille_totale = 0;
	time_t debut,fin;

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
        
        // Ouverture du fichier dans lequel on veut écrire
        // AMELIORER : il ne faut l'ouvrir que si le fichier est trouvé côté serveur
        strcpy(nomDossier,"client/");
        strcpy(nomFichier,l->seq[0][1]);    
        strcat(nomDossier,nomFichier);
        fd = open(nomDossier,O_WRONLY | O_CREAT);

		debut = clock();
		// Récupération de la réponse
		while ((taille = Rio_readnb(&rio, buf, TAILLE_MAX_FICHIER)) > 0) {
            if(taille == 1 && buf[0] == 0){
                break;
            }
            taille_totale += taille;
			write(fd,buf,taille);
        } 
        fin = clock();
        printf("Fichier reçu ! %d octets en %d ms => %f ko/s\n",taille,(int) (fin-debut),(float) (taille/ (1+fin-debut)));
        printf("ftp> ");
    }
    Close(clientfd);
    exit(0);
}
