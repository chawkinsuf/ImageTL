#ifndef __SOWAITERATOR_CPP__
#define __SOWAITERATOR_CPP__

#include "SOWAIterator.h"

namespace ImageTL
{
	template<class Type> SOWAIterator<Type>::SOWAIterator(const Image<Type>* image) :
		OWAIterator<Type>(image)
	{
		this->m_tLinkS  = NULL;
	}

	template<class Type> SOWAIterator<Type>::SOWAIterator(const Image<Type>* image, ConstantTemplate<Type>* tLinkC, ConstantTemplate<Type>* tLinkS) :
		OWAIterator<Type>(image, tLinkC)
	{
		this->m_tLinkS = tLinkS;

		if( this->m_tLinkS->width() != this->m_tLinkC->width() || this->m_tLinkS->height() != this->m_tLinkC->height() ) {
			throw ImageException("SOWAIterator::SOWAIterator [The linked templates must be the same dimensions]"); }
	}

	template<class Type> typename ConvolutionIterator<Type>::value_type SOWAIterator<Type>::operator*()
	{
		if(this->m_tLinkS == NULL) {
			throw ImageException("SOWAIterator::operator* [A template must be linked in order to dereference]"); }

		int xMin = this->m_imageX - this->m_templateNegOffsetX;
		int xMax = this->m_imageX + this->m_templatePosOffsetX;
		int yMin = this->m_imageY - this->m_templateNegOffsetY;
		//int yMax = this->m_imageY + this->m_templatePosOffsetY;

		this->clearData(this->m_data);
		this->clearData(this->m_temData);

		int yLoop = yMin;
		int xLoop = xMin;
		int temSize = this->m_tLinkC->size();
		typename ConvolutionIterator<Type>::data_iterator data_i    = this->m_data->begin();
		typename ConvolutionIterator<Type>::data_iterator temData_i = this->m_temData->begin();
		for(int i = 0; i < temSize; i++)
		{
			try {
				*(data_i++) = this->m_tLinkS->operator()(i) + this->m_image->getPixel(xLoop++, yLoop); }
			catch(ImageException&) {
				continue; }

			*(temData_i++) = this->m_tLinkC->operator()(i);

			if(xLoop > xMax)
			{
				xLoop = xMin;
				yLoop++;
			}
		}

		sort(this->m_data->begin(), this->m_data->end());

		Type sum = Type(0);
		data_i    = this->m_data->begin();
		temData_i = this->m_temData->begin();
		typename ConvolutionIterator<Type>::data_iterator data_e = this->m_data->end();
		while(data_i != data_e)
		{
			sum += (*(data_i++)) * (*(temData_i++));
		}

		return sum;
	}
}

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class SOWAIterator<char>;
	template class SOWAIterator<short>;
	template class SOWAIterator<int>;
	template class SOWAIterator<long>;
	template class SOWAIterator<float>;
	template class SOWAIterator<double>;
}
#endif

#endif