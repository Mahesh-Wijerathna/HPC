#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 8

char *messages[NUM_THREADS];

struct thread_data
{
   int	thread_id;
   int  sum;
   char *message;
};

struct thread_data thread_data_array[NUM_THREADS];

void *PrintHello(void *threadarg)
{
   sleep(1);
   struct thread_data *my_data = (struct thread_data *) threadarg;
   int taskid = my_data->thread_id;
   int sum = my_data->sum;
   char *hello_msg = my_data->message;
   printf("Thread %d: %s  Sum=%d\n", taskid, hello_msg, sum);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc, t, sum = 0;

   messages[0] = "English: Hello World!";
   messages[1] = "French: Bonjour, le monde!";
   messages[2] = "Spanish: Hola al mundo";
   messages[3] = "Klingon: Nuq neH!";
   messages[4] = "German: Guten Tag, Welt!"; 
   messages[5] = "Russian: Zdravstvuy, mir!";
   messages[6] = "Japan: Sekai e konnichiwa!";
   messages[7] = "Latin: Orbis, te saluto!";

   for(t = 0; t < NUM_THREADS; t++) {
      sum += t;
      thread_data_array[t].thread_id = t;
      thread_data_array[t].sum = sum;
      thread_data_array[t].message = messages[t];
      printf("Creating thread %d\n", t);
      rc = pthread_create(&threads[t], NULL, PrintHello, (void *)&thread_data_array[t]);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

   for(t = 0; t < NUM_THREADS; t++) {
      pthread_join(threads[t], NULL);
   }

   pthread_exit(NULL);
}



/*

┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/d_3_27]
└─$ ./a_3
Creating thread 0
Creating thread 1
Creating thread 2
Creating thread 3
Creating thread 4
Creating thread 5
Creating thread 6
Creating thread 7
Thread 0: English: Hello World!  Sum=0
Thread 2: Spanish: Hola al mundo  Sum=3
Thread 1: French: Bonjour, le monde!  Sum=1
Thread 3: Klingon: Nuq neH!  Sum=6
Thread 4: German: Guten Tag, Welt!  Sum=10
Thread 5: Russian: Zdravstvuy, mir!  Sum=15
Thread 6: Japan: Sekai e konnichiwa!  Sum=21
Thread 7: Latin: Orbis, te saluto!  Sum=28

┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/d_3_27]
*/