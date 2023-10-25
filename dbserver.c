
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include "msg.h"
#define BUF 256

#define DATABASE_FILE "database.txt"

void Usage(char *progname);
int StartServer(unsigned short port, int *ret_fd);
void *HandleClient(void *arg);

void HandlePut(int client_fd, struct msg *message) {
  FILE *file = fopen(DATABASE_FILE, "ab+");
  if (!file) {
    perror("fopen");
    message->type = FAIL;
    write(client_fd, message, sizeof(struct msg));
    return;
  }

  if (fwrite(&message->rd, sizeof(struct record), 1, file) != 1) {
    perror("fwrite");
    message->type = FAIL;
  } else {
    message->type = SUCCESS;
  }

  fclose(file);
  write(client_fd, message, sizeof(struct msg));
}

void HandleGet(int client_fd, struct msg *message) {
  FILE *file = fopen(DATABASE_FILE, "rb");
  if (!file) {
    perror("fopen");
    message->type = FAIL;
    write(client_fd, message, sizeof(struct msg));
    return;
  }

  struct record rd;
  while (fread(&rd, sizeof(struct record), 1, file) == 1) {
    if (rd.id == message->rd.id) {
      memcpy(&message->rd, &rd, sizeof(struct record));
      message->type = SUCCESS;
      write(client_fd, message, sizeof(struct msg));
      fclose(file);
      return;
    }
  }

  message->type = FAIL;
  write(client_fd, message, sizeof(struct msg));
  fclose(file);
}

void *HandleClient(void *arg) {
  int client_fd = *(int *)arg;
  free(arg);

  struct msg message;
  while (1) {
    if (read(client_fd, &message, sizeof(struct msg)) <= 0) {
      break;
    }

    switch (message.type) {
      case PUT:
        HandlePut(client_fd, &message);
        break;

      case GET:
        HandleGet(client_fd, &message);
        break;

      default:
        printf("Invalid message type.\n");
    }
  }

  close(client_fd);
  return NULL;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    Usage(argv[0]);
  }

  unsigned short port = 0;
  if (sscanf(argv[1], "%hu", &port) != 1) {
    Usage(argv[0]);
  }

  int server_fd;
  if (!StartServer(port, &server_fd)) {
    Usage(argv[0]);
  }

  while (1) {
    int *client_fd = malloc(sizeof(int));
    *client_fd = accept(server_fd, NULL, NULL);
    if (*client_fd < 0) {
      perror("accept");
      continue;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, HandleClient, client_fd) != 0) {
      perror("pthread_create");
      close(*client_fd);
      free(client_fd);
    } else {
      pthread_detach(thread);
    }
  }

  close(server_fd);
  return EXIT_SUCCESS;
}
void Usage(char *progname) {
  printf("usage: %s port \n", progname);
  exit(EXIT_FAILURE);
}

int StartServer(unsigned short port, int *ret_fd) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    return 0;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(server_fd);
    return 0;
  }

  if (listen(server_fd, 10) < 0) {
    perror("listen");
    close(server_fd);
    return 0;
  }

  *ret_fd = server_fd;
  return 1;
}
