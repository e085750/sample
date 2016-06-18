#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>

#define FD_SIZE 1024
#define MAX_EVENTS 1024
#define EP_TIMEOUT 10000

struct data_set {
  int epfd;
  struct epoll_event *ev;
};

void *func(void *args)
{
  printf("threds start\n");
  char buf[64]; 
  uint32_t a[2];
  struct data_set *set = (struct data_set*) args;
  int epfd = set->epfd;
  struct epoll_event *ev = set->ev;
  struct epoll_event events[FD_SIZE];
  while(1) {
    int nfd = epoll_wait(epfd, events, MAX_EVENTS, EP_TIMEOUT);
    if (nfd == 0) {
      printf("accepted epoll wait timeout continue...\n");
      continue;
    }
    int i;
    for (i=0; i < nfd; i++) {
      printf("accepted event happened\n");
      memset(buf, 0, sizeof(buf));
      memset(&a, 0, sizeof(a));
      int sock = events[i].data.fd;
      int n = recv(sock, buf, sizeof(buf), 0);
      if (n == -1) {
        printf("error. %d %s\n", errno, strerror(errno));
        epoll_ctl(epfd, EPOLL_CTL_DEL, sock, ev);
        close(sock);
        break;
      }
      memcpy(a, buf, sizeof(a));
      printf("get message -> %d, %s\n", n, buf);
      printf("a0 %x\n", a[0]);
      a[0] = ntohl(a[0]);
      printf("ntoh a0 %x\n", a[0]);
      printf("a1 %x\n", a[1]);
      //a[1] = htonl(a[1]);
      //printf("hton a1 %x\n", a[1]);

      //TODO polling send
      n = send(sock, buf, sizeof(buf), 0);
      if (n == -1) {
        printf("send erroo. %d %s", errno, strerror(errno));
        epoll_ctl(epfd, EPOLL_CTL_DEL, sock, ev);
        close(sock);
        break;
      }
    }
  }
  return NULL;
}

int
main()
{
  int sock0;
  struct sockaddr_in addr;
  struct sockaddr_in client;
  socklen_t len;
  int sock;
  sock0 = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET; 
  addr.sin_port = htons(12345);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
  listen(sock0, 5);
  len = sizeof(client); 

  int epfd = epoll_create(FD_SIZE);
  struct epoll_event ev;
  memset(&ev, 0, sizeof ev);

  int epfd_ac = epoll_create(FD_SIZE);
  struct epoll_event ev_ac;
  memset(&ev_ac, 0, sizeof ev_ac);

  ev.events = EPOLLIN;
  ev.data.fd = sock0;
  epoll_ctl(epfd, EPOLL_CTL_ADD, sock0, &ev);
  struct epoll_event events[FD_SIZE];

  struct data_set set;
  set.epfd = epfd_ac;
  set.ev = &ev_ac;
  pthread_t thread;
  pthread_create(&thread, NULL, &func, (void *)&set);

  while(1){
    printf("connection wait...\n");
    int nfd = epoll_wait(epfd, events, MAX_EVENTS, EP_TIMEOUT);
    if (nfd == 0) {
      printf("epoll wait timeout continue...\n");
      continue;
    }
    int i;
    for (i=0; i < nfd; i++) {
      printf("event happened\n");
      if (events[i].data.fd == sock0) {
        sock = accept(sock0, (struct sockaddr *)&client, &len);
        printf("connected\n");
        ev_ac.events = EPOLLIN;
        ev_ac.data.fd = sock;
        epoll_ctl(epfd_ac, EPOLL_CTL_ADD, sock, &ev_ac);
      }
    }
  }

  pthread_join(thread, NULL);
  close(sock);
  close(sock0);

  return 0;
}
