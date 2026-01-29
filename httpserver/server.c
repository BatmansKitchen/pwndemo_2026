#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define PORT 8080
#define BUF_SIZE 1024

void send_response(int client_sock, const char *status, const char *content_type, const char *body) {
    dprintf(client_sock,
        "HTTP/1.0 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n%s",
        status, content_type, strlen(body), body);
}

void list_directory(int client_sock, const char *dir_path, const char *url_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        send_response(client_sock, "403 Forbidden", "text/plain", "Access denied.");
        return;
    }

    char html[BUF_SIZE] = "<html><body><h1>Directory listing</h1><ul>";
    struct dirent *entry;

    // Add a link to parent directory if not root
    if (strcmp(url_path, "/") != 0) {
        strcat(html, "<li><a href=\"../\">../</a></li>");
    }

    while ((entry = readdir(dir)) != NULL) {

        char entry_path[1024];
        snprintf(entry_path, sizeof(entry_path), "%s%s", url_path, entry->d_name);

        struct stat st;
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        stat(full_path, &st);

        // trailing slash for directories
        if (S_ISDIR(st.st_mode)) {
            strcat(entry_path, "/");
        }

        char line[1024];
        if (entry != NULL) {
            snprintf(line, sizeof(line), "<li><a href=\"%s\">%s</a></li>", entry_path, entry->d_name);
            strcat(html, line);
        }
    }
    strcat(html, "</ul></body></html>");
    closedir(dir);

    send_response(client_sock, "200 OK", "text/html", html);
}

void serve_path(int client_sock, const char *url_path) {
    char disallow[32] = "./private";
    char path[32] = ".";
    strncat(path, url_path, 64); // sus???

    printf("%s %s\n", disallow, path);

    // security check
    if (strstr(path, "..") || strncmp(disallow, path, strlen(disallow)) == 0) {
        send_response(client_sock, "403 Forbidden", "text/plain", "Forbidden");
        close(client_sock);
        return;
    }

    // terminate url at '?' if it exists
    for (int i = 0; i < 32; i++) {
        if (path[i] == '?') {
            path[i] = '\0';
            break;
        }
    }

    struct stat st;
    if (stat(path, &st) == -1) {
        send_response(client_sock, "404 Not Found", "text/plain", "File not found.");
        close(client_sock);
        return;
    }

    if (S_ISDIR(st.st_mode)) {
        list_directory(client_sock, path, url_path);
    } else if (S_ISREG(st.st_mode)) {
        int fd = open(path, O_RDONLY);
        if (fd == -1) {
            send_response(client_sock, "403 Forbidden", "text/plain", "Access denied.");
            close(client_sock);
            return;
        }

        dprintf(client_sock,
            "HTTP/1.0 200 OK\r\n"
            "Content-Length: %ld\r\n"
            "Content-Type: text/plain\r\n\r\n",
            st.st_size);

        char buffer[BUF_SIZE];
        ssize_t bytes;
        while ((bytes = read(fd, buffer, BUF_SIZE)) > 0) {
            write(client_sock, buffer, bytes);
        }

        close(fd);
    } else {
        send_response(client_sock, "403 Forbidden", "text/plain", "Unsupported file type.");
    }

    close(client_sock);
}
int main() {
    int server_fd, client_sock;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 32);
    printf("HTTP file server running at http://localhost:%d/\n", PORT);

    while (1) {
        client_sock = accept(server_fd, (struct sockaddr *)&addr, &addrlen);
        if (client_sock < 0) {
            perror("accept failed");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            close(client_sock);
            continue;
        }

        if (pid == 0) {
            // Child process
            close(server_fd);  // Child doesn't need the listener socket

            char buffer[BUF_SIZE];
            ssize_t len = recv(client_sock, buffer, BUF_SIZE - 1, 0);
            if (len <= 0) {
                close(client_sock);
                exit(0);
            }

            buffer[len] = '\0';
            char method[8], path[128];
            sscanf(buffer, "%s %s", method, path);

            printf("request to %s\n", path);

            if (strcmp(method, "GET") == 0) {
                serve_path(client_sock, path);
            } else {
                send_response(client_sock, "501 Not Implemented", "text/plain", "Method not supported.");
                close(client_sock);
            }

            exit(0);  // Exit child process
        } else {
            // Parent process
            close(client_sock);  // Parent doesn't handle the client socket
        }
    }

    close(server_fd);
    return 0;
}