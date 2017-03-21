/*----------------------------------------------------------------------------*/
/* computes MY 2D gaussian smoothing kernel */
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*----------------------------------------------------------------------------*/
#define PI 3.14159265
/*----------------------------------------------------------------------------*/
double fgaussian2d(double x, double y, double sigma)
{
	/* isotropic (circularly symmetric) mean at (0,0) */
	double temp = sigma*sigma*2;
	return exp(-(((x*x)+(y*y))/(temp)))/(temp*PI);
}
/*----------------------------------------------------------------------------*/
double fgaussian1d(double x, double sigma)
{
	double temp = x/sigma;
	return (float) exp(-(temp*temp)/2.0)/(sigma*sqrt(2.0*PI));
}
/*----------------------------------------------------------------------------*/
int get_param_int(int argc, char* argv[], int *index, int *value)
{
	(*index)++;
	if(*index>=argc) return -1;
	*value = atoi(argv[*index]);
	return 0;
}
/*----------------------------------------------------------------------------*/
int get_param_float(int argc, char* argv[], int *index, float *value)
{
	(*index)++;
	if(*index>=argc) return -1;
	*value = (float)atof(argv[*index]);
	return 0;
}
/*----------------------------------------------------------------------------*/
char* get_param_str(int argc, char* argv[], int *index)
{
	char* pparam = 0x0;
	(*index)++;
	if(*index<argc) pparam = argv[*index];
	return pparam;
}
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	int loop, test, windowsize, count;
	float temp;
	double *pkernel, *gkernel, *dkernel;
	double corner, thresh = 1.0/256.0, sigma = 1.0, sumall = 1.0;

	for(loop=1;loop<argc;loop++)
	{
		if(strncmp(argv[loop],"-t",2)==0)
		{
			if(get_param_int(argc,argv,&loop,&test)<0)
			{
				printf("Cannot get threshold value?\n");
				return -1;
			}
			thresh = 1.0/(double)test;
		}
		else if(strncmp(argv[loop],"-s",2)==0)
		{
			if(get_param_float(argc,argv,&loop,&temp)<0)
			{
				printf("Cannot get sigma value?\n");
				return -1;
			}
			sigma = (double)temp;
		}
	}

	loop=0;
	do
	{
		corner = fgaussian2d(loop,loop,sigma);
		if(corner>thresh)
		{
			sumall = corner;
			loop++;
		}
		else
		{
			corner = sumall;
			break;
		}
	}
	while(1);

	printf("Gaussian kernel build parameters:\n");
	printf("  Sigma: %lf, Threshold: %lf, Corner: %lf\n",sigma,thresh,corner);

	windowsize = (loop*2)-1;
	count = windowsize/2;

	gkernel = (double*) malloc(windowsize*windowsize*sizeof(double));
	dkernel = (double*) malloc(windowsize*sizeof(double));

	printf("Gaussian 1D kernel\n");
	pkernel = dkernel; sumall = 0.0;
	for(loop=0;loop<windowsize;loop++)
	{
		*pkernel = fgaussian1d(loop-count,sigma);
		sumall += *pkernel;
		pkernel++;
	}
	pkernel = dkernel;
	for(loop=0;loop<windowsize;loop++)
	{
		*pkernel /= sumall;
		printf("  %.6lf  ",*pkernel);
		pkernel++;
	}
	printf("\n");

	printf("Gaussian 2D kernel from 1D!\n");
	for(loop=0;loop<windowsize;loop++)
	{
		for(test=0;test<windowsize;test++)
		{
			printf("  %.6lf  ",(dkernel[loop]*dkernel[test]));
		}
		printf("\n");
	}

	printf("Gaussian 2D kernel (%dx%d)\n",windowsize,windowsize);
	pkernel = gkernel; sumall = 0.0;
	for(loop=0;loop<windowsize;loop++)
	{
		for(test=0;test<windowsize;test++)
		{
			*pkernel = fgaussian2d(loop-count, test-count, sigma);
			sumall += *pkernel;
			pkernel++;
		}
	}
	corner /= sumall;
	// normalize and show it!
	pkernel = gkernel;
	for(loop=0;loop<windowsize;loop++)
	{
		for(test=0;test<windowsize;test++)
		{
			*pkernel /= sumall;
			printf("  %.6lf  ",*pkernel);
			pkernel++;
		}
		printf("\n");
	}

	printf("Gaussian 2D kernel (1/%d)\n",(int)(1.0/corner));
	pkernel = gkernel;
	for(loop=0;loop<windowsize;loop++)
	{
		for(test=0;test<windowsize;test++)
		{
			printf("  %6d  ",(int)((*pkernel)/corner));
			pkernel++;
		}
		printf("\n");
	}

	free(dkernel);
	free(gkernel);
	return 0;
}
/*----------------------------------------------------------------------------*/
