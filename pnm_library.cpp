#include "pnm.h"



// Library
bool
fcommentf(FILE *fp, int *ret)
{
	char ctmp[NUM_READ_STRING + 1];
	char c;
	int c_int;
	int flag;
	int read;

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
	if (sscanf(ctmp, "%7d", &read) != 1) {
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
	const char *extension = nullptr;

	if (filename == nullptr) {
		fprintf(stderr, "*** %s() error - The pointer (*filename) is nullptr ***\n", FunctionName);
		return nullptr;
	}
	fixed = filename;
	extension = fixed.data() + strlen(filename) - 4;
	if (strlen(filename) > 4
	    && *extension == '.' && *(extension + 1) == 'p' && *(extension + 3) == 'm') {
		extension = fixed.data() + fixed.length() - 4;
		switch (desc) {
			case PORTABLE_BITMAP_ASCII:
			case PORTABLE_BITMAP_BINARY:
				if (*(extension + 2) != 'b') {
					fprintf(stderr, "*** %s() warning - File extension is incorrect. Automatically fixed it to '%s'\n", FunctionName, ".pbm");
					fixed.replace(fixed.length() - 2, 1, "b");
				}
				break;
			case PORTABLE_GRAYMAP_ASCII:
			case PORTABLE_GRAYMAP_BINARY:
				if (*(extension + 2) != 'g') {
					fprintf(stderr, "*** %s() warning - File extension is incorrect. Automatically fixed it to '%s'\n", FunctionName, ".pgm");
					fixed.replace(fixed.length() - 2, 1, "g");
				}
				break;
			case PORTABLE_PIXMAP_ASCII:
			case PORTABLE_PIXMAP_BINARY:
				if (*(extension + 2) != 'p') {
					fprintf(stderr, "*** %s() warning - File extension is incorrect. Automatically fixed it to '%s'\n", FunctionName, ".ppm");
					fixed.replace(fixed.length() - 2, 1, "p");
				}
		}
	} else {
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

	pnm_img_double *imgd_data = nullptr;
	double *Image = nullptr;
	double alpha;
	int desc;
	int width_i, height_i;
	int MaxInt;
	double scale_x = .0;
	double scale_y = .0;
	int area_x;
	int area_y;
	int m, n;
	int x, y;
	double sum;

	if (width_o <= 0) {
		ErrorValueName = "width_o";
		goto ErrorIncorrectValue;
	} else if (height_o <= 0) {
		ErrorValueName = "height_o";
		goto ErrorIncorrectValue;
	}
	desc = pnm_in.Desc();
	width_i = pnm_in.Width();
	height_i = pnm_in.Height();
	MaxInt = pnm_in.MaxInt();
	scale_x = (double)width_o / width_i;
	scale_y = (double)height_o / height_i;
	switch (Method) {
		case PNM_Resize_ZeroOrderHold: // Shrink with mean filter
			try {
				if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
					Image = new double[3 * width_o * height_o];
				} else {
					Image = new double[width_o * height_o];
				}
			}
			catch (const std::bad_alloc &bad) {
				ErrorValueName = "Image";
				goto ErrorMalloc;
			}
			area_x = ceil((double)width_i / width_o);
			area_y = ceil((double)height_i / height_o);
			imgd_data = pnm_in.Data();
			for (y = 0; y < height_o; y++) {
				for (x = 0; x < width_o; x++) {
					sum = .0;
					for (m = 0; m < area_y; m++) {
						for (n = 0; n< area_x; n++) {
							sum += imgd_data[width_i * ((int)floor(y / scale_y) + m) + (int)floor(x / scale_x) + n];
						}
					}
					Image[width_o * y + x] = sum / (area_x * area_y);
					if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
						for (m = 0; m < area_y; m++) {
							for (n = 0; n < area_x; n++) {
								sum += imgd_data[width_i * height_i + width_i * ((int)floor(y / scale_y) + m) + (int)floor(x / scale_x) + n];
							}
						}
						Image[height_o * width_o + width_o * y + x] = sum / (area_x * area_y);
						for (m = 0; m < area_y; m++) {
							for (n = 0; n < area_x; n++) {
								sum += imgd_data[2 * width_i * height_i + width_i * ((int)floor(y / scale_y) + m) + (int)floor(x / scale_x) + n];
							}
						}
						Image[2 * height_o * width_o + width_o * y + x] = sum / (area_x * area_y);
					}
				}
			}
			imgd_data = nullptr;
			if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
				for (x = 0; x < 3 * width_o * height_o; x++) {
					if (Image[x] < .0) {
						Image[x] = .0;
					} else if (Image[x] > MaxInt) {
						Image[x] = (double)MaxInt;
					}
				}
			} else {
				for (x = 0; x < width_o * height_o; x++) {
					if (Image[x] < .0) {
						Image[x] = .0;
					} else if (Image[x] > MaxInt) {
						Image[x] = (double)MaxInt;
					}
				}
			}
			pnm_out->copy(desc, width_o, height_o, MaxInt, Image);
			delete[] Image;
			Image = nullptr;
			break;
		default: // Default method is bicubic
		case PNM_Resize_Bicubic: // (alpha = -0.5)
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
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) by %s() ***\n", FunctionName, ErrorValueName.c_str(), ErrorFunctionName.c_str());
	goto ErrorReturn;
ErrorIncorrectValue:
	fprintf(stderr, "*** %s() error - The variable (*%s) is out of bound or incorrect value ***\n", FunctionName, ErrorValueName.c_str());
	goto ErrorReturn;
ErrorFunctionFailed:
	fprintf(stderr, "*** %s() error - %s() failed to compute (%s) ***\n", FunctionName, ErrorFunctionName.c_str(), ErrorValueName.c_str());
ErrorReturn:
	pnm_out->free();
	delete[] Image;
	return PNM_FUNCTION_ERROR;
}

int
pnm_Bicubic(PNM_DOUBLE *pnm_out, const PNM_DOUBLE &pnm_in, double alpha, int width_o, int height_o)
{
	const char *FunctionName = "pnm_Bicubic";
	std::string ErrorFunctionName;
	std::string ErrorValueName;

	pnm_img_double *imgd_data = nullptr;
	double *Image = nullptr;
	double *Tmp = nullptr;
	double *conv = nullptr;
	int desc;
	int width_i, height_i;
	int MaxInt;
	double scale_x, scale_y;
	double scale_conv;
	int L, L_center;
	double dx, dy;
	int x, y;
	int m, n;
	int index;

	if (pnm_out == nullptr) {
		ErrorValueName = "pnm_out";
		goto ErrorPointerNull;
	}
	desc = pnm_in.Desc();
	width_i = pnm_in.Width();
	height_i = pnm_in.Height();
	MaxInt = pnm_in.MaxInt();
	scale_x = (double)width_o / width_i;
	scale_y = (double)height_o / height_i;
	if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
		try {
			Tmp = new double[width_o * height_i * 3];
			Image = new double[width_o * height_o * 3];
		}
		catch (const std::bad_alloc &bad) {
			ErrorValueName = "Tmp, Image";
			goto ErrorMalloc;
		}
	} else {
		try {
			Tmp = new double[width_o * height_i];
			Image = new double[width_o * height_o];
		}
		catch (const std::bad_alloc &bad) {
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
		conv = new double[(int)scale_conv * 4];
	}
	catch (const std::bad_alloc &bad) {
		ErrorValueName = "conv";
		goto ErrorMalloc;
	}

	// Horizontal convolution
	imgd_data = pnm_in.Data();
	for (x = 0; x < width_o; x++) {
		if (scale_x >= 1.0) {
			scale_conv = 1.0;
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
		for (y = 0; y < height_i; y++) {
			Tmp[width_o * y + x] = .0;
			if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
				Tmp[width_o * height_i + width_o * y + x] = .0;
				Tmp[2 * width_o * height_i + width_o * y + x] = .0;
			}
			for (n = 0; n < L; n++) {
				index = (int)floor(dx) + n - L_center;
				if (index < 0) {
					index = abs(index) - 1;
				} else if (index >= width_i) {
					index = 2 * width_i - 1 - index;
				}
				Tmp[width_o * y + x] += conv[n] * imgd_data[width_i * y + index];
				if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
					Tmp[width_o * height_i + width_o * y + x] += conv[n] * imgd_data[width_i * height_i + width_i * y + index];
					Tmp[2 * width_o * height_i + width_o * y + x] += conv[n] * imgd_data[2 * width_i * height_i + width_i * y + index];
				}
			}
		}
	}
	imgd_data = nullptr;
	// Vertical convolution
	for (y = 0; y < height_o; y++) {
		if (scale_y >= 1.0) {
			scale_conv = 1.0;
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
		for (x = 0; x < width_o; x++) {
			Image[width_o * y + x] = .0;
			if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
				Image[width_o * height_o + width_o * y + x] = .0;
				Image[2 * width_o * height_o + width_o * y + x] = .0;
			}
			for (m = 0; m < L; m++) {
				index = (int)floor(dy) + m - L_center;
				if (index < 0) {
					index = abs(index) - 1;
				} else if (index >= height_i) {
					index = 2 * height_i - 1 - index;
				}
				Image[width_o * y + x] += conv[m] * Tmp[width_o * index + x];
				if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
					Image[width_o * height_o + width_o * y + x] += conv[m] * Tmp[width_o * height_i + width_o * index + x];
					Image[2 * width_o * height_o + width_o * y + x] += conv[m] * Tmp[2 * width_o * height_i + width_o * index + x];
				}
			}
		}
	}
	if (desc % PNM_DESCRIPTOR_PIXMAPS == 0) {
		for (x = 0; x < 3 * width_o * height_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > MaxInt) {
				Image[x] = (double)MaxInt;
			}
		}
	} else {
		for (x = 0; x < width_o * height_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > MaxInt) {
				Image[x] = (double)MaxInt;
			}
		}
	}
	pnm_out->copy(desc, width_i, height_i, MaxInt, Image);
	delete[] Image;
	delete[] Tmp;
	delete[] conv;
	return PNM_FUNCTION_SUCCESS;
// Error
ErrorMalloc:
	fprintf(stderr, "*** %s() error - Cannot allocate memory for (*%s) ***\n", FunctionName, ErrorValueName.c_str());
	goto ErrorReturn;
ErrorPointerNull:
	fprintf(stderr, "*** %s() error - The pointer (*%s) is nullptr ***\n", FunctionName, ErrorValueName.c_str());
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

