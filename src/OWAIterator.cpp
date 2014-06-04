#ifndef __OWAITERATOR_CPP__
#define __OWAITERATOR_CPP__

#include "OWAIterator.h"

namespace ImageTL
{
	template<class Type> OWAIterator<Type>::OWAIterator(const Image<Type>* image) :
		ConvolutionIterator<Type>(image)
	{
		m_temData = NULL;
		m_tLinkC  = NULL;
	}

	template<class Type> OWAIterator<Type>::OWAIterator(const Image<Type>* image, ConstantTemplate<Type>* tLinkC) :
		ConvolutionIterator<Type>(image, tLinkC, NULL, NULL)
	{
		m_tLinkC = tLinkC;

		m_temData = new typename ConvolutionIterator<Type>::data_container( m_tLinkC->size() );
	}

	template<class Type> OWAIterator<Type>::~OWAIterator()
	{
		if(m_temData != NULL)
		{
			delete m_temData;
			m_temData = NULL;
		}
	}

	template<class Type> void OWAIterator<Type>::normalizeLinkedTemplate()
	{
		Type sum = Type(0);
		int size = m_tLinkC->size();
		for(int i = 0; i < size; i++) {
			sum += m_tLinkC->operator()(i); }

		m_tLinkC->operator/=(sum);
	}

	template<class Type> typename ConvolutionIterator<Type>::value_type OWAIterator<Type>::operator*()
	{
		if(m_tLinkC == NULL) {
			throw ImageException("OWAIterator::operator* [A template must be linked in order to dereference]"); }

		int xMin = this->m_imageX - this->m_templateNegOffsetX;
		int xMax = this->m_imageX + this->m_templatePosOffsetX;
		int yMin = this->m_imageY - this->m_templateNegOffsetY;
		//int yMax = this->m_imageY + this->m_templatePosOffsetY;

		this->clearData(this->m_data);
		this->clearData(this->m_temData);

		int yLoop = yMin;
		int xLoop = xMin;
		int temSize = m_tLinkC->size();
		typename ConvolutionIterator<Type>::data_iterator data_i    = this->m_data->begin();
		typename ConvolutionIterator<Type>::data_iterator temData_i = this->m_temData->begin();
		for(int i = 0; i < temSize; i++)
		{
			try {
				*(data_i++) = this->m_image->getPixel(xLoop++, yLoop); }
			catch(ImageException&) {
				continue; }

			*(temData_i++) = m_tLinkC->operator()(i);

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
	template class OWAIterator<char>;
	template class OWAIterator<short>;
	template class OWAIterator<int>;
	template class OWAIterator<long>;
	template class OWAIterator<float>;
	template class OWAIterator<double>;
}
#endif

#endif