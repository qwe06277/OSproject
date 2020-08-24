#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct sharedobject {
	FILE *rfile;
	int linenum;
	char *line;
	pthread_mutex_t lock;
	int full;
} so_t;

void *producer(void *arg) {//파일을 한줄한줄 읽어오는 thread funtion
	so_t *so = arg;//구조체 선언
	int *ret = malloc(sizeof(int));
	FILE *rfile = so->rfile;
	int i = 0;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	while (1) {
		read = getdelim(&line, &len, '\n', rfile);
		if (read == -1) {//파일이 없을때
                        pthread_mutex_lock(&so->lock);
			so->full = 1;
			so->line = NULL;
                        pthread_mutex_unlock(&so->lock);
			break;
		}
                else
		{pthread_mutex_lock(&so->lock);
                 so->linenum = i;
		 so->line = strdup(line);      /* share the line */
		 i++;
		 so->full = 1;
                 pthread_mutex_unlock(&so->lock);
                 }
	}
	free(line);
	printf("Prod_%x: %d lines\n", (unsigned int)pthread_self(), i);
	*ret = i;
	pthread_exit(ret);
}

void *consumer(void *arg) {//라인의 수를 계산하는 소비자 thread funtion
	so_t *so = arg;
	int *ret = malloc(sizeof(int));
	int i = 0;//line수
	int len;
	char *line;

	while (1) {
                pthread_mutex_lock(&so->lock);
		line = so->line;
		if (line == NULL) {//파일이 읽히지 않았으면
                        so->full=0;
                        pthread_mutex_unlock(&so->lock);
			break;
		}
		len = strlen(line);
		printf("Cons_%x: [%02d:%02d] %s",
			(unsigned int)pthread_self(), i, so->linenum, line);
		free(so->line);
		i++;
		so->full = 0;
                pthread_mutex_unlock(&so->lock);
	}
	printf("Cons: %d lines\n", i);
	*ret = i;
	pthread_exit(ret);
}


int main (int argc, char *argv[])
{
	pthread_t prod[100];//생산자 thread
	pthread_t cons[100];//소비자 thread
	int Nprod, Ncons;//생산자, 소비자 갯수
	int rc;   long t;
	int *ret;//line 수
	int i;
	FILE *rfile;
	if (argc == 1) {
		printf("usage: ./prod_cons <readfile> #Producer #Consumer\n");
		exit (0);
	}
	so_t *share = malloc(sizeof(so_t));
	memset(share, 0, sizeof(so_t));
	rfile = fopen((char *) argv[1], "r");//파일오픈
	if (rfile == NULL) {//파일이없으면
		perror("rfile");
		exit(0);
	}
	if (argv[2] != NULL) {//생산자수를 입력했다면
		Nprod = atoi(argv[2]);
		if (Nprod > 100) Nprod = 100;
		if (Nprod == 0) Nprod = 1;
	} else Nprod = 1;
	if (argv[3] != NULL) {//소비자수를 입력했다면
		Ncons = atoi(argv[3]);
		if (Ncons > 100) Ncons = 100;
		if (Ncons == 0) Ncons = 1;
	} else Ncons = 1;

	share->rfile = rfile;
	share->line = NULL;
	pthread_mutex_init(&share->lock, NULL);
	for (i = 0 ; i < Nprod ; i++)
		pthread_create(&prod[i], NULL, producer, share);//생산자 thread 생성
	for (i = 0 ; i < Ncons ; i++)
		pthread_create(&cons[i], NULL, consumer, share);//소비자 thread 생성
	printf("main continuing\n");

	for (i = 0 ; i < Ncons ; i++) {
		rc = pthread_join(cons[i], (void **) &ret);
		printf("main: consumer_%d joined with %d\n", i, *ret);//소비자수 출력
	}
	for (i = 0 ; i < Nprod ; i++) {
		rc = pthread_join(prod[i], (void **) &ret);
		printf("main: producer_%d joined with %d\n", i, *ret);//생산자수 출력
	}
	pthread_exit(NULL);
	exit(0);
}


