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


void
pnm_resize(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const int width_o, const int height_o, const int Method)
{
	const char *FunctionName = "pnm_resize";
	std::string ErrorFunctionName;
	std::string ErrorValueName;

	if (width_o <= 0) {
		ErrorValueName = "width_o";
		fprintf(stderr, "pnm_resize(PNM_DOUBLE*, const PNM_DOUBLE&, const int, const int, const int) : %s", "const int width_o");
		throw std::out_of_range("const int width_o");
	} else if (height_o <= 0) {
		ErrorValueName = "height_o";
		fprintf(stderr, "pnm_resize(PNM_DOUBLE*, const PNM_DOUBLE&, const int, const int, const int) : %s", "const int height_o");
		throw std::out_of_range("const int height_o");
	}
	switch (Method) {
		case PNM_Resize_ZeroOrderHold: // Shrink with mean filter
			try {
				pnm_ZeroOrderHold(pnm_out, pnm_in, width_o, height_o);
			}
			catch (const std::bad_alloc& bad) {
				std::cerr << bad.what() << std::endl;
				ErrorFunctionName = "pnm_Bicubic";
				ErrorValueName = "pnm_out <- pnm_in";
				fprintf(stderr, "*** %s() error - %s() failed to compute (%s) ***\n", FunctionName, ErrorFunctionName.c_str(), ErrorValueName.c_str());
				throw;
			}
			break;
		default: // Default method is bicubic
		case PNM_Resize_Bicubic: // (alpha = -0.5)
			double alpha = -0.5;
			try {
				pnm_Bicubic(pnm_out, pnm_in, alpha, width_o, height_o);
			}
			catch (const std::bad_alloc& bad) {
				std::cerr << bad.what() << std::endl;
				ErrorFunctionName = "pnm_Bicubic";
				ErrorValueName = "pnm_out <- pnm_in";
				fprintf(stderr, "*** %s() error - %s() failed to compute (%s) ***\n", FunctionName, ErrorFunctionName.c_str(), ErrorValueName.c_str());
				throw;
			}
			catch (const std::invalid_argument& arg) {
				std::cerr << arg.what() << std::endl;
				ErrorFunctionName = "pnm_Bicubic";
				ErrorValueName = "pnm_out <- pnm_in";
				fprintf(stderr, "*** %s() error - %s() failed to compute (%s) ***\n", FunctionName, ErrorFunctionName.c_str(), ErrorValueName.c_str());
				throw;
			}
	}
}

void
pnm_ZeroOrderHold(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const int width_o, const int height_o)
{
	const int width_i = pnm_in.Width();
	const int height_i = pnm_in.Height();
	double* Image = nullptr;

	try {
		if (pnm_in.isRGB()) {
			Image = new double[3 * width_o * height_o];
		} else {
			Image = new double[width_o * height_o];
		}
	}
	catch (const std::bad_alloc &bad) {
		std::cerr << bad.what() << std::endl;
		throw;
	}
	int area_x = int(ceil(double(width_i) / double(width_o)));
	int area_y = int(ceil(double(height_i) / double(height_o)));
	double scale_x = double(width_o) / double(width_i);
	double scale_y = double(height_o) / double(height_i);
	pnm_img_double* imgd_data = pnm_in.Data();
	for (int y = 0; y < height_o; y++) {
		for (int x = 0; x < width_o; x++) {
			{
				double sum = .0;
				for (int m = 0; m < area_y; m++) {
					for (int n = 0; n < area_x; n++) {
						sum += imgd_data[width_i * (int(floor(y / scale_y)) + m) + int(floor(x / scale_x)) + n];
					}
				}
				Image[width_o * y + x] = sum / (area_x * area_y);
			}
			if (pnm_in.isRGB()) {
				{
					double sum = .0;
					for (int m = 0; m < area_y; m++) {
						for (int n = 0; n < area_x; n++) {
							sum += imgd_data[width_i * height_i + width_i * (int(floor(y / scale_y)) + m) + int(floor(x / scale_x)) + n];
						}
					}
					Image[height_o * width_o + width_o * y + x] = sum / (area_x * area_y);
				}
				{
					double sum = .0;
					for (int m = 0; m < area_y; m++) {
						for (int n = 0; n < area_x; n++) {
							sum += imgd_data[2 * width_i * height_i + width_i * (int(floor(y / scale_y)) + m) + int(floor(x / scale_x)) + n];
						}
					}
					Image[2 * height_o * width_o + width_o * y + x] = sum / (area_x * area_y);
				}
			}
		}
	}
	imgd_data = nullptr;
	if (pnm_in.isRGB()) {
		for (int x = 0; x < 3 * width_o * height_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.MaxInt()) {
				Image[x] = double(pnm_in.MaxInt());
			}
		}
	} else {
		for (int x = 0; x < width_o * height_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.MaxInt()) {
				Image[x] = double(pnm_in.MaxInt());
			}
		}
	}
	pnm_out->copy(pnm_in.Desc(), width_o, height_o, pnm_in.MaxInt(), Image);
	delete[] Image;
}

void
pnm_Bicubic(PNM_DOUBLE* pnm_out, const PNM_DOUBLE& pnm_in, const double alpha, const int width_o, const int height_o)
{
	double *Image = nullptr;
	double *Tmp = nullptr;
	double *conv = nullptr;

	if (pnm_out == nullptr) {
		throw std::invalid_argument("PNM_DOUBLE* pnm_out");
	}
	int width_i = pnm_in.Width();
	int height_i = pnm_in.Height();
	double scale_x = double(width_o) / double(width_i);
	double scale_y = double(height_o) / double(height_i);
	try {
		Tmp = new double[width_o * height_i * (pnm_in.isRGB() ? 3 : 1)];
		Image = new double[width_o * height_o * (pnm_in.isRGB() ? 3 : 1)];
	}
	catch (const std::bad_alloc &bad) {
		std::cerr << bad.what() << std::endl;
		throw;
	}
	// The length of cubic convolution coefficient
	double scale_conv = 1.0;
	if (scale_x < 1.0 || scale_y < 1.0) {
		scale_conv = ceil(1.0 / (scale_x < scale_y ? scale_x : scale_y));
	}
	try {
		conv = new double[int(scale_conv) * 4];
	}
	catch (const std::bad_alloc &bad) {
		std::cerr << bad.what() << std::endl;
		delete[] Tmp;
		delete[] Image;
		throw;
	}

	// Horizontal convolution
	pnm_img_double* imgd_data = pnm_in.Data();
	for (int x = 0; x < width_o; x++) {
		double dx;
		if (scale_x >= 1.0) {
			scale_conv = 1.0;
			dx = (x - (scale_x - 1.0) / 2.0) / scale_x;
		} else {
			scale_conv = 1.0 / scale_x;
			dx = x / scale_x + (1.0 / scale_x - 1.0) / 2.0;
		}
		int L = 4 * int(ceil(scale_conv));
		int L_center = int(floor((L - 1.0) / 2));
		for (int n = 0; n < L; n++) {
			conv[n] = pnm_Cubic((double(n - L_center) - (dx - floor(dx))) / scale_conv, alpha);
			conv[n] /= scale_conv;
		}
		for (int y = 0; y < height_i; y++) {
			Tmp[width_o * y + x] = .0;
			if (pnm_in.isRGB()) {
				Tmp[width_o * height_i + width_o * y + x] = .0;
				Tmp[2 * width_o * height_i + width_o * y + x] = .0;
			}
			for (int n = 0; n < L; n++) {
				int index = int(floor(dx)) + n - L_center;
				if (index < 0) {
					index = abs(index) - 1;
				} else if (index >= width_i) {
					index = 2 * width_i - 1 - index;
				}
				Tmp[width_o * y + x] += conv[n] * imgd_data[width_i * y + index];
				if (pnm_in.isRGB()) {
					Tmp[width_o * height_i + width_o * y + x]
					    += conv[n] * imgd_data[width_i * height_i + width_i * y + index];
					Tmp[2 * width_o * height_i + width_o * y + x]
					    += conv[n] * imgd_data[2 * width_i * height_i + width_i * y + index];
				}
			}
		}
	}
	imgd_data = nullptr;
	// Vertical convolution
	for (int y = 0; y < height_o; y++) {
		double dy;
		if (scale_y >= 1.0) {
			scale_conv = 1.0;
			dy = (y - (scale_y - 1.0) / 2.0) / scale_y;
		} else {
			scale_conv = 1.0 / scale_y;
			dy = y / scale_y + (1.0 / scale_y - 1.0) / 2.0;
		}
		int L = 4 * int(ceil(scale_conv));
		int L_center = int(floor((L - 1.0) / 2));
		for (int m = 0; m < L; m++) {
			conv[m] = pnm_Cubic((double(m - L_center) - (dy - floor(dy))) / scale_conv, alpha);
			conv[m] /= scale_conv;
		}
		for (int x = 0; x < width_o; x++) {
			Image[width_o * y + x] = .0;
			if (pnm_in.isRGB()) {
				Image[width_o * height_o + width_o * y + x] = .0;
				Image[2 * width_o * height_o + width_o * y + x] = .0;
			}
			for (int m = 0; m < L; m++) {
				int index = int(floor(dy)) + m - L_center;
				if (index < 0) {
					index = abs(index) - 1;
				} else if (index >= height_i) {
					index = 2 * height_i - 1 - index;
				}
				Image[width_o * y + x] += conv[m] * Tmp[width_o * index + x];
				if (pnm_in.isRGB()) {
					Image[width_o * height_o + width_o * y + x]
					    += conv[m] * Tmp[width_o * height_i + width_o * index + x];
					Image[2 * width_o * height_o + width_o * y + x]
					    += conv[m] * Tmp[2 * width_o * height_i + width_o * index + x];
				}
			}
		}
	}
	if (pnm_in.isRGB()) {
		for (int x = 0; x < 3 * width_o * height_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.MaxInt()) {
				Image[x] = double(pnm_in.MaxInt());
			}
		}
	} else {
		for (int x = 0; x < width_o * height_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.MaxInt()) {
				Image[x] = double(pnm_in.MaxInt());
			}
		}
	}
	pnm_out->copy(pnm_in.Desc(), width_i, height_i, pnm_in.MaxInt(), Image);
	delete[] Image;
	delete[] Tmp;
	delete[] conv;
}

double
pnm_Cubic(const double x, const double a)
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

