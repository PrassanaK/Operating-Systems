/*Name: Prassana Kamalakannan*/
/*Date last modified: 18/05/2020*/
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "lift.h"

pthread_mutex_t mutex;
pthread_cond_t prod = PTHREAD_COND_INITIALIZER;
pthread_cond_t cons = PTHREAD_COND_INITIALIZER;
int num = 0;/*number of elements in buffer*/
int add = 0;/*the next index to add to*/
int rem = 0;/*index that has been used*/
Lift *buffer;/*Array of lifts*/
int bufSize;/*size of Lift array called buffer*/
FILE *f = NULL;/*input file -- sim_input*/
FILE *output = NULL;/*output file -- sim_output*/
int previousFloor = 1;/*Previous Floor*/
int requestsServed = 0;/*number of requests served*/
int requestNo = 0;/*number of requests put into the buffer*/

/*Consumer  thread Lift-x*/
void* lift(void * arg)
{
    int* buffSize = (void*)arg;
    int bufferSize = abs(*buffSize);
    int movement;/*previous position*/
    output = fopen("sim_output.txt", "w+");
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(num == 0)
        {
            pthread_cond_wait(&cons, &mutex);
        }
        fprintf(output,"\n%s %d" , "Previous Position: Floor ", previousFloor);
        fprintf(output, "\n%s %d %s %d " , "Floor ", buffer[rem].start, " to Floor ", buffer[rem].dest);
        fprintf(output,"\n%s" , "Detail Operations:");
        fprintf(output, "\n%s %d %s %d" , "    Go from Floor ", previousFloor, " to Floor ", buffer[rem].start);
        fprintf(output, "\n%s %d %s %d" , "    Go from Floor ", buffer[rem].start, " to Floor ", buffer[rem].dest);
        if((previousFloor > buffer[rem].start)&&(buffer[rem].dest > buffer[rem].start))
        {
            movement = (previousFloor - buffer[rem].start) + (buffer[rem].dest - buffer[rem].start);
        }
        else if((previousFloor > buffer[rem].start)&&(buffer[rem].start > buffer[rem].dest))
        {
            movement = (previousFloor - buffer[rem].start) + (buffer[rem].start - buffer[rem].dest);
        }
        else if((previousFloor < buffer[rem].start)&&(buffer[rem].dest > buffer[rem].start))
        {
            movement = (buffer[rem].start - previousFloor) + (buffer[rem].dest - buffer[rem].start);
        }
        else
        {
            movement = (buffer[rem].start - previousFloor) + (buffer[rem].start - buffer[rem].dest);
        }
        requestsServed++;
        fprintf(output, "\n%s %d", "    #movement for this request: ", movement);
        fprintf(output, "\n%s %d", "    #request: ", requestsServed);
        previousFloor = buffer[rem].dest;
        fprintf(output, "\n%s %d", "Current position: Floor ", previousFloor);
        rem = (rem + 1) % bufferSize;
        num--;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&prod);
    }
    return NULL;
}

/*Producer thread Lift-R*/
void* request(void * arg)
{
    int *buffSize = (void*)arg;
    int bufferSize = abs(*buffSize);
    int ii;
    int start, dest;
    f = fopen("sim_input.txt", "r");
    output = fopen("sim_output.txt", "w+");
    buffer = (Lift*)malloc(bufferSize*sizeof(Lift));/*Array of lifts*/
    for(ii=1; ii<=50; ii++)
    {
        pthread_mutex_lock(&mutex);
        while(num == bufferSize)
        {
            pthread_cond_wait(&prod, &mutex);
        }
        fscanf(f, "\n%d %d", &start, &dest);/*put the number of the starting floor and destination floor into variables "start and dest"*/
        (buffer[add].start) = start;
        (buffer[add].dest) = dest;
        add = (add+1) % bufferSize;
        num++;
        fprintf(output, "\n%s %d %s %d", "New Lift Request from Floor ", buffer[add].start, " to ", buffer[add].dest);
        requestNo++;
        fprintf(output, "\n%s %d", "Request No: ", requestNo);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cons);
        
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char *argv[])
{
    int i;
    pthread_t tid;
 
    pthread_create(&tid, NULL, lift, &argv[1]);

    for(i=1; i<=3; i++)
    {
        pthread_create(&tid, NULL, request, &argv[1]);
    }

    pthread_join(tid, NULL);
    fclose(f);
    fclose(output);
    free(buffer);
    return 0;
}
