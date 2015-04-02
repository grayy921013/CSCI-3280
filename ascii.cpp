#include "stdio.h"
#include "malloc.h"
#include "memory.h"
#include "math.h"

#define MAX_SHADES 10

char shades[MAX_SHADES] = {'#','$','O','=','+','|','-','^','.',' '};
char return1 = 0x0D;
char return2 = 0x0A;

#define SAFE_FREE(p)  { if(p){ free(p);  (p)=NULL;} }

struct BYTE3
{
	unsigned char x;
	unsigned char y;
	unsigned char z;
};

BYTE3 *image_bm=NULL;
BYTE3 **image_pm=NULL;

int image_width=0;
int image_height=0;

void SwapByte( void *src, int n )
{
	int i,j,n2;
	unsigned char a;
	unsigned char *s0;
	s0 = (unsigned char*)src;

	n2 = n/2;
	for( i=0, j=n-1; i<n2; i++, j-- )
	{
		a = s0[i];
		s0[i] = s0[j];
		s0[j] = a;
	}
}

BYTE3 * LoadPPMImage(const char *spath)
{
	char str[256], info[2];
	int width=0, height=0;
	short mode;
	short mode_swap;


	FILE *f0 = fopen( spath, "rb" );
	if( f0==NULL )
	{
		printf("unable to find ppm file!\n");
		return NULL;
	}

	fread( &mode, sizeof(short), 1, f0 ); 
	mode_swap=mode;
	SwapByte( &mode_swap, sizeof(mode) );
	fread( info, sizeof(char), 2, f0 );

	fclose(f0);

	if( mode=='P5' || mode=='P6' || mode_swap=='P5'|| mode_swap=='P6')
	{
		FILE *f0 = fopen( spath, "rb" );
		if(f0==NULL)
		{
			printf("unable to find ppm file!\n");
			return NULL;
		}

		if( info[0]=='\n' || info[1]=='\n' )
		{
			fgets( str, 256, f0 );
			while( fgets( str, 256, f0 ) && str[0] == '#' );
			sscanf(str, "%i %i", &width, &height);
			fgets( str, 256, f0 );
		}
		else if( info[0]==' ' )
		{
			fgets( str, 256, f0 );
			char tmp[16];
			sscanf( str, "%s %i %i", tmp, &width, &height );
		}
		else if( info[0]=='\r' && info[1]!='\n' )
		{
			fscanf( f0, "%[^\r]", str ); fgetc(f0);
			do{ fscanf( f0, "%[^\r]", str ); fgetc(f0); }while( str[0] == '#' );
			sscanf(str, "%i %i", &width, &height);
			fscanf( f0, "%[^\r]", str ); fgetc(f0);
		}
		else
		{
			printf("format error!\n");
			return NULL;
		}

		SAFE_FREE( image_bm );
		SAFE_FREE( image_pm );


		image_bm = (BYTE3*) malloc( width * height * sizeof(BYTE3) );
		if(image_bm==NULL)
		{
			printf("unable to alloc memory!\n");
			return NULL;
		}

		memset( image_bm, 0, width*height*sizeof(BYTE3) );

		image_pm = (BYTE3**) malloc( height * sizeof(BYTE3*) );
		if(image_pm==NULL)
		{
			printf("unable to alloc memory!\n");
			return false;
		}

		for(int j=0; j<height; j++ )
			image_pm[j] = &image_bm[j*width];

		image_width = width;
		image_height = height;

		fread( image_bm, sizeof(BYTE3), width*height, f0 );

		fclose(f0);
	}

	return image_bm;
}

int main(int argc, char *argv[])
{
	////////////////read ppm files/////////////
	BYTE3 *image_data=LoadPPMImage(argv[1]);

	if(image_data==NULL)
	{
		printf("unable to load source.ppm!\n");
		return -1;
	}
	FILE *f0 = fopen( argv[2], "wb" );
	if( f0==NULL )
	{
		printf("unable to create output file!\n");
		return NULL;
	}
	int numofpixel=image_width*image_height;
	int *pixel=new int[numofpixel];
	for(int i=0;i<numofpixel;i++)
		pixel[i]=0.299 * image_data[i].x + 0.587 * image_data[i].y + 0.114 * image_data[i].z;
    for(int i=0;i<image_height;i++){
		for(int j=0;j<image_width;j++){
			int grayLevel=(double)pixel[i*image_width+j]/255*10;
			if(grayLevel==10)
				grayLevel=9;
			fputc(shades[grayLevel],f0);
		}
		fputc('\r',f0);
		fputc('\n',f0);
		
		}
	fclose(f0);
	return 0;
} 