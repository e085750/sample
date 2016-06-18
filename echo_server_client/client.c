#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

int
main()
{
  int sock;
  char buf[64];
  char message[64];
  int n;


  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  int geta;
  if ((geta = getaddrinfo("127.0.0.1", "12345", &hints, &res)) != 0 ){
    printf("getaadr info error %d\n", geta);
    return 0;
  }

  sock = socket(PF_INET, res->ai_socktype, res->ai_protocol);
  if (sock == -1) {
    printf("socket error.\n");
  }
  
  if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
    printf("fcntl error.\n");
  }

  nfds_t nfds = 2;
  struct pollfd fds[nfds];
  fds[0].fd = sock;
  fds[0].events = POLLOUT;
  fds[1].fd = sock;
  fds[1].events = POLLIN;
  time_t timeout = 1000;
    
  printf("connecting...\n");
  //int ret = connect(sock, res->ai_addr, res->ai_addrlen);
  int ret = connect(sock, res->ai_addr, res->ai_addrlen);
  int e = errno;
  if (ret == -1) {
    if (e != EALREADY)
      printf("connection error. [%d][%s]\n", e, strerror(e));
      printf("addr len %d\n",res->ai_addrlen);
      //return 0;
  }
  while (1) {
    int rt = poll(fds, nfds, timeout);
    if (rt <= 0) {
      printf("poll error. %d\n", rt);
      close(sock);
      return 0;
    }
    if ((fds[1].revents & POLLIN) == 0) {
      printf("not pollin.\n");
    } else {
      memset(buf, 0, sizeof(buf));
      n = recv(sock, buf, sizeof(buf), 0);
      printf("get message from server -> %d, %s\n", n, buf);
      break;
    }


    if ((fds[0].revents & POLLOUT) == 0) {
      printf("not pollout.\n");
    } else {
      uint32_t a[] = {0x41424344, 0x45464748};
      printf("a0 %x\n", a[0]);
      a[0] = htonl(a[0]);
      printf("hton a0 %x\n", a[0]);
      printf("a1 %x\n", a[1]);
      memset(message, 0, sizeof(message));
      memcpy(message, a, sizeof(a));
      printf("sending message...\n");
      int sent;
      if ((sent = send(sock, message, sizeof(a), MSG_NOSIGNAL)) == -1) {
        e = errno;
        printf("send error %d\n", e);
      } else {
        printf("send %d byte.\n", sent);
      }
      printf("sent message (%s)\n", message);
    }

    sleep(2);
  }
  close(sock);
  freeaddrinfo(res);
  printf("end\n");
  return 0;
}
