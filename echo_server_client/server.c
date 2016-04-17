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

int
main()
{
    int sock0;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t len;
    int sock;
    char buf[64]; 
    uint32_t a[2];
    sock0 = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));
    listen(sock0, 5);
    len = sizeof(client); 
    while(1){
      memset(buf, 0, sizeof(buf));
      memset(&a, 0, sizeof(a));
      printf("connection wait...\n");
      sock = accept(sock0, (struct sockaddr *)&client, &len);
      printf("connected\n");
      int n = recv(sock, buf, sizeof(buf), 0);
      memcpy(a, buf, sizeof(a));
      printf("get message -> %d, %s\n", n, buf);
      printf("a0 %x\n", a[0]);
      a[0] = ntohl(a[0]);
      printf("ntoh a0 %x\n", a[0]);
      printf("a1 %x\n", a[1]);
      //a[1] = htonl(a[1]);
      //printf("hton a1 %x\n", a[1]);

      write(sock, buf, sizeof(buf));
      printf("sleep 1s...\n");
      sleep(1);
    }

    close(sock);
    close(sock0);

    return 0;
}
