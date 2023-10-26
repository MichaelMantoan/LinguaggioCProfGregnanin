#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 100

int main() {
    
    struct sockaddr_in server, client;
    int server_socket, client_socket,fromlen = sizeof(server);
    char buffer[BUFFER_SIZE];
    char search_string[BUFFER_SIZE];
    int fd[2];
    int c;
    
    memset((char *)&server, 0, sizeof(server));


    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(40000);

    bind(server_socket, (struct sockaddr *)&server, sizeof(server));
    listen(server_socket, 3);

    while(1) {
      
        client_socket = accept(server_socket, (struct sockaddr *)&client, &fromlen);

        if (fork() == 0) {
            close(server_socket);

            // Leggi la stringa di ricerca dal client
            read(client_socket, search_string, sizeof(search_string));

            int temp_fd = open("temp.txt", O_WRONLY | O_CREAT, 0666);
            while (read(client_socket, buffer, sizeof(buffer)) > 0) {
                write(temp_fd, buffer, strlen(buffer));
            }
            close(temp_fd);

            // Usa pipe, grep e wc per contare le occorrenze
            pipe(fd);
            if (fork() == 0) {
                close(fd[0]);
                close(1);
                dup(fd[1]);
                close(fd[1]);
                execl("/bin/grep", "grep", "-o", search_string, "temp.txt", NULL);
                return -1;
            }

            close(fd[1]);
            close(0);
            dup(fd[0]);
            close(fd[0]);
            execl("/usr/bin/wc", "wc", "-l", NULL);
            return -1;
        }
        close(client_socket);
    }
    return 0;
}
