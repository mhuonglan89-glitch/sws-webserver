#include "sws.h"

void log_access(const char *logfile, const char *client_ip, const HttpRequest *req, int status, long bytes) {
    (void)logfile; (void)client_ip; (void)req; (void)status; (void)bytes;
}
void handle_http_response(int client_fd, const HttpRequest *req, const ServerConfig *cfg) {
    (void)client_fd; (void)req; (void)cfg;
}
