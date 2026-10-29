#include "sws.h"

/*
 * Hàm xác định kiểu MIME dựa trên phần mở rộng của tệp
 */
static const char *get_mime_type(const char *path) {
    if (strstr(path, ".html") || strstr(path, ".htm")) return "text/html";
    if (strstr(path, ".txt"))  return "text/plain";
    if (strstr(path, ".jpeg") || strstr(path, ".jpg")) return "image/jpeg";
    if (strstr(path, ".png"))  return "image/png";
    return "application/octet-stream";
}

/*
 * Hàm ghi nhật ký truy cập theo định dạng chuẩn Apache Combined Log / RFC
 */
void log_access(const char *logfile, const char *client_ip, const HttpRequest *req, int status, long bytes) {
    FILE *fp = logfile ? fopen(logfile, "a") : stdout;
    if (!fp) return;

    time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%d/%b/%Y:%H:%M:%S %z", localtime(&now));

    fprintf(fp, "%s - - [%s] \"%s %s %s\" %d %ld\n",
            client_ip, time_str, req->method, req->uri, req->version, status, bytes);

    if (logfile) fclose(fp);
}

/*
 * Hàm xử lý phản hồi yêu cầu HTTP và truyền tệp tĩnh
 */
void handle_http_response(int client_fd, const HttpRequest *req, const ServerConfig *cfg) {
    char full_path[2048];
    snprintf(full_path, sizeof(full_path), "%s%s", cfg->dir, req->uri);

    /* 1. Kiểm tra an toàn: Chặn tấn công duyệt thư mục (Directory Traversal) */
    if (strstr(req->uri, "..")) {
        char err_res[] = "HTTP/1.0 403 Forbidden\r\nContent-Length: 9\r\n\r\nForbidden";
        write(client_fd, err_res, strlen(err_res));
        log_access(cfg->logfile, "127.0.0.1", req, 403, 9);
        return;
    }

    /* 2. Mở file yêu cầu */
    FILE *f = fopen(full_path, "rb");
    if (!f) {
        char not_found[] = "HTTP/1.0 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
        write(client_fd, not_found, strlen(not_found));
        log_access(cfg->logfile, "127.0.0.1", req, 404, 9);
        return;
    }

    /* 3. Lấy kích thước file */
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    /* 4. Gửi HTTP Response Header chuẩn */
    char header[512];
    snprintf(header, sizeof(header),
             "HTTP/1.0 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Connection: close\r\n\r\n",
             get_mime_type(full_path), fsize);
    write(client_fd, header, strlen(header));

    /* 5. Nếu là phương thức GET thì gửi toàn bộ nội dung file */
    if (strcmp(req->method, "GET") == 0) {
        char send_buf[BUFFER_SIZE];
        size_t n;
        while ((n = fread(send_buf, 1, sizeof(send_buf), f)) > 0) {
            write(client_fd, send_buf, n);
        }
    }

    fclose(f);
    log_access(cfg->logfile, "127.0.0.1", req, 200, fsize);
}
