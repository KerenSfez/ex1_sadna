#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <valarray>


int close_before_exit(char* error, int client_fd, int server_fd) {
  perror (error);
  if (client_fd != -1)
    {
      close (client_fd);
    }
  close(server_fd);
  return 1;
}

int set_up_socket(struct sockaddr_in address) {
  int server_fd;
  int port = 8080;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      perror("socket failed");
      return 1;
    }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      return close_before_exit ((char *)"bind failed", -1, server_fd);
    }

  if (listen(server_fd, 3) < 0) {
      return close_before_exit ((char *)"listen", -1, server_fd);
    }

  return server_fd;
}

int main() {
  int client_fd, valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  int server_fd = set_up_socket(address);
  if (server_fd == 1) return 1;


  bool is_warm_up_phase = true;
  while (true) {
      if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
          return close_before_exit ((char *)"accept", client_fd, server_fd);
        }

      size_t message_size = 1;
      size_t max_size = std::pow(2, 20);

      while (message_size <= max_size) {
          char buffer[message_size];
          size_t total_bytes_sent = 0;
          while (total_bytes_sent < message_size * 10000) {
              int bytes_recv = recv(client_fd, buffer, message_size, 0);
              if (bytes_recv == -1) {
                  return close_before_exit ((char *)"recv", client_fd, server_fd);
                }
              total_bytes_sent += bytes_recv;
            }

          char ack = 0;
          if (send(client_fd, &ack, sizeof(ack), 0) != sizeof(ack)) {
              return close_before_exit ((char *)"ack", client_fd, server_fd);
            }

          if (!is_warm_up_phase) {
              message_size *= 2;
            } else {
              is_warm_up_phase = false;
          }
        }

      send(client_fd, "OK", 2, 0);
      close(client_fd);
      close(server_fd);
    }

  return 0;
}
