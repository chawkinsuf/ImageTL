#ifndef __OWAITERATOR_H__
#define __OWAITERATOR_H__

// Disable warnings about ignoring throws declarations
#pragma warning( disable : 4290 )

#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
#include "Image.h"
#include "ImageException.h"
#include "Template.h"
#include "ConvolutionIterator.h"

namespace ImageTL
{
	template<class Type> class OWAIterator : public ConvolutionIterator<Type>
	{
	public:
		OWAIterator(const Image<Type>* image);
		OWAIterator(const Image<Type>* image, ConstantTemplate<Type>* tLinkC);

		~OWAIterator();

		virtual typename ConvolutionIterator<Type>::value_type operator*();

		void normalizeLinkedTemplate();

	protected:
		typename ConvolutionIterator<Type>::data_container* m_temData;

		ConstantTemplate<Type>* m_tLinkC;
	};
}

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "OWAIterator.cpp"
#endif

#endif