/**
 * program to calculate crc16 values
 * - implemented as function crc_byte
 * - based on aquaread blackbox instruction manual
 * - by azman@my1matrix.net 20161001
**/

/*----------------------------------------------------------------------------*/
typedef unsigned char abyte;
typedef unsigned short aword;
/*----------------------------------------------------------------------------*/
#define CRC16_POLY 0xA001
/*----------------------------------------------------------------------------*/
aword crc_byte(aword ccrc, abyte data)
{
	int loop;
	ccrc = ccrc ^ data;
	for(loop=0;loop<8;loop++)
	{
		if(ccrc & 0x0001) ccrc = (ccrc >> 1) ^ CRC16_POLY;
		else ccrc = ccrc >> 1;
	}
	return ccrc;
}
/*----------------------------------------------------------------------------*/
#include<stdio.h>
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	aword mcrc = 0xffff;
	int loop, test;
	printf("Data: ");
	for(loop=1;loop<argc;loop++)
	{
		if(argv[loop][0]=='0'&&(argv[loop][1]=='x'||argv[loop][1]=='X'))
		{
			if(sscanf(argv[loop],"%x",&test)!=1)
				printf("\nError reading value '%s'!\n",argv[loop]);
		}
		else
		{
			if(sscanf(argv[loop],"%d",&test)!=1)
				printf("\nError reading value '%s'!\n",argv[loop]);
		}
		if(test<0||test>0xff)
			printf("Invalid byte '%s'!\n",argv[loop]);
		mcrc = crc_byte(mcrc,(abyte)test&0xff);
		printf("%d (0x%02x), ",test,test);
	}
	printf("CRC: [%d,%d] (0x%02x)\n",((mcrc>>8)&0xff),mcrc&0xff,mcrc);
	return 0;
}
/*----------------------------------------------------------------------------*/
