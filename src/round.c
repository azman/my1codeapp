/**
 * program to round numbers
**/
/*----------------------------------------------------------------------------*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
/*----------------------------------------------------------------------------*/
int get_param_int(int argc, char* argv[], int *index, int *value)
{
	(*index)++;
	if(*index>=argc) return -1;
	*value = atoi(argv[*index]);
	return 0;
}
/*----------------------------------------------------------------------------*/
int get_param_real(int argc, char* argv[], int *index, double *value)
{
	(*index)++;
	if(*index>=argc) return -1;
	*value = atof(argv[*index]);
	return 0;
}
/*----------------------------------------------------------------------------*/
#define PRECISION 0.0001
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	int loop, done = 0;
	double prec = PRECISION, test;

	for(loop=1;loop<argc;loop++)
	{
		if(strncmp(argv[loop],"-p",2)==0)
		{
			if(get_param_real(argc,argv,&loop,&prec)<0)
			{
				printf("Cannot get precision value?\n");
				return -1;
			}
		}
		else /* must be value? */
		{
			if (done) printf("Value already given! (%lf)\n",test);
			else test = atof(argv[loop]);
		}
	}
	printf("Value: %lf\n",test);
	prec = (1.0/prec);
	printf("  Round: %lf\n",round(test*prec)/prec);
	printf("  Round Up: %lf\n",ceil(test*prec)/prec);
	printf("  Round Down: %lf\n",floor(test*prec)/prec);
	return 0;
}
/*----------------------------------------------------------------------------*/
