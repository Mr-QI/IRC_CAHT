#ifndef  __IO_HPP
#define  __IO_HPP

#include "head.hpp"

/* thread task*/
typedef struct{
    void *(*function)(void*);
    void *argc;

}threadpoll_task_t;

/*manager thread poll*/
struct threadpoll_t{
    pthread_mutex_t lock;               //lock the whole struct
    pthread_mutex_t thread_number;      //lock numbers used for the busy thread 
    pthread_cond_t  queue_not_full;     //conditional variable for thread queue is not full
    pthread_cond_t  queue_not_empty;    //conditional variable for thread queue is not empty
    pthread_t       *tid;               //store all thread pid and manage thread array
    pthread_t       admin_tid;          // thread pid of administrator

    /**/
    
};

int 

#endif // ! __IO_HPP
