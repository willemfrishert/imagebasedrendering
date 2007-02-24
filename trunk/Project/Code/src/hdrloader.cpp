
/***********************************************************************************
	Created:	17:9:2002
	FileName: 	hdrloader.cpp
	Author:		Igor Kravtchenko
	
	Info:		Load HDR image and convert to a set of float32 RGB triplet.
************************************************************************************/

#include "hdrloader.h"

#include <math.h>
#include <memory.h>
#include <stdio.h>

typedef unsigned char RGBE[4];
#define R			0
#define G			1
#define B			2
#define E			3

#define  MINELEN	8				// minimum scanline length for encoding
#define  MAXELEN	0x7fff			// maximum scanline length for encoding


HDRLoaderResult::HDRLoaderResult()
: maxExponent(FLT_MIN)
, minExponent(FLT_MAX)
{
};

HDRLoaderResultEncoded::HDRLoaderResultEncoded()
: maxExponent(0)
, minExponent(UCHAR_MAX)
{
};



static void CopyRGBE(RGBE *scan, int len, unsigned char* cols, HDRLoaderResultEncoded& result);
static void workOnRGBE(RGBE *scan, int len, float *cols, HDRLoaderResult& result);
static bool decrunch(RGBE *scanline, int len, FILE *file);
static bool oldDecrunch(RGBE *scanline, int len, FILE *file);

bool HDRLoader::load(const char *fileName, HDRLoaderResult &res)
{
	int i;
	char str[200];
	FILE *file;

	file = fopen(fileName, "rb");
	if (!file)
		return false;

	fread(str, 10, 1, file);
	if (memcmp(str, "#?RADIANCE", 10)) {
		fclose(file);
		return false;
	}

	fseek(file, 1, SEEK_CUR);

	char cmd[200];
	i = 0;
	char c = 0, oldc;
	while(true) {
		oldc = c;
		c = fgetc(file);
		if (c == 0xa && oldc == 0xa)
			break;
		cmd[i++] = c;
	}

	char reso[200];
	i = 0;
	while(true) {
		c = fgetc(file);
		reso[i++] = c;
		if (c == 0xa)
			break;
	}

	int w, h;
	if (!sscanf(reso, "-Y %ld +X %ld", &h, &w)) {
		fclose(file);
		return false;
	}

	res.width = w;
	res.height = h;

	float *cols = new float[w * h * 3];
	res.cols = cols;

	RGBE *scanline = new RGBE[w];
	if (!scanline) {
		fclose(file);
		return false;
	}

	// convert image 
	for (int y = h - 1; y >= 0; y--) {
		if (decrunch(scanline, w, file) == false)
			break;
		workOnRGBE(scanline, w, cols, res);
		cols += w * 3;
	}

	delete [] scanline;
	fclose(file);

	return true;
}

bool HDRLoader::load(const char *fileName, HDRLoaderResultEncoded &res)
{
	int i;
	char str[200];
	FILE *file;

	file = fopen(fileName, "rb");
	if (!file)
		return false;

	fread(str, 10, 1, file);
	if (memcmp(str, "#?RADIANCE", 10)) {
		fclose(file);
		return false;
	}

	fseek(file, 1, SEEK_CUR);

	char cmd[200];
	i = 0;
	char c = 0, oldc;
	while(true) {
		oldc = c;
		c = fgetc(file);
		if (c == 0xa && oldc == 0xa)
			break;
		cmd[i++] = c;
	}

	char reso[200];
	i = 0;
	while(true) {
		c = fgetc(file);
		reso[i++] = c;
		if (c == 0xa)
			break;
	}

	int w, h;
	if (!sscanf(reso, "-Y %ld +X %ld", &h, &w)) {
		fclose(file);
		return false;
	}

	res.width = w;
	res.height = h;

	unsigned char* cols = new unsigned char[w * h * 4];
	res.cols = cols;

	RGBE *scanline = new RGBE[w];
	if (!scanline) {
		fclose(file);
		return false;
	}

	// convert image 
	for (int y = h - 1; y >= 0; y--) {
		if (decrunch(scanline, w, file) == false)
			break;
		CopyRGBE(scanline, w, cols, res);
		cols += w * 4;
	}

	delete [] scanline;
	fclose(file);

	return true;
}

/**
* convertComponent
* decodes every 8 bit channel to float
* @param expo exponent in 8 bit integer
* @param val channel value in 8 bit integer
* @return float
*/
float convertComponent(int expo, int val)
{
	float v = val / 256.0f;
	float d = (float) pow(2.0f, static_cast<float>(expo));
	return v * d;
}

/**
* workOnRGBE
* decodes the 32bit RGBE pixel from the .hdr file to float
* @param scan struct contains a pixel value in RGBE
* @param len the length of the pixel column
* @param cols contains the float value
* @param result container keeping track of the minimum and maximum exponent
* @return void
*/
void workOnRGBE(RGBE *scan, int len, float *cols, HDRLoaderResult & result)
{
	while (len-- > 0) {
		int expo = scan[0][E] - 128;
		cols[0] = convertComponent(expo, scan[0][R]);
		cols[1] = convertComponent(expo, scan[0][G]);
		cols[2] = convertComponent(expo, scan[0][B]);

		// store the minimum and maximum exponent
		float exponent = static_cast<float>(expo);
		if (exponent > -128.0f)
		{
			result.minExponent = (result.minExponent > exponent) ?  exponent: result.minExponent;
			result.maxExponent = (result.maxExponent < exponent) ?  exponent: result.maxExponent;
		}

		cols += 3;
		scan++;
	}
}

/**
* CopyRGBE
* copies the 32bit RGBE pixel from the .hdr file directly to the columns
* @param scan struct contains a pixel value in RGBE
* @param len the length of the pixel column
* @param cols contains the byte values
* @param result container keeping track of the minimum and maximum exponent
* @return void
*/
void CopyRGBE(RGBE *scan, int len, unsigned char* cols, HDRLoaderResultEncoded &result)
{
	while (len-- > 0) 
	{
		cols[0] = scan[0][R];
		cols[1] = scan[0][G];
		cols[2] = scan[0][B];
		cols[3] = scan[0][E];

		// store the minimum and maximum exponent
		if (scan[0][E] != 0)
		{
			result.minExponent = (result.minExponent > scan[0][E]) ?  scan[0][E]: result.minExponent;
			result.maxExponent = (result.maxExponent < scan[0][E]) ?  scan[0][E]: result.maxExponent;
		}

 		cols += 4;
		scan++;
	}
}

bool decrunch(RGBE *scanline, int len, FILE *file)
{
	int  i, j;
					
	if (len < MINELEN || len > MAXELEN)
		return oldDecrunch(scanline, len, file);

	i = fgetc(file);
	if (i != 2) {
		fseek(file, -1, SEEK_CUR);
		return oldDecrunch(scanline, len, file);
	}

	scanline[0][G] = fgetc(file);
	scanline[0][B] = fgetc(file);
	i = fgetc(file);

	if (scanline[0][G] != 2 || scanline[0][B] & 128) {
		scanline[0][R] = 2;
		scanline[0][E] = i;
		return oldDecrunch(scanline + 1, len - 1, file);
	}

	// read each component
	for (i = 0; i < 4; i++) {
	    for (j = 0; j < len; ) {
			unsigned char code = fgetc(file);
			if (code > 128) { // run
			    code &= 127;
			    unsigned char val = fgetc(file);
			    while (code--)
					scanline[j++][i] = val;
			}
			else  {	// non-run
			    while(code--)
					scanline[j++][i] = fgetc(file);
			}
		}
    }

	return feof(file) ? false : true;
}

bool oldDecrunch(RGBE *scanline, int len, FILE *file)
{
	int i;
	int rshift = 0;
	
	while (len > 0) {
		scanline[0][R] = fgetc(file);
		scanline[0][G] = fgetc(file);
		scanline[0][B] = fgetc(file);
		scanline[0][E] = fgetc(file);
		if (feof(file))
			return false;

		if (scanline[0][R] == 1 &&
			scanline[0][G] == 1 &&
			scanline[0][B] == 1) {
			for (i = scanline[0][E] << rshift; i > 0; i--) {
				memcpy(&scanline[0][0], &scanline[-1][0], 4);
				scanline++;
				len--;
			}
			rshift += 8;
		}
		else {
			scanline++;
			len--;
			rshift = 0;
		}
	}
	return true;
}
