/**
 * program to calculate crc values
 * - implemented as function crc_byte
 * - by azman@my1matrix.net 20190922
**/

/*----------------------------------------------------------------------------*/
typedef unsigned char byte08_t;
typedef unsigned short word16_t;
typedef unsigned int word32_t;
/*----------------------------------------------------------------------------*/
#define CRC16_POLY 0xA001
#define CRC32_POLY_FORWARD 0x04C11DB7
#define CRC32_POLY_REVERSE 0xEDB88320
/*----------------------------------------------------------------------------*/
word32_t bit_reverse(word32_t data, word32_t mask)
{
	word32_t brev = 0;
	while (mask&0x1)
	{
		brev <<= 1;
		brev |= data & 0x1;
		data >>= 1;
		mask >>= 1;
	}
	return brev;
}
/*----------------------------------------------------------------------------*/
word32_t crc_byte_reflected(word32_t ccrc, word32_t poly, byte08_t data)
{
	int loop;
	/* reflected! */
	ccrc = ccrc ^ data;
	/* for every data bit! */
	for (loop=0;loop<8;loop++)
	{
		if (ccrc&0x01) ccrc = (ccrc >> 1) ^ poly;
		else ccrc = ccrc >> 1;
	}
	return ccrc;
}
/*----------------------------------------------------------------------------*/
word32_t crc_byte_normal(word32_t ccrc, word32_t poly, byte08_t data)
{
	int loop;
	/* normal */
	word32_t high = bit_reverse(1,~0U);
	ccrc = ccrc ^ (bit_reverse(data,~0U));
	/* for every data bit! */
	for(loop=0;loop<8;loop++)
	{
		if(ccrc&high) ccrc = (ccrc << 1) ^ poly;
		else ccrc = ccrc << 1;
	}
	return ccrc;
}
/*----------------------------------------------------------------------------*/
#ifndef __NOT_CRC_APP__
/*----------------------------------------------------------------------------*/
#include<stdio.h>
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	word32_t crcf = ~0U, crcr = ~0U;
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
		crcf = crc_byte_normal(crcf,CRC32_POLY_FORWARD,test&0xff);
		crcr = crc_byte_reflected(crcr,CRC32_POLY_REVERSE,test&0xff);
		if (loop>1) printf(", ");
		printf("%d (0x%02x)",test,test);
	}
	if (loop==1)
	{
		/* use basic string */
		for(test=0x31;loop<10;loop++,test++)
		{
			crcf = crc_byte_normal(crcf,CRC32_POLY_FORWARD,test&0xff);
			crcr = crc_byte_reflected(crcr,CRC32_POLY_REVERSE,test&0xff);
			if (loop>1) printf(", ");
			printf("%d (0x%02x)",test,test);
		}
	}
	/* notice that we need to invert! normal needs bit reverse! */
	printf("\nCRCF: 0x%08x",bit_reverse(~crcf,~0U));
	printf("\nCRCR: 0x%08x\n",~crcr);
	return 0;
}
/*----------------------------------------------------------------------------*/
#endif /** __NOT_CRC_APP__ */
/*----------------------------------------------------------------------------*/
