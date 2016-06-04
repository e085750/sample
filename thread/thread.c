#include <stdio.h>
#include <pthread.h>

struct data_set {
  int id;
  int intv;
};

void *func(void *args)
{
  printf("threds start\n");
  struct data_set *set = (struct data_set*) args;
  int id = set->id;
  int intv = set->intv;
  int i;
  for(i = 0;i < 3;i++) {
    printf("thred %d run. sleep %d\n",id, intv);
    sleep(intv);
  }
  return 0;
}

int main()
{
  printf("hello\n");
  pthread_t threds;
  printf("set date\n");
  struct data_set set;
  set.id = 1;
  set.intv = 3;

  struct data_set set2;
  set2.id = 2;
  set2.intv = 3;

  printf("threds create\n");
  pthread_create(&threds, NULL, &func, (void *)&set);
  pthread_create(&threds, NULL, &func, (void *)&set2);
  pthread_join(threds,NULL);
  return 0;
}
