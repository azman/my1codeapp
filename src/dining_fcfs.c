/**
 *
 * A sample program to show the dining philosophers' problem (OS concept).
 *
 * A few things to note:
 * - uses process instead of thread
 *   = the 'shared' resources (i.e. forks) are represented by files
 *   = mutual exclusion handled by file exclusive access
 * - tries to implement first come first serve
 *   = queue to get forks if cannot get both
 *   = gets first try at it every time unit
 *
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define DINER_COUNT 5

/* global variable! */
int which, start = 0, check = 0;

/* default total dining time in time units */
#define DINER_DTIME_TOTAL 20
/* default eating time in time units */
#define DINER_CHEW_TIME 5

void start_dinner(int signum)
{
	switch(signum)
	{
	case SIGUSR1:
		start = 1;
		break;
	case SIGUSR2:
		check = 1;
		break;
	/** other signals are ignored? */
	}
}

void eat(int len, int *total)
{
	if(len>*total) len = *total;
	while(len>0)
	{
		while(check==0);
		check=0;
		kill(getppid(),SIGUSR2);
		printf("[GENIUS%d] Eating... (%d)[%d]\n",which,len,*total);
		len--; (*total)--;
	}
}

int starving(int *total)
{
	int len = 0;
	while(start==0)
	{
		if(check==1)
		{
			check=0;
			kill(getppid(),SIGUSR2);
			printf("[GENIUS%d] Starving... (%d)\n",which,len);
			len++; (*total)++;
		}
	}
	start = 0;
	kill(getppid(),SIGUSR1);
	return len;
}

int burping(int remaining)
{
	/* first we release utensils */
	while(check==0); check=0;
	kill(getppid(),SIGUSR1);
	kill(getppid(),SIGUSR2);
	printf("[GENIUS%d] Break...\n",which);
	while(start==0); start = 0;
	/* then we check if done */
	while(check==0); check=0;
	if(!remaining)
		kill(getppid(),SIGUSR1);
	kill(getppid(),SIGUSR2);
	return !remaining;
}

void do_dining(int dining_time, int chew_time)
{
	int total_starve = 0, total_think = 0, think_time = dining_time;
	printf("[GENIUS%d] Starting dinner...\n",which);
	/** dining philosopher here!!!! */
	do
	{
		/** try to get dining tools a.k.a. forks */
		starving(&total_starve);
		/** start to eat */
		eat(chew_time,&dining_time);
		/** release dining tools a.k.a. forks - check if done */
		if(burping(dining_time)) break;
	}
	while(1);
	printf("[GENIUS%d] Finishing dinner... (Starve=%d)\n",which,total_starve);
	exit(0);
}

int main(int argc, char *argv[])
{
	pid_t diner[DINER_COUNT],tools[DINER_COUNT];
	int queue[DINER_COUNT],xflag[DINER_COUNT];
	int dining_time = DINER_DTIME_TOTAL, chew_time = DINER_CHEW_TIME;
	int loop, test, time_unit = 0;
	int rtool[DINER_COUNT],ltool[DINER_COUNT];

	/* unbuffer stdout */
	setbuf(stdout,0x0);
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
			else if(!strcmp(&argv[loop][2],"chew"))
			{
				if(loop>=argc-1) break;
				test = atoi(argv[++loop]);
				if(test) chew_time = test;
			}
		}
	}

	/** creating signal handler here! */
	if(signal(SIGUSR1,&start_dinner)==SIG_ERR)
	{
		printf("Cannot set signal handler! Aborting!\n");
		exit(1);
	}
	if(signal(SIGUSR2,&start_dinner)==SIG_ERR)
	{
		printf("Cannot set signal handler! Aborting!\n");
		exit(1);
	}

	printf("[MAIN] Inviting dining philosophers... (Time=%d)\n",dining_time);
	/* create diner_count processes */
	for(which=0;which<DINER_COUNT;which++)
	{
		/* assign tool a.k.a. fork */
		ltool[which] = which;
		rtool[which] = (which+1)%DINER_COUNT;
		printf("[MAIN] Diner %d using (%d)(%d)...\n",which,
			ltool[which],rtool[which]);
		diner[which] = fork();
		if(diner[which]==0)
		{
			do_dining(dining_time,chew_time);
			/** you shall NEVER pass! */
		}
		sleep(1); /* give diners time to setup */
	}

	/* start dinner! */
	printf("[MAIN] Dinner is served...\n");

	/* reset queue... and others */
	for(which=0;which<DINER_COUNT;which++)
	{
		tools[which] = 0;
		xflag[which] = 0;
		queue[which] = -1;
	}

	/* manage dinner! */
	while(1)
	{
		int status, count = 0;
		printf("[MAIN] TIME UNIT %d (",time_unit);
		for(which=0;which<DINER_COUNT;which++)
		{
			printf("{%d}",xflag[which]);
			if(diner[which]) count++;
		}
		printf(":");
		for(loop=0;loop<DINER_COUNT;loop++)
		{
			if(queue[loop]<0) break;
			else printf("[%d]",queue[loop]);
		}
		printf(")\n");
		/* check if all done? */
		if(!count){
			printf("[MAIN] Dinner ends!\n");
			break;
		}
		/* check if any diner in queue */
		for(loop=0;loop<DINER_COUNT;loop++)
		{
			if(queue[loop]<0) break;
			else
			{
				which = queue[loop];
				if(!tools[ltool[which]]&&!tools[rtool[which]])
				{
					printf("[MAIN] Diner %d given utensils (Q)...\n",which);
					tools[ltool[which]] = diner[which];
					tools[rtool[which]] = diner[which];
					xflag[which] = 1;
					/* start it */
					kill(diner[which],SIGUSR1);
					while(start==0); start = 0;
					/* update queue */
					test = loop+1;
					while(test<DINER_COUNT)
					{
						queue[test-1] = queue[test];
						if(queue[test]<0) break;
						test++;
					}
					loop--;
				}
			}
		}
		/* assign available fork based on fcfs basis */
		for(which=0;which<DINER_COUNT;which++)
		{
			if(!diner[which]) continue;
			if(!tools[ltool[which]]&&!tools[rtool[which]])
			{
				printf("[MAIN] Diner %d given utensils...\n",which);
				tools[ltool[which]] = diner[which];
				tools[rtool[which]] = diner[which];
				xflag[which] = 1;
				/* start it */
				kill(diner[which],SIGUSR1);
				while(start==0); start = 0;
			}
			else if(!xflag[which])
			{
				/* queue it! */
				for(loop=0;loop<DINER_COUNT;loop++)
				{
					if(queue[loop]==which)
					{
						/* already in! */
						break;
					}
					else if(queue[loop]<0)
					{
						printf("[MAIN] Diner %d put into queue...\n",which);
						queue[loop] = which;
						break;
					}
				}
			}
		}
		/* time step! */
		time_unit++;
		/* signal child process */
		for(which=0;which<DINER_COUNT;which++)
		{
			if(!diner[which]) continue;
			kill(diner[which],SIGUSR2);
			while(check==0); check = 0;
			if(start==1)
			{
				start = 0;
				printf("[MAIN] Diner %d released tool...\n",which);
				/* child done with tools? */
				tools[ltool[which]] = 0;
				tools[rtool[which]] = 0;
				xflag[which] = 0;
				/* acknowledge it */
				kill(diner[which],SIGUSR1);
				/* check if done! */
				kill(diner[which],SIGUSR2);
				while(check==0); check = 0;
				if(start==1)
				{
					start = 0;
					waitpid(diner[which],0x0,0);
					printf("[MAIN] Diner %d done!\n",which);
					diner[which] = 0;
				}
			}
		}
	}

	return 0;
}
