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

PNM_DOUBLE::PNM_DOUBLE(const PNM_DOUBLE &pnmd) : PNM_FORMAT(pnmd)
{
	const char *FunctionName = "PNM_DOUBLE::PNM_DOUBLE(const PNM_DOUBLE &)";
	std::string ErrorValueName;

	if (pnmd.isNULL() != false) {
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
	fprintf(stderr, "*** %s error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this->free();
	return;
}

PNM_DOUBLE::~PNM_DOUBLE(void)
{
	delete[] imgd;
	imgd = nullptr;
}

pnm_img_double *
PNM_DOUBLE::Data(void) const
{
	return imgd;
}

pnm_img_double
PNM_DOUBLE::Image(int x, int y) const
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
	const char *FunctionName = "PNM_DOUBLE::copy(const PNM_DOUBLE &)";
	std::string ErrorValueName;

	if (pnmd.isNULL() != false) {
		ErrorValueName = "pnmd.imgd";
		goto ErrorPointerNull;
	}
	if (imgd != nullptr) {
		this->free();
	}
	desc = pnmd.desc;
	width = pnmd.width;
	height = pnmd.height;
	maxint = pnmd.maxint;
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
	fprintf(stderr, "*** %s error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this->free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::copy(const PNM &pnm_int, double coeff)
{
	const char *FunctionName = "PNM_DOUBLE::copy(const PNM &, const double &, const PNM_OFFSET &)";
	std::string ErrorValueName;
	pnm_img *img_data = nullptr;
	int i;

	if (pnm_int.isNULL() != false) {
		ErrorValueName = "pnm_int.img";
		goto ErrorPointerNull;
	}
	if (fabs(coeff) < 1.0 / pnm_int.MaxInt()) {
		fprintf(stderr, "*** %s() warning - The coefficient is nearly ZERO ***\n", FunctionName);
	}
	if (imgd != nullptr) {
		this->free();
	}

	desc = pnm_int.Desc();
	width = pnm_int.Width();
	height = pnm_int.Height();
	maxint = pnm_int.MaxInt();
	img_data = pnm_int.Data();
	switch (desc) {
		case PORTABLE_BITMAP_ASCII:
		case PORTABLE_GRAYMAP_ASCII:
		case PORTABLE_BITMAP_BINARY:
		case PORTABLE_GRAYMAP_BINARY:
			try {
				imgd = new pnm_img_double[width * height];
			}
			catch (const std::bad_alloc &bad) {
				ErrorValueName = "imgd";
				goto ErrorMalloc;
			}
			for (i = 0; i < width * height; i++) {
				imgd[i] = coeff * (pnm_img_double)img_data[i];
			}
			break;
		case PORTABLE_PIXMAP_ASCII:
		case PORTABLE_PIXMAP_BINARY:
			try {
				imgd = new pnm_img_double[3 * width * height];
			}
			catch (const std::bad_alloc &bad) {
				ErrorValueName = "imgd";
				goto ErrorMalloc;
			}
			for (i = 0; i < width * height; i++) {
				imgd[i] = coeff * (pnm_img_double)img_data[i];
				imgd[width * height + i] = coeff * (pnm_img_double)img_data[width * height + i];
				imgd[2 * width * height + i] = coeff * (pnm_img_double)img_data[2 * width * height + i];
			}
			break;
		default: // ERROR
			fprintf(stderr, "*** %s() error - Descriptor is incorrect (P%d) ***\n", FunctionName, desc);
			goto ExitError;
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this->free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::copy(int Descriptor, int Width, int Height, int MaxInt, double *Data)
{
	const char *FunctionName = "PNM_DOUBLE::copy(int, int, int, int, double *)";
	std::string ErrorValueName;
	std::string ErrorDesc;

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
	if (imgd != nullptr) {
		this->free();
	}
	desc = Descriptor;
	width = Width;
	height = Height;
	maxint = MaxInt;
	if (Descriptor % PNM_DESCRIPTOR_PIXMAPS == 0) {
		try {
			imgd = new pnm_img_double[3 * width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (int i = 0; i < 3 * width * height; i++) {
			imgd[i] = Data[i];
		}
	} else {
		try {
			imgd = new pnm_img_double[width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			imgd[i] = Data[i];
		}
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorOthers:
	fprintf(stderr, "*** %s error - %s ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this->free();
	return PNM_FUNCTION_ERROR;
}

double *
PNM_DOUBLE::get_double(void) const
{
	const char *FunctionName = "PNM_DOUBLE::get_double";
	double *Image = nullptr;

	try {
		Image = new double[width * height];
	}
	catch (const std::bad_alloc &bad) {
		fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, "Image");
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
	pnm_img_double *imgd_from = nullptr;

	if (imgd != nullptr) {
		this->free();
	}
	if (from.isNULL() != false) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}
	width = from.Width();
	height = from.Height();
	maxint = from.MaxInt();
	if ((from.Desc() % 3) != 0) {
		fprintf(stderr, "*** %s() warning - The input image is binarymap or graymap ***\n", FunctionName);
		if (from.Desc() <= PNM_DESCRIPTOR_ASCII_MAX) {
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
		imgd_from = from.Data();
		for (int i = 0; i < width * height; i++) {
			imgd[i] = imgd_from[i];
		}
		imgd_from = nullptr;
	} else {
		if (from.Desc() <= PNM_DESCRIPTOR_ASCII_MAX) {
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
		imgd_from = from.Data();
		for (int i = 0; i < width * height; i++) {
			imgd[i] =
			    PNM_YUV_Y_RED * imgd_from[i]
			    + PNM_YUV_Y_GREEN * imgd_from[width * height + i]
			    + PNM_YUV_Y_BLUE * imgd_from[2 * width * height + i];
		}
		imgd_from = nullptr;
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this->free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::Gray2RGB(const PNM_DOUBLE &from)
{
	const char *FunctionName = "PNM_DOUBLE::Gray2RGB";
	std::string ErrorValueName;
	pnm_img_double *imgd_from = nullptr;

	if (from.imgd == nullptr) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	} else if (from.isRGB() != false) {
		fprintf(stderr, "*** %s() error - PNM Descriptor of (*from) is NOT right ***\n", FunctionName);
		goto ExitError;
	}

	width = from.Width();
	height = from.Height();
	maxint = from.MaxInt();
	try {
		imgd = new pnm_img_double[3 * width * height];
	}
	catch (const std::bad_alloc &bad) {
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	if (from.Desc() <= PNM_DESCRIPTOR_ASCII_MAX) {
		desc = PORTABLE_PIXMAP_ASCII;
	} else {
		desc = PORTABLE_PIXMAP_BINARY;
	}
	imgd_from = from.Data();
	for (int i = 0; i < width * height; i++) {
		imgd[i] = imgd_from[i];
		imgd[width * height + i] = imgd_from[i];
		imgd[2u * width * height + i] = imgd_from[i];
	}
	imgd_from = nullptr;
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ExitError:
	this->free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::RGB2YCbCr(const PNM_DOUBLE &from)
{
	const char *FunctionName = "PNM_DOUBLE::RGB2YCbCr";
	std::string ErrorValueName;
	pnm_img_double *imgd_from = nullptr;

	if (from.isNULL() != false) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}

	desc = PORTABLE_PIXMAP_BINARY;
	width = from.Width();
	height = from.Height();
	maxint = from.MaxInt();
	if ((from.Desc() % 3) != 0) {
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
	imgd_from = from.Data();
	for (int i = 0; i < width * height; i++) {
		imgd[i] =
		    PNM_YUV_Y_RED * imgd_from[i]
		    + PNM_YUV_Y_GREEN * imgd_from[width * height + i]
		    + PNM_YUV_Y_BLUE * imgd_from[2 * width * height + i];
		imgd[width * height + i] =
		    PNM_YUV_Cb_RED * imgd_from[i]
		    + PNM_YUV_Cb_GREEN * imgd_from[width * height + i]
		    + PNM_YUV_Cb_BLUE * imgd_from[2 * width * height + i];
		imgd[2 * width * height + i] =
		    PNM_YUV_Cr_RED * imgd_from[i]
		    + PNM_YUV_Cr_GREEN * imgd_from[width * height + i]
		    + PNM_YUV_Cr_BLUE * imgd_from[2 * width * height + i];
	}
	imgd_from = nullptr;
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this->free();
	return PNM_FUNCTION_ERROR;
}

int
PNM_DOUBLE::YCbCr2RGB(const PNM_DOUBLE &from)
{
	const char *FunctionName = "PNM_DOUBLE::YCbCr2RGB";
	std::string ErrorValueName;
	pnm_img_double *imgd_from = nullptr;

	if (from.isNULL() != false) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}
	if ((from.Desc() % 3) != 0) {
		fprintf(stderr, "*** %s() error - The input image is binary or grayscale ***\n", FunctionName);
		goto ExitError;
	}

	width = from.Width();
	height = from.Height();
	maxint = from.MaxInt();
	try {
		imgd = new pnm_img_double[3 * width * height];
	}
	catch (const std::bad_alloc &bad) {
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	imgd_from = from.Data();
	for (int i = 0; i < width * height; i++) {
		imgd[i] =
		    PNM_RGB_RED_Y * imgd_from[i]
		    + PNM_RGB_RED_Cb * imgd_from[width * height + i]
		    + PNM_RGB_RED_Cr * imgd_from[2 * width * height + i];
		imgd[width * height + i] =
		    PNM_RGB_GREEN_Y * imgd_from[i]
		    + PNM_RGB_GREEN_Cb * imgd_from[width * height + i]
		    + PNM_RGB_GREEN_Cr * imgd_from[2 * width * height + i];
		imgd[2 * width * height + i] =
		    PNM_RGB_BLUE_Y * imgd_from[i]
		    + PNM_RGB_BLUE_Cb * imgd_from[width * height + i]
		    + PNM_RGB_BLUE_Cr * imgd_from[2 * width * height + i];
	}
	imgd_from = nullptr;
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is NULL ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	this->free();
	return PNM_FUNCTION_ERROR;
}

