#ifndef __IMAGEFUNCTIONS_H__
#define __IMAGEFUNCTIONS_H__

#include <complex>

#include "Image.h"
#include "PgmImage.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944592

namespace ImageTL
{
	// ***** Generic Unary operations *****
	// ************************************
	// Returns the complex conjugate of a complex image
	std::complex<double> conjugate(std::complex<double> n);

	// Returns the norm of a complex image
	std::complex<double> normalize(std::complex<double> n);


	// ***** Domain transform functions *****
	// **************************************
	// Returns the transpose image
	void transpose(int &x, int &y);


	// ***** Algorithms *****
	// **********************
	// Creates the x and y component templates used to perform gaussian filtering
	// If gaussWidth is <=0 the width is calculated based on sigma
	void GaussianTemplates(ConstantTemplate<double>*& x, ConstantTemplate<double>*& y,
						   double sigma, int gaussWidth = 0);

	// Performs Marr-Hilsreth Edge detection on the given image using sigma as
	// a Gaussian smoothing parameter
	void MarrHildrethEdges(Image<double>& input, double sigma = 4, int debug = 0);

	// Performs non-linear diffusion filtering on the image input
	void CoherenceEnhancingDiffusion(Image<double>& input, int    steps = 15,  double stepSize = 0.5,
														   double sigma = 0.5, double rho      = 4.,
														   double alpha = 0.1, double k        = 1e4,
														   int    debug = 0);

	// Draw a line from P0(x0,y0) to P1(x1,y1) onto the image i with intensity value
	void DrawLine(Image<double> &i, double value, int x0, int y0, int x1, int y1);
}

#endif