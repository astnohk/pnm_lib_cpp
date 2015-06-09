#include "pnm.h"



// PNM_DOUBLE Methods
PNM_DOUBLE::PNM_DOUBLE(void)
{
	desc = 0;
	width = 0;
	height = 0;
	maxint = 0;
	imgd = nullptr;
}

PNM_DOUBLE::PNM_DOUBLE(const PNM_DOUBLE &pnmd)
{
	const char *FunctionName = "PNM_DOUBLE::PNM_DOUBLE(const PNM_DOUBLE &)";
	std::string ErrorValueName;

	desc = pnmd.desc;
	width = pnmd.width;
	height = pnmd.height;
	maxint = pnmd.maxint;
	if (pnmd.imgd == nullptr) {
		ErrorValueName = "pnmd.imgd";
		goto ErrorPointerNull;
	}
	if (desc % 3 == 0) {
		try {
			imgd = new pnm_img_double[3 * width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < 3 * width * height; i++) {
			imgd[i] = pnmd.imgd[i];
		}
	} else {
		try {
			imgd = new pnm_img_double[width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			imgd[i] = pnmd.imgd[i];
		}
	}
	return;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this.free();
	return;
}

PNM_DOUBLE::~PNM_DOUBLE(void)
{
	delete[] imgd;
	imgd = nullptr;
}

pnm_img_double
PNM_DOUBLE::imgd(int x, int y)
{
	if (0 <= x && x < width && 0 <= y && y < height) {
		return imgd[width * y + x];
	} else {
		return 0;
	}
}

void
PNM_DOUBLE::free(void)
{
	desc = 0;
	width = 0;
	height = 0;
	maxint = 0;
	delete[] imgd;
	imgd = nullptr;
}

int
PNM_DOUBLE::copy(const PNM_DOUBLE &pnmd)
{
	const char *FuntioName = "PNM_DOUBLE::copy(const PNM_DOUBLE &)";
	std::string ErrorValueName;

	if (pnmd.imgd == nullptr) {
		ErrorValueName = "pnmd.imgd";
		goto ErrorPointerNull;
	}
	desc = pnmd.desc;
	width = pnmd.width;
	height = pnmd.height;
	maxint = pnmd.maxint;
	delete[] imgd;
	if (desc % 3 == 0) {
		try {
			imgd = new pnm_img_double[3 * width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < 3 * width * height; i++) {
			imgd[i] = pnmd.imgd[i];
		}
	} else {
		try {
			imgd = new pnm_img_double[width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			imgd[i] = pnmd.imgd[i];
		}
	}
	return PNM_FUNCTION_SUCCESS;
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this.free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::copy(const PNM &pnm_int, const double &coeff, const PNM_OFFSET &add_offset)
{
	const char *FunctionName = "PNM_DOUBLE::copy(const PNM &, const double &, const PNM_OFFSET &)";
	std::string ErrorValueName;

	unsigned int M;
	unsigned int N;
	unsigned int i;

	if (pnm_int.img == nullptr) {
		ErrorValueName = "(pnm_int.img)";
		goto ErrorPointerNull;
	}
	if (fabs(coeff) <= 1.0 / pnm_int->maxint) {
		fprintf(stderr, "*** %s() warning - The coefficient is ZERO ***\n", FunctionName);
	}

	desc = pnm_int.desc;
	width = pnm_int.width;
	height = pnm_int.height;
	maxint = pnm_int.maxint;
	M = pnm_int->height;
	N = pnm_int->width;
	// Clear *imgd
	delete[] imgd;
	switch (pnm_int->desc) {
		case PORTABLE_BITMAP_ASCII:
		case PORTABLE_GRAYMAP_ASCII:
		case PORTABLE_BITMAP_BINARY:
		case PORTABLE_GRAYMAP_BINARY:
			try {
				imgd = new pnm_double[M * N];
			}
			catch (const std::bad_alloc &bad) {
				ErrorValueName = "imgd";
				goto ErrorMalloc;
			}
			for (i = 0; i < M * N; i++) {
				if (add_offset == nullptr) {
					imgd[i] = coeff * (pnm_img_double)pnm_int.img[i];
				} else {
					imgd[i] = coeff * (pnm_img_double)pnm_int.img[i] + add_offset.r;
				}
			}
			break;
		case PORTABLE_PIXMAP_ASCII:
		case PORTABLE_PIXMAP_BINARY:
			try {
				imgd = new pnm_double[3 * M * N];
			}
			catch (const std::bad_alloc &bad) {
				ErrorValueName = "imgd";
				goto ErrorMalloc;
			}
			for (i = 0; i < M * N; i++) {
				if (add_offset == nullptr) {
					imgd[i] = coeff * (pnm_img_double)pnm_int.img[i];
					imgd[M * N + i] = coeff * (pnm_img_double)pnm_int.img[M * N + i];
					imgd[2 * M * N + i] = coeff * (pnm_img_double)pnm_int.img[2 * M * N + i];
				} else {
					imgd[i] = coeff * (pnm_img_double)pnm_int.img[i] + add_offset.r;
					imgd[M * N + i] = coeff * (pnm_img_double)pnm_int.img[M * N + i] + add_offset.g;
					imgd[2 * M * N + i] = coeff * (pnm_img_double)pnm_int.img[2 * M * N + i] + add_offset.b;
				}
			}
			break;
		default: // ERROR
			fprintf(stderr, "*** %s() error - Descriptor is incorrect (P%d) ***\n", FunctionName.c_str(), pnm_int.desc);
			goto ExitError;
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorValueIncorrect:
	fprintf(stderr, "*** %s() error - The variable (%s) has incorrect value ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this.free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::copy(int Descriptor, int Width, int Height, int MaxInt, double *Data)
{
	const char *FunctionName = "PNM_DOUBLE::copy(int, int, int, int, double *)";
	std::string ErrorValueName;

	if (Data == nullptr) {
		ErrorValueName = "Data";
		goto ErrorPointerNull;
	}
	if (Descriptor < PNM_DESCRIPTOR_MIN
	    || Descriptor > PNM_DESCRIPTOR_MAX) {
		ErrorDesc = "PNM descriptor is out of range [1, 6]";
		goto ErrorOthers;
	} else if (Width < 0 || Height < 0) {
		ErrorDesc = "The size of image is invalid";
		goto ErrorOthers;
	}
	desc = Descriptor;
	width = Width;
	height = Height;
	maxint = MaxInt;
	if (Descriptor % PNM_DESCRIPTOR_PIXMAPS == 0) {
		try {
			imgd = new pnm_double_img[3 * width * height];
		}
		catch (std::bad_alloc bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < 3 * width * height; i++) {
			imgd[i] = Data[i];
		}
	} else {
		try {
			imgd = new pnm_img[width * height];
		}
		catch (std::bad_alloc bad) {
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			imgd[i] = Data[i];
		}
	}
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorOthers:
	fprintf(stderr, "*** %s() error - %s ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this.free();
}

double *
PNM_DOUBLE::get_double(void)
{
	ERROR Error("PNM_DOUBLE::get_double");
	double *Image = nullptr;

	try {
		Image = new double[width * height];
	}
	catch (const std::bad_alloc &bad) {
		Error.Value("Image");
		Error.Malloc();
		return nullptr;
	}
	for (int n = 0; n < width * height; n++) {
		Image[n] = imgd[n];
	}
	return Image;
}

int
PNM_DOUBLE::RGB2Gray(const PNM_DOUBLE &from)
{
	const char *FunctionName = "PNM_DOUBLE::RGB2Gray";
	std::string ErrorValueName;
	int M = 0;
	int N = 0;

	if (imgd != nullptr) {
		this.free();
	}
	if (from.imgd == nullptr) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}

	width = from.width;
	height = from.height;
	maxint = from.maxint;
	if ((from->desc % 3) != 0) {
		fprintf(stderr, "*** %s() warning - The input image is binarymap or graymap ***\n", FunctionName);
		if (from.desc <= PNM_DESCRIPTOR_ASCII_MAX) {
			desc = PORTABLE_GRAYMAP_ASCII;
		} else {
			desc = PORTABLE_GRAYMAP_BINARY;
		}
		try {
			imgd = new pnm_img_double[width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			imgd[i] = from->imgd[i];
		}
	} else {
		if (from.desc <= PNM_DESCRIPTOR_ASCII_MAX) {
			desc = PORTABLE_GRAYMAP_ASCII;
		} else {
			desc = PORTABLE_GRAYMAP_BINARY;
		}
		try {
			imgd = new pnm_img_double[width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			imgd[i] =
			    PNM_YUV_Y_RED * from->imgd[i]
			    + PNM_YUV_Y_GREEN * from->imgd[width * height + i]
			    + PNM_YUV_Y_BLUE * from->imgd[2 * width * height + i];
		}
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this.free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::Gray2RGB(const PNM_DOUBLE &from)
{
	const char *FunctionName = "PNM_DOUBLE::Gray2RGB";
	std::string ErrorValueName;

	if (from.imgd == nullptr) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	} else if (from.isRGB() != false) {
		fprintf(stderr, "*** %s() error - PNM Descriptor of (*from) is NOT right ***\n", FunctionName);
		goto ExitError;
	}

	width = from.width;
	height = from.height;
	maxint = from.maxint;
	try {
		imgd = new pnm_img_double[3 * width * height];
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	if (from.desc <= PNM_DESCRIPTOR_ASCII_MAX) {
		desc = PORTABLE_PIXMAP_ASCII;
	} else {
		desc = PORTABLE_PIXMAP_BINARY;
	}
	for (int i = 0; i < width * height; i++) {
		imgd[i] = from.imgd[i];
		imgd[width * height + i] = from.imgd[i];
		imgd[2u * width * height + i] = from.imgd[i];
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorNotInitialized:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NOT initialized ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this.free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::RGB2YCbCr(const PNM_DOUBLE &from)
{
	const char *FunctionName = "PNM_DOUBLE::RGB2YCbCr";
	std::string ErrorValueName;

	if (from.imgd == nullptr) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}

	desc = PORTABLE_PIXMAP_BINARY;
	width = from.width;
	height = from.height;
	maxint = from.maxint;
	if ((from.desc % 3) != 0) {
		fprintf(stderr, "*** %s() error - The input image is binary or grayscale ***\n", FunctionName);
		goto ExitError;
	}

	try {
		imgd = new pnm_img_double[3 * width * height];
	}
	catch (const std::bad_alloc &bad) {
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	for (int i = 0; i < width * height; i++) {
		imgd[i] =
		    PNM_YUV_Y_RED * from.imgd[i]
		    + PNM_YUV_Y_GREEN * from.imgd[width * height + i]
		    + PNM_YUV_Y_BLUE * from.imgd[2 * width * height + i];
		imgd[width * height + i] =
		    PNM_YUV_Cb_RED * from.imgd[i]
		    + PNM_YUV_Cb_GREEN * from.imgd[width * height + i]
		    + PNM_YUV_Cb_BLUE * from.imgd[2 * width * height + i];
		imgd[2 * width * height + i] =
		    PNM_YUV_Cr_RED * from.imgd[i]
		    + PNM_YUV_Cr_GREEN * from.imgd[width * height + i]
		    + PNM_YUV_Cr_BLUE * from.imgd[2 * width * height + i];
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
	this.free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::YCbCr2RGB(const PNM_DOUBLE &from)
{
	const char *FunctionName = "PNM_DOUBLE::YCbCr2RGB";
	std::string ErrorValueName;

	if (from.imgd == nullptr) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}
	if ((from.desc % 3) != 0) {
		fprintf(stderr, "*** %s() error - The input image is binary or grayscale ***\n", FunctionName);
		goto ExitError;
	}

	width = from.width;
	height = from.height;
	maxint = from.maxint;
	try {
		imgd = new pnm_img_double[3 * width * height];
	}
	catch (const std::bad_alloc &bad) {
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	for (i = 0; i < width * height; i++) {
		imgd[i] =
		    PNM_RGB_RED_Y * from.imgd[i]
		    + PNM_RGB_RED_Cb * from.imgd[width * height + i]
		    + PNM_RGB_RED_Cr * from.imgd[2 * width * height + i];
		imgd[width * height + i] =
		    PNM_RGB_GREEN_Y * from.imgd[i]
		    + PNM_RGB_GREEN_Cb * from.imgd[width * height + i]
		    + PNM_RGB_GREEN_Cr * from.imgd[2 * width * height + i];
		imgd[2 * width * height + i] =
		    PNM_RGB_BLUE_Y * from.imgd[i]
		    + PNM_RGB_BLUE_Cb * from.imgd[width * height + i]
		    + PNM_RGB_BLUE_Cr * from.imgd[2 * width * height + i];
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this.free();
	return PNM_FUNCTION_ERROR;
}

