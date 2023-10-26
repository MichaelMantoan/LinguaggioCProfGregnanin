#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>  // per open() e O_RDONLY

#define BUFFER_SIZE 100

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    int file_fd; // Descrittore di file per il file di input
    int nread;

    if (argc != 5) {
        printf("Uso: %s <percorso_file> <stringa_di_ricerca> <indirizzo_ip> <porta>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Impossibile creare il socket.\n");
        exit(1);
    }
    
    memset((char *)&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[3]);
    server.sin_port = htons(atoi(argv[4]));

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Errore nella connessione al server.\n");
        exit(1);
    }

    // Invia la stringa di ricerca al server
    write(sock, argv[2], strlen(argv[2]));

    // Apri il file in modalità di sola lettura
    file_fd = open(argv[1], O_RDONLY);
    if (file_fd < 0) {
        printf("Errore nell'apertura del file.\n");
        close(sock);
        exit(1);
    }

    // Invia il contenuto del file al server usando read()
    while ((nread = read(file_fd, buffer, sizeof(buffer))) > 0) {
        write(sock, buffer, nread);
    }

    close(file_fd);
    close(sock);
    return 0;
}
