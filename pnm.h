// For old compiler before C++11
/*
#ifndef nullptr
#define nullptr NULL
#endif
*/


// C++
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
// C
#include <stdint.h>


// Start definitions of pnm_lib_cpp
#ifndef LIB_PNM_LIB_CPP
#define LIB_PNM_LIB_CPP




#define BITS_OF_BYTE 8

#define PNM_FUNCTION_ERROR -1
#define PNM_FUNCTION_SUCCESS 0

#define PNM_DESCRIPTOR_LENGTH 3
#define PNM_DESCRIPTOR_MIN 1
#define PNM_DESCRIPTOR_MAX 6
#define PNM_DESCRIPTOR_PIXMAPS 3
#define PNM_DESCRIPTOR_ASCII_MAX 3

#define PORTABLE_BITMAP_ASCII 1
#define PORTABLE_GRAYMAP_ASCII 2
#define PORTABLE_PIXMAP_ASCII 3
#define PORTABLE_BITMAP_BINARY 4
#define PORTABLE_GRAYMAP_BINARY 5
#define PORTABLE_PIXMAP_BINARY 6

#define PNM_MAXINT_8BIT 255
#define PNM_MAXINT_16BIT 65535


// RGB to YUV (Y, Cb, Cr)
#define PNM_YUV_Y_RED 0.299
#define PNM_YUV_Y_GREEN 0.587
#define PNM_YUV_Y_BLUE 0.114
#define PNM_YUV_Cb_RED -0.168736
#define PNM_YUV_Cb_GREEN -0.331264
#define PNM_YUV_Cb_BLUE 0.5
#define PNM_YUV_Cr_RED 0.5
#define PNM_YUV_Cr_GREEN -0.418688
#define PNM_YUV_Cr_BLUE -0.081312
// YUV (Y, Cb, Cr) to RGB
#define PNM_RGB_RED_Y 1.0
#define PNM_RGB_RED_Cb 0.0
#define PNM_RGB_RED_Cr 1.402
#define PNM_RGB_GREEN_Y 1.0
#define PNM_RGB_GREEN_Cb -0.344136
#define PNM_RGB_GREEN_Cr -0.714136
#define PNM_RGB_BLUE_Y 1.0
#define PNM_RGB_BLUE_Cb 1.772
#define PNM_RGB_BLUE_Cr 0.0

// for pnm_resize()
#define PNM_Resize_ZeroOrderHold 0
#define PNM_Resize_Bicubic 1

// for fcommentf()
#define NUM_READ_STRING 256
#define STRING_NUM_READ_STRING "%256s"



typedef int pnm_img;
typedef double pnm_img_double;


class PNM_FORMAT
{
	protected:
		int desc;
		size_t width;
		size_t height;
		int maxint;
		size_t size;
	public:
		PNM_FORMAT(void);
		PNM_FORMAT(const PNM_FORMAT &pnm);
		virtual ~PNM_FORMAT(void);
		// Access
		int Desc() const;
		size_t Width() const;
		size_t Height() const;
		int MaxInt() const;
		size_t Size() const;
		// Library
		int bitdepth(void) const;
		bool isNULL(void) const;
		bool isSameFormat(const PNM_FORMAT &pnm) const;
		bool isSameDescriptor(const PNM_FORMAT &pnm) const;
		bool isRGB(void) const;
};

class PNM_DOUBLE;

class PNM : public PNM_FORMAT
{
	private:
		pnm_img *img;
	public:
		PNM(void);
		PNM(const PNM &pnm);
		PNM(const int Descriptor, const size_t &Width, const size_t &Height, const int MaxInt, const pnm_img *Data = nullptr);
		virtual ~PNM(void);
		pnm_img* Data(void) const;
		pnm_img& operator[](const size_t &n) const;
		pnm_img& at(const size_t &x, const size_t &y) const;
		pnm_img& at(const size_t &x, const size_t &y, const size_t &c) const;
		pnm_img Image(const size_t &x, const size_t &y) const;
		pnm_img Image(const size_t &x, const size_t &y, const size_t &c) const;
		void free(void);
		int copy(const PNM &pnm);
		int copy(const PNM_DOUBLE &pnm_double, const double &coeff, const char *process);
		int copy(const int Descriptor, const size_t &Width, const size_t &Height, const int MaxInt, const pnm_img *Data);
		int copy(const int Descriptor, const size_t &Width, const size_t &Height, const int MaxInt, const double *Data, const double &coeff);
		int* get_new_int(void) const;
		double* get_new_double(void) const;
		int read(const char *filename);
		int write(const char *filename);
		int RGB2Gray(const PNM &from);
		int Gray2RGB(const PNM &from);
};

class PNM_DOUBLE : public PNM_FORMAT
{
	private:
		pnm_img_double *imgd;
	public:
		PNM_DOUBLE(void);
		PNM_DOUBLE(const PNM_DOUBLE &pnmd);
		PNM_DOUBLE(const int Descriptor, const size_t &Width, const size_t &Height, const int MaxInt, const pnm_img_double *Data = nullptr);
		virtual ~PNM_DOUBLE(void);
		// Library
		pnm_img_double* Data(void) const;
		pnm_img_double& operator[](const size_t &n) const;
		pnm_img_double& at(const size_t &x, const size_t &y) const;
		pnm_img_double& at(const size_t &x, const size_t &y, const size_t &c) const;
		pnm_img_double Image(const size_t &x, const size_t &y) const;
		pnm_img_double Image(const size_t &x, const size_t &y, const size_t &c) const;
		void free(void);
		int copy(const PNM_DOUBLE &pnmd);
		int copy(const PNM &pnm_int, const double &coeff);
		int copy(const int Descriptor, const size_t &Width, const size_t &Height, const int MaxInt, const pnm_img_double *Data);
		double* get_double(void) const;
		int RGB2Gray(const PNM_DOUBLE &from);
		int Gray2RGB(const PNM_DOUBLE &from);
		int RGB2YCbCr(const PNM_DOUBLE &from);
		int YCbCr2RGB(const PNM_DOUBLE &from);
};



// Library for Read and Write
template<class T> extern bool fcommentf(FILE *, T *);
extern std::string pnm_FixExtension(const char *filename, int desc);

// Resize
extern void pnm_resize(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const int width_o, const int height_o, const int Method);
extern void pnm_ZeroOrderHold(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const int width_o, const int height_o);
extern void pnm_Bicubic(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const double alpha, const int width_o, const int height_o);
extern double pnm_Cubic(const double x, const double a);

// End of definition of pnm_lib_cpp


template <class T>
bool
fcommentf(FILE *fp, T *ret)
{
	char ctmp[NUM_READ_STRING + 1];
	char c;
	int c_int;
	int flag;
	int read;

	ctmp[NUM_READ_STRING] = '\0'; // To prevent Buffer Overrun
	flag = 1;
	while (flag != 0) {
		flag = 0;
		long ftold = ftell(fp);
		if (fscanf(fp, STRING_NUM_READ_STRING, ctmp) != 1) {
			fprintf(stderr, "*** fcommentf error - Failed to read by fscanf() ***\n");
			return false;
		}
		if (ctmp[0] == '#') {
			if (fseek(fp, ftold, SEEK_SET) != 0) {
				fprintf(stderr, "*** fcommentf error - Failed to do fseek() ***\n");
				return false;
			}
			printf("\"");
			while ((flag < 2) && (feof(fp) == 0)) {
				if ((c_int = fgetc(fp)) == EOF) {
					fprintf(stderr, "*** pnmread error - fgetc returns EOF ***\n");
					return false;
				}
				c = char(c_int);
				if (c != '\n') {
					printf("%c", c);
				}
				if ((flag == 0) && (c == '#')) {
					flag = 1;
				} else if ((flag != 0) && (c == '\n')) {
					flag = 2;
				}
			}
			printf("\"\n");
		}
	}
	ctmp[NUM_READ_STRING] = '\0'; // To prevent Buffer Overrun
	if (sscanf(ctmp, "%7d", &read) != 1) {
		fprintf(stderr, "*** fcommentf error - Failed to read from ctmp by sscanf() ***\n");
		return false;
	}
	*ret = read;
	return true;
}

#endif

