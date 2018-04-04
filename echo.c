/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include "readcmd.h"
#include <stdio.h>

#define TAILLE_MAX_BLOC 1024

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
	int fd;
	struct cmdline *l;
	char *nomFichier;
	char buffer[TAILLE_MAX_BLOC];
	int taille, taille_restante, fsize;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		//printf("server received %u bytes\n", (unsigned int)n);

		// Retrait du \n dans la commande
        char *pos;
        if ((pos=strchr(buf, '\n')) != NULL) {
            *pos = '\0';
		}

		// Lecture de la commande reçue
	    l = readcmd(buf);
	
		if (l->err) {
			/* Syntax error, read another command */
			printf("Erreur: %s\n", l->err);
		}

	    // Get
		if (strcmp(l->seq[0][0],"get") == 0)  {
			nomFichier = l->seq[0][1];
			printf("Récupération du fichier %s par client %d\n", nomFichier, connfd);
			
			fd = open(nomFichier,O_RDONLY);
			if(fd==-1){
				fsize = -1;
				Rio_writen(connfd, &fsize,sizeof(int));
				printf("Fichier introuvable\n");
			} else { // Fichier trouvé, on l'envoie

				// Récupération et envoi de la taille du fichier
				fsize = lseek(fd, 0, SEEK_END);
				lseek(fd,0,SEEK_SET);
				Rio_writen(connfd, &fsize,sizeof(int));
				taille_restante = fsize;
				while(taille_restante > 0){
					taille = read(fd,buffer,TAILLE_MAX_BLOC);
					taille_restante -= taille;
					Rio_writen(connfd, buffer, taille);	
				}
				printf("Fichier envoyé (%d octets)\n", fsize);
			}

		    close(fd);
		}

    }

    printf("Client %d deconnecté\n",connfd);

}

