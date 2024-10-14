#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int flag = 1;
int current_thread = 1;
int thread3_ready = 0;

void *thread1(void *arg)
{
    printf("Thread 1 started\n");
    while (flag) {
        pthread_mutex_lock(&mutex);
        while (current_thread != 1 && flag) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (!flag) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        for (int i = 0; i < 10; i++) {
            printf(".1.");
            fflush(stdout);
            usleep(100000);
        }
        printf("\nThread 1 finished\n");
        current_thread = 3;  // Always set thread3 to be next
        thread3_ready = 1;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *thread2(void *arg)
{
    printf("Thread 2 started\n");
    while (flag) {
        pthread_mutex_lock(&mutex);
        while (current_thread != 2 && flag) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (!flag) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        for (int i = 0; i < 10; i++) {
            printf(".2.");
            fflush(stdout);
            usleep(100000);
        }
        printf("\nThread 2 finished\n");
        current_thread = 3;  // Always set thread3 to be next
        thread3_ready = 1;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *thread3(void *arg)
{
    printf("Thread 3 started\n");
    while (flag) {
        pthread_mutex_lock(&mutex);
        while ((current_thread != 3 || !thread3_ready) && flag) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (!flag) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        thread3_ready = 0;  // Reset flag since thread3 is running
        for (int i = 0; i < 10; i++) {
            printf(".3.");
            fflush(stdout);
            usleep(100000);
        }
        printf("\nThread 3 finished\n");
        // Alternate between thread 1 and 2
        current_thread = (current_thread == 1) ? 2 : 1;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void)
{
    pthread_t t1, t2, t3;
    
    if (pthread_create(&t1, NULL, thread1, NULL) != 0 ||
        pthread_create(&t2, NULL, thread2, NULL) != 0 ||
        pthread_create(&t3, NULL, thread3, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    sleep(10);  // Let the threads run for 10 seconds
    
    pthread_mutex_lock(&mutex);
    flag = 0;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
