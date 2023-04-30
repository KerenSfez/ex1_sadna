#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <chrono>
#include <cstdint>
#include <valarray>



int close_before_exit(char* error, int socket_fd) {
  perror (error);
  close(socket_fd);
  return 1;
}

int set_up_socket(int argc, char const *argv[]) {
  if (argc != 2) {
      std::cerr << "Usage: client <server-ip>" << std::endl;
      return 1;
    }

  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  const char *server_ip = argv[1];

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket");
      return 1;
    }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);

  if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
      return close_before_exit ("innet_pton", sock);
    }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      return close_before_exit ("connect", sock);
    }
    return sock;
}


int main(int argc, char const *argv[]) {
  int valread;
  int sock = set_up_socket (argc, argv);
  if (sock == 1) return 1;

  size_t message_size = 1;
  size_t max_size = std::pow(2, 20);
  bool is_warm_up_phase = true;
  while (message_size <= max_size) {
      char buffer[message_size];
      memset(buffer, 'A', message_size);

      auto start = std::chrono::high_resolution_clock::now();
      size_t total_bytes_sent = 0;
      while (total_bytes_sent < message_size * 100000) {
          int byte_sent = send(sock, buffer, message_size, 0);

          if (byte_sent != message_size) {
              return close_before_exit ("send", sock);
            }
          total_bytes_sent += byte_sent;
        }

      char ack;
      if (recv(sock, &ack, sizeof(ack), 0) != sizeof(ack)) {
          return close_before_exit ("ack", sock);
        }

      if (!is_warm_up_phase)
      {
        auto end = std::chrono::high_resolution_clock::now ();
        double elapsed = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count ();

        double throughput = (message_size * 10000) / elapsed;
        std::cout << message_size << "\t" << throughput << "\tbytes/microseconds" << std::endl;

        message_size *= 2;
      } else {
        is_warm_up_phase = false;
      }
    }

  char server_reply[3];
  valread = read(sock, server_reply, 2);
  server_reply[valread] = '\0';
  std::cout << "Server: " << server_reply << std::endl;
  close(sock);
  return 0;
}
