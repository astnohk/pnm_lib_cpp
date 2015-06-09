#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>


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
#define NUM_RESIZE_METHOD 2

// for fcommentf()
#define NUM_READ_STRING 512
#define STRING_NUM_READ_STRING "%512s"



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
		int Width();
		int Height();
		int MaxInt();
		int bitdepth(void);
		bool isSameFormat(const PNM_FORMAT &pnm);
		bool isSameDescriptor(const PNM_FORMAT &pnm);
		bool isRGB(void);
};

class PNM : PNM_FORMAT
{
	private:
		pnm_img *img;
	public:
		PNM(void);
		PNM(const PNM &pnm);
		~PNM(void);
		pnm_img img(int x, int y);
		void free(void);
		void copy(const PNM &pnm);
		int read(const char *filename);
		int write(const char *filename);
}

class PNM_DOUBLE : PNM_FORMAT
{
	private:
		pnm_img_double *imgd;
	public:
		PNM_DOUBLE(void);
		PNM_DOUBLE(const PNM_DOUBLE &pnmd);
		~PNM_DOUBLE(void);
		pnm_img_double imgd(int x, int y);
		void free(void);
		void copy(const PNM_DOUBLE &pnmd);
};

struct PNM_OFFSET
{
	double r;
	double g;
	double b;
	PNM_OFFSET(void);
};



// Read and Write
extern int fcommentf(FILE *, unsigned int *);
extern int pnmread(PNM *, char *);
extern char* pnm_FixExtension(char *filename, int desc);
extern int pnmwrite(PNM *, char *);

// Memory Allocation
extern int pnmnew(PNM *, int, unsigned int, unsigned int, unsigned int);
extern int pnmdouble_new(PNM_DOUBLE *, int, unsigned int, unsigned int, unsigned int);
extern PNM* pnmnew_NULL(void);
extern PNM_DOUBLE* pnmdouble_new_NULL(void);

// Memory Free
extern void pnmfree(PNM *);
extern void pnmdouble_free(PNM_DOUBLE *);
extern void pnmdestroy(PNM *);
extern void pnmdouble_destroy(PNM_DOUBLE *);

// Copy
extern int pnmcp(PNM *pnm_to, PNM *pnm_from);
extern int pnmdouble_cp(PNM_DOUBLE *pnm_to, PNM_DOUBLE *pnm_from);

// Convert integer, double
extern int pnm_offset(PNM_OFFSET *offset, double r, double g, double b);
extern int pnm_double2int(PNM *, PNM_DOUBLE *, double, const char *, PNM_OFFSET *);
extern int pnm_int2double(PNM_DOUBLE *, PNM *, double, PNM_OFFSET *);

// Read descriptions
extern int pnm_bitdepth(PNM *);
extern int pnmdouble_bitdepth(PNM_DOUBLE *);
extern int pnm_isNULL(PNM *);
extern int pnmdouble_isNULL(PNM_DOUBLE *pnm);
extern int pnm_isSameFormat(PNM *pnm1, PNM *pnm2);
extern int pnmdouble_isSameFormat(PNM_DOUBLE *pnm1, PNM_DOUBLE *pnm2);
extern int pnm_isSameDescriptor(PNM *pnm1, PNM *pnm2);
extern int pnmdouble_isSameDescriptor(PNM_DOUBLE *pnm1, PNM_DOUBLE *pnm2);
extern int pnm_isRGB(PNM *);
extern int pnmdouble_isRGB(PNM_DOUBLE *);

// Convert Color Space
extern int pnm_RGB2Gray(PNM_DOUBLE *out, PNM_DOUBLE *in);
extern int pnm_Gray2RGB(PNM *out, PNM *in);
extern int pnmdouble_Gray2RGB(PNM_DOUBLE *out, PNM_DOUBLE *in);
extern int pnm_RGB2YCbCr(PNM_DOUBLE *out, PNM_DOUBLE *in);
extern int pnm_YCbCr2RGB(PNM_DOUBLE *out, PNM_DOUBLE *in);

// Resize
extern int pnm_resize(PNM_DOUBLE *pnm_out, PNM_DOUBLE *pnm_in, unsigned int width_o, unsigned int height_o, const char *Method);
extern int pnm_Bicubic(PNM_DOUBLE *pnm_out, PNM_DOUBLE *pnm_in, double alpha, unsigned int width_o, unsigned int height_o);
extern double pnm_Cubic(double x, double a);

// Extract and Convert PNM to Array
extern int* pnm2int(PNM *pnm);
extern double* pnm2double(PNM *pnm);
extern double* pnmdouble2double(PNM_DOUBLE *pnmd);

