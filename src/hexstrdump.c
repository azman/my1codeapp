/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
//#include <string.h>
/*----------------------------------------------------------------------------*/
void about(void)
{
	printf("Use: hexstrdump [options]\n");
	printf("Options are:\n");
	printf("  <data>  : numeric data byte (hexadecimal prefix is '0x')\n\n");
}
/*----------------------------------------------------------------------------*/
void hexdumpC(unsigned char *data, int size)
{
	int test, loop;
	int addr = 0, cols = 0;
	char buff[16]; /* line buffer */
	while (1)
	{
		/* show address @start of line */
		if (cols==0)
		{
			char format[] = "%12x  ";
			/* prepare address formatting */
			if (addr<0x100000000)
			{
				format[1] = '0';
				format[2] = '8';
			}
			if (addr<size) printf(format,addr);
			else if (addr<0x100000000) printf("xxxxxxxx  ");
			else printf("xxxxxxxxxxxx  ");
		}
		if (addr<size)
		{
			test = (int) data[addr++];
			printf("%02x ",(test&0xff));
		}
		else
		{
			test = 0;
			printf("   ");
		}
		/* show data in ascii - '.' if non-printable */
		buff[cols++] = isprint(test) ? (char) test : '.';
		/* check maximum line buffer */
		if (cols==8) printf(" ");
		else if (cols==16)
		{
			printf(" |");
			for(loop=0;loop<16;loop++)
				putchar(buff[loop]);
			printf("|\n");
			if (addr==size) break;
			cols = 0;
		}
	}
}
/*----------------------------------------------------------------------------*/
#define DATA_BLOCK_SIZE 32
/*----------------------------------------------------------------------------*/
unsigned char* more_data(unsigned char** curr, int *size)
{
	int next = *size + DATA_BLOCK_SIZE;
	unsigned char* prev = *curr;
	unsigned char* temp = (unsigned char*) realloc(prev,next);
	if (temp)
	{
		*curr = temp;
		*size = next;
	}
	return temp;
}
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	unsigned char *data;
	int loop, test, size, fill;
	/* initialize data struct */
	size = DATA_BLOCK_SIZE;
	data = (unsigned char*)malloc(size);
	fill = 0;
	/* check program arguments */
	if(argc>1)
	{
		for(loop=1;loop<argc;loop++)
		{
			if(argv[loop][0]>='0'&&argv[loop][0]<='9') /* data! */
			{
				char format[] = "%x";
				if (argv[loop][0]!='0'||argv[loop][1]!='x')
					format[1] = 'd';
				sscanf(argv[loop],format,&test);
				if ((test&0xff)!=test)
					printf("[WARN] Invalid byte '%d'? [%s]\n",test,argv[loop]);
				if (fill==size) more_data(&data,&size);
				if (fill<size)
					data[fill++] = (unsigned char) test&0xff;
				else
					printf("[WARN] Cannot fill data %d/%d\n",fill,size);
			}
			else /* not an option? */
			{
				printf("-- Unknown parameter %s!\n",argv[loop]);
			}
		}
	}
	/* check if we have data to dump */
	if (!fill)
		printf("No data to dump?\n");
	else
	{
		/* now send bytes! */
		printf("-- Data:\n");
		hexdumpC(data,fill);
	}
	/* free memory */
	free((void*)data);
	return 0;
}
/*----------------------------------------------------------------------------*/
