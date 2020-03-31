#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
int ta_sleeping = 0;
int waiting_students = 0;
int nextseatingposition = 0;
int nextteachingposition = 0;
int waiting_room_chairs[3];
pthread_mutex_t mute_t;//object of type mutex ;here mute_t is mutex variable to lock and unlock a mutex
sem_t semastudents;//semaphore type
sem_t semata;//semaphore type
void* student_turn( void* student_id );
void* ta_turn();
void* ta_turn() {
	printf( "scenario of ta and student\n" );	
while(1)
{
	//case-1 students waiting case
		if ( waiting_students > 0 ) {
            ta_sleeping = 0;
			sem_wait( &semastudents );//locks the semaphore semastudents
			pthread_mutex_lock( &mute_t );//locks the mutex variable
			//case-2 TA helping student.
			int time = rand() % 10;
          sleep(time);
			printf( "Student %d interacts with ta.\n",waiting_room_chairs[nextteachingposition] );
          sleep(time);
			printf( "No of students waiting = %d.\n", (waiting_students - 1) );
			waiting_room_chairs[nextteachingposition]=0;
			waiting_students--;
			nextteachingposition = ( nextteachingposition + 1 ) % 3;
			pthread_mutex_unlock( &mute_t);//unlocks the mutex variable
			sem_post( &semata );//unlocks the semaphore semata
		}
		//when there are no students
		else {
			if ( ta_sleeping == 0 ) {
				printf( "ta Sleeping.\n" );
				ta_sleeping = 1;
			}
		}	
}
}
void* student_turn( void* student_id ) {
	int id_student = *(int*)student_id;
	while(1)
	 {
	 	int time = rand() % 10;
          sleep(time);
	 //if student is waiting, continue waiting
		if ( isWaiting( id_student ) == 1 ) {continue;}
		pthread_mutex_lock( &mute_t );//lock
		if( waiting_students < 3) {
waiting_room_chairs[nextseatingposition] = id_student;
			waiting_students++;
			//students takes a seat
		    sleep(time);
			printf( "Student %d takes a seat.Number of Students waiting = %d.\n", id_student, waiting_students );
			nextseatingposition = ( nextseatingposition + 1 ) % 3;
			pthread_mutex_unlock( &mute_t );//unlock
			//wake TA if sleeping
			sem_post( &semastudents );//unlocks semaphore students
			sem_wait( &semata );//locks the semaphore ta		
		}
		else {
			int time = rand() % 5;
          sleep(time);
			pthread_mutex_unlock( &mute_t );
//No chairs available.
			printf( "Student %d will try later.\n",id_student );
		}
	}
}
// isWaiting function is used to check whether the particular student is  sitting in any one of three chairs
int isWaiting( int student_id ) {
	int i;
	for ( i = 0; i < 3; i++ ) {
		if ( waiting_room_chairs[i] == student_id ) { return 1; }
	}
	return 0;
}
int main( ){
	int i;	
	int student_ids[5];
	pthread_t students[5];//datatype used to identify thread students, returned by students pthread_create function 
	pthread_t ta;//datatype used to identify thread ta, returned by ta pthread_create function
	sem_init( &semastudents, 0, 0 );//initializes semaphore object pointed to by semastudents by setting its internal count to zero
	sem_init( &semata, 0, 1 );//semaphore is shared between threads of process
	pthread_mutex_init( &mute_t, NULL );//initialize a mutex
	pthread_create( &ta, NULL, ta_turn, NULL );//creation of ta thread
	for( i = 0; i < 5; i++ )	{
		student_ids[i] = i + 1;
		pthread_create( &students[i], NULL, student_turn, (void*) &student_ids[i] );//creation of student thread	
	}
	pthread_join(ta, NULL);//wait for a ta thread to terminate
	for( i =0; i < 5; i++ )
	{
		pthread_join( students[i],NULL );//wait for a students thread to terminate
	}
	return 0;
}

