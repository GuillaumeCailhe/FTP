#define DEFAULT_PORT 2121

#define TAILLE_MAX_BLOC 1024

#include "csapp.h"
#include "readcmd.h"
#include "time.h"



int main(int argc, char **argv)
{
    int clientfd;
    char *host, buf[TAILLE_MAX_BLOC];
    rio_t rio;
    int fd;
    char nomFichier[50], nomDossier[20];
    struct cmdline *l;
    int taille, taille_totale, taille_attendue, taille_restante;
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
        
        char *pos;
        if ((pos=strchr(l->seq[0][0], '\n')) != NULL) {
            *pos = '\0';
        }

        if(strcmp(l->seq[0][0],"bye")==0){
            printf("Au revoir !\n");
            Close(clientfd);
            exit(0);
        }
        else if(strcmp(l->seq[0][0],"get")==0){
            // Envoi de la commande
            Rio_writen(clientfd, buf, strlen(buf));
            
            // Récupération de la réponse
            debut = clock();
            Rio_readnb(&rio,&taille_totale,sizeof(int));

            if(taille_totale == -1){
                printf("Fichier inexistant.\n");
            } else {
                // Ouverture du fichier dans lequel on veut écrire
                // AMELIORER : il ne faut l'ouvrir que si le fichier est trouvé côté serveur
                strcpy(nomDossier,"client/");
                strcpy(nomFichier,l->seq[0][1]);    
                strcat(nomDossier,nomFichier);
                char *pos;
                if ((pos=strchr(nomDossier, '\n')) != NULL) {
                    *pos = '\0';
                }
                fd = open(nomDossier,O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG);
                
                taille_restante = taille_totale;
                taille_attendue = (TAILLE_MAX_BLOC < taille_restante ? TAILLE_MAX_BLOC : taille_restante);
                while (taille_restante > 0 && (taille = Rio_readnb(&rio, buf, taille_attendue)) > 0) {
                    taille_restante -= taille;
                    printf("%d\n",taille);
                    write(fd,buf,taille);
                    taille_attendue = (TAILLE_MAX_BLOC < taille_restante ? TAILLE_MAX_BLOC : taille_restante);
                } 
                fin = clock();
                close(fd);

                if(taille_restante > 0){
                    printf("Fichier incomplet, manque %d octets\n", taille_restante);
                } else {
                    printf("Fichier reçu ! %d octets en %d ms => %f ko/s\n",taille_totale,(int) (fin-debut),(float) (taille/ (1+fin-debut)));
                }
            }
        }

    
        printf("ftp> ");
    }   
    Close(clientfd);
    exit(0);
}
