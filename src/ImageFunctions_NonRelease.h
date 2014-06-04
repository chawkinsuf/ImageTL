#ifndef __IMAGEFUNCTIONS_NONRELEASE_H__
#define __IMAGEFUNCTIONS_NONRELEASE_H__

#include <cmath>
#include "Image.h"
#include "ComplexImage.h"
#include "ImageFunctions.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944592

namespace ImageTL
{
	// ***** Domain transform functions *****
	// **************************************
	// Flips the bits in each index of the domain
	void bitReversal(int &x, int &y);

	// ***** Algorithms *****
	// **********************
	// Replaces input with the fourier transform of itself
	// direction = 1:  Forward transform
	// direction = -1: Reverse transform
	void fft(ComplexImage &input, int direction = 1);
}

#endif