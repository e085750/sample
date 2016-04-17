#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int
main()
{
    struct sockaddr_in server;
    int sock;
    char buf[64];
    char message[64];
    int n;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    char c[] = "hogehoge";
    memset(message, 0, sizeof(message));
    memcpy(message, c, sizeof(c));
    printf("connecting...\n");
    connect(sock, (struct sockaddr *)&server, sizeof(server));
    printf("connected\n");
    printf("sending message...\n");
    write(sock, message, sizeof(c));
    printf("sent message (%s)\n", message);
    memset(buf, 0, sizeof(buf));
    n = read(sock, buf, sizeof(buf));
    printf("get message from server -> %d, %s\n", n, buf);
    //close(sock);

    sleep(2);

    uint32_t a[] = {0x41424344, 0x45464748};
    printf("a0 %x\n", a[0]);
    a[0] = htonl(a[0]);
    printf("hton a0 %x\n", a[0]);
    //a[1] = ntohl(a[1]);
    //printf("ntoh a1 %x\n", a[1]);
    printf("a1 %x\n", a[1]);
    //a[2] = ntohl(a[2]);
    //a[2] = htonl(a[2]);
    memset(message, 0, sizeof(message));
    memcpy(message, a, sizeof(a));
    sock = socket(AF_INET, SOCK_STREAM, 0);
    printf("connecting...\n");
    int ret;
    ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (ret != 0) {
      printf("error\n");
      close(sock);
      return(1);
    }
    printf("connected\n");
    printf("sending message...\n");
    write(sock, message, sizeof(a));
    printf("sent message (%s)\n", message);
    memset(buf, 0, sizeof(buf));
    n = read(sock, buf, sizeof(buf));
    printf("get message from server -> %d, %s\n", n, buf);
    close(sock);

    return 0;
}
