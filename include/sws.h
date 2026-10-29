#ifndef SWS_H
#define SWS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 8192

typedef struct {
    int port;
    char *dir;
    char *logfile;
    int debug_mode;
} ServerConfig;

typedef struct {
    char method[16];
    char uri[1024];
    char version[16];
} HttpRequest;

/* Hàm do Người 2 viết */
int parse_http_request(const char *raw_data, HttpRequest *req);
const char *get_status_message(int status_code);

/* Hàm do Người 3 viết */
void handle_http_response(int client_fd, const HttpRequest *req, const ServerConfig *cfg);
void log_access(const char *logfile, const char *client_ip, const HttpRequest *req, int status, long bytes);

#endif
