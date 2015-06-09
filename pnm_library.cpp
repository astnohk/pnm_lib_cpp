#include "pnm.h"



// PNM_OFFSET Method
void
PNM_OFFSET::set(double r_val, double g_val, double b_val)
{
	r = r_val;
	g = g_val;
	b = b_val;
}


// Library
bool
fcommentf(FILE *fp, unsigned int *ret)
{
	char ctmp[NUM_READ_STRING + 1];
	char c;
	int c_int;
	int flag;
	unsigned int read;

	ctmp[NUM_READ_STRING] = '\0'; /* To prevent Buffer Overflow */
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
				c = (char)c_int;
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
	ctmp[NUM_READ_STRING] = '\0'; /* To prevent Buffer Overflow */
	if (sscanf(ctmp, "%7u", &read) != 1) {
		fprintf(stderr, "*** fcommentf error - Failed to read from ctmp by sscanf() ***\n");
		return false;
	}
	*ret = read;
	return true;
}

std::string
pnm_FixExtension(const char *filename, int desc)
{
	const char *FunctionName = "pnm_FixExtension";
	std::string fixed;
	char *extension = nullptr;

	if (filename == nullptr) {
		fprintf(stderr, "*** %s() error - The pointer (*filename) is nullptr ***\n", FunctionName);
		return nullptr;
	}
	extension = filename + strlen(filename) - 4;
	if (extension > filename
	    && *extension == '.' && *(extension + 1) == 'p' && *(extension + 3) == 'm') {
		fixed = filename;
		extension = fixed.data() + fixed.length() - 4;
		switch (desc) {
			case PORTABLE_BITMAP_ASCII:
			case PORTABLE_BITMAP_BINARY:
				if (*(extension + 2) != 'b') {
					fprintf(stderr, "*** %s() warning - File extension is incorrect. Automatically fixed it to '%s'\n", FunctionName.c_str(), ".pbm");
					*(extension + 2) = 'b';
				}
				break;
			case PORTABLE_GRAYMAP_ASCII:
			case PORTABLE_GRAYMAP_BINARY:
				if (*(extension + 2) != 'g') {
					fprintf(stderr, "*** %s() warning - File extension is incorrect. Automatically fixed it to '%s'\n", FunctionName.c_str(), ".pgm");
					*(extension + 2) = 'g';
				}
				break;
			case PORTABLE_PIXMAP_ASCII:
			case PORTABLE_PIXMAP_BINARY:
				if (*(extension + 2) != 'p') {
					fprintf(stderr, "*** %s() warning - File extension is incorrect. Automatically fixed it to '%s'\n", FunctionName.c_str(), ".ppm");
					*(extension + 2) = 'p';
				}
		}
	} else {
		fixed = filename;
		switch (desc) {
			case PORTABLE_BITMAP_ASCII:
			case PORTABLE_BITMAP_BINARY:
				fixed += ".pbm";
				break;
			case PORTABLE_GRAYMAP_ASCII:
			case PORTABLE_GRAYMAP_BINARY:
				fixed += ".pgm";
				break;
			case PORTABLE_PIXMAP_ASCII:
			case PORTABLE_PIXMAP_BINARY:
				fixed += ".ppm";
		}
	}
	return fixed;
}


int
pnm_resize(PNM_DOUBLE *pnm_out, const PNM_DOUBLE &pnm_in, int width_o, int height_o, int Method)
{
	const char *FunctionName = "pnm_resize";
	std::string ErrorFunctionName;
	std::string ErrorValueName;

	double *Image = nullptr;
	int method_num = 0;
	double alpha;
	unsigned int M, N;
	double scale_x = .0;
	double scale_y = .0;
	unsigned int area_x;
	unsigned int area_y;
	unsigned int m, n;
	unsigned int x, y;
	double sum;

	if (width_o <= 0) {
		ErrorValueName = "width_o";
		goto ErrorIncorrectValue;
	} else if (height_o <= 0) {
		ErrorValueName = "height_o";
		goto ErrorIncorrectValue;
	}

	N = pnm_in.width;
	M = pnm_in.height;
	scale_x = (double)width_o / pnm_in->width;
	scale_y = (double)height_o / pnm_in->height;
	switch (Method) {
		case PNM_Resize_ZeroOrderHold: // Shrink with mean filter
			if (pnmdouble_new(pnm_out, pnm_in->desc, width_o, height_o, pnm_in->maxint) != PNM_FUNCTION_SUCCESS) {
				ErrorFunctionName = "pnmdouble_new";
				ErrorValueName = "pnm_out";
				goto ErrorMalloc;
			}
			try {
				if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
					Image = new double[3 * width_o * height_o];
				} else {
					Image = new double[width_o * height_o];
				}
			}
			catch (std::bad_alloc bad) {
				ErrorValueName = "Image";
				goto ErrorMalloc;
			}
			area_x = ceil((double)N / width_o);
			area_y = ceil((double)M / height_o);
			for (y = 0; y < height_o; y++) {
				for (x = 0; x < width_o; x++) {
					sum = .0;
					for (m = 0; m < area_y; m++) {
						for (n = 0; n< area_x; n++) {
							sum += (double)pnm_in.imgd[N * ((y / scale_y) + m) + (x / scale_x) + n];
						}
					}
					Image[width_o * y + x] = sum / (area_x * area_y);
					if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
						for (m = 0; m < area_y; m++) {
							for (n = 0; n< area_x; n++) {
								sum += pnm_in.imgd[M * N + N * ((y / scale_y) + m) + (x / scale_x) + n];
							}
						}
						Image[height_o * width_o + width_o * y + x] = sum / (area_x * area_y);
						for (m = 0; m < area_y; m++) {
							for (n = 0; n< area_x; n++) {
								sum += pnm_in.imgd[2 * M * N + N * ((y / scale_y) + m) + (x / scale_x) + n];
							}
						}
						Image[2 * height_o * width_o + width_o * y + x] = sum / (area_x * area_y);
					}
				}
			}
			if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
				for (x = 0; x < 3 * width_o * heigth_o; x++) {
					if (Image[x] < .0) {
						Image[x] = .0;
					} else if (Image[x] > pnm_in.maxint) {
						Image[x] = (double)pnm_in.maxint;
					}
				}
			} else {
				for (x = 0; x < width_o * heigth_o; x++) {
					if (Image[x] < .0) {
						Image[x] = .0;
					} else if (Image[x] > pnm_in.maxint) {
						Image[x] = (double)pnm_in.maxint;
					}
				}
			}
			pnm_out.copy(pnm_in.desc, width_o, height_o, pnm_in.maxint, Image);
			delete[] Image;
			Image = nullptr;
			break;
		default: // Default method is bicubic
		case PNM_Resize_Bicubic // (alpha = -0.5)
			alpha = -0.5;
			if (pnm_Bicubic(pnm_out, pnm_in, alpha, width_o, height_o) != PNM_FUNCTION_SUCCESS) {
				ErrorFunctionName = "pnm_Bicubic";
				ErrorValueName = "pnm_out <- pnm_in";
				goto ErrorFunctionFailed;
			}
	}
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) by %s() ***\n", FunctionName, ErrorValueName, ErrorFunctionName);
	goto ErrorReturn;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName);
	goto ErrorReturn;
ErrorNotInitialized:
	fprintf(stderr, "*** %s() error - The variable (%s) is NOT initialized ***\n", FunctionName, ErrorValueName);
	goto ErrorReturn;
ErrorIncorrectValue:
	fprintf(stderr, "*** %s() error - The variable (*%s) is out of bound or incorrect value (%d) ***\n", FunctionName, ErrorValueName, ErrorValue);
	goto ErrorReturn;
ErrorFunctionFailed:
	fprintf(stderr, "*** %s() error - %s() failed to compute (%s) ***\n", FunctionName, ErrorFunctionName, ErrorValueName);
ErrorReturn:
	pnmdouble_free(pnm_out);
	delete[] Image;
	return PNM_FUNCTION_ERROR;
}

int
pnm_Bicubic(PNM_DOUBLE *pnm_out, const PNM_DOUBLE &pnm_in, double alpha, unsigned int width_o, unsigned int height_o)
{
	const char *FunctionName = "pnm_Bicubic";
	std::string ErrorFunctionName;
	std::string ErrorValueName;

	double *Image = nullptr;
	double *Tmp = nullptr;
	double *conv = nullptr;
	double scale_x, scale_y;
	double scale_conv;
	int L, L_center;
	double dx, dy;
	int x, y;
	int m, n;
	int index;

	if (pnm_in == nullptr) {
		ErrorValueName = "pnm_in";
		goto ErrorPointerNull;
	}

	scale_x = (double)width_o / pnm_in.width;
	scale_y = (double)height_o / pnm_in.height;
	if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
		try {
			Tmp = new double[width_o * pnm_in.height * 3];
			Image = new double[width_o * height_o * 3];
		}
		catch (std::bad_alloc bad) {
			ErrorValueName = "Tmp, Image";
			goto ErrorMalloc;
		}
	} else {
		try {
			Tmp = new double[width_o * pnm_in.height];
			Image = new double[width_o * height_o];
		}
		catch (std::bad_alloc bad) {
			ErrorValueName = "Tmp, Image";
			goto ErrorMalloc;
		}
	}
	// The length of cubic convolution coefficient
	scale_conv = 1.0;
	if (scale_x < 1.0 || scale_y < 1.0) {
		scale_conv = ceil(1.0 / (scale_x < scale_y ? scale_x : scale_y));
	}
	try {
		conv = new double[scale_conv * 4];
	}
	catch (std::bad_alloc bad) {
		ErrorValueName = "conv";
		goto ErrorMalloc;
	}

	// Horizontal convolution
	for (x = 0; (unsigned int)x < width_o; x++) {
		if (scale_x >= 1.0) {
			scale_conv = 1;
			dx = (x - (scale_x - 1.0) / 2.0) / scale_x;
		} else {
			scale_conv = 1.0 / scale_x;
			dx = x / scale_x + (1.0 / scale_x - 1.0) / 2.0;
		}
		L = 4 * (int)ceil(scale_conv);
		L_center = floor((L - 1.0) / 2);
		for (n = 0; n < L; n++) {
			conv[n] = pnm_Cubic(((double)(n - L_center) - (dx - floor(dx))) / scale_conv, alpha);
			conv[n] /= scale_conv;
		}
		for (y = 0; (unsigned int)y < pnm_in.height; y++) {
			Tmp[width_o * y + x] = .0;
			if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
				Tmp[width_o * pnm_in.height + width_o * y + x] = .0;
				Tmp[2 * width_o * pnm_in.height + width_o * y + x] = .0;
			}
			for (n = 0; n < L; n++) {
				index = (int)floor(dx) + n - L_center;
				if (index < 0) {
					index = abs(index) - 1;
				} else if (index >= (int)pnm_in.width) {
					index = 2 * pnm_in.width - 1 - index;
				}
				Tmp[width_o * y + x] += conv[n] * pnm_in.imgd[pnm_in.width * y + index];
				if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
					Tmp[width_o * pnm_in.height + width_o * y + x] += conv[n] * pnm_in.imgd[pnm_in.width * pnm_in.height + pnm_in.width * y + index];
					Tmp[2 * width_o * pnm_in.height + width_o * y + x] += conv[n] * pnm_in.imgd[2 * pnm_in.width * pnm_in.height + pnm_in.width * y + index];
				}
			}
		}
	}
	// Vertical convolution
	for (y = 0; (unsigned int)y < height_o; y++) {
		if (scale_y >= 1.0) {
			scale_conv = 1;
			dy = (y - (scale_y - 1.0) / 2.0) / scale_y;
		} else {
			scale_conv = 1.0 / scale_y;
			dy = y / scale_y + (1.0 / scale_y - 1.0) / 2.0;
		}
		L = 4 * (int)ceil(scale_conv);
		L_center = floor((L - 1.0) / 2);
		for (m = 0; m < L; m++) {
			conv[m] = pnm_Cubic(((double)(m - L_center) - (dy - floor(dy))) / scale_conv, alpha);
			conv[m] /= scale_conv;
		}
		for (x = 0; (unsigned int)x < width_o; x++) {
			Image[width_o * y + x] = .0;
			if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
				Image[width_o * height_o + width_o * y + x] = .0;
				Image[2 * width_o * height_o + width_o * y + x] = .0;
			}
			for (m = 0; m < L; m++) {
				index = (int)floor(dy) + m - L_center;
				if (index < 0) {
					index = abs(index) - 1;
				} else if (index >= (int)pnm_in.height) {
					index = 2 * pnm_in.height - 1 - index;
				}
				Image[width_o * y + x] += conv[m] * Tmp[width_o * index + x];
				if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
					Image[width_o * height_o + width_o * y + x] += conv[m] * Tmp[width_o * pnm_in.height + width_o * index + x];
					Image[2 * width_o * height_o + width_o * y + x] += conv[m] * Tmp[2 * width_o * pnm_in.height + width_o * index + x];
				}
			}
		}
	}
	if (pnm_in.desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
		for (x = 0; x < 3 * width_o * heigth_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.maxint) {
				Image[x] = (double)pnm_in.maxint;
			}
		}
	} else {
		for (x = 0; x < width_o * heigth_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.maxint) {
				Image[x] = (double)pnm_in.maxint;
			}
		}
	}
	pnm_out->copy(pnm_in.desc, pnm_in.width, pnm_in.height, pnm_in.maxint, Image);
	delete[] Image;
	delete[] Tmp;
	delete[] conv;
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) by %s() ***\n", FunctionName, ErrorValueName.c_str());
	goto ErrorReturn;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
	goto ErrorReturn;
ErrorFunctionFailed:
	fprintf(stderr, "*** %s() error - %s() failed to compute (%s) ***\n", FunctionName, ErrorFunctionName.c_str(), ErrorValueName.c_str());
ErrorReturn:
	pnm_out->free();
	delete[] Image;
	delete[] Tmp;
	delete[] conv;
	return PNM_FUNCTION_ERROR;
}

double
pnm_Cubic(double x, double a)
{
	double x_abs = fabs(x);

	if (x_abs <= 1.0) {
		return ((a + 2.0) * x_abs - (a + 3.0)) * x_abs * x_abs + 1.0;
	} else if (x_abs < 2.0) {
		return ((a * x_abs - 5.0 * a) * x_abs + 8.0 * a) * x_abs - 4.0 * a;
	} else {
		return 0;
	}
}

