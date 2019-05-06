#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define SOCKET_PATH "./endpoint"

void* task(void* socket) {
  int fd = *(int *) socket;
  char rd_buffer[BUFFER_SIZE];
  char wr_buffer[BUFFER_SIZE];

  memset(rd_buffer, 0, sizeof(rd_buffer));
  memset(wr_buffer, 0, sizeof(wr_buffer));

  fcntl(fd, F_SETFL, O_NONBLOCK);
  snprintf(wr_buffer, BUFFER_SIZE, "Connection confirmed\n");
  send(fd, wr_buffer, BUFFER_SIZE, 0);

  // IO handling
  // Doesn't actually do much right now
  do {
    memset(rd_buffer, 0, sizeof(rd_buffer));
    memset(wr_buffer, 0, sizeof(wr_buffer));

    if(recv(fd, rd_buffer, BUFFER_SIZE, 0) != -1) {
      printf("%d sent: <%s>\n", fd, rd_buffer);
      snprintf(wr_buffer, BUFFER_SIZE, "Recieved: %s", rd_buffer);
    }
    if(strcmp(wr_buffer, "") != 0) {
      send(fd, wr_buffer, BUFFER_SIZE, 0);
    }

  } while(strcmp(rd_buffer, "exit") != 0);

  // Terminates threads and socket
  printf("Thread %ld finished.\n", pthread_self());
  shutdown(fd, SHUT_RDWR);
  pthread_exit(0);
}

int main(int argc, char** argv) {
  int s_fd;
  int c_fd;
  size_t thread_num = (argc > 1) ? strtol(argv[1], NULL, 10) : 256;
  pthread_t tid[thread_num];
  socklen_t s_len;
  socklen_t c_len;
  struct sockaddr_un* s_add = calloc(1, sizeof(struct sockaddr_un));
  struct sockaddr_un* c_add = calloc(1, sizeof(struct sockaddr_un));

  // Setup socket server

  s_len = sizeof(*s_add);
  c_len = sizeof(*c_add);
  s_add->sun_family = AF_UNIX;
  sprintf(s_add->sun_path, SOCKET_PATH);
  unlink(s_add->sun_path);

  if((s_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("Socket creation: failed\n");
    exit(0);
  } else {
    printf("Socket creation: passed\n");
  }
  if(bind(s_fd, (struct sockaddr*) s_add, s_len) == -1) {
    printf("Socket binding: failed\n");
    exit(0);
  } else {
    printf("Socket binding: passed\n");
  }
  if(listen(s_fd, 0) == -1) {
    printf("Socket listen: failed\n");
    exit(0);
  } else {
    printf("Socket listen: passed\n");
  }

  // Terminating condition undecided
  // Handles connections
  for(u_int32_t i = 0; 1; i++) {
    if((c_fd = accept(s_fd, (struct sockaddr*) c_add, &c_len)) == -1) {
      printf("Socket accept: failed\n");
      exit(0);
    } else {
      printf("Socket accept: passed\n");
      if(pthread_create(&tid[i % thread_num], NULL, task, (void*) &c_fd) == -1) {
        printf("Thread creation (%d): failed \n", i);
        exit(0);
      } else {
        printf("Thread creation (%d): passed\n", i);
      }
    }
  }

  // Ends
  shutdown(s_fd, SHUT_RDWR);
  return 0;
}
