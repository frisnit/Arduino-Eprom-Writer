/*

bin2c

create a C array from a binary file
for Arduino EPROM writer

*/

#include "stdlib.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
	FILE *in, *out;
	char outfilename[260];
	long n;
	long filesize;
	int c;

	if(argc!=2)
	{
		printf("Usage: %s <binfile>\n\n",argv[0]);
		return 1;
	}

	in = fopen(argv[1],"rb");

	if(in==NULL)
	{
		printf("Can't open input file %s\n\n",argv[1]);
		return 1;
	}

	sprintf(outfilename,"%s.c",argv[1]);

	out = fopen(outfilename,"w");

	if(out==NULL)
	{
		printf("Can't open output file %s\n\n",outfilename);
		fclose(in);
		return 1;
	}

	printf("Writing file %s\n",outfilename);

	// get file size
	fseek(in, 0L, SEEK_END);
	filesize = ftell(in);
	rewind(in);

	fprintf(out,"#define DATA_LENGTH 0x%04x\n",(unsigned int)(filesize&0xffff));
	fprintf(out,"const PROGMEM byte ROM_DATA[DATA_LENGTH]=\n{");

	for(n=0;n<filesize;n++)
	{
		if(n%16==0)
		{
			// newline and indent
			fprintf(out,"\n\t");
		}

		c = fgetc(in);

		fprintf(out,"0x%02x, ",(unsigned char)(c&0xff));
	}

	fprintf(out,"\n};");

	printf("Finished, read %d bytes\n\n",(unsigned int)filesize);

	fclose(in);
	fclose(out);

	return 0;
}


