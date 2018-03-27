/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include "readcmd.h"
#include <stdio.h>

#define TAILLE_MAX_FICHIER 4096

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
	int fd;
	struct cmdline *l;
	char *nomFichier;
	char buffer[TAILLE_MAX_FICHIER];
	int taille;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		printf("server received %u bytes\n", (unsigned int)n);

		// Retrait du \n dans la demande reçue
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
			printf("Récupération du fichier %s par %d\n", nomFichier, connfd);
			fd = open(nomFichier,O_RDONLY);
			if(fd==-1){
				printf("Fichier introuvable");
			} else {
				while((taille = read(fd,buffer,TAILLE_MAX_FICHIER)) > 0){
					Rio_writen(connfd, buffer, taille);		
				}
				buffer[0] = 0;
				Rio_writen(connfd,buffer,1);
			}

		    close(fd);
		}

    }
}

