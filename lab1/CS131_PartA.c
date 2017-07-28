//Timothy Pranoto
//38964311
//CS131 Lab1 Part A

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

/*Global variables */
int num_threads; 
pthread_mutex_t *mutexes;

/* For representing the status of each philosopher */
typedef enum{
	none,   // No forks
	one,    // One fork
	two     // Both forks to consume
} utensil;

/* Representation of a philosopher */
typedef struct phil_data{
	int phil_num;
	int course;
	utensil forks; 
}phil_data;

/* ****************Change function below ***************** */
void *eat_meal(void* arg){
/* 3 course meal: Each need to acquire both forks 3 times.
 *  First try for fork in front.
 * Then for the one on the right, if not fetched, put the first one back.
 * If both acquired, eat one course.
 */
	phil_data* temp = (phil_data*) arg;
	
	while(temp->course<=2){
		int lock1 = pthread_mutex_trylock(&mutexes[temp->phil_num]);
		if(lock1 ==0){
			temp->forks = one;
			int lock2 =  pthread_mutex_trylock(&mutexes[(temp->phil_num+1)%5]);
			if(lock2==0){
				temp->forks = two;
				temp->course+=1;
				sleep(1);
				printf("Philosopher %d is eating(%d times).\n",temp->phil_num+1,temp->course);
			}
			else{
				printf("Philosopher %d can't take fork to the right, dropping fork in front.\n",temp->phil_num+1);
			}
			pthread_mutex_unlock(&mutexes[(temp->phil_num)]);
			temp->forks = one;
			pthread_mutex_unlock(&mutexes[(temp->phil_num+1)%5]);
			temp->forks = none;
		}
	}	
	
	pthread_exit(NULL);
	return NULL;
}

/* ****************Add the support for pthreads in function below ***************** */
int main( int argc, char **argv ){
	int num_philosophers;

	if (argc < 2) {
          fprintf(stderr, "Format: %s <Number of philosophers>\n", argv[0]);
          return 0;
	}
    
	num_philosophers = atoi(argv[1]);
	num_threads = num_philosophers;
	pthread_t threads[num_threads];
	phil_data* philosophers = malloc(sizeof(phil_data)*num_philosophers); //Struct for each philosopher
	mutexes = malloc(sizeof(pthread_mutex_t)*num_philosophers); //Each mutex element represent a fork

	/* Initialize structs */
	for( int i = 0; i < num_philosophers; i++ ){
		philosophers[i].phil_num = i;
		philosophers[i].course   = 0;
		philosophers[i].forks    = none;
	}
/* Each thread will represent a philosopher */

/* Initialize Mutex, Create threads, Join threads and Destroy mutex */
	for(int i=0;i<num_philosophers;++i){
		pthread_mutex_init(&mutexes[i],NULL);
	}
	for(int i=0;i<num_philosophers;++i){
		pthread_create(&threads[i],NULL,eat_meal,&philosophers[i]);
	}
	for(int i=0;i<num_philosophers;++i){
		pthread_join(threads[i],NULL);
	}
	for(int i=0;i<num_philosophers;++i){
		pthread_mutex_destroy(&mutexes[i]);
	}
	pthread_exit(NULL);
	return 0;
}
