#ifndef __SOWAITERATOR_H__
#define __SOWAITERATOR_H__

// Disable warnings about ignoring throws declarations
#pragma warning( disable : 4290 )

#include <numeric>
#include <iterator>
#include <algorithm>
#include <cmath>
#include "Image.h"
#include "ImageException.h"
#include "Template.h"
#include "OWAIterator.h"

namespace ImageTL
{
	template<class Type> class SOWAIterator : public OWAIterator<Type>
	{
	public:
		SOWAIterator(const Image<Type>* image);
		SOWAIterator(const Image<Type>* image, ConstantTemplate<Type>* tLinkC, ConstantTemplate<Type>* tLinkS);

		virtual typename ConvolutionIterator<Type>::value_type operator*();

		void normalizeLinkedTemplate();

	protected:
		ConstantTemplate<Type>* m_tLinkS;
	};
}

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "SOWAIterator.cpp"
#endif

#endif