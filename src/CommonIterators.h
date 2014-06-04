#ifndef __COMMONTERATORS_H__
#define __COMMONTERATORS_H__

// Disable warnings about ignoring throws declarations
#pragma warning( disable : 4290 )

// These might be used in included headers
namespace ImageTL
{
	template<class Type> class MulSumIterator;
	template<class Type> class MulMaxIterator;
	template<class Type> class MulMinIterator;
}

#include <numeric>
#include <iterator>
#include "Image.h"
#include "ImageException.h"
#include "Template.h"
#include "ConvolutionIterator.h"

namespace ImageTL
{
	// Right linear convolution product.
	template<class Type> class MulSumIterator : public ConvolutionIterator<Type>
	{
	public:
		MulSumIterator(const Image<Type>* image) : ConvolutionIterator<Type>(image) {}
		MulSumIterator(const Image<Type>* image, Template<Type>* tLink) : ConvolutionIterator<Type>(image, tLink, NULL, NULL) {}

		Type operator*();
	};

	// Right multiplicative maximum convolution product.
	template<class Type> class MulMaxIterator : public ConvolutionIterator<Type>
	{
	public:
		MulMaxIterator(const Image<Type>* image) : ConvolutionIterator<Type>(image) {}
		MulMaxIterator(const Image<Type>* image, Template<Type>* tLink) : ConvolutionIterator<Type>(image, tLink, NULL, NULL) {}

		Type operator*();
	};

	// Right multiplicative minimun convolution product.
	template<class Type> class MulMinIterator : public ConvolutionIterator<Type>
	{
	public:
		MulMinIterator(const Image<Type>* image) : ConvolutionIterator<Type>(image) {}
		MulMinIterator(const Image<Type>* image, Template<Type>* tLink) : ConvolutionIterator<Type>(image, tLink, NULL, NULL) {}

		Type operator*();
	};
}

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "CommonIterators.cpp"
#endif

#endif