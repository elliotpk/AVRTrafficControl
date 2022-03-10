#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h> 

#define GREEN 1
#define RED 0

#define NORTH 0
#define SOUTH 1

#define MAXCARS 40

struct bridge{
	int northQ;
	int southQ;
	int onBridge;
	int northLight;
	int southLight;
	int enabled;
	int stateChanged;
};
struct bridge bridge;
int Com1;
sem_t northMut;
sem_t southMut;
pthread_mutex_t bridgeMutex;
pthread_mutex_t comMutex;

void* carthread(void* arg) {
	pthread_detach(pthread_self);
	int transmitData = 0;
	int pos = (int)arg;
	sem_t *waiting;
	if(pos == NORTH) {
		waiting = &northMut;
	} else {
		waiting = &southMut;
	}

	sem_wait(waiting);
	pthread_mutex_lock(&bridgeMutex);
	bridge.onBridge++;
	if(pos == NORTH) {
		bridge.northQ--;
		bridge.stateChanged = 1;
		transmitData = 2;
		pthread_mutex_lock(&comMutex);
		write(Com1, &transmitData, 1);
		pthread_mutex_unlock(&comMutex);
	} else {
		bridge.southQ--;
		bridge.stateChanged = 1;
		transmitData = 8;
		pthread_mutex_lock(&comMutex);
		write(Com1, &transmitData, 1);
		pthread_mutex_unlock(&comMutex);
	}
	pthread_mutex_unlock(&bridgeMutex);

	sleep(1);
	pthread_mutex_lock(&bridgeMutex);
	if(pos == NORTH) {
		if(bridge.northLight){
			sem_post(waiting);
		} 
	} else if(bridge.southLight){
		sem_post(waiting);
	}
	pthread_mutex_unlock(&bridgeMutex);
	
	sleep(4);
	pthread_mutex_lock(&bridgeMutex);
	bridge.onBridge--;
	bridge.stateChanged = 1;
	pthread_mutex_unlock(&bridgeMutex);
	pthread_exit(NULL);
}

void* uithread(void) {
	while(bridge.enabled) {
		pthread_mutex_lock (&bridgeMutex);	//Lock the mutex for bridge
		if(bridge.stateChanged) {
			printf("\n-------------------\n");
			printf("Press \"s\" to add southbound car\n");
			printf("Press \"n\" to add northbound car\n");
			printf("Press \"e\" to add exit\n");

			printf("\n ---Status--- \n");
			printf("Northbound queue: %d\n", bridge.northQ);		
			printf("Southbound queue: %d\n", bridge.southQ);
			printf("Cars currently on bridge: %d\n", bridge.onBridge);

			if(bridge.northLight == GREEN) {
				printf("Northbound light is green.\n");
			} else {
				printf("Northbound light is red.\n");
			}

			if(bridge.southLight == GREEN) {
				printf("Southbound light is green.\n");
			} else {
				printf("Southbound light is red.\n");
			}

			printf("\n-------------------\n");
			bridge.stateChanged = 0;
		}
		pthread_mutex_unlock(&bridgeMutex);
	}
	pthread_exit(NULL);
}

void* comthread(void) {
	pthread_t *cars;
	cars = calloc(MAXCARS, sizeof(pthread_t));
	int carindex = 0;
	struct termios options;
	int comRead = 0;
	int kbRead = 0;
	int signal = 0;
	int transmit = 0;

	Com1 = open("/dev/ttyS0", O_RDWR | O_NDELAY);
	if(Com1 == -1) {
		printf("Couldn't open Com1 port\n");
		bridge.enabled = 0;
		pthread_exit(NULL);
	} else {
		fcntl(Com1, F_SETFL, 0);
	}

	//Com1 config setup, 9600N81
	tcgetattr(Com1, &options);

	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8 | CREAD | CLOCAL;
	options.c_lflag = 0;
	options.c_iflag = 0;
	options.c_oflag = 0;
	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 5;

	cfsetispeed(&options, B9600);
	cfsetospeed(&options, B9600);	

	tcsetattr(Com1, TCSANOW, &options);
	
	//file descriptor setup
	fd_set input;
	FD_ZERO(&input);		//Clear fd_set
	FD_SET(0, &input);		//Keyboard
	FD_SET(Com1, &input);	//Com1 port
	
	while(select(4, &input, NULL, NULL, NULL)) {
		if(carindex >= MAXCARS) carindex = 0;

		if(FD_ISSET(0, &input)) {
			kbRead = 1;
		} else if(FD_ISSET(Com1, &input)) {
			comRead = 1;
		}
		
		if(comRead) {
			read(Com1, &signal, 1);
			switch(signal){
				case 1:		//north green
					pthread_mutex_lock(&bridgeMutex);
					if(bridge.northLight == RED) {
						sem_post(&northMut);
						bridge.northLight = GREEN;
						bridge.stateChanged = 1;
					}
					pthread_mutex_unlock(&bridgeMutex);
					printf("North GREEN.\n");
					break;
				case 2:		//north red
					pthread_mutex_lock(&bridgeMutex);
					bridge.northLight = RED;
					bridge.stateChanged = 1;
					pthread_mutex_unlock(&bridgeMutex);
					printf("North RED\n");
					break;
				case 4:		//south green
					pthread_mutex_lock(&bridgeMutex);
					if(bridge.southLight == RED){
						sem_post(&southMut);
						bridge.southLight = GREEN;
						bridge.stateChanged = 1;
					}
					pthread_mutex_unlock(&bridgeMutex);
					printf("South GREEN.\n");
					break;
				case 8:		//south red
					pthread_mutex_lock(&bridgeMutex);
					bridge.southLight = RED;
					bridge.stateChanged = 1;
					pthread_mutex_unlock(&bridgeMutex);
					printf("South RED\n");
					break;
			}
			FD_SET(0, &input);
			comRead = 0;
		}

		if(kbRead) {
			signal = getchar(); 
			switch(signal){
				case 10:
					break;
				case 110: // "n"
					pthread_mutex_lock(&bridgeMutex);
					bridge.northQ++;
					bridge.stateChanged = 1;
					pthread_mutex_unlock(&bridgeMutex);
					transmit = 1;
					pthread_mutex_lock(&comMutex);
					write(Com1, &transmit, 1);
					pthread_mutex_unlock(&comMutex);
					pthread_create(&cars[carindex], NULL, carthread, (void *)0);
					carindex++;
					break;
				case 115: // "s"
					pthread_mutex_lock(&bridgeMutex);
					bridge.southQ++;
					bridge.stateChanged = 1;
					pthread_mutex_unlock(&bridgeMutex);
					transmit = 4;
					pthread_mutex_lock(&comMutex);
					write(Com1, &transmit, 1);
					pthread_mutex_unlock(&comMutex);
					pthread_create(&cars[carindex], NULL, carthread, (void *)1);
					carindex++;
					break;
				case 101: // "e"
					pthread_mutex_lock(&bridgeMutex);
					bridge.enabled = 0;
					pthread_mutex_unlock(&bridgeMutex);
					break;
			}
			FD_SET(Com1, &input);
			kbRead = 0;
			transmit = 0;
		}
		if(bridge.enabled == 0){
			break;
		}
	}
	close(Com1);
	for(int i = 0; i < MAXCARS; i++){	// Hopefully makes sure all threads are closed down so out program closes...
		pthread_cancel(cars[i]);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]){
	//Small setup
	bridge.northLight = RED;
	bridge.southLight = RED;
	bridge.enabled = 1;
	bridge.stateChanged = 1;
	pthread_t thread1;
	pthread_t thread2;
	sem_init(&northMut, 0, 0);
	sem_init(&southMut, 0, 0);

	//Create threads
	pthread_create(&thread1, NULL, uithread, NULL);
	pthread_create(&thread2, NULL, comthread, NULL);

	pthread_exit(NULL);
}
