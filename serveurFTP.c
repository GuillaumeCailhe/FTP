#include "csapp.h"
#include "readcmd.h"
#include <stdio.h>
#define MAX_NAME_LEN 256
#define NPROC 1
#define DEFAULT_PORT 2121
#define DEFAULT_PORT_SLAVE 1212
#define MAX_CLIENT 10
#define TAILLE_MAX_BLOC 1024

void handler(int sig){
    kill(-2,SIGKILL);
    kill(getpid(),SIGKILL);
}

int positionProchainClient(int client[]){
    for(int i=0; i<MAX_CLIENT; i++){
        if(client[i] == 0){
            return i;
        }
    }
    return -1;
}

void traiterRequete(int clientfd, int esclavefd){
    char buf[MAXLINE];
    rio_t rioC;
    rio_t rioE;
    int taille_totale, octet_debut, taille_restante, taille, taille_attendue;



    // lecture du message^client
    Rio_readinitb(&rioC, clientfd);
    Rio_readinitb(&rioE, esclavefd);

    // Lecture de la commande reçue
    Rio_readlineb(&rioC, buf, MAXLINE);
    struct cmdline *l;
    l = readcmd(buf);

    // Retransmission de la commande reçue
    Rio_writen(esclavefd,buf,MAXLINE);

    if (strcmp(l->seq[0][0],"get") == 0)  {
        // Retransmission de la taille du fichier (esclave -> client)
        Rio_readnb(&rioE, &taille_totale, sizeof(int));
        Rio_writen(clientfd, &taille_totale, sizeof(int));

        printf("Echange taille fichier ok %d\n", taille_totale);

        if(taille_totale != -1){
            // Retransmission de l'octet de départ (client -> esclave)
            Rio_readnb(&rioC,&octet_debut, sizeof(int));
            Rio_writen(esclavefd, &octet_debut,sizeof(int));
            printf("Echange octet depart ok\n");

            // Retransmission des données (esclave -> client)
            taille_restante = taille_totale-octet_debut;
            taille_attendue = (TAILLE_MAX_BLOC < taille_restante ? TAILLE_MAX_BLOC : taille_restante);
            while(taille_restante > 0 && (taille = Rio_readnb(&rioE, buf, taille_attendue)) > 0){
                taille_restante -= taille;
                printf("%d\n",taille);
                Rio_writen(clientfd, buf, taille); 
                taille_attendue = (TAILLE_MAX_BLOC < taille_restante ? TAILLE_MAX_BLOC : taille_restante);
            }
        }
    }
}

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    Signal(SIGINT,handler);
    
    int clients[MAX_CLIENT]; // Contient les connfd des clients connectés
    int esclaves[NPROC]; // connfd des serveurs esclaves

    int listenfd, connfd;
    /**************** ESCLAVES ******************/

    socklen_t esclavelen;
    struct sockaddr_in esclaveaddr;
    char esclave_ip_string[INET_ADDRSTRLEN];
    char esclave_hostname[MAX_NAME_LEN];
    int cpt_esc = 0; 

    esclavelen = (socklen_t)sizeof(esclaveaddr);
    listenfd = Open_listenfd(DEFAULT_PORT_SLAVE);

    while(cpt_esc < NPROC){
        // connexion nouveau client
        connfd = Accept(listenfd, (SA *)&esclaveaddr, &esclavelen);
        esclaves[cpt_esc] = connfd;
        cpt_esc++;

        /* determine the name of the client */
        Getnameinfo((SA *) &esclaveaddr, esclavelen,
                    esclave_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &esclaveaddr.sin_addr, esclave_ip_string,
                  INET_ADDRSTRLEN);
        
        printf("Nouvel esclave : %s (%s)\n", esclave_hostname,
               esclave_ip_string);

    }

    printf("Tout les esclaves sont connectés, on accepte les connexions.\n");

    /*************** CLIENTS *****************/

    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    int cpt_rr = 0; // compteur round robin, va de 0 à NPROC-1
    int esclavefd;
    int pid;

    clientlen = (socklen_t)sizeof(clientaddr);
    listenfd = Open_listenfd(DEFAULT_PORT);

    int i;
    while(1){
        i = positionProchainClient(clients);
        if(i == -1){
            Close(listenfd);
            continue;
        }
        // connexion nouveau client
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        clients[i] = connfd;

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                  INET_ADDRSTRLEN);
        
        printf("Nouveau client : %s (%s)\n", client_hostname,
               client_ip_string);

        esclavefd = esclaves[cpt_rr];
        if((pid = Fork()) == 0){ // fils
            traiterRequete(connfd,esclavefd);
            exit(0);
        } else { // père
            cpt_rr = (cpt_rr + 1) % NPROC;
        }

        Close(connfd);
        clients[i] = 0;

    }


    exit(0);
}

