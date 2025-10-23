#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <pthread.h>
#include <asm/prctl.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>


extern int arch_prctl(int code, unsigned long addr);


#define READ_THREAD_VARIABLE(name) \
({ \
uint64_t v; \
size_t off = offsetof(pthread_block, name); \
asm volatile("movq %%gs:%1, %0" : "=r"(v) : "m"(*(uint64_t *)off)); \
(typeof(((pthread_block*)0)->name))v; \
})


#define WRITE_THREAD_VARIABLE(name, value) \
do { \
size_t off = offsetof(pthread_block, name); \
uint64_t v = (uint64_t)(value); \
asm volatile("movq %0, %%gs:%1" : : "r"(v), "m"(*(uint64_t *)off)); \
} while(0)


#define PER_THREAD_MEMORY_START typedef struct pthread_block {

#define PER_THREAD_MEMORY_END } pthread_block;


PER_THREAD_MEMORY_START
    int i;
    short j;
PER_THREAD_MEMORY_END

void* the_thread(void* dummy) {

    unsigned long me = (unsigned long)dummy;

    //mappo area riservata del thread
    void *thread_area = mmap(NULL, sizeof(pthread_block), PROT_WRITE|PROT_READ, MAP_ANONYMOUS| MAP_PRIVATE, 0 , 0);
    if (thread_area==MAP_FAILED) {
        printf("thread area install error\n");
        exit(1);
    }

    //faccio puntare GS a tale area riservata
    arch_prctl(ARCH_SET_GS, (unsigned long)thread_area);

    if (me==0) {
        WRITE_THREAD_VARIABLE(i, 3);
        WRITE_THREAD_VARIABLE(j, 33);
        printf("thread %lu variable i: %d \n", me, READ_THREAD_VARIABLE(i));
        printf("thread %lu variable j: %d \n", me, READ_THREAD_VARIABLE(j));
    }else {
        WRITE_THREAD_VARIABLE(i, 1);
        WRITE_THREAD_VARIABLE(j, 11);
        printf("thread %lu variable i: %d \n", me, READ_THREAD_VARIABLE(i));
        printf("thread %lu variable j: %d \n", me, READ_THREAD_VARIABLE(j));
    }

    return NULL;

}




int main(void) {

    pthread_t tid;

    pthread_create(&tid,NULL,the_thread,(void*)0);
    pthread_create(&tid,NULL,the_thread,(void*)1);
    pause();

}