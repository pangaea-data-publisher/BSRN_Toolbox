/********************************************************************************/
/***	Programname:	F_CHECK.C					      ***/
/***	Author:		Vlado Nespor					      ***/
/***	Version:	3.3,  01-OCT-2000				      ***/
/***									      ***/
/***	Function:	Checks station-to-archive data files for errors	      ***/
/***			in format of metadata and atm. data and writes	      ***/
/***			a report file					      ***/
/***									      ***/
/***	Input:		Station-to-archive filename1.dat [filename2.dat ...]  ***/
/***									      ***/
/***	Output:		filename1.rep [filename2.rep ...]		      ***/
/***									      ***/
/***	Note:		Please report problems, suggestions, etc. to:	      ***/
/***				Vlado Nespor	nespor@geo.umnw.ethz.ch       ***/
/***			or	Guido Mueller	muller@geo.umnw.ethz.ch	      ***/
/***	Compilation:	cc -o f_check f_check.c				      ***/
/********************************************************************************/
/***	Modifications:	v.3.0 -> 3.1:	-log. records 310 and 330 changed to  ***/
/***					 log. records 3010 and 3030           ***/
/***					-added log. record 3300               ***/
/***	                v.3.1 -> 3.2:	-year check changed to 4 difits       ***/
/***	                v.3.2 -> 3.3:	-handels 3-band and 4-band radiation  ***/
/***					 instruments                          ***/
/********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void f_check_usage(char *name)
{
printf ("\n Usage : %s filename1.dat [filename2.dat ...]\n\n",name);
printf (" Input : filename1.dat,filename2.dat,... - names of station-to-archive data\n");
printf ("                                           files to be checked for format\n");
printf (" Output: filename1.rep,filename2.rep,... - report files\n\n");
printf (" Note  :	Please report problems, suggestions, etc. to:\n");
printf ("   			Vlado Nespor	nespor@geo.umnw.ethz.ch \n");
printf ("   		or	Guido Mueller	muller@geo.umnw.ethz.ch \n\n");
exit(1);
}

FILE *fin;			/* pointer to inputfile 	      		*/
FILE *fout;			/* pointer to outputfile	      		*/


int main(int narg, char *arg[])

{
int OS = 3;	/* Operating system: 1 = Windows, 2 = MacOS X, 3 = UNIX */

char file_out[100];		/* name of report file				*/
int sta_id;			/* station identification number		*/
int month;			/* month in input file name			*/
int year;			/* year in input file name			*/
int year4;			/* 4 digit year in input file name		*/
int year_min=1992,year_max=2030;/* min. and max. legal years			*/
char buffer[120];		/* buffer for input line			*/
int rec_num;			/* logical record number			*/
char rec_name[20];		/* logical record name				*/
char msg1[1500];		/* 1st field for error messages			*/
char msg2[1500];		/* 2nd field for error messages			*/
char msg3[1500];		/* 3rd field for error messages			*/
char sformat[100];		/* scan    format for record line		*/
char pformat[100];		/* print   format for record line		*/
char xformat[150];		/* correct format for record line		*/
int day0,day1;			/* previous and current day of measurement	*/
int min0,min1;			/* previous and current minute of measurement	*/
int seq0,seq1;			/* previous and current seq. no. in logrec 1100	*/
int hei0,hei1;			/* previous and current height in logrec 1100	*/
int band4=0;			/* flag for 3-band or 4-band rad. instrument	*/

char c_temp;			/* temporary scan fields			*/
char *pfilename;		/* pointer to the filename (without whole path)	*/
int i1[30];
float f1[30];
char c1[81],c2[81],c3[81],c4[81];

int err=0;			/* exit value from main: # of files with error	*/

int lines;
int i,j;
int count1;
int count2;
int count3;
int buf_len;
if(narg==1)			/* if no arguments on command line		*/
	f_check_usage(arg[0]);

fin=fout=NULL;			/* stream pointer initialization		*/

/********************************************************************************/
/* main loop for all files specified at command line				*/
/********************************************************************************/
for(i=1;i<narg;i++)		
	{
	if(fin!=NULL) fclose(fin);	/* close files if opened		*/
	if(fout!=NULL) fclose(fout);

	switch(OS)
	{
		case 1:
			pfilename=strrchr(arg[i],'\\');
			break;
		case 2:	
			pfilename=strrchr(arg[i],'/');
			break;
		case 3:	
			pfilename=strrchr(arg[i],'/');
			break;
		default:	
			pfilename=strrchr(arg[i],'/');
			break;
	}

	if(pfilename==NULL)
		pfilename=arg[i];
	else
		pfilename++;

	/*======================================================================*/
	/* Check for legal input filenames					*/
	/*======================================================================*/
	sprintf(buffer,"%.2s\0",pfilename+3);
	month=atoi(buffer);
	if(month<1 || month>12)
		{
		strcpy(msg1,"*ERROR: Illegal month '%02d' - 4th and 5th characters\n");
		strcat(msg1,"        in input file name '%s'.\n\0");
		printf(msg1,month,pfilename);
		err=1;
		continue;
		}

	sprintf(buffer,"%.2s\0",pfilename+5);
	year=atoi(buffer);
	/* convert year to four digit number					*/
	year4=year+2000 - (int)((year+50)/100)*100;
	if(year4<year_min || year4>year_max)
		{
		strcpy(msg1,"*ERROR: Illegal year '%02d' - 6th and 7th characters\n");
		strcat(msg1,"        in input file name '%s'.\n");
		strcat(msg1,"        (Expected year should be between %4d and %4d.)\n\0");
		printf(msg1,year,pfilename,year_min,year_max);
		err=1;
		continue;
		}

	if(strncmp(pfilename+7,".dat\0",5))
		{
		strcpy(msg1,"*ERROR: Illegal extension '%s' - from 8th character on\n");
		strcat(msg1,"        in input file name '%s'.\n");
		strcat(msg1,"        (Expected extension is '.dat'.)\n\0");
		printf(msg1,pfilename+7,pfilename);
		err=1;
		continue;
		}

	/************************************************************************/
	/* open input file and report file					*/
	/************************************************************************/
	if((fin=fopen(arg[i],"r"))==NULL)
		{
		printf("Error while opening '%s' file!\n",arg[i]);
		err=1;
		continue;		/* continue with next file		*/
		}

	strcpy(file_out,arg[i]);
	sprintf(strrchr(file_out,'.'),".rep");

	if((fout=fopen(file_out,"w"))==NULL)
		{
		printf("Error while opening '%s' file!\n",file_out);
		err=1;
		continue;		/* continue with next file		*/
		}

	strcpy(msg1,"\n**********\nFile name: %s\n**********\n"); /* message	*/
	printf(msg1,pfilename);		/* print message on screen		*/
	fprintf(fout,msg1,pfilename);	/* print message into report file	*/


	/*======================================================================*/
	/* Check of the input file for line length (80 characters)		*/
	/*======================================================================*/
	count1=0;
	count2=0;
	count3=0;

	strcpy(msg1,"*ERROR: A line longer than 80 characters or missing line separator.\n\n");
	strcat(msg1,"        For help the number and the first 20 characters of the line\n");
	strcat(msg1,"        causing the problem are printed below.\n");
	strcat(msg1,"        -------------------------------------------\n");
	strcat(msg1,"        | line number:  |  first 20 characters:   |\n");
	strcat(msg1,"        -------------------------------------------\n\0");

	while (fscanf(fin,"%c",&c_temp) != EOF)
		{
		count1++;		/* counter of characters in line	*/

		if(count1<=20)		/* store first 20 characters		*/
			buffer[count1-1]=c_temp;

		if(c_temp=='\n')		/* the end of line		*/
			{
			count2++;	/* counter of lines in input file	*/
			if(count1>81)
				{
				count3++; /* counter of errors			*/
				if(count3==1)
					{
					printf("%s",msg1);
					fprintf(fout,"%s",msg1);
					}
				strcpy(msg2,"        |   %6d      |  %.20s   |\n");
				strcat(msg2,"        -------------------------------------------\n");
				printf(msg2,count2,buffer);
				fprintf(fout,msg2,count2,buffer);

				if(count3>4)
					{
					strcpy(msg2,"        NOTE: Only first 5 wrong lines are printed.\n");
					printf(msg2);
					fprintf(fout,msg2);
					count1=0;
					break;
					}
				}
			count1=0;
			}
		}
	if(count1!=0)
		{
		strcpy(msg2,"*ERROR: Missing 'new line' at the end of file.\n");
		printf(msg2);
		fprintf(fout,msg2);
		err=1;
		continue;
		}
	if(count3==0)
		{
		strcpy(msg2,"*Check for line length.......... OK\n");
		printf(msg2);
		fprintf(fout,msg2);
		}
	else
		{
		err=1;
		continue;
		}


	/*======================================================================*/
	/* Check of the input file for illegal characters			*/
	/*======================================================================*/
	count1=0;
	count2=0;
	count3=0;

	strcpy(msg1,"*ERROR: An illegal character occured.\n\n");
	strcat(msg1,"        Allowed ASCII characters:\n");
	strcat(msg1,"           - in logical records 1000 and less than 100:\n");
	strcat(msg1,"                         all printable characters from 'space' to '~',\n");
	strcat(msg1,"                         and in addition, for logical record 3 also \n");
	strcat(msg1,"                         'tabulator' (09 hex) is alloved.\n");
	strcat(msg1,"           - in all other logical records:\n");
	strcat(msg1,"                         'space','+','-','.' and digits from '0' to '9'.\n\n");
	strcat(msg1,"        For help the logical record number, the number of the line from\n");
	strcat(msg1,"        beginning of the file, position of the illegal character in the\n");
	strcat(msg1,"        line, character itself (if printable) and its hexadecimal value\n");
	strcat(msg1,"        are printed below.\n");
	strcat(msg1,"        --------------------------------------------------------------------\n");
	strcat(msg1,"        |  log. record:  | line number:  |  position:  |  wrong character  |\n");
	strcat(msg1,"        --------------------------------------------------------------------\n\0");

	rec_num=0;
	rewind(fin);		/* go to the beginning of input file		*/
	while (fgets(buffer,100,fin) != NULL)
		{
		count2++;		/* counter of line in input file	*/
		buf_len=strlen(buffer);

		if (buffer[0]=='*')	/* get logical record number		*/
			{
			sprintf(rec_name,"%.6s\0",buffer);
			rec_num=atoi(rec_name+2);
			continue;
			}

		if(rec_num==0)
			{
			count3++;	/* counter of errors			*/
			strcpy(msg2,"*ERROR: Illegal characters at the line %d of the file,\n");
			strcat(msg2,"        or wrong record number.\n\n");
			printf(msg2,count2-1);
			fprintf(fout,msg2,count2-1);
			break;
			}

		/****************************************************************/
		/* logical record dependent check				*/
		/****************************************************************/
		if(rec_num>99 && rec_num!=1000)
			{
			for(j=0;j<buf_len;j++)
				if(buffer[j]=='\n' || buffer[j]=='-' || buffer[j]=='+' || buffer[j]=='.' || 
				   buffer[j]==' ' || buffer[j]>='0' && buffer[j]<='9')
					continue;
				else
					break;

			if(j<buf_len) /* illegal character			*/ 
				{
				/* not printable character replace with ' '	*/
				c_temp=(buffer[j]<' ' || buffer[j]>'~')?' ':buffer[j];

				count3++;	/* counter of errors		*/

				if(count3==1)
					{
					printf(msg1);
					fprintf(fout,msg1);
					}
				strcpy(msg2,"        |      %4d      |   %6d      |      %2d     |   %c   |  %02X (hex) |\n");
				strcat(msg2,"        --------------------------------------------------------------------\n");
				printf(msg2,rec_num,count2,j+1,c_temp,buffer[j]);
				fprintf(fout,msg2,rec_num,count2,j+1,c_temp,buffer[j]);

				if(count3>4)
					{
					strcpy(msg2,"        NOTE: Only first 5 lines with illegal characters are printed.\n");
					printf(msg2);
					fprintf(fout,msg2);
					count1=0;
					break;
					}
				}
			}
		else
			{
			for(j=0;j<buf_len;j++)
				if(buffer[j]=='\n' || buffer[j]>=' ' && buffer[j]<='~' || buffer[j]=='\t' && rec_num==3)
					continue;
				else
					break;
			if(j<buf_len)
				{
				c_temp=' ';
				count3++;
				if(count3==1)
					{
					printf("%s",msg1);
					fprintf(fout,"%s",msg1);
					}
				strcpy(msg2,"        |      %4d      |   %6d      |      %2d     |   %c   |  %02X (hex) |\n");
				strcat(msg2,"        --------------------------------------------------------------------\n");
				printf(msg2,rec_num,count2,j+1,c_temp,buffer[j]);
				fprintf(fout,msg2,rec_num,count2,j+1,c_temp,buffer[j]);

				if(count3>4)
					{
					strcpy(msg2,"        NOTE: Only first 5 lines with illegal characters are printed.\n");
					printf(msg2);
					fprintf(fout,msg2);
					count1=0;
					break;
					}
				}
			}


		}
	if(count3==0)
		{
		strcpy(msg2,"*Check for illegal characters... OK\n");
		printf(msg2);
		fprintf(fout,msg2);
		}
	else
		{
		err=1;
		continue;
		}


	/*======================================================================*/
	/* Check of the input file for record line formats			*/
	/*======================================================================*/
	count1=0;			/* indicator of line in logical record	*/
	count2=0;			/* counter of line in input file	*/
	count3=0;

	strcpy(msg1,"*ERROR: Incorrect format.\n\n");
	strcat(msg1,"        For each logical record the line number from the beginning of\n");
	strcat(msg1,"        the file and the message only for the first line with wrong\n");
	strcat(msg1,"        format is printed below.\n\n\0");

	strcpy(msg2,"     Log. record: %6d\n");
	strcat(msg2,"     Line number: %6d\n");

	rec_num=0;
	rewind(fin);
	while (fgets(buffer,100,fin) != NULL)
		{
		count2++;		/* counter of line in input file	*/

		if (buffer[0]=='*')	/* get logical record number		*/
			{
			sprintf(rec_name,"%.6s\0",buffer);
			rec_num=atoi(rec_name+2);
			day0=min0=count1=0; 	/* reset day, min. and counter	*/
			seq0=hei0=0;		/* reset seq. no. and height	*/
			continue;		/* of lines in record		*/
			}

		/* do not check these logical records				*/
		if(day0==-1 || rec_num==3 || rec_num==1000)
			continue;

		switch(rec_num)	/* switch according to logical record number	*/
			{
			case    1:
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
						strcpy(sformat,"%d%d%d%d");
						strcpy(pformat," %2d %2d %4d %2d\n");
						strcpy(xformat," ## ## #### ##|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&sta_id,&month,&year,i1);
						sprintf(file_out,pformat,sta_id,month,year,i1[0]);

						break;

					default: count1++;
						strcpy(sformat,"%d%d%d%d%d%d%d%d");
						strcpy(pformat," %9d %9d %9d %9d %9d %9d %9d %9d\n");
						strcpy(xformat," ######### ######### ######### ######### ######### ######### ######### #########");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2,i1+3,i1+4,i1+5,i1+6,i1+7);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2],i1[3],i1[4],i1[5],i1[6],i1[7]);

						break;
					}
				break;

			case    2:
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
						strcpy(sformat,"%d%d%d");
						strcpy(pformat," %2d %2d %2d\n");
						strcpy(xformat," ## ## ##|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2]);

						break;

					case 1: count1=2;
						strcpy(sformat,"%38c%1c%20c%1c%20c");
						strcpy(pformat,"%-38.38s %-20.20s %-20.20s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXX");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c2,&c_temp,c3);
						sprintf(file_out,pformat,c1,c2,c3);

						break;

					case 2: count1=3;
						strcpy(sformat,"%15c%1c%50c");
						strcpy(pformat,"%-15.15s %-50.50s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c2);
						sprintf(file_out,pformat,c1,c2);

						break;

					case 3: count1=0;
						strcpy(sformat,"%80c");
						strcpy(pformat,"%-80.80s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1);
						sprintf(file_out,pformat,c1);

						break;
					}
				break;

			case    4:
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
					case 6: if(count1==6) count1=7;
						strcpy(sformat,"%d%d%d");
						strcpy(pformat," %2d %2d %2d\n");
						strcpy(xformat," ## ## ##|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2]);

						break;

					case 1: count1=2;
						strcpy(sformat,"%d%d");
						strcpy(pformat," %2d %2d\n");
						strcpy(xformat," ## ##|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1);
						sprintf(file_out,pformat,i1[0],i1[1]);

						break;

					case 2: count1=3;
						strcpy(sformat,"%80c");
						strcpy(pformat,"%-80.80s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1);
						sprintf(file_out,pformat,c1);

						break;

					case 3: count1=4;
						strcpy(sformat,"%20c%1c%20c");
						strcpy(pformat,"%-20.20s %-20.20s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXX|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c2);
						sprintf(file_out,pformat,c1,c2);

						break;

					case 4: count1=5;
						strcpy(sformat,"%15c%1c%50c");
						strcpy(pformat,"%-15.15s %-50.50s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c2);
						sprintf(file_out,pformat,c1,c2);

						break;

					case 5: count1=6;
						strcpy(sformat,"%f%f%d%1c%5c");
						strcpy(pformat," %7.3f %7.3f %4d %-5.5s\n");
						strcpy(xformat," ###.### ###.### #### XXXXX|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,f1,f1+1,i1,&c_temp,c1);
						sprintf(file_out,pformat,f1[0],f1[1],i1[0],c1);

						break;

					case 7: count1=7;
						strcpy(sformat,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d");
						strcpy(pformat," %3d %2d %3d %2d %3d %2d %3d %2d %3d %2d %3d %2d %3d %2d %3d %2d %3d %2d %3d %2d %3d %2d\n");
						strcpy(xformat," ### ## ### ## ### ## ### ## ### ## ### ## ### ## ### ## ### ## ### ## ### ##|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2,i1+3,i1+4,i1+5,i1+6,i1+7,i1+8,i1+9,i1+10,
							i1+11,i1+12,i1+13,i1+14,i1+15,i1+16,i1+17,i1+18,i1+19,i1+20,i1+21);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2],i1[3],i1[4],i1[5],i1[6],i1[7],i1[8],i1[9],i1[10],
							i1[11],i1[12],i1[13],i1[14],i1[15],i1[16],i1[17],i1[18],i1[19],i1[20],i1[21]);

						break;
					}
				break;

			case    5:
			case    6:
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
						strcpy(sformat,"%d%d%d%1c%1c");
						strcpy(pformat," %2d %2d %2d %c\n");
						strcpy(xformat," ## ## ## X|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2,&c_temp,c1);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2],c1[0]);

						break;

					case 1: count1=2;
						if(rec_num==5)
							{
							strcpy(sformat,"%30c%1c%25c%d%d%d%d%d%1c%5c");
							strcpy(pformat,"%-30.30s %-25.25s %3d %2d %2d %2d %2d %-5.5s\n");
							strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXX ### ## ## ## ## XXXXX|");

							/* scan numbers and print them with correct format */
							sscanf(buffer,sformat,c1,&c_temp,c2,i1,i1+1,i1+2,i1+3,i1+4,&c_temp,c3);
							sprintf(file_out,pformat,c1,c2,i1[0],i1[1],i1[2],i1[3],i1[4],c3);
							}
						else
							{
							strcpy(sformat,"%30c%1c%25c%d%1c%5c");
							strcpy(pformat,"%-30.30s %-25.25s %3d %-5.5s\n");
							strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXX ### XXXXX|");

							/* scan numbers and print them with correct format */
							sscanf(buffer,sformat,c1,&c_temp,c2,i1,&c_temp,c3);
							sprintf(file_out,pformat,c1,c2,i1[0],c3);
							}

						break;

					case 2: count1=0;
						strcpy(sformat,"%80c");
						strcpy(pformat,"%-80.80s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1);
						sprintf(file_out,pformat,c1);

						break;
					}
				break;

			case    7:
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
						strcpy(sformat,"%d%d%d");
						strcpy(pformat," %2d %2d %2d\n");
						strcpy(xformat," ## ## ##|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2]);

						break;

					case 6: count1=0;
						strcpy(sformat,"%1c%1c%1c%1c%1c%1c%1c%1c%1c%1c%1c");
						strcpy(pformat,"%c %c %c %c %c %c\n");
						strcpy(xformat,"X X X X X X|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c1+1,&c_temp,c1+2,&c_temp,c1+3,&c_temp,c1+4,&c_temp,c1+5);
						sprintf(file_out,pformat,c1[0],c1[1],c1[2],c1[3],c1[4],c1[5]);

						break;

					default: count1++;
						strcpy(sformat,"%80c");
						strcpy(pformat,"%-80.80s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1);
						sprintf(file_out,pformat,c1);

						break;
					}
				break;

			case    8:
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
						strcpy(sformat,"%d%d%d%1c%1c");
						strcpy(pformat," %2d %2d %2d %c\n");
						strcpy(xformat," ## ## ## X|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2,&c_temp,c1);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2],c1[0]);

						break;

					case 1: count1=2;
						strcpy(sformat,"%30c%1c%15c%1c%18c%1c%8c%d");
						strcpy(pformat,"%-30.30s %-15.15s %-18.18s %-8.8s %5d\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXX XXXXXXXX #####");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c2,&c_temp,c3,&c_temp,c4,i1);
						sprintf(file_out,pformat,c1,c2,c3,c4,i1[0]);

						break;

					case 2: count1=3;
						strcpy(sformat,"%80c");
						strcpy(pformat,"%-80.80s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1);
						sprintf(file_out,pformat,c1);

						break;

					case 3: count1=4;
						if(sscanf(buffer,"%s%s%s%s%s%s%s%s%s%s%s%s",c1,c1,c1,c1,c1,c1,c1,c1,c1,c1,c1,c1)<=10)
							{
							band4=0; /* 3-band rad. instrument */
							strcpy(sformat,"%d%d%f%f%f%f%f%f%d%d");
							strcpy(pformat," %2d %2d %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f %2d %2d\n");
							strcpy(xformat," ## ## ###.### ###.### ###.### ###.### ###.### ###.### ## ##|");

							/* scan numbers and print them with correct format */
							sscanf(buffer,sformat,i1,i1+1,f1,f1+1,f1+2,f1+3,f1+4,f1+5,i1+2,i1+3);
							sprintf(file_out,pformat,i1[0],i1[1],f1[0],f1[1],f1[2],f1[3],f1[4],f1[5],i1[2],i1[3]);
							}
						else
							{
							band4=1; /* 4-band rad. instrument */
							strcpy(sformat,"%d%d%f%f%f%f%f%f%f%f%d%d");
							strcpy(pformat," %2d %2d %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f %2d %2d\n");
							strcpy(xformat," ## ## ###.### ###.### ###.### ###.### ###.### ###.### ###.### ###.### ## ##|");

							/* scan numbers and print them with correct format */
							sscanf(buffer,sformat,i1,i1+1,f1,f1+1,f1+2,f1+3,f1+4,f1+5,f1+6,f1+7,i1+2,i1+3);
							sprintf(file_out,pformat,i1[0],i1[1],f1[0],f1[1],f1[2],f1[3],f1[4],f1[5],f1[6],f1[7],i1[2],i1[3]);
							}

						break;

					case 4: count1=5;
						strcpy(sformat,"%30c%1c%40c");
						strcpy(pformat,"%-30.30s %-40.40s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c2);
						sprintf(file_out,pformat,c1,c2);

						break;

					case 5:
					case 6:
					case 7: count1++;
						strcpy(sformat,"%8c%1c%8c%d%f%f");
						strcpy(pformat,"%-8.8s %-8.8s %2d %12.4f %12.4f\n");
						strcpy(xformat,"XXXXXXXX XXXXXXXX ## #######.#### #######.####|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1,&c_temp,c2,i1,f1,f1+1);
						sprintf(file_out,pformat,c1,c2,i1[0],f1[0],f1[1]);

						/* 4-band rad. instrument: read one line more with the same format */
						if(band4==1 && count1==8)
							{
							count1--;
							band4=0;
							}

						break;

					case 8:
					case 9: count1++;
						if(count1==10) count1=0;
						strcpy(sformat,"%80c");
						strcpy(pformat,"%-80.80s\n");
						strcpy(xformat,"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,c1);
						sprintf(file_out,pformat,c1);

						break;
					}
				break;

			case    9:
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=0;
						strcpy(sformat,"%d%d%d%d%d%d");
						strcpy(pformat," %2d %2d %2d %9d %5d %2d\n");
						strcpy(xformat," ## ## ## ######### ##### ##|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,i1+1,i1+2,i1+3,i1+4,i1+5);
						sprintf(file_out,pformat,i1[0],i1[1],i1[2],i1[3],i1[4],i1[5]);

						break;
					}
				break;

			case  100:
				lines=0;
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
						strcpy(sformat,"%d%d%d%f%d%d%d%f%d%d");
						strcpy(pformat," %2d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d\n");
						strcpy(xformat," ## ####   -### -##.# -### -###   -### -##.# -### -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5);
						sprintf(file_out,pformat,day1,min1,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5]);

						break;

					case 1: count1=0;
						strcpy(sformat,"%d%f%d%d%d%f%d%d%f%f%d");
						strcpy(pformat,"           %4d %5.1f %4d %4d   %4d %5.1f %4d %4d    %5.1f %5.1f %4d\n");
						strcpy(xformat,"           -### -##.# -### -###   -### -##.# -### -###    -##.# -##.# -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5,f1+2,f1+3,i1+6);
						sprintf(file_out,pformat,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5],f1[2],f1[3],i1[6]);

						break;
					}
				break;

			case  200:
			case  300:
				lines=1;
				switch(count1)
					{
					case 0: count1=0;
						strcpy(sformat,"%d%d%d%f%d%d%d%f%d%d%d%f%d%d");
						strcpy(pformat," %2d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d\n");
						strcpy(xformat," ## ####   -### -##.# -### -###   -### -##.# -### -###   -### -##.# -### -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5,i1+6,f1+2,i1+7,i1+8);
						sprintf(file_out,pformat,day1,min1,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5],i1[6],f1[2],i1[7],i1[8]);

						break;
					}
				break;

			case  400:
				lines=0;
				switch(count1)
					{
					case 0: count1=1;
						strcpy(sformat,"%d%d%d%f%d%d%d%f%d%d%d%f%d%d");
						strcpy(pformat," %2d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d\n");
						strcpy(xformat," ## ####   -### -##.# -### -###   -### -##.# -### -###   -### -##.# -### -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5,i1+6,f1+2,i1+7,i1+8);
						sprintf(file_out,pformat,day1,min1,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5],i1[6],f1[2],i1[7],i1[8]);

						break;

					case 1: count1=2;
						strcpy(sformat,"%d%f%d%d%d%f%d%d%d%f%d%d");
						strcpy(pformat,"           %4d %5.1f %4d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d\n");
						strcpy(xformat,"           -### -##.# -### -###   -### -##.# -### -###   -### -##.# -### -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5,i1+6,f1+2,i1+7,i1+8);
						sprintf(file_out,pformat,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5],i1[6],f1[2],i1[7],i1[8]);

						break;

					case 2: count1=0;
						strcpy(sformat,"%d%f%d%d%d%f%d%d%d%f%d%d");
						strcpy(pformat,"           %4d %5.1f %4d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d\n");
						strcpy(xformat,"           -### -##.# -### -###   -### -##.# -### -###   -### -##.# -### -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5,i1+6,f1+2,i1+7,i1+8);
						sprintf(file_out,pformat,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5],i1[6],f1[2],i1[7],i1[8]);

						break;
					}
				break;

			case  500:
				lines=0;
				switch(count1) 	/* switch according to line 	*/
					{	/* in record			*/
					case 0: count1=1;
						strcpy(sformat,"%d%d%f%f%f%f%f%f%f%f");
						strcpy(pformat," %2d %4d %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f\n");
						strcpy(xformat," ## #### -##.# -##.# -##.# -##.# -##.# -##.# -##.# -##.#|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,f1,f1+1,f1+2,f1+3,f1+4,f1+5,f1+6,f1+7);
						sprintf(file_out,pformat,day1,min1,f1[0],f1[1],f1[2],f1[3],f1[4],f1[5],f1[6],f1[7]);

						break;

					case 1: count1=0;
						strcpy(sformat,"%f%f%f%f%f%f%f%f%f%f%f%f");
						strcpy(pformat,"         %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f\n");
						strcpy(xformat,"         -##.# -##.# -##.# -##.# -##.# -##.# -##.# -##.# -##.# -##.# -##.# -##.#|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,f1,f1+1,f1+2,f1+3,f1+4,f1+5,f1+6,f1+7,f1+8,f1+9,f1+10,f1+11);
						sprintf(file_out,pformat,f1[0],f1[1],f1[2],f1[3],f1[4],f1[5],f1[6],f1[7],f1[8],f1[9],f1[10],f1[11]);

						break;
					}
				break;

			case 1100:
				lines=1;
				switch(count1)
					{
					case 0: count1=0;
						strcpy(sformat,"%d%d%d%d%d%f%f%d%d%f");
						strcpy(pformat," %2d %4d   %4d %4d %5d %5.1f %6.1f %3d %3d %4.1f\n");
						strcpy(xformat," ## ####   -### -### -#### -##.# -###.# -## -## -#.#|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,&seq1,i1,&hei1,f1,f1+1,i1+1,i1+2,f1+2);
						sprintf(file_out,pformat,day1,min1,seq1,i1[0],hei1,f1[0],f1[1],i1[1],i1[2],f1[2]);

						break;
					}
				break;

			case 1200:
				lines=1;
				switch(count1)
					{
					case 0: count1=0;
						strcpy(sformat,"%d%d%d");
						strcpy(pformat," %2d %4d   %4d\n");
						strcpy(xformat," ## ####   -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,i1);
						sprintf(file_out,pformat,day1,min1,i1[0]);

						break;
					}
				break;

			case 1300:
				lines=1;
				switch(count1)
					{
					case 0: count1=0;
						strcpy(sformat,"%d%d%d%d%f%f%f%f");
						strcpy(pformat," %2d %4d   %2d %5d %5.1f   %6.3f %6.3f %6.3f\n");
						strcpy(xformat," ## ####   -# ##### -##.#   -#.### -#.### -#.###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,i1,i1+1,f1,f1+1,f1+2,f1+3);
						sprintf(file_out,pformat,day1,min1,i1[0],i1[1],f1[0],f1[1],f1[2],f1[3]);

						break;
					}
				break;

			case 1500:
				lines=1;
				switch(count1)
					{
					case 0: count1=0;
						strcpy(sformat,"%d%d%d%d%d%d%d%d");
						strcpy(pformat," %2d %4d   %4d %4d %4d   %4d %4d %4d\n");
						strcpy(xformat," ## ####   -### -### -###   -### -### -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,i1,i1+1,i1+2,i1+3,i1+4,i1+5);
						sprintf(file_out,pformat,day1,min1,i1[0],i1[1],i1[2],i1[3],i1[4],i1[5]);

						break;
					}
				break;

			case 3010:
			case 3025:
			case 3030:
			case 3300:	/* NEW RECORD COMES HERE */
				lines=0;
				switch(count1)
					{
					case 0: count1=1;
						strcpy(sformat,"%d%d%d%f%d%d%d%f%d%d");
						strcpy(pformat," %2d %4d   %4d %5.1f %4d %4d   %4d %5.1f %4d %4d\n");
						strcpy(xformat," ## ####   -### -##.# -### -###   -### -##.# -### -###|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,&day1,&min1,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5);
						sprintf(file_out,pformat,day1,min1,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5]);

						break;

					case 1: count1=0;
						strcpy(sformat,"%d%f%d%d%d%f%d%d%f%f");
						strcpy(pformat,"           %4d %5.1f %4d %4d   %4d %5.1f %4d %4d    %5.1f %5.1f\n");
						strcpy(xformat,"           -### -##.# -### -###   -### -##.# -### -###    -##.# -##.#|");

						/* scan numbers and print them with correct format */
						sscanf(buffer,sformat,i1,f1,i1+1,i1+2,i1+3,f1+1,i1+4,i1+5,f1+2,f1+3);
						sprintf(file_out,pformat,i1[0],f1[0],i1[1],i1[2],i1[3],f1[1],i1[4],i1[5],f1[2],f1[3]);

						break;
					}
				break;

			default:
				count3++;
				strcpy(msg3,"*ERROR: Incorrect record number.\n\n");
				printf(msg3);
				fprintf(fout,msg3);
				printf(msg2,rec_num,count2-1);
				fprintf(fout,msg2,rec_num,count2-1);
				day0=-1;	/* indicator that there was	*/
						/* already error in this logrec	*/
				continue;
			}

		/* empty line							*/
		if(buffer[0]=='\n')
			{
			count3++;	/* counter of errors			*/

			if(count3==1)
				{
				printf("%s",msg1);
				fprintf(fout,"%s",msg1);
				}
			printf(msg2,rec_num,count2);
			fprintf(fout,msg2,rec_num,count2);
			strcpy(msg3,"        +Empty line...\n\n");
			printf(msg3);
			fprintf(fout,msg3);
			day0=-1;		/* indicator that there was	*/
						/* already error in this logrec	*/
			continue;
			}

		/* wrong seq. no. or height of measurement in logrec 1100	*/
		if(rec_num==1100)
			{
			if(count1+lines==1 && (seq0>=seq1 || hei0>=hei1) && day0==day1 && min0==min1)
				{
				count3++;
				if(count3==1)
					{
					printf("%s",msg1);
					fprintf(fout,"%s",msg1);
					}
				printf(msg2,rec_num,count2);
				fprintf(fout,msg2,rec_num,count2);
				strcpy(msg3,"        +Wrong seq. no. or height... (both have to be increasing)\n");
				printf(msg3);
				fprintf(fout,msg3);
				strcpy(msg3,"        ------------------------------------------------------------\n");
				strcat(msg3,"        |            |    day     |  minute  | seq. no. |  height  |\n");
				strcat(msg3,"        ------------------------------------------------------------\n");
				printf(msg3);
				fprintf(fout,msg3);
				strcpy(msg3,"        | Previous   |  %5d     | %6d   | %6d   | %6d   |\n");
				printf(msg3,day0,min0,seq0,hei0);
				fprintf(fout,msg3,day0,min0,seq0,hei0);
				strcpy(msg3,"        | Current    |  %5d     | %6d   | %6d   | %6d   |\n");
				strcat(msg3,"        ------------------------------------------------------------\n\n");
				printf(msg3,day1,min1,seq1,hei1);
				fprintf(fout,msg3,day1,min1,seq1,hei1);
				day0=-1;
				continue;
				}
			else
				{
				seq0=seq1;/* set cur. seq. no and heig. to prev.*/
				hei0=hei1;
				}
			}

		/* wrong day and minute of measurement				*/
		if(rec_num>=100)
			{
			if(count1+lines==1 && (day0>day1 || day0==day1 && (min0>=min1 && rec_num!=1100 ||
		                                                                   min0> min1 && rec_num==1100)))
				{
				count3++;
				if(count3==1)
					{
					printf("%s",msg1);
					fprintf(fout,"%s",msg1);
					}
				printf(msg2,rec_num,count2);
				fprintf(fout,msg2,rec_num,count2);
				strcpy(msg3,"        +Wrong day and minute... ");
				if(rec_num!=1100)
					strcat(msg3,"(time of measurement have to be increasing)\n");
				else
					strcat(msg3,"(time of measurement can not be decreasing)\n");
				printf(msg3);
				fprintf(fout,msg3);
				strcpy(msg3,"        --------------------------------------\n");
				strcat(msg3,"        |            |    day     |  minute  |\n");
				strcat(msg3,"        --------------------------------------\n");
				printf(msg3);
				fprintf(fout,msg3);
				strcpy(msg3,"        | Previous   |  %5d     | %6d   |\n");
				printf(msg3,day0,min0);
				fprintf(fout,msg3,day0,min0);
				strcpy(msg3,"        | Current    |  %5d     | %6d   |\n");
				strcat(msg3,"        --------------------------------------\n\n");
				printf(msg3,day1,min1);
				fprintf(fout,msg3,day1,min1);
				day0=-1;
				continue;
				}
			else
				{
				day0=day1;/* set cur. day and min. to prev.	*/
				min0=min1;
				}
			}

		/* wrong format of record line					*/
		if(strncmp(file_out,buffer,strlen(file_out)) ||  strlen(buffer) != strlen(xformat) && strlen(buffer)<=80)
			{
			count3++;
			if(count3==1)
				{
				printf("%s",msg1);
				fprintf(fout,"%s",msg1);
				}
			printf(msg2,rec_num,count2);
			fprintf(fout,msg2,rec_num,count2);

			/* replace new line '\n' by '|' to indicate the line end*/
			if((int)strlen(buffer)<=80)
				buffer[strlen(buffer)-1]='|';
			else
				buffer[strlen(buffer)-1]='\0';

			strcpy(msg3,"        +Wrong format of the line...\n");
			strcat(msg3,"         In the following lines the expected format and the current\n");
			strcat(msg3,"         format of the line are printed.\n\n");
			strcat(msg3,"         '#' stands for a digit (leading zeros are not allowed)\n");
			strcat(msg3,"         'X' stands for a character\n");
			strcat(msg3,"         '|' stands for the line end if it is present within first 80 characters\n");
			strcat(msg3,"             (additional characters or spaces are not allowed)\n");
			if(rec_num==8 && count1==4)
				if(band4==0)
					strcat(msg3,"3-BAND RADIATION INSTRUMENT?\n");
				else
					strcat(msg3,"4-BAND RADIATION INSTRUMENT?\n");
			strcat(msg3,"--------------------------------------------------------------------------------\n");
			strcat(msg3,"%s\n");
			strcat(msg3,"%s\n");
			strcat(msg3,"--------------------------------------------------------------------------------\n\n");
			printf(msg3,xformat,buffer);
			fprintf(fout,msg3,xformat,buffer);
			day0=-1;

			continue;
			}
		}
	if(count3==0)
		{
		strcpy(msg2,"*Check for line format.......... OK\n");
		printf(msg2);
		fprintf(fout,msg2);
		}
	else
		err=1;
	}
fclose(fin);
fclose(fout);

exit(err);
}
