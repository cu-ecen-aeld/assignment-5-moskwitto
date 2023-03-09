#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9000
#define BACKLOG 5

static int server_fd;
static volatile int keep_running = 1;

void sigint_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        // deleting /var/tmp/aesdsocketdata
        unlink("/var/tmp/aesdsocketdata");
        printf("Caught signal, exiting\n");
        keep_running = 0;
    }
}

int create_socket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        return -1;
    }

   return socket_fd;
}

int bind_socket(int socket_fd) {
    struct sockaddr_in address;
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        close(socket_fd);
        perror("bind");
        return -1;
    }

    return 0;
}

void handle_client(int client_fd) {
    // Get IP address of the client
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    getpeername(client_fd, (struct sockaddr *)&addr, &addrlen);
    char *client_ip = inet_ntoa(addr.sin_addr);

    printf("Accepted connection from %s\n", client_ip);

    FILE *file_ptr = fopen("/var/tmp/aesdsocketdata", "w+");
    if (file_ptr == NULL) {
        perror("fopen");
        close(client_fd);
        return;
    }

    char buffer[1024];
    ssize_t bytes_received;
    do {
        bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            fclose(file_ptr);
            close(client_fd);
            return;
        } else if (bytes_received > 0) {
            fseek(file_ptr, 0, SEEK_END);
            fwrite(buffer, 1, bytes_received, file_ptr);    
        }
    } while (bytes_received > 0);

    fclose(file_ptr);
    close(client_fd);
    printf("Closed connection from %s\n", client_ip);

    return;
}

void run_server() {
    if (create_socket() == -1) {
        exit(EXIT_FAILURE);
    }

    if (bind_socket(server_fd) == -1) {
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (keep_running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR && !keep_running) {
                break;
            } else {
                perror("accept");
                continue;
            }
        }

        handle_client(client_fd);
    }

    close(server_fd);
}

int main(int argc, char *argv[]) {
    if (argc >= 2 && strcmp(argv[1], "-d") == 0) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process - exit successfully
            exit(EXIT_SUCCESS);
        } else {
            // Child process - continue execution
            umask(0);
            if (setsid() == -1) {
                perror("setsid");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Set signal handlers
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    run_server();

    return 0;
}



