/*----------------------------------------------------------------------------*/
/**
 * - this is now part of my1uarttool repo
 * - kept here to show how to build against my1codelib stuffs
**/
/*----------------------------------------------------------------------------*/
#include "my1uart.h"
#include "my1keys.h"
#include "my1bytes.h"
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
/*----------------------------------------------------------------------------*/
#ifndef PROGNAME
#define PROGNAME "uartsend"
#endif
/*----------------------------------------------------------------------------*/
void about(void)
{
	printf("\nUse: %s [options] <data>\n",PROGNAME);
	printf("Options   :\n");
	printf("  --port <number> : port number between 1-%d.\n",MAX_COM_PORT);
	printf("  --baud <number> : baudrate e.g. 9600(default),38400,115200.\n");
	printf("  --tty <device>  : alternate device name (useful in Linux).\n");
	printf("  --help  : show this message. overrides other options.\n");
	printf("  --scan  : scan and list ports. overrides other options.\n\n");
	printf("  <data>  : numeric data byte (hexadecimal prefix is '0x')\n\n");
}
/*----------------------------------------------------------------------------*/
void print_portscan(my1uart_t* port)
{
	int test, size = 0;
	printf("\n--------------------\n");
	printf("COM Port Scan Result\n");
	printf("--------------------\n");
	for (test=1;test<=MAX_COM_PORT;test++)
	{
		if (uart_prep(port,test))
		{
			printf("%s: Ready\n",port->temp);
			size++;
		}
	}
	printf("\nDetected Port(s): %d\n\n",size);
}
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	my1uart_t port;
	my1key_t key, esc;
	my1bytes_t data;
	int loop, test, term = 1, baud = 0, scan = 0;
	char *ptty = 0x0;
	/* initialize data struct */
	bytes_init(&data);
	bytes_make(&data,32); /* assume we don't need more than that */
	/* using unbuffered output */
	setbuf(stdout,0);
	/* check program arguments */
	for (loop=1;loop<argc;loop++)
	{
		if (argv[loop][0]=='-') /* options! */
		{
			if (!strcmp(argv[loop],"--port"))
			{
				if (get_param_int(argc,argv,&loop,&test)<0)
				{
					printf("Cannot get port number!\n\n");
					return -1;
				}
				else if (test>MAX_COM_PORT)
				{
					printf("Invalid port number! (%d)\n\n", test);
					return -1;
				}
				term = test;
			}
			else if (!strcmp(argv[loop],"--baud"))
			{
				if (get_param_int(argc,argv,&loop,&test)<0)
				{
					printf("Cannot get baud rate!\n\n");
					return -1;
				}
				baud = test;
			}
			else if (!strcmp(argv[loop],"--tty"))
			{
				if (!(ptty=get_param_str(argc,argv,&loop)))
				{
					printf("Error getting tty name!\n\n");
					return -1;
				}
			}
			else if (!strcmp(argv[loop],"--help"))
			{
				about();
				return 0;
			}
			else if (!strcmp(argv[loop],"--scan"))
			{
				scan = 1;
			}
			else
			{
				printf("Unknown option '%s'!\n",argv[loop]);
				return -1;
			}
		}
		else if (argv[loop][0]>='0'&&argv[loop][0]<='9') /* data! */
		{
			char format[] = "%x";
			if (argv[loop][0]!='0'||argv[loop][1]!='x')
				format[1] = 'd';
			sscanf(argv[loop],format,&test);
			if ((test&0xff)!=test)
				printf("[WARN] Invalid byte '%d'? [%s]\n",test,argv[loop]);
			bytes_slip(&data,(byte08_t)test);
		}
		else /* not an option? */
		{
			printf("Unknown parameter %s!\n",argv[loop]);
			return -1;
		}
	}
	/* initialize port */
	uart_init(&port);
	/* check user requested name change */
	if (ptty) uart_name(&port,ptty);
	/* check if user requested a port scan */
	if (scan)
	{
		print_portscan(&port);
		return 0;
	}
	/* check if we have data to send */
	if (!data.fill)
	{
		about();
		return 0;
	}
	/* try to prepare port with requested terminal */
	if (!term) term = uart_find(&port,0x0);
	if (!uart_prep(&port,term))
	{
		printf("Cannot prepare port '%s'!\n\n",port.temp);
		return -1;
	}
	/* apply custom baudrate */
	if (baud)
	{
		if (!uart_set_baudrate(&port,baud))
			printf("-- Invalid baudrate (%d)! ",baud);
		printf("-- Baudrate: %d\n",uart_get_baudrate(&port));
	}
	/* try opening port */
	if (!uart_open(&port))
	{
		printf("Cannot open port '%s'!\n\n",port.temp);
		return -1;
	}
	/* clear input buffer */
	uart_purge(&port);

	/* show port settings */
	baud = uart_get_baudrate(&port);
	printf("Port:'%s'{Baud:%d},DataSize:(%d)\n",port.temp,baud,data.fill);

	/* now send bytes! */
	printf("TX:");
	for (loop=0;loop<data.fill;loop++)
	{
		printf("[%02X]",data.data[loop]);
		uart_send_byte(&port,data.data[loop]);
	}
	bytes_free(&data);

	/* start main loop - wait for reply */
	printf("\nRX:");
	while(1)
	{
		/* check user abort request */
		key = get_keyhit_extended(&esc);
		if(esc==KEY_NONE&&key==KEY_ESCAPE)
			break;
		/* check serial port for incoming data */
		if(uart_incoming(&port))
		{
			test = uart_read_byte(&port);
			printf("[%02X]",test);
		}
		/* check port status? */
		if (!uart_okstat(&port))
		{
			printf("Port Error (%d)! Aborting!\n",port.stat);
			break;
		}
	}
	/* close port */
	uart_done(&port);
	/* done */
	return 0;
}
/*----------------------------------------------------------------------------*/
