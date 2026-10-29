#include "sws.h"

static void daemonize(void) {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    if (setsid() < 0) exit(EXIT_FAILURE);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
    ServerConfig cfg = {DEFAULT_PORT, ".", NULL, 0};
    int opt;

    while ((opt = getopt(argc, argv, "p:d:l:D")) != -1) {
        switch (opt) {
            case 'p': cfg.port = atoi(optarg); break;
            case 'd': cfg.dir = optarg; break;
            case 'l': cfg.logfile = optarg; break;
            case 'D': cfg.debug_mode = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-D] [-d dir] [-l log] [-p port]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!cfg.debug_mode) {
        daemonize();
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) exit(EXIT_FAILURE);

    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(cfg.port);

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 64);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) continue;

        if (fork() == 0) {
            close(server_fd);
            char buffer[BUFFER_SIZE];
            ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                HttpRequest req;
                if (parse_http_request(buffer, &req) == 0) {
                    handle_http_response(client_fd, &req, &cfg);
                }
            }
            close(client_fd);
            exit(0);
        }
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
