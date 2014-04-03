/**
 *
 * A sample program to show the dining philosophers' problem (OS concept).
 *
 * A few things to note:
 * - still need to figure WHAT to measure as performance indicator
 *   = an example on how to measure lapsed time (check out starving function)
 *   = a reward/penalty feature for think_time is introduced
 * - the way starvation is handled should be optimized
 *   = e.g. wait random time before checking again?
 * - the way dining tools are prepared can be modified
 *   = e.g. give priority for priority based scheduling
 *
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define DINER_THINK_MAX 5
#define DINER_THINK_MIN 1
#define DINER_DTIME_MAX 5
#define DINER_DTIME_MIN 1
#define DINER_COUNT 5

/* global variable! */
int which, ltool, rtool, start = 0;
int rchk = -1, lchk = -1;
int total_starve = 0;
int total_think = 0;
int total_attempt = 0;

/* tool filename - shared resource! */
#define DTOOL_COUNT DINER_COUNT
char DTOOL_FNAME[DTOOL_COUNT][8] = { "TOOL0","TOOL1","TOOL2","TOOL3","TOOL4" };

/* default total dining time in seconds */
#define DINER_DTIME_TOTAL 20

void start_dinner(int signum)
{
	switch(signum)
	{
	case SIGUSR1:
		start = 1;
		break;
	/** other signals are ignored? */
	}
}

void eat(int len, int *total)
{
	if(len>*total) len = *total;
	*total -= len;
	printf("[GENIUS%d] Eating... (%d)[%d]\n",which,len,*total);
	sleep(len);
}

int starving(void)
{
	int len, count = 0;
	time_t init , last;
	printf("[GENIUS%d] Trying to eat...\n",which);
	fflush(stdout);
	init = time(0x0); /* mark begin time */
	while(lchk<0&&rchk<0)
	{
		lchk = open(DTOOL_FNAME[ltool],O_RDWR|O_CREAT|O_EXCL,0666);
		rchk = open(DTOOL_FNAME[rtool],O_RDWR|O_CREAT|O_EXCL,0666);
		if(lchk<0||rchk<0)
		{
			if(lchk!=-1)
			{
				remove(DTOOL_FNAME[ltool]);
				close(lchk);
				lchk = -1;
			}
			if(rchk!=-1)
			{
				remove(DTOOL_FNAME[rtool]);
				close(rchk);
				rchk = -1;
			}
		}
		else break;
		count++;
	}
	last = time(0x0); /* mark end time */
	len = last - init;
	total_starve += len;
	total_attempt += count;
	if(len) printf("[GENIUS%d] Starving... (%d)\n",which,len);
	return len;
}

void burping(void)
{
	remove(DTOOL_FNAME[ltool]); close(lchk); lchk = -1;
	remove(DTOOL_FNAME[rtool]); close(rchk); rchk = -1;
}

void think(int len)
{
	total_think += len;
	printf("[GENIUS%d] Thinking... (%d)\n",which,len);
	sleep(len);
}

void do_dining(int dining_time, int eat_min, int eat_max,
	int think_min, int think_max)
{
	int think_time, eat_time, think_range, eat_range;
	if(signal(SIGUSR1,&start_dinner)==SIG_ERR)
	{
		printf("Cannot set signal handler! Aborting!\n");
		exit(1);
	}
	/** start random number generator */
	srand(time(0x0));
	think_range = think_max-think_min;
	think_time = (rand()%think_range)+think_min;
	eat_range = eat_max-eat_min;
	eat_time = (rand()%eat_range)+eat_min;
	/* wait for start signal! */
	while(start==0);
	printf("[GENIUS%d] Starting dinner...\n",which);
	/** dining philosopher here!!!! */
	while(dining_time>0)
	{
		/** start to think */
		think(think_time);
		/** try to get dining tools a.k.a. forks */
		if(starving()&&think_time>think_min) think_time--;
		else if(think_time<think_max) think_time++;
		/** start to eat */
		eat(eat_time,&dining_time);
		/** release dining tools a.k.a. forks - shouldn't we wash first? */
		burping();
	}
	fprintf(stderr,"[GENIUS%d] Finishing dinner...",which);
	fprintf(stderr," (Think=%d)(Starve=%d)(Attempt=%d)\n",
		total_think,total_starve,total_attempt);
	exit(0);
}

int main(int argc, char *argv[])
{
	pid_t diner[DINER_COUNT];
	int diner_count = DINER_COUNT, dining_time = DINER_DTIME_TOTAL;
	int emin = DINER_DTIME_MIN, emax = DINER_DTIME_MAX;
	int tmin = DINER_THINK_MIN, tmax = DINER_THINK_MAX;
	int loop, test;

	/* process parameters */
	for(loop=1;loop<argc;loop++)
	{
		if(argv[loop][0]=='-'&&argv[loop][1]=='-')
		{
			if(!strcmp(&argv[loop][2],"length"))
			{
				if(loop>=argc-1) break;
				test = atoi(argv[++loop]);
				if(test) dining_time = test;
			}
			else if(!strcmp(&argv[loop][2],"count"))
			{
				if(loop>=argc-1) break;
				test = atoi(argv[++loop]);
				if(test) diner_count = test;
			}
			else if(!strcmp(&argv[loop][2],"emin"))
			{
				if(loop>=argc-1) break;
				test = atoi(argv[++loop]);
				if(test) emin = test;
			}
			else if(!strcmp(&argv[loop][2],"emax"))
			{
				if(loop>=argc-1) break;
				test = atoi(argv[++loop]);
				if(test) emax = test;
			}
			else if(!strcmp(&argv[loop][2],"tmin"))
			{
				if(loop>=argc-1) break;
				test = atoi(argv[++loop]);
				if(test) tmin = test;
			}
			else if(!strcmp(&argv[loop][2],"tmax"))
			{
				if(loop>=argc-1) break;
				test = atoi(argv[++loop]);
				if(test) tmax = test;
			}
		}
	}

	printf("[MAIN] Inviting dining philosophers... (Time=%d)\n",dining_time);
	fflush(stdout);
	/* create diner_count processes */
	for(which=0;which<diner_count;which++)
	{
		/* assign tool a.k.a. fork */
		ltool = which;
		rtool = (which+1) % DINER_COUNT;
		printf("[MAIN] Diner %d using (%d)(%d)...\n",which,ltool,rtool);
		fflush(stdout);
		diner[which] = fork();
		if(diner[which]==0)
		{
			do_dining(dining_time,emin,emax,tmin,tmax);
			break;
		}
		sleep(1); /* give diners time to setup */
	}

	/* make sure table is ready (forks cleaned?) */
	printf("[MAIN] Checking table...\n");
	fflush(stdout);
	for(which=0;which<DTOOL_COUNT;which++)
		remove(DTOOL_FNAME[which]);

	/* start dinner! */
	printf("[MAIN] Dinner is served...\n");
	fflush(stdout);
	for(which=0;which<DINER_COUNT;which++)
		kill(diner[which],SIGUSR1);

	/* wait for all child to finish! */
	while(1)
	{
		int status, count;
		pid_t check = wait(&status);
		for(which = 0; which < DINER_COUNT; which++)
		{
			if(WIFEXITED(status)) status = WEXITSTATUS(status);
			else status = -1;
			if(check==diner[which])
			{
				printf("Diner %d exits with %d\n",which,status);
				diner[which] = 0;
				break;
			}
		}
		for(which = 0, count = 0; which < DINER_COUNT; which++)
			if(diner[which]) count ++;
		if(!count) break;
	}

	return 0;
}
