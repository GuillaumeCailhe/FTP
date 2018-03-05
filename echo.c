/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include "readcmd.h"
#include <stdio.h>

#define TAILLE_MAX_FICHIER 1024

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;
	struct cmdline *l;
	char *nomFichier;
	FILE *fp;
	char buffer[TAILLE_MAX_FICHIER];

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
			printf("Récupération du fichier %s\n", nomFichier);
			
			fp = fopen(nomFichier, "r");
		    if(fp == NULL) {
	      		printf("Error opening file : %d\n", errno);
		    }



		    if(fgets(buffer, TAILLE_MAX_FICHIER, fp)!=NULL){
				Rio_writen(connfd, buffer, TAILLE_MAX_FICHIER);
				printf("Fichier envoyé !");
		    }



		    fclose(fp);


		}

        //int fd = open(buf, O_RDONLY);
        //printf()
    }
}

