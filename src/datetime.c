/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
/*----------------------------------------------------------------------------*/
int get_week_day(int year, int month, int day)
{
	/* using zeller's rule (Gregorian Calendar valid from 14/09/1752) */
	int yc, yy;
	/* adjust month */
	if(month<3) { month += 10; year--; }
	else { month -= 2; }
	/* represent year as century & year */
	yc = year / 100;
	yy = year % 100;
	return (day + ((13*month-1)/5) + yy + (yy/4) + (yc/4) - (yc*2)) % 7;
}
/*----------------------------------------------------------------------------*/
void print_tm_value(struct tm* ptime)
{
	printf("Sec:%2d Min:%2d Hour:%2d Day:%2d Mon:%2d Yr:%4d (%d,%d,%d)\n",
		ptime->tm_sec, ptime->tm_min, ptime->tm_hour,
		ptime->tm_mday, ptime->tm_mon, ptime->tm_year,
		ptime->tm_wday, ptime->tm_yday, ptime->tm_isdst);
}
/*----------------------------------------------------------------------------*/
void print_date_time(struct tm* ptime)
{
	static const char day_name[][4] = {"Sun", "Mon", "Tue", "Wed",
			"Thu", "Fri", "Sat" };
	static const char mon_name[][4] = { "Jan", "Feb", "Mar", "Apr",
			"May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	printf("%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
		day_name[ptime->tm_wday], mon_name[ptime->tm_mon], ptime->tm_mday,
		ptime->tm_hour, ptime->tm_min, ptime->tm_sec, 1900 + ptime->tm_year);
}
/*----------------------------------------------------------------------------*/
int main(void)
{
	time_t currtime;
	struct tm thistime, *infotime;
	int year = 1973, month = 3, day = 1, loop, test;
	const char* dayname[] = { "Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"};

	printf ("Enter year: ");
	scanf ("%d",&year);
	printf ("Enter month: ");
	scanf ("%d",&month);
	printf ("Enter day: ");
	scanf ("%d",&day);
	memset((void*)&thistime,0,sizeof(struct tm));

	/* try simple approach */
	thistime.tm_year = year - 1900;
	thistime.tm_mon = month - 1;
	thistime.tm_mday = day;
	//print_tm_value(&thistime);
	mktime(&thistime);
	//print_tm_value(&thistime);
	print_date_time(&thistime);
	printf ("  => That day is a %s.\n", dayname[get_week_day(year,month,day)]);
	printf ("  => That day is a %s.\n", dayname[infotime->tm_wday]);

	/* use current time as base */
/*
	time(&currtime);
	infotime = localtime(&currtime);
	infotime->tm_year = year - 1900;
	infotime->tm_mon = month - 1;
	infotime->tm_mday = day;
	print_tm_value(infotime);
	mktime(infotime);
	print_tm_value(infotime);
	print_date_time(infotime);
*/

	for(loop=0;loop<366; loop++)
	{
		thistime.tm_mday++;
		mktime(&thistime);
		test = get_week_day(thistime.tm_year+1900, thistime.tm_mon+1,
			thistime.tm_mday);
		if(thistime.tm_wday!=test)
		{
			print_date_time(&thistime);
			printf ("  => Mismatched [%d] <=> [%d]\n",thistime.tm_wday,test);
		}
	}

	return 0;
}
/*----------------------------------------------------------------------------*/
