#include<stdio.h>
#include<stdlib.h>
#include<string.h> /* just for strcmp! */

#define ERROR_NOT_ENOUGH_MEMORY -1
#define ERROR_SSCANF_FAILED -2
#define ERROR_VARIABLE_ROWSIZE -3
#define ERROR_VALUE_TOO_LONG -4
#define ERROR_NO_INPUT_FILE -5

#define MY1_DEBUG0

typedef struct __my1matrix
{
	int xsize, ysize;
	double *pdata;
}
my1matrix;

my1matrix mymatrix;
int do_print = 0;
int result, loopx, loopy;
double* ploop;

#define BUFFSIZE 32
#define BUFFSTART 1000

int read_matrix(char* pfilename, my1matrix* pmatrix)
{
	int error = 0, was_whitespace = 0, was_newline = 0, bindex = 0;
	int xsize = 0, ysize = 0, vsize = BUFFSTART, reset = 0, loop, check;
	char buffer[BUFFSIZE];
	double *ptemp, *pvector = (double*) malloc(sizeof(double)*vsize);

	if(!pvector) return ERROR_NOT_ENOUGH_MEMORY;

	FILE* pfile = fopen(pfilename,"rt");
	if(pfile)
	{
		/** reset values if we can read the file */
		pmatrix->xsize = -1;
		pmatrix->ysize = -1;
		pmatrix->pdata = 0x0;
		/** get row and col sizes */
		while(!feof(pfile))
		{
			check = fgetc(pfile);
			if(was_whitespace)
			{
				if(check==' '||check=='\t')
				{
					/* ignore redundant whitespace */
				}
				else if(check=='\n'||check=='\r')
				{
					was_newline = 1;
					was_whitespace = 0;
					ysize++;
					reset = 1;
				}
				else /** assume part of number? */
				{
					buffer[bindex++] = (char) check;
					was_whitespace = 0;
					if(bindex>=BUFFSIZE-1)
					{
						error = ERROR_VALUE_TOO_LONG;
						break;
					}
				}
			}
			else if(was_newline)
			{
				if(check==' '||check=='\t')
				{
					was_whitespace = 1;
					was_newline = 0;
				}
				else if(check=='\n'||check=='\r')
				{
					/* ignore redundant newline */
				}
				else /** assume part of number? */
				{
					buffer[bindex++] = (char) check;
					was_newline = 0;
					if(bindex>=BUFFSIZE-1)
					{
						error = ERROR_VALUE_TOO_LONG;
						break;
					}
				}
			}
			else
			{
				if(check==' '||check=='\t')
				{
					was_whitespace = 1;
					if(!bindex) continue;
					buffer[bindex] = 0x0;
					if(sscanf(buffer,"%le",&pvector[xsize++])!=1)
					{
#ifdef MY1_DEBUG
	printf("[DEBUG] Buffer: '%s' (%d) => {%le}\n",buffer, bindex,pvector[xsize-1]);
#endif
						/** unexpected error? */
						error = ERROR_SSCANF_FAILED;
						break;
					}
#ifdef MY1_DEBUG
	printf("[DEBUG] WSValue[%d]: %le\n",xsize,pvector[xsize-1]);
#endif
					bindex = 0;
					/* is pvector overflowing? */
					if(xsize==vsize)
					{
						vsize += BUFFSTART;
						/** resize buffer vector - can we trust realloc?? */
						ptemp = (double*) realloc(pvector,sizeof(double)*vsize);
						if(!ptemp)
						{
							error = ERROR_NOT_ENOUGH_MEMORY;
							break;
						}
						pvector = ptemp;
					}
				}
				else if(check=='\n'||check=='\r')
				{
					was_newline = 1;
					if(!bindex) continue;
					buffer[bindex] = 0x0;
					if(sscanf(buffer,"%le",&pvector[xsize++])!=1)
					{
						/** unexpected error? */
						error = ERROR_SSCANF_FAILED;
						break;
					}
#ifdef MY1_DEBUG
	printf("[DEBUG] NLValue[%d]: %le\n",xsize,pvector[xsize-1]);
#endif
					bindex = 0;
					/* is pvector overflowing? */
					if(xsize==vsize)
					{
						vsize += BUFFSTART;
						/** resize buffer vector - can we trust realloc?? */
						ptemp = (double*) realloc(pvector,sizeof(double)*vsize);
						if(!ptemp)
						{
							error = ERROR_NOT_ENOUGH_MEMORY;
							break;
						}
						pvector = ptemp;
					}
					ysize++;
					reset = 1;
				}
				else /** assume part of number? */
				{
					/* accumulate value */
					buffer[bindex++] = (char) check;
					if(bindex>=BUFFSIZE-1)
					{
						error = ERROR_VALUE_TOO_LONG;
						break;
					}
				}
			}
			/* check if need to reset vector (new row) */
			if(reset)
			{
				if(pmatrix->xsize<0)
				{
					/** first row */
					pmatrix->xsize = xsize;
					/** create full matrix here? not! */
					ptemp = (double*) malloc(sizeof(double)*xsize);
					if(!ptemp)
					{
						error = ERROR_NOT_ENOUGH_MEMORY;
						break;
					}
					/** actual assignment */
					pmatrix->pdata = ptemp;
					/** copy to matrix? */
					for(loop=0;loop<xsize;loop++)
					{
						pmatrix->pdata[loop] = pvector[loop];
					}
				}
				else
				{
					/** subsequent rows - we check validity */
					if(pmatrix->xsize!=xsize)
					{
#ifdef MY1_DEBUG
	printf("[DEBUG] ReadSize: %d, InitSize: %d\n",xsize,pmatrix->xsize);
#endif
						error = ERROR_VARIABLE_ROWSIZE;
						break;
					}
					/** resize matrix - can we trust realloc?? */
					ptemp = (double*) realloc(pmatrix->pdata,(sizeof(double)*xsize)*ysize);
					if(!ptemp)
					{
						error = ERROR_NOT_ENOUGH_MEMORY;
						break;
					}
					/** actual assignment */
					pmatrix->pdata = ptemp;
					/** copy to matrix? */
					for(loop=0;loop<xsize;loop++)
					{
						pmatrix->pdata[(ysize-1)*xsize+loop] = pvector[loop];
					}
				}
				xsize = 0;
				reset = 0;
			}
		}
		fclose(pfile);
	}

	if(!error&&xsize)
	{
#ifdef MY1_DEBUG
	printf("[DEBUG] ReadSize: %d, InitSize: %d (%d)\n",xsize,pmatrix->xsize,error);
#endif
		error = ERROR_VARIABLE_ROWSIZE;
	}

	/** if(pvector) => should not be the case! */
	free((void*)pvector);

	/** clean-up matrix structure if error */
	if(error)
	{
		free((void*)pmatrix->pdata);
		pmatrix->xsize = -1;
		pmatrix->ysize = -1;
		pmatrix->pdata = 0x0;
	}
	else
	{
		pmatrix->ysize = ysize;
	}

	return error;
}

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		printf("Need at least a filename! (%d)\n",argc);
		return ERROR_NO_INPUT_FILE;
	}
	else if(argc>2)
	{
		/* assume option after filename */
		if(strcmp(argv[2],"--print")==0)
			do_print = 1;
	}

	printf("Reading data file... ");
	result = read_matrix(argv[1],&mymatrix);
	printf("done!\n");
	switch(result)
	{
		case ERROR_NOT_ENOUGH_MEMORY:
			printf("[ERROR] Not enough memory!\n");
			break;
		case ERROR_SSCANF_FAILED:
			printf("[ERROR] Function sscanf failed!\n");
			break;
		case ERROR_VARIABLE_ROWSIZE:
			printf("[ERROR] Inconsistent row size!\n");
			break;
		case ERROR_VALUE_TOO_LONG:
			printf("[ERROR] Text value overfow!\n");
			break;
		default:
			if(result<0)
			{
				printf("[ERROR] Unknown error!\n");
			}
			else
			{
				printf("[CHECK] Columns: %d, Rows: %d!\n",
					mymatrix.xsize, mymatrix.ysize);
				if(do_print)
				{
					printf("[PRINT]\n");
					ploop = mymatrix.pdata;
					for(loopy=0;loopy<mymatrix.ysize;loopy++)
					{
						for(loopx=0;loopx<mymatrix.xsize;loopx++)
						{
							printf(" %15.7e",*ploop); /** on linux, exp digits defaults to 2, on ms it's 3! obviously the software that produces that .dat file runs on ms! */
							ploop++;
						}
						putchar('\n');
					}
				}
				/* free it up! */
				free((void*)pmatrix->pdata);
				pmatrix->xsize = -1;
				pmatrix->ysize = -1;
				pmatrix->pdata = 0x0;
			}
	}

	return 0;
}
