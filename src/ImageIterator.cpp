#ifndef __IMAGEITERATOR_CPP__
#define __IMAGEITERATOR_CPP__
/** @file ImageIterator.cpp
	Contains function definitions that are declared in ImageIterator.h
*/

#include "ImageIterator.h"

namespace ImageTL
{
	//post-increment
	template<class Type> ImageIterator<Type> ImageIterator<Type>::operator++(int)
	{
		ImageIterator<Type> tmp = *this;
		m_image++;
		return tmp;
	}
}	//end namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class ImageIterator<char>;
	template class ImageIterator<short>;
	template class ImageIterator<int>;
	template class ImageIterator<long>;
	template class ImageIterator<float>;
	template class ImageIterator<double>;
}
#endif

#endif