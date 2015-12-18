// For old compiler before C++11
/*
#ifndef nullptr
#define nullptr NULL
#endif
*/


// C++
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
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
		int width;
		int height;
		int maxint;
	public:
		PNM_FORMAT(void);
		PNM_FORMAT(const PNM_FORMAT &pnm);
		virtual ~PNM_FORMAT(void);
		// Access
		int Desc() const;
		int Width() const;
		int Height() const;
		int MaxInt() const;
		int Size() const;
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
		virtual ~PNM(void);
		pnm_img* Data(void) const;
		pnm_img& operator[](int n) const;
		pnm_img Image(int x, int y) const;
		void free(void);
		int copy(const PNM &pnm);
		int copy(const PNM_DOUBLE &pnm_double, double coeff, const char *process);
		int copy(int Descriptor, int Width, int Height, int MaxInt, int *Data);
		int copy(int Descriptor, int Width, int Height, int MaxInt, double *Data, double coeff);
		int* get_int(void) const;
		double* get_double(void) const;
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
		virtual ~PNM_DOUBLE(void);
		// Library
		pnm_img_double* Data(void) const;
		pnm_img_double& operator[](int n) const;
		pnm_img_double Image(int x, int y) const;
		void free(void);
		int copy(const PNM_DOUBLE &pnmd);
		int copy(const PNM &pnm_int, double coeff);
		int copy(int Descriptor, int Width, int Height, int MaxInt, double *Data);
		double* get_double(void) const;
		int RGB2Gray(const PNM_DOUBLE &from);
		int Gray2RGB(const PNM_DOUBLE &from);
		int RGB2YCbCr(const PNM_DOUBLE &from);
		int YCbCr2RGB(const PNM_DOUBLE &from);
};



// Library for Read and Write
extern bool fcommentf(FILE *, int *);
extern std::string pnm_FixExtension(const char *filename, int desc);

// Resize
extern void pnm_resize(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const int width_o, const int height_o, const int Method);
extern void pnm_ZeroOrderHold(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const int width_o, const int height_o);
extern void pnm_Bicubic(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const double alpha, const int width_o, const int height_o);
extern double pnm_Cubic(const double x, const double a);

// End of definition of pnm_lib_cpp
#endif

