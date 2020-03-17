/*----------------------------------------------------------------------------*/
#include "my1comlib.h"
#include "my1cons.h"
#include "my1bytes.h"
/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
/*----------------------------------------------------------------------------*/
#define ERROR_GENERAL -1
#define ERROR_PARAM_PORT -2
#define ERROR_PARAM_BAUD -3
#define ERROR_PARAM_DEVN -4
#define ERROR_PARAM_WHAT -5
/*----------------------------------------------------------------------------*/
void about(void)
{
	printf("Use: uartsend [options]\n");
	printf("Options are:\n");
	printf("  --port <number> : port number between 1-%d.\n",MAX_COM_PORT);
	printf("  --baud <number> : baudrate e.g. 9600(default),38400,115200.\n");
	printf("  --tty <device>  : alternate device name (useful in Linux).\n");
	printf("  --help  : show this message. overrides other options.\n");
	printf("  --scan  : scan and list ports. overrides other options.\n");
	printf("  <data>  : numeric data byte (hexadecimal prefix is '0x')\n\n");
}
/*----------------------------------------------------------------------------*/
void print_portscan(serial_port_t* port)
{
	int test, cCount = 0;
	printf("\n--------------------\n");
	printf("COM Port Scan Result\n");
	printf("--------------------\n");
	for(test=1;test<=MAX_COM_PORT;test++)
	{
		if(check_serial(port,test))
		{
			printf("%s%d: ",port->mPortName,COM_PORT(test));
			cCount++;
			printf("Ready.\n");
		}
	}
	printf("\nDetected Port(s): %d\n\n",cCount);
}
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	serial_port_t s_port;
	serial_conf_t s_conf;
	my1key_t key, esc;
	my1bytes data;
	int loop, test, term = 1, baud = 0, scan = 0;
	char *ptty = 0x0;
	/* initialize data struct */
	bytes_init(&data);
	bytes_make(&data,32); /* assume we don't need more than that */
	/* using unbuffered output */
	setbuf(stdout,0);
	/* check program arguments */
	if(argc>1)
	{
		for(loop=1;loop<argc;loop++)
		{
			if(argv[loop][0]=='-') /* options! */
			{
				if(!strcmp(argv[loop],"--port"))
				{
					if(get_param_int(argc,argv,&loop,&test)<0)
					{
						printf("Cannot get port number!\n\n");
						return ERROR_PARAM_PORT;
					}
					else if(test>MAX_COM_PORT)
					{
						printf("Invalid port number! (%d)\n\n", test);
						return ERROR_PARAM_PORT;
					}
					term = test;
				}
				else if(!strcmp(argv[loop],"--baud"))
				{
					if(get_param_int(argc,argv,&loop,&test)<0)
					{
						printf("Cannot get baud rate!\n\n");
						return ERROR_PARAM_BAUD;
					}
					baud = test;
				}
				else if(!strcmp(argv[loop],"--tty"))
				{
					if(!(ptty=get_param_str(argc,argv,&loop)))
					{
						printf("Error getting tty name!\n\n");
						return ERROR_PARAM_DEVN;
					}
				}
				else if(!strcmp(argv[loop],"--help"))
				{
					about();
					return 0;
				}
				else if(!strcmp(argv[loop],"--scan"))
				{
					scan = 1;
				}
				else
				{
					printf("Unknown option '%s'!\n",argv[loop]);
					return ERROR_PARAM_WHAT;
				}
			}
			else if(argv[loop][0]>='0'&&argv[loop][0]<='9') /* data! */
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
				return ERROR_PARAM_WHAT;
			}
		}
	}
	/* check if we have data to send */
	if (!data.fill)
	{
		printf("No data to send?\n");
		return 0;
	}
	/* initialize port */
	initialize_serial(&s_port);
#ifndef DO_MINGW
	sprintf(s_port.mPortName,"/dev/ttyUSB"); /* default on linux? */
#endif
	/* check user requested name change */
	if(ptty) sprintf(s_port.mPortName,"%s",ptty);
	/* check if user requested a port scan */
	if(scan)
	{
		print_portscan(&s_port);
		return 0;
	}
	/* try to prepare port with requested terminal */
	if(!term) term = find_serial(&s_port,0x0);
	if(!set_serial(&s_port,term))
	{
		printf("\n\nCannot prepare port '%s%d'!\n\n",s_port.mPortName,
			COM_PORT(term));
		return ERROR_GENERAL;
	}
	/* apply custom baudrate */
	if(baud)
	{
		get_serialconfig(&s_port,&s_conf);
		switch(baud)
		{
			case 1200: s_conf.mBaudRate = MY1BAUD1200; break;
			case 2400: s_conf.mBaudRate = MY1BAUD2400; break;
			case 4800: s_conf.mBaudRate = MY1BAUD4800; break;
			default: printf("Invalid baudrate (%d)! Using default!\n", baud);
			case 9600: s_conf.mBaudRate = MY1BAUD9600; break;
			case 19200: s_conf.mBaudRate = MY1BAUD19200; break;
			case 38400: s_conf.mBaudRate = MY1BAUD38400; break;
			case 57600: s_conf.mBaudRate = MY1BAUD57600; break;
			case 115200: s_conf.mBaudRate = MY1BAUD115200; break;
		}
		set_serialconfig(&s_port,&s_conf);
	}
	/* try opening port */
	if(!open_serial(&s_port))
	{
		printf("\n\nCannot open port '%s%d'!\n\n",s_port.mPortName,
			COM_PORT(s_port.mPortIndex));
		return ERROR_GENERAL;
	}
	/* clear input buffer */
	purge_serial(&s_port);

	/* show port settings */
	get_serialconfig(&s_port,&s_conf);
	baud = get_actual_baudrate(s_conf.mBaudRate);
	printf("Port:'%s%d'{Baud:%d},",
		s_port.mPortName,COM_PORT(s_port.mPortIndex),baud);
	printf("DataSize:(%d)\n",data.fill);

	/* now send bytes! */
	printf("TX:");
	for (loop=0;loop<data.fill;loop++)
	{
		printf("[%02X]",data.data[loop]);
		put_byte_serial(&s_port,data.data[loop]);
	}
	printf("\nRX:");

	/* start main loop */
	while(1)
	{
		key = get_keyhit_extended(&esc);
		if(esc==KEY_NONE&&key==KEY_ESCAPE)
			break;
		/* check serial port for incoming data */
		if(check_incoming(&s_port))
		{
			byte_t cTemp = get_byte_serial(&s_port);
			printf("[%02X]",cTemp);
		}
		/* check port status? */
		if(s_port.mPortStat!=MY1PORT_STATUS_OK)
		{
			printf("Port Error (%d)! Aborting!\n",s_port.mPortStat);
			break;
		}
	}
	/* close port */
	close_serial(&s_port);
	/* done */
	printf("\nRX:");
	return 0;
}
/*----------------------------------------------------------------------------*/
