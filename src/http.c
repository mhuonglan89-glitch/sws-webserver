#include "sws.h"

int parse_http_request(const char *raw_data, HttpRequest *req) {
    if (!raw_data || !req) return -1;
    
    memset(req, 0, sizeof(HttpRequest));

    if (sscanf(raw_data, "%15s %1023s %15s", req->method, req->uri, req->version) < 3) {
        return -1;
    }

    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "HEAD") != 0) {
        return 501;
    }

    if (strcmp(req->uri, "/") == 0) {
        strcpy(req->uri, "/index.html");
    }

    return 0;
}

const char *get_status_message(int status_code) {
    switch (status_code) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 501: return "Not Implemented";
        default:  return "Internal Server Error";
    }
}
