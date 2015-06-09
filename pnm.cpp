#include "pnm.h"



// PNM_FORMAT Methods
PNM_FORMAT::PNM_FORMAT(void)
{
	desc = 0;
	width = 0;
	height = 0;
	maxint = 0;
}

PNM_FORMAT::PNM_FORMAT(const PNM_FORMAT &pnm)
{
	desc = pnm.desc;
	width = pnm.width;
	height = pnm.height;
	maxint = pnm.maxint;
}

int
PNM_FORMAT::Width(void)
{
	return width;
}

int
PNM_FORMAT::Height(void)
{
	return height;
}

int
PNM_FORMAT::MaxInt(void)
{
	return maxint;
}

int
PNM_FORMAT::bitdepth(void)
{
	return (int)round(std::log2(maxint + 1.0));
}

bool
PNM_FORMAT::isSameFormat(const PNM_FORMAT &pnm)
{
	if ((desc % PNM_DESCRIPTOR_LENGTH) == (pnm.desc % PNM_DESCRIPTOR_LENGTH)) {
		return true;
	} else {
		return false;
	}
}

bool
PNM_FORMAT::isSameDescriptor(const PNM_FORMAT &pnm)
{
	if (desc == pnm.desc) {
		return true;
	} else {
		return false;
	}
}

bool
PNM_FORMAT::isRGB(void)
{
	if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
		return true;
	} else {
		return false;
	}
}




// PNM Methods
PNM::PNM(void)
{
	desc = 0;
	width = 0;
	height = 0;
	maxint = 0;
	img = nullptr;
}

PNM::PNM(const PNM &pnm)
{
	const char *FunctionName = "PNM::PNM(const PNM &)";
	std::string ErrorValueName;

	desc = pnm.desc;
	width = pnm.width;
	height = pnm.height;
	maxint = pnm.maxint;
	if (desc % 3 == 0) {
		try {
			img = new pnm_img[3 * width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "img";
			goto ExitError;
		}
		for (int i = 0; i < 3 * width * height; i++) {
			img[i] = pnm.img[i];
		}
	} else {
		try {
			img = new pnm_img[width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "img";
			goto ExitError;
		}
		for (int i = 0; i < width * height; i++) {
			img[i] = pnm.img[i];
		}
	}
	return;
// Error
ExitError:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	this.free();
	return;
}

PNM::~PNM(void)
{
	delete[] img;
	img = nullptr;
}

pnm_img
PNM::img(int x, int y)
{
	if (0 <= x && x < width && 0 <= y && y < height) {
		return img[width * y + x];
	} else {
		return 0;
	}
}

void
PNM::free(void)
{
	desc = 0;
	width = 0;
	height = 0;
	maxint = 0;
	delete[] img;
	img = nullptr;
}

int
PNM::copy(const PNM &pnm)
{
	const char *FunctionName = "PNM::copy(const PNM &)";
	std::string ErrorValueName;

	if (pnm.img == nullptr) {
		ErrorValueName = "pnm.img";
		goto ErrorPointerNull;
	}
	desc = pnm.desc;
	width = pnm.width;
	height = pnm.height;
	maxint = pnm.maxint;
	delete[] img;
	if (desc % 3 == 0) {
		try {
			img = new pnm_img[3 * width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		for (int i = 0; i < 3 * width * height; i++) {
			img[i] = pnm.img[i];
		}
	} else {
		try {
			img = new pnm_img[width * height];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			img[i] = pnm.img[i];
		}
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

int
PNM::copy(const PNM_DOUBLE &pnm_double, const double &coeff, const char *process, const PNM_OFFSET &add_offset)
{
	const char *FunctionName = "PNM::copy(const PNM_DOUBLE &, const double &, const char *, const PNM_OFFSET &)";
	std::string ErrorValueName;

	double FRC = 0.5;
	unsigned int M, N;

	if (pnm_double.imgd == nullptr) {
		ErrorValueName = "(pnm_double->imgd)";
		goto ErrorPointerNull;
	}
	if (fabs(coeff) < 1.0 / pnm_double.maxint) {
		fprintf(stderr, "*** %s() warning - The coefficient is ZERO ***\n", FunctionName);
	}

	desc = pnm_double.desc;
	width = pnm_double.width;
	height = pnm_double.height;
	maxint = pnm_double.maxint;
	M = pnm_double.height;
	N = pnm_double.width;
	// Clear *img
	delete[] img;
	if (process != nullptr) {
		if (strcmp(process, "floor") == 0) {
			FRC = .0;
		} else if (strcmp(process, "round") == 0) {
			FRC = 0.5;
		} else if (strcmp(process, "ceil") == 0) {
			FRC = 1.0;
		}
	}
	switch (pnm_double.desc) {
		case PORTABLE_BITMAP_ASCII:
		case PORTABLE_GRAYMAP_ASCII:
		case PORTABLE_BITMAP_BINARY:
		case PORTABLE_GRAYMAP_BINARY:
			try {
				img = new pnm_img[M * N];
			}
			catch (const std::bad_alloc &bad) {
				ErrorValueName = "img";
				goto ErrorMalloc;
			}
			for (int i = 0; i < M * N; i++) {
				if (add_offset == nullptr) {
					img[i] = (pnm_img)floor(coeff * pnm_double.imgd[i] + FRC);
				} else {
					img[i] = (pnm_img)floor(coeff * pnm_double.imgd[i] + add_offset.r + FRC);
				}
			}
			break;
		case PORTABLE_PIXMAP_ASCII:
		case PORTABLE_PIXMAP_BINARY:
			try {
				img = new pnm_img[3 * M * N];
			}
			catch (const std::bad_alloc &bad) {
				ErrorValueName = "img";
				goto ErrorMalloc;
			}
			for (int i = 0; i < M * N; i++) {
				if (add_offset == nullptr) {
					img[i] = (pnm_img)floor(coeff * pnm_double.imgd[i] + FRC);
					img[M * N + i] = (pnm_img)floor(coeff * pnm_double.imgd[M * N + i] + FRC);
					img[2 * M * N + i] = (pnm_img)floor(coeff * pnm_double.imgd[2 * M * N + i] + FRC);
				} else {
					img[i] = (pnm_img)floor(coeff * pnm_double.imgd[i] + add_offset.r + FRC);
					img[M * N + i] = (pnm_img)floor(coeff * pnm_double.imgd[M * N + i] + add_offset.g + FRC);
					img[2 * M * N + i] = (pnm_img)floor(coeff * pnm_double.imgd[2 * M * N + i] + add_offset.b + FRC);
				}
			}
			break;
		default: // ERROR
			fprintf(stderr, "*** %s() error - Descriptor is incorrect (P%d) ***\n", FunctionName.c_str(), pnm_double.desc);
			goto ExitError;
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
PNM::copy(int Descriptor, int Width, int Height, int MaxInt, int *Data)
{
	const char *FunctionName = "PNM::copy(int, int, int *)";
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
			img = new pnm_img[3 * width * height];
		}
		catch (std::bad_alloc bad) {
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		for (int i = 0; i < 3 * width * height; i++) {
			img[i] = Data[i];
		}
	} else {
		try {
			img = new pnm_img[width * height];
		}
		catch (std::bad_alloc bad) {
			ErrorValueName = "img";
			goto ErrorMalloc;
		}
		for (int i = 0; i < width * height; i++) {
			img[i] = Data[i];
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

int *
PNM::get_int(void)
{
	ERROR Error("PNM::get_int");
	int *Image = nullptr;

	try {
		Image = new int[width * height];
	}
	catch (const std::bad_alloc &bad) {
		Error.Value("Image");
		Error.Malloc();
		return nullptr;
	}
	for (int n = 0; n < width * height; n++) {
		Image[n] = img[n];
	}
	return Image;
}

double *
PNM::get_double(void)
{
	ERROR Error("PNM::get_double");
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
		Image[n] = (double)img[n];
	}
	return Image;
}

int
PNM::read(const char *filename)
{
	const char *FunctionName = "PNM::read";
	std::string ErrorFunctionName;
	std::string ErrorValueName;
	std::string ErrorFileName;
	std::string ErrorDescription;

	uint8_t *img_uint8 = nullptr;
	size_t size_t_max = (size_t)(~0u);
	char FileDescriptor[PNM_DESCRIPTOR_LENGTH];
	unsigned int width_tmp = 0u;
	unsigned int m, n, byte;
	int test;
	FILE *fp = nullptr;

	if (img != nullptr) {
		this.free();
	}
	if ((fp = fopen(filename, "rb")) == nullptr) {
		ErrorFunctionName = "fopen";
		ErrorValueName = "fp";
		ErrorFileName = filename;
		goto ErrorFileOpen;
	}
	if ((fgets(FileDescriptor, PNM_DESCRIPTOR_LENGTH, fp)) == nullptr) {
		ErrorFunctionName = "fgets";
		ErrorValueName = "FileDescriptor";
		goto ErrorRead;
	}
	if (FileDescriptor[0] != 'P') {
		ErrorDescription = "The file is not PNM format";
		goto ErrorOthers;
	}

	printf("\n--- Reading from \"%s\" ---\n  FileDescriptor : %s\n", filename, FileDescriptor);
	desc = (int)FileDescriptor[1] - (int)'0';
	switch (desc) {
		case PORTABLE_BITMAP_ASCII: // Portable Bitmap ASCII
		case PORTABLE_BITMAP_BINARY: // Portable Bitmap Binary
			if (fcommentf(fp, &width) == false
			    || fcommentf(fp, &height) == false) {
				ErrorFunctionName = "fcommentf";
				ErrorValueName = "width and height";
				goto ErrorRead;
			}
			if (width <= 0 || height <= 0) {
				goto ErrorImageSize;
			}
			maxint = 1;
			if (fgetc(fp) == EOF) { // Pass through '\n'
				ErrorFunctionName = "fgetc";
				ErrorValueName = "\\n";
				goto ErrorRead;
			}
			try {
				img = new pnm_img[width * height];
			}
			catch (const std::bad_alloc &bad) {
				ErrorFunctionName = "new";
				ErrorValueName = "img";
				goto ErrorMalloc;
			}
			if (desc == PORTABLE_BITMAP_ASCII) {
				test = 0;
				for (m = 0; m < width * height; m++) {
					test |= fscanf(fp, "%1d", &(img[m]));
					img[m] = 1 - img[m];
				}
				if (test != 1) {
					ErrorFunctionName = "fscanf";
					ErrorValueName = "img";
					goto ErrorRead;
				}
			} else {
				width_tmp = (unsigned int)ceil((double)width / BITS_OF_BYTE);
				try {
					img_uint8 = new uint8_t[width_tmp * height];
				}
				catch (const std::bad_alloc &bad) {
					ErrorFunctionName = "new";
					ErrorValueName = "img_uint8";
					goto ErrorMalloc;
				}
				if ((unsigned int)fread(img_uint8, sizeof(uint8_t), (size_t)(width_tmp * height), fp) != width_tmp * height) {
					ErrorFunctionName = "fread";
					ErrorValueName = "img_uint8";
					goto ErrorRead;
				}
				for (m = 0; m < height; m++) {
					for (n = 0; n < width_tmp; n++) {
						for (byte = 0; byte < BITS_OF_BYTE && BITS_OF_BYTE * n + byte < width; byte++) {
							img[width * m + BITS_OF_BYTE * n + byte]
							    = ((img_uint8[width_tmp * m + n] & (1 << (BITS_OF_BYTE - byte - 1))) == 0) ? 1 : 0;
						}
					}
				}
			}
			break;
		case PORTABLE_GRAYMAP_ASCII: // Portable Graymap ASCII
		case PORTABLE_GRAYMAP_BINARY: // Portable Graymap Binary
			if (fcommentf(fp, &width) == false
			    || fcommentf(fp, &height) == false
			    || fcommentf(fp, &maxint) == false) {
				ErrorFunctionName = "fcommentf";
				ErrorValueName = "width, height and maxint";
				goto ErrorRead;
			}
			if (width <= 0 || height <= 0) {
				goto ErrorImageSize;
			}
			if (fgetc(fp) == EOF) { // Pass through '\n'
				ErrorFunctionName = "fgetc";
				ErrorValueName = "\\n";
				goto ErrorRead;
			}
			try {
				img = new pnm_img[width * height];
			}
			catch (const std::bad_alloc &bad) {
				ErrorFunctionName = "new";
				ErrorValueName = "img";
				goto ErrorMalloc;
			}
			if (desc == PORTABLE_GRAYMAP_ASCII) {
				test = 0;
				for (m = 0; m < width * height; m++) {
					test |= fscanf(fp, "%7d", &(img[m]));
				}
				if (test != 1) {
					ErrorFunctionName = "fscanf";
					ErrorValueName = "img";
					goto ErrorRead;
				}
			} else {
				if (maxint > 0xFF) {
					// 16-bit data
					try {
						img_uint8 = new uint8_t[2 * width * height]; // 2 times the size for 16-bit
					}
					catch (const std::bad_alloc &bad) {
						ErrorFunctionName = "new";
						ErrorValueName = "img_uint8";
						goto ErrorMalloc;
					}
					if ((unsigned int)fread(img_uint8, sizeof(uint8_t), (size_t)(2 * width * height), fp) != 2 * width * height) {
						ErrorFunctionName = "fread";
						ErrorValueName = "img_uint8";
						goto ErrorRead;
					}
					for (m = 0; m < width * height; m++) {
						img[m] = (int)(img_uint8[2 * m] << 8) + (int)img_uint8[2 * m + 1];
					}
				} else {
					// 8-bit data
					try {
						img_uint8 = new uint8_t[width * height];
					}
					catch (const std::bad_alloc &bad) {
						ErrorFunctionName = "new";
						ErrorValueName = "img_uint8";
						goto ErrorMalloc;
					}
					if ((unsigned int)fread(img_uint8, sizeof(uint8_t), (size_t)(width * height), fp) != width * height) {
						ErrorFunctionName = "fread";
						ErrorValueName = "img_uint8";
						goto ErrorRead;
					}
					for (m = 0; m < width * height; m++) {
						img[m] = (int)img_uint8[m];
					}
				}
			}
			break;
		case PORTABLE_PIXMAP_ASCII: // Portable Pixmap ASCII
		case PORTABLE_PIXMAP_BINARY: // Portable Pixmap Binary
			if (fcommentf(fp, &width) == false
			    || fcommentf(fp, &height) == false
			    || fcommentf(fp, &maxint) == false) {
				ErrorFunctionName = "fcommentf";
				ErrorValueName = "width, height and maxint";
				goto ErrorRead;
			}
			if (width <= 0 || height <= 0) {
				goto ErrorImageSize;
			}
			if (fgetc(fp) == EOF) { // Pass through '\n'
				ErrorFunctionName = "fgetc";
				ErrorValueName = "\\n";
				goto ErrorRead;
			}
			try {
				img = new pnm_img[3 * width * height];
			}
			catch (const std::bad_alloc &bad) {
				ErrorFunctionName = "new";
				ErrorValueName = "img";
				goto ErrorMalloc;
			}
			if (desc == PORTABLE_PIXMAP_ASCII) {
				test = 0;
				for (m = 0; m < width * height; m++) {
					test |= fscanf(fp, "%7d", &(img[m]));
					test |= fscanf(fp, "%7d", &(img[width * height + m]));
					test |= fscanf(fp, "%7d", &(img[2 * width * height + m]));
				}
				if (test != 1) {
					ErrorFunctionName = "fscanf";
					ErrorValueName = "img";
					goto ErrorRead;
				}
			} else {
				if (maxint > 0xFF) {
					// 16-bit data
					try {
						img_uint8 = new uint8_t[2 * 3 * width]; // 2 times the size for 16-bit
					}
					catch (const std::bad_alloc &bad) {
						ErrorFunctionName = "new";
						ErrorValueName = "img_uint8";
						goto ErrorMalloc;
					}
					for (m = 0; m < height; m++) {
						if ((unsigned int)fread(img_uint8, sizeof(uint8_t), (size_t)(2 * 3 * width), fp) != 2 * 3 * width) {
							ErrorFunctionName = "fread";
							ErrorValueName = "img_uint8";
							goto ErrorRead;
						}
						for (n = 0; n < width; n++) {
							img[width * m + n] = (int)(img_uint8[2 * 3 * n] << 8) + (int)img_uint8[2 * 3 * n + 1];
							img[width * height + width * m + n] = (int)(img_uint8[2 * 3 * n + 2] << 8) + (int)img_uint8[2 * 3 * n + 3];
							img[2 * width * height + width * m + n] = (int)(img_uint8[2 * 3 * n + 4] << 8) + (int)img_uint8[2 * 3 * n + 5];
						}
					}
				} else {
					// 8-bit data
					try {
						img_uint8 = uint8_t[3 * width];
					}
					catch (const std::bad_alloc &bad) {
						ErrorFunctionName = "new";
						ErrorValueName = "img_uint8";
						goto ErrorMalloc;
					}
					for (m = 0; m < height; m++) {
						if ((unsigned int)fread(img_uint8, sizeof(uint8_t), (size_t)(3 * width), fp) != 3 * width) {
							ErrorFunctionName = "fread";
							ErrorValueName = "img_uint8";
							goto ErrorRead;
						}
						for (n = 0; n < width; n++) {
							img[width * m + n] = (int)img_uint8[3 * n];
							img[width * height + width * m + n] = (int)img_uint8[3 * n + 1];
							img[2 * width * height + width * m + n] = (int)img_uint8[3 * n + 2];
						}
					}
				}
			}
			break;
		default: // ERROR
			goto ErrorIncorrectDescriptor;
	}
	delete[] img_uint8;
	img_uint8 = nullptr;
	if (fclose(fp) == EOF) {
		fprintf(stderr, "*** %s() error - Failed to close the file by fclose() ***\n", FunctionName);
		this.free();
		return PNM_FUNCTION_ERROR;
	}
#ifndef PNM_NO_PRINT
	printf("  width : %d\n  height %d\n  Bit Depth : %d\n", width, height, this.bitdepth());
	printf("--- Successfully read the image ---\n\n");
#endif
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) by %s() ***\n", FunctionName, ErrorValueName.c_str(), ErrorFunctionName.c_str());
	goto ErrorReturn;
ErrorPointernullptr:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
	goto ErrorReturn;
ErrorPointerNotInitialized:
	fprintf(stderr, "*** %s() error - The pointer (*%s) may already be allocated or not initialized ***\n", FunctionName, ErrorValueName.c_str());
	goto ErrorReturn;
ErrorRead:
	fprintf(stderr, "*** %s() error - Cannot read %s correctly by %s() ***\n", FunctionName, ErrorValueName.c_str(), ErrorFunctionName.c_str());
	goto ErrorReturn;
ErrorFileOpen:
	fprintf(stderr, "*** %s() error - Cannot find \"%s\" by %s() for (%s) ***\n", FunctionName, ErrorFileName.c_str(), ErrorFunctionName.c_str(), ErrorValueName.c_str());
	goto ErrorReturn;
ErrorImageSize:
	fprintf(stderr, "*** %s() error - The size of image is larger than size_t ***\n", FunctionName);
	goto ErrorReturn;
ErrorIncorrectDescriptor:
	fprintf(stderr, "*** %s() error - Descriptor is incorrect (\"%s\") ***\n", FunctionName, FileDescriptor);
	goto ErrorReturn;
ErrorOthers:
	fprintf(stderr, "*** %s() error - %s ***\n", FunctionName, ErrorDescription.c_str());
ErrorReturn:
	if (fp != nullptr && fclose(fp) == EOF) {
		fprintf(stderr, "*** %s() error - Failed to close the file by fclose() ***\n", FunctionName);
	}
	delete[] img_uint8;
	this.free();
	return PNM_FUNCTION_ERROR;
}

int
PNM::write(const char *filename)
{
	const char *FunctionName = "PNM::write";
	std::string ErrorFunctionName;
	std::string ErrorValueName;

	std::string fixed_filename;
	uint8_t *img_uint8 = nullptr;
	unsigned int width_tmp = 0u;
	unsigned int m, n;
	unsigned int byte;
	FILE *fp = nullptr;

	if (img == nullptr) {
		ErrorValueName = "img";
		goto ErrorPointerNull;
	} else if (filename == nullptr) {
		ErrorValueName = "filename";
		goto ErrorPointerNull;
	}
	fixed_filename = pnm_FixExtension(filename, desc);
	if ((fp = fopen(fixed_filename.c_str(), "wb")) == nullptr) {
		fprintf(stderr, "*** %s() error - Cannot open the file \"%s\" ***\n", FunctionName, fixed_filename.c_str());
		return PNM_FUNCTION_ERROR;
	}
	printf("\n--- Writing to \"%s\" ---\n  Output PNM descriptor : %d\n", fixed_filename.c_str(), desc);
	printf("  width     = %d\n  height    = %d\n  Intensity = %d\n", width, height, maxint);

	switch (desc) {
		case PORTABLE_BITMAP_ASCII:
			fprintf(fp, "P1\n%d %d\n", width, height);
			for (m = 0; m < height; m++) {
				for (n = 0; n < width; n++) {
					fprintf(fp, "%d ", img[width * m + n] > 0 ? 0 : 1);
				}
				fprintf(fp, "\n");
			}
			break;
		case PORTABLE_GRAYMAP_ASCII:
			fprintf(fp, "P2\n%d %d\n%d\n", width, height, maxint);
			for (m = 0; m < height; m++) {
				for (n = 0; n < width; n++) {
					fprintf(fp, "%d ", img[width * m + n]);
				}
				fprintf(fp, "\n");
			}
			break;
		case PORTABLE_PIXMAP_ASCII:
			fprintf(fp, "P3\n%d %d\n", width, height);
			for (m = 0; m < height; m++) {
				for (n = 0; n < width; n++) {
					fprintf(fp, "%d %d %d", img[width * m + n], img[height * width + width * m + n], img[2 * height * width + width * m + n]);
				}
				fprintf(fp,"\n");
			}
			break;
		case PORTABLE_BITMAP_BINARY: // Bitmap
			// 16-bit and 8-bit data
			width_tmp = (unsigned int)ceil((double)width / BITS_OF_BYTE);
			try {
				img_uint8 = new uint8_t[width_tmp * height];
			}
			catch (const std::bad_alloc &bad) {
				ErrorFunctionName = "new";
				ErrorValueName = "img_uint8";
				goto ErrorMalloc;
			}
			for (m = 0; m < height; m++) {
				for (n = 0; n < width_tmp; n++) {
					for (byte = 0; byte < BITS_OF_BYTE; byte++) {
						img_uint8[width_tmp * m + n] <<= 1;
						if (BITS_OF_BYTE * n + byte < width
						    && img[width * m + BITS_OF_BYTE * n + byte] < (int)(maxint + 1) / 2) {
							img_uint8[width_tmp * m + n] |= 1u;
						}
					}
				}
			}
			fprintf(fp, "P4\n%d %d\n", width, height);
			if ((unsigned int)fwrite(img_uint8, sizeof(uint8_t), (size_t)(width_tmp * height), fp) != width_tmp * height) {
				ErrorFunctionName = "fwrite";
				ErrorValueName = "*img_uint8";
				goto ErrorFunctionFailed;
			}
			break;
		case PORTABLE_GRAYMAP_BINARY: // Portable Graymap Binary
			fprintf(fp, "P5\n%d %d\n%d\n", width, height, maxint);
			if (maxint > 0xFF) {
				// 16-bit data align in Big Endian order in 8-bit array
				try {
					img_uint8 = new uint8_t[2 * width * height]; // 2 times the size for 16-bit
				}
				catch (const std::bad_alloc &bad) {
					ErrorFunctionName = "new";
					ErrorValueName = "img_uint8";
					goto ErrorMalloc;
				}
				for (m = 0; m < width * height; m++) {
					img_uint8[2 * m] = (uint8_t)(((unsigned int)img[m] >> 8) & 0xFF);
					img_uint8[2 * m + 1] = (uint8_t)((unsigned int)img[m] & 0xFF);
				}
				if ((unsigned int)fwrite(img_uint8, sizeof(uint8_t), (size_t)(2 * width * height), fp) != 2 * width * height) {
					ErrorFunctionName = "fwrite";
					ErrorValueName = "img_uint8";
					goto ErrorFunctionFailed;
				}
			} else {
				// 8-bit data
				try {
					img_uint8 = new uint8_t[width * height];
				}
				catch (const std::bad_alloc &bad) {
					ErrorFunctionName = "new";
					ErrorValueName = "img_uint8";
					goto ErrorMalloc;
				}
				for (m = 0; m < width * height; m++) {
					img_uint8[m] = (uint8_t)((unsigned int)img[m] & 0xFF);
				}
				if ((unsigned int)fwrite(img_uint8, sizeof(uint8_t), (size_t)(width * height), fp) != width * height) {
					ErrorFunctionName = "fwrite";
					ErrorValueName = "img_uint8";
					goto ErrorFunctionFailed;
				}
			}
			break;
		case PORTABLE_PIXMAP_BINARY: // Portable Pixmap Binary
			fprintf(fp, "P6\n%d %d\n%d\n", width, height, maxint);
			if (maxint > 0xFF) {
				// 16-bit data align in Big Endian order in 8-bit array
				try {
					img_uint8 = uint8_t[2 * 3 * width * height]; // 2 times the size for 16-bit
				}
				catch (const std::bad_alloc &bad) {
					ErrorFunctionName = "new";
					ErrorValueName = "img_uint8";
					goto ErrorMalloc;
				}
				for (m = 0; m < width * height; m++) {
					img_uint8[6 * m + 1] = (uint8_t)(((unsigned int)img[m] >> 8) & 0xFF); /* Higher 8-bit */
					img_uint8[6 * m + 2] = (uint8_t)((unsigned int)img[m] & 0xFF); /* Lower 8-bit */
					img_uint8[6 * m + 3] = (uint8_t)(((unsigned int)img[width * height + m] >> 8) & 0xFF); /* Higher 8-bit */
					img_uint8[6 * m + 4] = (uint8_t)((unsigned int)img[width * height + m] & 0xFF); /* Lower 8-bit */
					img_uint8[6 * m + 5] = (uint8_t)(((unsigned int)img[2 * width * height + m] >> 8) & 0xFF); /* Higher 8-bit */
					img_uint8[6 * m + 6] = (uint8_t)((unsigned int)img[2 * width * height + m] & 0xFF); /* Lower 8-bit */
				}
				if ((unsigned int)fwrite(img_uint8, sizeof(uint8_t), (size_t)(2 * 3 * width * height), fp) != 2 * 3 * width * height) {
					ErrorFunctionName = "fwrite";
					ErrorValueName = "img_uint8";
					goto ErrorFunctionFailed;
				}
			} else {
				// 8-bit data
				try {
					img_uint8 = new uint8_t[3 * width * height];
				}
				catch (const std::bad_alloc &bad) {
					ErrorFunctionName = "new";
					ErrorValueName = "img_uint8";
					goto ErrorMalloc;
				}
				for (m = 0; m < width * height; m++) {
					img_uint8[3 * m] = (uint8_t)((unsigned int)img[m] & 0xFF);
					img_uint8[3 * m + 1] = (uint8_t)((unsigned int)img[width * height + m] & 0xFF);
					img_uint8[3 * m + 2] = (uint8_t)((unsigned int)img[2 * width * height + m] & 0xFF);
				}
				if ((unsigned int)fwrite(img_uint8, sizeof(uint8_t), (size_t)(3 * width * height), fp) != 3 * width * height) {
					ErrorFunctionName = "fwrite";
					ErrorValueName = "img_uint8";
					goto ErrorFunctionFailed;
				}
			}
			break;
		default: // ERROR
			fprintf(stderr, "*** %s() error - Descriptor is incorrect (%d) ***\n", FunctionName, desc);
			if (fclose(fp) == EOF) {
				fprintf(stderr, "*** %s() error - Failed to close the File by fclose() ***\n", FunctionName);
			}
			fp = nullptr;
			return PNM_FUNCTION_ERROR;
	}
	delete[] img_uint8;
	img_uint8 = nullptr;
	if (fclose(fp) == EOF) {
		fprintf(stderr, "*** %s() error - Failed to close the File by fclose() ***\n", FunctionName);
		return PNM_FUNCTION_ERROR;
	}
	printf("--- Successfully wrote the image ---\n\n");
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) by %s() ***\n", FunctionName, ErrorValueName.c_str(), ErrorFunctionName.c_str());
	goto ExitError;
ErrorFunctionFailed:
	fprintf(stderr, "*** %s() error - %s() failed to compute (%s) ***\n", FunctionName, ErrorFunctionName.c_str(), ErrorValueName.c_str());
	goto ExitError;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer to (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
ExitError:
	delete[] img_uint8;
	if (fclose(fp) == EOF) {
		fprintf(stderr, "*** %s() error - Failed to close the File by fclose() ***\n", FunctionName);
	}
	return PNM_FUNCTION_ERROR;
}

int
PNM::Gray2RGB(const PNM &from)
{
	const char *FunctionName = "PNM::Gray2RGB";
	std::string ErrorValueName;

	if (img != nullptr) {
		this.free();
	}
	if (from.img == nullptr) {
		ErrorValueName = "from.img";
		goto ErrorPointerNull;
	} else if (from.isRGB() != false) {
		fprintf(stderr, "*** %s() error - PNM Descriptor of is incorrect ***\n", FunctionName);
		goto ExitError;
	}

	width = from.width;
	height = from.height;
	maxint = from.maxint;
	try {
		img = new pnm_img[3 * width * height];
	}
	catch (const std::bad_alloc &bad) {
		ErrorValueName = "img";
		goto ErrorMalloc;
	}
	if (from.desc <= PNM_DESCRIPTOR_ASCII_MAX) {
		desc = PORTABLE_PIXMAP_ASCII;
	} else {
		desc = PORTABLE_PIXMAP_BINARY;
	}
	for (int i = 0; i < width * height; i++) {
		img[i] = from.img[i];
		img[width * height + i] = from.img[i];
		img[2u * width * height + i] = from.img[i];
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

