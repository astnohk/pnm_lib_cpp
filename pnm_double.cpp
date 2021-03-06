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

	if (pnmd.isNULL()) {
		ErrorValueName = "pnmd.imgd";
		goto ErrorPointerNull;
	}
	if (desc % 3 == 0) {
		try {
			imgd = new pnm_img_double[3 * size];
		}
		catch (const std::bad_alloc &bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (size_t i = 0; i < 3 * size; i++) {
			imgd[i] = pnmd.imgd[i];
		}
	} else {
		try {
			imgd = new pnm_img_double[size];
		}
		catch (const std::bad_alloc &bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (size_t i = 0; i < size; i++) {
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

PNM_DOUBLE::PNM_DOUBLE(const int Descriptor, const size_t &Width, const size_t &Height, const int MaxInt, const pnm_img_double *Data)
{
	const char *FunctionName = "PNM_DOUBLE::PNM_DOUBLE(int, size_t, size_t, int, pnm_img_double *)";
	std::string ErrorValueName;
	std::string ErrorDesc;

	if (Descriptor < PNM_DESCRIPTOR_MIN
	    || Descriptor > PNM_DESCRIPTOR_MAX) {
		ErrorDesc = "PNM descriptor is out of range [1, 6]";
		goto ErrorOthers;
	}
	desc = Descriptor;
	width = Width;
	height = Height;
	maxint = MaxInt;
	size = size_t(Width) * size_t(Height);
	if (Descriptor % PNM_DESCRIPTOR_PIXMAPS == 0) {
		try {
			imgd = new pnm_img_double[3 * size];
		}
		catch (const std::bad_alloc& bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		if (Data != nullptr) {
			for (size_t i = 0; i < 3 * size; i++) {
				imgd[i] = Data[i];
			}
		} else {
			for (size_t i = 0; i < 3 * size; i++) {
				imgd[i] = 0.0;
			}
		}
	} else {
		try {
			imgd = new pnm_img_double[size];
		}
		catch (const std::bad_alloc& bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		if (Data != nullptr) {
			for (size_t i = 0; i < size; i++) {
				imgd[i] = Data[i];
			}
		} else {
			for (size_t i = 0; i < size; i++) {
				imgd[i] = 0.0;
			}
		}
	}
	return;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ExitError;
ErrorOthers:
	fprintf(stderr, "*** %s error - %s ***\n", FunctionName, ErrorValueName.c_str());
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

pnm_img_double &
PNM_DOUBLE::operator[](const size_t &n) const
{
	return imgd[n];
}

pnm_img_double &
PNM_DOUBLE::at(const size_t &x, const size_t &y) const
{
	assert(x < width && y < height);
	return imgd[width * y + x];
}

pnm_img_double &
PNM_DOUBLE::at(const size_t &x, const size_t &y, const size_t &c) const
{
	assert(x < width && y < height && c < 3);
	return imgd[width * height * c + width * y + x];
}

pnm_img_double
PNM_DOUBLE::Image(const size_t &x, const size_t &y) const
{
	if (x < width && y < height) {
		return imgd[width * y + x];
	} else {
		return 0;
	}
}

pnm_img_double
PNM_DOUBLE::Image(const size_t &x, const size_t &y, const size_t &c) const
{
	if (x < width && y < height && c < 3) {
		return imgd[width * height * c + width * y + x];
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
	size = 0;
	delete[] imgd;
	imgd = nullptr;
}

int
PNM_DOUBLE::copy(const PNM_DOUBLE &pnmd)
{
	const char *FunctionName = "PNM_DOUBLE::copy(const PNM_DOUBLE &)";
	std::string ErrorValueName;

	if (pnmd.isNULL()) {
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
	size = pnmd.size;
	if (desc % 3 == 0) {
		try {
			imgd = new pnm_img_double[3 * size];
		}
		catch (const std::bad_alloc &bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (size_t i = 0; i < 3 * size; i++) {
			imgd[i] = pnmd.imgd[i];
		}
	} else {
		try {
			imgd = new pnm_img_double[size];
		}
		catch (const std::bad_alloc& bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (size_t i = 0; i < size; i++) {
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
PNM_DOUBLE::copy(const PNM &pnm_int, const double &coeff)
{
	const char *FunctionName = "PNM_DOUBLE::copy(const PNM &, const double &, const PNM_OFFSET &)";
	std::string ErrorValueName;
	pnm_img *img_data = nullptr;

	if (pnm_int.isNULL()) {
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
	size = pnm_int.Size();
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
				std::cerr << bad.what() << std::endl;
				ErrorValueName = "imgd";
				goto ErrorMalloc;
			}
			for (size_t i = 0; i < size; i++) {
				imgd[i] = coeff * pnm_img_double(img_data[i]);
			}
			break;
		case PORTABLE_PIXMAP_ASCII:
		case PORTABLE_PIXMAP_BINARY:
			try {
				imgd = new pnm_img_double[3 * size];
			}
			catch (const std::bad_alloc& bad) {
				std::cerr << bad.what() << std::endl;
				ErrorValueName = "imgd";
				goto ErrorMalloc;
			}
			for (size_t i = 0; i < size; i++) {
				imgd[i] = coeff * pnm_img_double(img_data[i]);
				imgd[size + i] = coeff * pnm_img_double(img_data[size + i]);
				imgd[2 * size + i] = coeff * pnm_img_double(img_data[2 * size + i]);
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
PNM_DOUBLE::copy(const int Descriptor, const size_t &Width, const size_t &Height, const int MaxInt, const pnm_img_double *Data)
{
	const char *FunctionName = "PNM_DOUBLE::copy(int, size_t, size_t, int, pnm_img_double *)";
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
	}
	if (imgd != nullptr) {
		this->free();
	}
	desc = Descriptor;
	width = Width;
	height = Height;
	maxint = MaxInt;
	size = size_t(Width) * size_t(Height);
	if (Descriptor % PNM_DESCRIPTOR_PIXMAPS == 0) {
		try {
			imgd = new pnm_img_double[3 * size];
		}
		catch (const std::bad_alloc& bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		for (size_t i = 0; i < 3 * size; i++) {
			imgd[i] = Data[i];
		}
	} else {
		try {
			imgd = new pnm_img_double[size];
		}
		catch (const std::bad_alloc& bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		for (size_t i = 0; i < size; i++) {
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
		Image = new double[size];
	}
	catch (const std::bad_alloc& bad) {
		std::cerr << bad.what() << std::endl;
		fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, "Image");
		return nullptr;
	}
	for (size_t n = 0; n < size; n++) {
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
	if (from.isNULL()) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}
	width = from.Width();
	height = from.Height();
	maxint = from.MaxInt();
	size = from.Size();
	if (from.isRGB() == false) {
		fprintf(stderr, "*** %s() warning - The input image is binarymap or graymap ***\n", FunctionName);
		if (from.Desc() <= PNM_DESCRIPTOR_ASCII_MAX) {
			desc = PORTABLE_GRAYMAP_ASCII;
		} else {
			desc = PORTABLE_GRAYMAP_BINARY;
		}
		try {
			imgd = new pnm_img_double[size];
		}
		catch (const std::bad_alloc& bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		imgd_from = from.Data();
		for (size_t i = 0; i < size; i++) {
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
			imgd = new pnm_img_double[size];
		}
		catch (const std::bad_alloc& bad) {
			std::cerr << bad.what() << std::endl;
			ErrorValueName = "imgd";
			goto ErrorMalloc;
		}
		imgd_from = from.Data();
		for (size_t i = 0; i < size; i++) {
			imgd[i] =
			    PNM_YUV_Y_RED * imgd_from[i]
			    + PNM_YUV_Y_GREEN * imgd_from[size + i]
			    + PNM_YUV_Y_BLUE * imgd_from[2 * size + i];
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
	size = from.Size();
	try {
		imgd = new pnm_img_double[3 * size];
	}
	catch (const std::bad_alloc& bad) {
		std::cerr << bad.what() << std::endl;
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	if (from.Desc() <= PNM_DESCRIPTOR_ASCII_MAX) {
		desc = PORTABLE_PIXMAP_ASCII;
	} else {
		desc = PORTABLE_PIXMAP_BINARY;
	}
	imgd_from = from.Data();
	for (size_t i = 0; i < size; i++) {
		imgd[i] = imgd_from[i];
		imgd[size + i] = imgd_from[i];
		imgd[2 * size + i] = imgd_from[i];
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

	if (from.isNULL()) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}

	desc = PORTABLE_PIXMAP_BINARY;
	width = from.Width();
	height = from.Height();
	maxint = from.MaxInt();
	size = from.Size();
	if (from.isRGB() == false) {
		fprintf(stderr, "*** %s() error - The input image is binary or grayscale ***\n", FunctionName);
		goto ExitError;
	}
	try {
		imgd = new pnm_img_double[3 * size];
	}
	catch (const std::bad_alloc& bad) {
		std::cerr << bad.what() << std::endl;
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	imgd_from = from.Data();
	for (size_t i = 0; i < size; i++) {
		imgd[i] =
		    PNM_YUV_Y_RED * imgd_from[i]
		    + PNM_YUV_Y_GREEN * imgd_from[size + i]
		    + PNM_YUV_Y_BLUE * imgd_from[2 * size + i];
		imgd[size + i] =
		    PNM_YUV_Cb_RED * imgd_from[i]
		    + PNM_YUV_Cb_GREEN * imgd_from[size + i]
		    + PNM_YUV_Cb_BLUE * imgd_from[2 * size + i];
		imgd[2 * size + i] =
		    PNM_YUV_Cr_RED * imgd_from[i]
		    + PNM_YUV_Cr_GREEN * imgd_from[size + i]
		    + PNM_YUV_Cr_BLUE * imgd_from[2 * size + i];
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

	if (from.isNULL()) {
		ErrorValueName = "from.imgd";
		goto ErrorPointerNull;
	}
	if (from.isRGB() == false) {
		fprintf(stderr, "*** %s() error - The input image is binary or grayscale ***\n", FunctionName);
		goto ExitError;
	}

	width = from.Width();
	height = from.Height();
	maxint = from.MaxInt();
	size = from.Size();
	try {
		imgd = new pnm_img_double[3 * size];
	}
	catch (const std::bad_alloc& bad) {
		std::cerr << bad.what() << std::endl;
		ErrorValueName = "imgd";
		goto ErrorMalloc;
	}
	imgd_from = from.Data();
	for (size_t i = 0; i < size; i++) {
		imgd[i] =
		    PNM_RGB_RED_Y * imgd_from[i]
		    + PNM_RGB_RED_Cb * imgd_from[size + i]
		    + PNM_RGB_RED_Cr * imgd_from[2 * size + i];
		imgd[size + i] =
		    PNM_RGB_GREEN_Y * imgd_from[i]
		    + PNM_RGB_GREEN_Cb * imgd_from[size + i]
		    + PNM_RGB_GREEN_Cr * imgd_from[2 * size + i];
		imgd[2 * size + i] =
		    PNM_RGB_BLUE_Y * imgd_from[i]
		    + PNM_RGB_BLUE_Cb * imgd_from[size + i]
		    + PNM_RGB_BLUE_Cr * imgd_from[2 * size + i];
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

