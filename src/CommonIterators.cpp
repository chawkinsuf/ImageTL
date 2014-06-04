#ifndef __COMMONTERATORS_CPP__
#define __COMMONTERATORS_CPP__

#include "CommonIterators.h"

namespace ImageTL
{
	template<class Type> Type MulSumIterator<Type>::operator*()
	{
		if(this->m_tLink == NULL) {
			throw ImageException("OWAIterator::operator* [A template must be linked in order to dereference]"); }

		this->m_tLink->setCenter(this->m_imageX, this->m_imageY);

		int xMin = this->m_imageX - this->m_templateNegOffsetX;
		int xMax = this->m_imageX + this->m_templatePosOffsetX;
		int yMin = this->m_imageY - this->m_templateNegOffsetY;
		int yMax = this->m_imageY + this->m_templatePosOffsetY;

		Type sum = Type(0);
		Type imageData, templateData;
		for(int yLoop = yMin; yLoop <= yMax; yLoop++)
		{
			for(int xLoop = xMin; xLoop <= xMax; xLoop++)
			{
				try {
					imageData = this->m_image->getPixel(xLoop, yLoop); }
				catch(ImageException&) {
					continue; }

				templateData = this->m_tLink->operator()(xLoop, yLoop);
				sum += imageData * templateData;
			}
		}

		return sum;
	}

	template<class Type> Type MulMaxIterator<Type>::operator*()
	{
		if(this->m_tLink == NULL) {
			throw ImageException("OWAIterator::operator* [A template must be linked in order to dereference]"); }

		this->m_tLink->setCenter(this->m_imageX, this->m_imageY);

		int xMin = this->m_imageX - this->m_templateNegOffsetX;
		int xMax = this->m_imageX + this->m_templatePosOffsetX;
		int yMin = this->m_imageY - this->m_templateNegOffsetY;
		int yMax = this->m_imageY + this->m_templatePosOffsetY;

		//this->m_data->clear();
		Type imageData, templateData;
		typename ConvolutionIterator<Type>::data_iterator data_i = this->m_data->begin();
		for(int yLoop = yMin; yLoop <= yMax; yLoop++)
		{
			for(int xLoop = xMin; xLoop <= xMax; xLoop++)
			{
				try {
					imageData = this->m_image->getPixel(xLoop, yLoop); }
				catch(ImageException&) {
					continue; }

				templateData = this->m_tLink->operator()(xLoop, yLoop);
				*(data_i++) = imageData * templateData;
			}
		}

		Type max = std::numeric_limits<Type>::min();
		typename ConvolutionIterator<Type>::data_iterator data_e = data_i;
		for(data_i = this->m_data->begin(); data_i != data_e; ++data_i) {
			max = (*data_i>max)?*data_i:max; }

		return max;
	}

	template<class Type> Type MulMinIterator<Type>::operator*()
	{
		if(this->m_tLink == NULL) {
			throw ImageException("OWAIterator::operator* [A template must be linked in order to dereference]"); }

		this->m_tLink->setCenter(this->m_imageX, this->m_imageY);

		int xMin = this->m_imageX - this->m_templateNegOffsetX;
		int xMax = this->m_imageX + this->m_templatePosOffsetX;
		int yMin = this->m_imageY - this->m_templateNegOffsetY;
		int yMax = this->m_imageY + this->m_templatePosOffsetY;

		//this->m_data->clear();
		Type imageData, templateData;
		typename ConvolutionIterator<Type>::data_iterator data_i = this->m_data->begin();
		for(int yLoop = yMin; yLoop <= yMax; yLoop++)
		{
			for(int xLoop = xMin; xLoop <= xMax; xLoop++)
			{
				try {
					imageData = this->m_image->getPixel(xLoop, yLoop); }
				catch(ImageException&) {
					continue; }

				templateData = this->m_tLink->operator()(xLoop, yLoop);
				*(data_i++) = imageData * templateData;
			}
		}

		Type min = std::numeric_limits<Type>::max();
		typename ConvolutionIterator<Type>::data_iterator data_e = data_i;
		for(data_i = this->m_data->begin(); data_i != data_e; ++data_i) {
			min = (*data_i<min)?*data_i:min; }

		return min;
	}
}

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class MulSumIterator<char>;
	template class MulSumIterator<short>;
	template class MulSumIterator<int>;
	template class MulSumIterator<long>;
	template class MulSumIterator<float>;
	template class MulSumIterator<double>;

	template class MulMaxIterator<char>;
	template class MulMaxIterator<short>;
	template class MulMaxIterator<int>;
	template class MulMaxIterator<long>;
	template class MulMaxIterator<float>;
	template class MulMaxIterator<double>;

	template class MulMinIterator<char>;
	template class MulMinIterator<short>;
	template class MulMinIterator<int>;
	template class MulMinIterator<long>;
	template class MulMinIterator<float>;
	template class MulMinIterator<double>;
}
#endif

#endif