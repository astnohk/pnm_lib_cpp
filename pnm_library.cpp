#include "pnm.h"



// Library
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
			const double alpha = -0.5;
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
	const size_t size_o = size_t(width_o) * size_t(height_o);
	const size_t width_i = static_cast<size_t>(pnm_in.Width());
	const size_t height_i = static_cast<size_t>(pnm_in.Height());
	const size_t size_i = width_i * height_i;
	double* Image = nullptr;

	try {
		if (pnm_in.isRGB()) {
			Image = new double[3 * size_o];
		} else {
			Image = new double[size_o];
		}
	}
	catch (const std::bad_alloc& bad) {
		std::cerr << bad.what() << std::endl
		    << "void pnm_ZeroOrderHold(PNM_DOUBLE*, const PNM_DOUBLE&, const int, const int)" << std::endl;
		throw;
	}
	int area_x = int(ceil(double(width_i) / double(width_o)));
	int area_y = int(ceil(double(height_i) / double(height_o)));
	double scale_x = double(width_o) / double(width_i);
	double scale_y = double(height_o) / double(height_i);
	pnm_img_double* imgd_data = pnm_in.Data();
	for (size_t y = 0; y < size_t(height_o); y++) {
		for (size_t x = 0; x < size_t(width_o); x++) {
			{
				double sum = .0;
				for (size_t m = 0; m < size_t(area_y); m++) {
					for (size_t n = 0; n < size_t(area_x); n++) {
						sum += imgd_data[width_i * (size_t(floor(y / scale_y)) + m) + size_t(floor(x / scale_x)) + n];
					}
				}
				Image[size_t(width_o) * y + x] = sum / (area_x * area_y);
			}
			if (pnm_in.isRGB()) {
				{
					double sum = .0;
					for (size_t m = 0; m < size_t(area_y); m++) {
						for (size_t n = 0; n < size_t(area_x); n++) {
							sum += imgd_data[size_i + width_i * (size_t(floor(y / scale_y)) + m) + size_t(floor(x / scale_x)) + n];
						}
					}
					Image[size_o + size_t(width_o) * y + x] = sum / (area_x * area_y);
				}
				{
					double sum = .0;
					for (size_t m = 0; m < size_t(area_y); m++) {
						for (size_t n = 0; n < size_t(area_x); n++) {
							sum += imgd_data[2 * size_i + width_i * (size_t(floor(y / scale_y)) + m) + size_t(floor(x / scale_x)) + n];
						}
					}
					Image[2 * size_o + size_t(width_o) * y + x] = sum / (area_x * area_y);
				}
			}
		}
	}
	imgd_data = nullptr;
	if (pnm_in.isRGB()) {
		for (size_t x = 0; x < 3 * size_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.MaxInt()) {
				Image[x] = double(pnm_in.MaxInt());
			}
		}
	} else {
		for (size_t x = 0; x < size_o; x++) {
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
	const size_t size_o = size_t(width_o) * size_t(height_o);
	const size_t size_i = size_t(width_i) * size_t(height_i);

	double scale_x = double(width_o) / double(width_i);
	double scale_y = double(height_o) / double(height_i);
	try {
		Tmp = new double[size_t(width_o) * size_t(height_i) * (pnm_in.isRGB() ? 3 : 1)];
		Image = new double[size_o * (pnm_in.isRGB() ? 3 : 1)];
	}
	catch (const std::bad_alloc& bad) {
		std::cerr << bad.what() << std::endl
		    << "void pnm_Bicubic(PNM_DOUBLE*, const PNM_DOUBLE&, const double, const int , const int)" << std::endl;
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
	catch (const std::bad_alloc& bad) {
		std::cerr << bad.what() << std::endl
		    << "void pnm_Bicubic(PNM_DOUBLE*, const PNM_DOUBLE&, const double, const int , const int)" << std::endl;
		delete[] Tmp;
		delete[] Image;
		throw;
	}

	// Horizontal convolution
	pnm_img_double* imgd_data = pnm_in.Data();
	for (size_t x = 0; x < size_t(width_o); x++) {
		double dx;
		if (scale_x >= 1.0) {
			scale_conv = 1.0;
			dx = (double(x) - (scale_x - 1.0) / 2.0) / scale_x;
		} else {
			scale_conv = 1.0 / scale_x;
			dx = double(x) / scale_x + (1.0 / scale_x - 1.0) / 2.0;
		}
		long L = 4 * long(ceil(scale_conv));
		long L_center = long(floor((L - 1.0) / 2));
		for (long n = 0; n < L; n++) {
			conv[n] = pnm_Cubic((double(n - L_center) - (dx - floor(dx))) / scale_conv, alpha);
			conv[n] /= scale_conv;
		}
		for (size_t y = 0; y < size_t(height_i); y++) {
			Tmp[size_t(width_o) * y + x] = .0;
			if (pnm_in.isRGB()) {
				Tmp[size_t(width_o) * size_t(height_i) + size_t(width_o) * y + x] = .0;
				Tmp[2 * size_t(width_o) * size_t(height_i) + size_t(width_o) * y + x] = .0;
			}
			for (long n = 0; n < L; n++) {
				long index = long(floor(dx)) + n - L_center;
				if (index < 0) {
					index = labs(index) - 1;
				} else if (index >= width_i) {
					index = 2 * width_i - 1 - index;
				}
				Tmp[size_t(width_o) * y + x] += conv[n] * imgd_data[size_t(width_i) * y + size_t(index)];
				if (pnm_in.isRGB()) {
					Tmp[size_t(width_o) * size_t(height_i) + size_t(width_o) * y + x]
					    += conv[n] * imgd_data[size_i + size_t(width_i) * y + size_t(index)];
					Tmp[2 * size_t(width_o) * size_t(height_i) + size_t(width_o) * y + x]
					    += conv[n] * imgd_data[2 * size_i + size_t(width_i) * y + size_t(index)];
				}
			}
		}
	}
	imgd_data = nullptr;
	// Vertical convolution
	for (size_t y = 0; y < size_t(height_o); y++) {
		double dy;
		if (scale_y >= 1.0) {
			scale_conv = 1.0;
			dy = (double(y) - (scale_y - 1.0) / 2.0) / scale_y;
		} else {
			scale_conv = 1.0 / scale_y;
			dy = double(y) / scale_y + (1.0 / scale_y - 1.0) / 2.0;
		}
		long L = 4 * long(ceil(scale_conv));
		long L_center = long(floor((L - 1.0) / 2));
		for (long m = 0; m < L; m++) {
			conv[m] = pnm_Cubic((double(m - L_center) - (dy - floor(dy))) / scale_conv, alpha);
			conv[m] /= scale_conv;
		}
		for (size_t x = 0; x < size_t(width_o); x++) {
			Image[size_t(width_o) * y + x] = .0;
			if (pnm_in.isRGB()) {
				Image[size_o + size_t(width_o) * y + x] = .0;
				Image[2 * size_o + size_t(width_o) * y + x] = .0;
			}
			for (long m = 0; m < L; m++) {
				long index = int(floor(dy)) + m - L_center;
				if (index < 0) {
					index = labs(index) - 1;
				} else if (index >= height_i) {
					index = 2 * height_i - 1 - index;
				}
				Image[size_t(width_o) * y + x] += conv[m] * Tmp[size_t(width_o) * size_t(index) + x];
				if (pnm_in.isRGB()) {
					Image[size_o + size_t(width_o) * y + x]
					    += conv[m] * Tmp[size_t(width_o) * size_t(height_i) + size_t(width_o) * size_t(index) + x];
					Image[2 * size_o + size_t(width_o) * y + x]
					    += conv[m] * Tmp[2 * size_t(width_o) * size_t(height_i) + size_t(width_o) * size_t(index) + x];
				}
			}
		}
	}
	if (pnm_in.isRGB()) {
		for (size_t x = 0; x < 3 * size_o; x++) {
			if (Image[x] < .0) {
				Image[x] = .0;
			} else if (Image[x] > pnm_in.MaxInt()) {
				Image[x] = double(pnm_in.MaxInt());
			}
		}
	} else {
		for (size_t x = 0; x < size_o; x++) {
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

