#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <chrono>
#include <cstdint>
#include <math.h>
#include <valarray>
#include <sys/time.h>

#define USAGE_INPUT "Usage: client <server-ip>"
#define CONSTANT_PORT 8080
#define SUCCESS 0
#define FAILURE 1
#define NUM_MESSAGES 10000
#define INITIAL_MESSAGE_SIZE 1

int close_before_exit(char* error, int socket_fd) {
  perror (error);
  close(socket_fd);
  return FAILURE;
}

int set_up_socket(int argc, char const *argv[]) {
  if (argc != 2) {
      std::cerr << USAGE_INPUT << std::endl;
      return FAILURE;
    }

  int sock = 0;
  struct sockaddr_in serv_addr;
  const char *server_ip = argv[1];

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      return FAILURE;
    }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(CONSTANT_PORT);

  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
      return close_before_exit ((char *)"innet_pton", sock);
    }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      return close_before_exit ((char *)"connect", sock);
    }
    return sock;
}


int main(int argc, char const *argv[]) {
  int sock = set_up_socket (argc, argv);
  if (sock == FAILURE) return FAILURE;

  size_t message_size = INITIAL_MESSAGE_SIZE;
  size_t max_size = std::pow(2, 20);
  bool is_warm_up_phase = true;
  while (message_size <= max_size) {
      char buffer[message_size];
      memset(buffer, 'A', message_size);

      timeval start, end ;
      gettimeofday (&start, nullptr);
      size_t total_bytes_sent = 0;
      while (total_bytes_sent < message_size * NUM_MESSAGES) {
          int byte_sent = send(sock, buffer, message_size, 0);

          if (byte_sent != message_size) {
              return close_before_exit ((char *)"send", sock);
            }
          total_bytes_sent += byte_sent;
        }

      char ack;
      if (recv(sock, &ack, sizeof(ack), 0) != sizeof(ack)) {
          return close_before_exit ((char *)"ack", sock);
        }

      if (!is_warm_up_phase)
      {
          gettimeofday (&end, nullptr);
          double duration = (double) (end.tv_sec - start.tv_sec)*1000000 + (double) (end.tv_usec - start.tv_usec);

        double throughput = (message_size * NUM_MESSAGES) / duration;
        std::cout << message_size << "\t" << round(throughput * 100000)
        / 100000 << "\tbytes/microseconds" << std::endl;

        message_size *= 2;
      } else {
        is_warm_up_phase = false;
      }
    }

  close(sock);
  return SUCCESS;
}
