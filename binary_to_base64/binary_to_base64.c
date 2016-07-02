#include <stdio.h>

#define SRC_BUF_MAX 1024*10
#define DST_BUF_MAX SRC_BUF_MAX*3/4+5
#define TBL_SIZE 64

static const char cnvTbl[TBL_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int b64Encode(unsigned char *srcStr, int srcLen, unsigned char *dstStr)
{
  int remain;
  int encLen;
  unsigned int i;
  unsigned char tmp[4];

  /* check params */
  if ((NULL == srcStr) || (0 >= srcLen) || (NULL == dstStr))
  {
#ifdef __DEBUG__
    printf("[ERR] param error.\n");
#endif /* __DEBUG__ */
    return -1;  /* FAIL */
  }

  /* initialize */
  remain = srcLen;
  encLen = 0;
  i = 0;

  /* encode */
  while (0 < remain)
  {
    if (3 <= remain)
    {
      tmp[0] = ((srcStr[3*i + 0] >> 2) & 0x3F);
      tmp[1] = (((srcStr[3*i + 0] << 4) & 0x30) | ((srcStr[3*i + 1] >> 4) & 0xF));
      tmp[2] = (((srcStr[3*i + 1] << 2) & 0x3C) | ((srcStr[3*i + 2] >> 6) & 0x3));
      tmp[3] = (srcStr[3*i + 2] & 0x3F);
      dstStr[4*i + 0] = cnvTbl[tmp[0]];
      dstStr[4*i + 1] = cnvTbl[tmp[1]];
      dstStr[4*i + 2] = cnvTbl[tmp[2]];
      dstStr[4*i + 3] = cnvTbl[tmp[3]];
      remain -= 3;
    }
    else if (2 == remain)
    {
      tmp[0] = ((srcStr[3*i + 0] >> 2) & 0x3F);
      tmp[1] = (((srcStr[3*i + 0] << 4) & 0x30) | ((srcStr[3*i + 1] >> 4) & 0xF));
      tmp[2] = ((srcStr[3*i + 1] << 2) & 0x3C);
      dstStr[4*i + 0] = cnvTbl[tmp[0]];
      dstStr[4*i + 1] = cnvTbl[tmp[1]];
      dstStr[4*i + 2] = cnvTbl[tmp[2]];
      dstStr[4*i + 3] = '=';
      remain -= 2;
    }
    else  /* 1 == remain */
    {
      tmp[0] = ((srcStr[3*i + 0] >> 2) & 0x3F);
      tmp[1] = ((srcStr[3*i + 0] << 4) & 0x30);
      dstStr[4*i + 0] = cnvTbl[tmp[0]];
      dstStr[4*i + 1] = cnvTbl[tmp[1]];
      dstStr[4*i + 2] = '=';
      dstStr[4*i + 3] = '=';
      remain -= 1;
    }

    encLen += 4;
    i++;
  }

  /* SUCCESS */
  return encLen;
}

int
main(int ac, char *av[])
{
  if (ac != 2) {
    printf("invalid cmd args\n");
    return 1;
  }

  FILE *fp = fopen(av[1], "rb");
  if( fp == NULL ){
    printf( "can not open %s¥n", av[1] );
    return -1;
  }

  unsigned char src_buf[SRC_BUF_MAX];
  int size = fread(src_buf, sizeof( unsigned char ), SRC_BUF_MAX, fp);

  unsigned char dst_buf[DST_BUF_MAX];
  int base64_len = b64Encode(src_buf, size, dst_buf);

  int i;
  for( i=0; i<base64_len; i++ ){
    if((i % 76 == 0) && i != 0) printf("\r\n");
    printf( "%c", dst_buf[i] );
  }
  

  fclose( fp );
  return 0;

}
