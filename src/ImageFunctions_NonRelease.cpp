#include "ImageFunctions_NonRelease.h"

namespace ImageTL
{
	// Domain transform functions
	static double dimension;
	void bitReversal(int &x, int &y)
	{
		int k = (int)(log((double)dimension)/log(2.)), bit, ret = 0;
		for(int i = k-1; i>=0; i--)
		{
			bit = x & 0x1;
			ret |= bit<<i;
			x = x>>1;
		}
		x = ret;
	}

	// Template functions
	// This is a template function that is used for finding the fft.  It it
	// only used in the fft function and therefore not declared in the header.
	complex<double> fft_template_function(const FunctionalTemplate<complex<double> > &t, int x, int y,
										  const complex<double> &parameter)
	{
		int p = (int)parameter.real();
		int u = t.xCenter();
		int d = (int)std::floor(u/(double)p);
		if(d%2 == 0)
		{
			if(x == u) {
				return std::complex<double>(1); }
			else if(x == u + p)
			{
				double arg = PI*(u%p)/(double)p;
				return std::complex<double>(cos(arg), -sin(arg));
			}
			else {
				return std::complex<double>(); }
		}
		else
		{
			if(x == u)
			{
				double arg = PI*(u%p)/(double)p;
				return std::complex<double>(-cos(arg), sin(arg));
			}
			else if(x == u - p) {
				return std::complex<double>(1); }
			else {
				return std::complex<double>(); }
		}
	}

	// Algorithms
	void fft(ComplexImage &input, int direction)
	{
		int imageWidth = input.width();
		dimension = imageWidth;

		if(direction < 0) {
			input.genericUnary(conjugate); }

		int k = (int)(log((double)imageWidth)/log(2.));
		FunctionalTemplate<std::complex<double> > fft_1d(imageWidth+1, 1);
		input.domainTransform(bitReversal);
		int param = 1;
		for(int i=1; i<=k; i++)
		{
			fft_1d.setWidth((param<<1) + 1);
			input = input + fft_1d(fft_template_function, std::complex<double>(param));
			param = param<<1;
		}

		input.domainTransform(transpose);
		input.domainTransform(bitReversal);

		param = 1;
		for(int i=1; i<=k; i++)
		{
			fft_1d.setWidth((param<<1) + 1);
			input = input + fft_1d(fft_template_function, std::complex<double>(param));
			param = param<<1;
		}

		input.domainTransform(transpose);

		if(direction < 0)
		{
			input.genericUnary(conjugate);
			input /= imageWidth*imageWidth;
		}
	}
}