#include "sws.h"

int parse_http_request(const char *raw_data, HttpRequest *req) {
    (void)raw_data; (void)req;
    return 0;
}
const char *get_status_message(int status_code) {
    (void)status_code;
    return "OK";
}
