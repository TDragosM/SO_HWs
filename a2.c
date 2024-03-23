#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"
#include <semaphore.h>

#include <stdlib.h>

pthread_mutex_t mutex;
int t1_started = 0;
pthread_cond_t t1_cond = PTHREAD_COND_INITIALIZER;
int t2_finished = 0;
pthread_cond_t t2_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t t2_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t t3_mutex = PTHREAD_MUTEX_INITIALIZER;
int t71_finished = 0;
int t3_finished = 0;
pthread_cond_t t71_end_cond =PTHREAD_COND_INITIALIZER;
pthread_cond_t t3_end_cond =PTHREAD_COND_INITIALIZER;
sem_t sem1;
sem_t sem2;


void* thread_func5(void* arg){
    int id = *(int*)arg;
    
   if(id==1){
        info(BEGIN,5,id);
        t1_started = 1;
        while(!t2_finished){

        }
        info(END,5,id);
   }
   else if(id==2){
        while(!t1_started){

        }
        info(BEGIN,5,id);
        info(END,5,id);
        t2_finished=1;
   }
    else if(id == 3){
        // T5.3
        //pthread_mutex_lock(&t3_mutex);
        //while(!t71_finished){
        //    pthread_cond_wait(&t71_end_cond, &t3_mutex);
        //}
        info(BEGIN, 5, id);
        //pthread_mutex_unlock(&t3_mutex);
        info(END, 5, id);

    }
    else{
        info(BEGIN, 5, id);

        info(END, 5, id);
    }
    
    pthread_exit(NULL);
}

#define NUM_THREADS 41


sem_t max_threads_sem;
int counter = 0;

sem_t s1;
sem_t s10;

void* thread_func(void* arg){
    int id = *(int*)arg;
    sem_wait(&s1);
    sem_wait(&max_threads_sem);  // acquire max_threads_sem
    counter++;
    sem_post(&max_threads_sem);
    if(id==10){
        sem_wait(&s10);
    }
    while(counter<4 && id==10){

    }
    info(BEGIN, 4, id);
    if(id!=10){
        sem_wait(&s10);
    }
    //sem_wait(&max_threads_sem);
    
    //sem_post(&max_threads_sem);
    info(END, 4, id);
    counter--;
    sem_post(&s10);
    sem_post(&s1);
    

    pthread_exit(NULL);
}

void* thread_func7(void* arg){
    int id=*(int*)arg;
    
    if(id==1){
        info(BEGIN,7,id);
        //pthread_mutex_lock(&t3_mutex);
        //t71_finished=1;
        //pthread_cond_signal(&t71_end_cond);
        info(END,7,id);
        //pthread_mutex_unlock(&t3_mutex);
    }
    else if(id==2){
        info(BEGIN,7,id);
        info(END,7,id);
    }
    else{
        info(BEGIN,7,id);
        info(END,7,id);
    }
    
    pthread_exit(NULL);
}

int main(){
    init();
    pthread_mutex_init(&mutex, NULL);
    info(BEGIN, 1, 0);
    int pid2;
    pid2=fork();
    if(pid2==0){
        info(BEGIN, 2,0);
        int pid6;
        pid6=fork();
        if(pid6==0){
            info(BEGIN, 6,0);
            int pid7;
            pid7=fork();
            if(pid7==0){
                info(BEGIN, 7,0);
                int id1=1,id2=2,id3=3,id4=4,id5=5;
                pthread_t thread1, thread2, thread3, thread4,thread5;
                pthread_create(&thread1, NULL, thread_func7, &id1);
                pthread_create(&thread2, NULL, thread_func7, &id2);
                pthread_create(&thread3, NULL, thread_func7, &id3);
                pthread_create(&thread4, NULL, thread_func7, &id4);
                pthread_create(&thread5, NULL, thread_func7, &id5);
                pthread_join(thread1, NULL);
                pthread_join(thread2, NULL);
                pthread_join(thread3, NULL);
                pthread_join(thread4, NULL);
                pthread_join(thread5, NULL);
                info(END, 7,0);
            }
            
            else{
            waitpid(pid7,NULL,0);
            info(END, 6,0);
            }
            
        }
        
        else{
            waitpid(pid6,NULL,0);
            info(END, 2,0);
        }
        
    }

    else{
        int pid3;
        pid3=fork();
        if(pid3==0){
            info(BEGIN, 3,0);
            int pid5=fork();
            if(pid5==0){
                info(BEGIN, 5,0);
                int id1 = 1, id2 = 2, id3 = 3, id4 = 4;
            pthread_t thread1, thread2, thread3, thread4;
            pthread_create(&thread1, NULL, thread_func5, &id1);
            pthread_create(&thread2, NULL, thread_func5, &id2);
            pthread_create(&thread3, NULL, thread_func5, &id3);
            pthread_create(&thread4, NULL, thread_func5, &id4);
            pthread_join(thread1, NULL);
            pthread_join(thread2, NULL);
            pthread_join(thread3, NULL);
            pthread_join(thread4, NULL);
            pthread_mutex_destroy(&mutex);
                info(END, 5,0);
            }
            else{
                waitpid(pid5,NULL,0);
                info(END, 3,0);
            }
        }
        else{
            int pid4;
            pid4=fork();
            if(pid4==0){
                info(BEGIN,4,0);
                sem_init(&s1,0,4);
                sem_init(&max_threads_sem, 0, 1);
                sem_init(&s10,0,1);
    
    int id[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    int i = 0;
    while( i < NUM_THREADS) {
        id[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_func, &id[i]);
        i++;
    }
    
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }
    info(END,4,0);

    sem_destroy(&s1);
    sem_destroy(&max_threads_sem);
   
                
            }
            else{
                waitpid(pid2,NULL,0);
                waitpid(pid3,NULL,0);
                waitpid(pid4,NULL,0);
                info(END, 1,0);
            }
        }
    }
    
    
    return 0;
}
