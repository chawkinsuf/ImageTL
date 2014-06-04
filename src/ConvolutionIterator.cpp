#ifndef __CONVOLUTIONITERATOR_CPP__
#define __CONVOLUTIONITERATOR_CPP__
/** @file ConvolutionIterator.cpp
	Contains function definitions that are declared in ConvolutionIterator.h
*/

#include "ConvolutionIterator.h"
#include "Image.h"

namespace ImageTL
{
	// Template operation functions
	template<class Type> Type mf_mul(Type& left, Type& right)
	{
		return (left * right);
	}

	template<class Type> Type mf_add(Type& left, Type& right)
	{
		return (left + right);
	}

	template<class Type> Type mf_sub(Type& left, Type& right)
	{
		return (left - right);
	}

	template<class Type> Type uf_sum(typename ConvolutionIterator<Type>::data_container& data)
	{
		Type sum = Type(0);
		typename ConvolutionIterator<Type>::data_iterator i     = data.begin();
		typename ConvolutionIterator<Type>::data_iterator i_end = data.end();
		for( ; i != i_end; ++i) {
			sum += *i; }
		return sum;
	}

	template<class Type> Type uf_max(typename ConvolutionIterator<Type>::data_container& data)
	{
		Type max = std::numeric_limits<Type>::min();
		typename ConvolutionIterator<Type>::data_iterator i     = data.begin();
		typename ConvolutionIterator<Type>::data_iterator i_end = data.end();
		for( ; i != i_end; ++i) {
			max = (*i>max)?*i:max; }
		return max;
	}

	template<class Type> Type uf_min(typename ConvolutionIterator<Type>::data_container& data)
	{
		Type min = std::numeric_limits<Type>::max();
		typename ConvolutionIterator<Type>::data_iterator i     = data.begin();
		typename ConvolutionIterator<Type>::data_iterator i_end = data.end();
		for( ; i != i_end; ++i) {
			min = (*i<min)?*i:min; }
		return min;
	}

	template<class Type> Type uf_mean(typename ConvolutionIterator<Type>::data_container& data)
	{
		Type sum = Type(0);
		typename ConvolutionIterator<Type>::data_iterator i     = data.begin();
		typename ConvolutionIterator<Type>::data_iterator i_end = data.end();
		for( ; i != i_end; ++i) {
			sum += *i; }
		return sum/(i_end - data.begin());
	}

	template<class Type> Type uf_median(typename ConvolutionIterator<Type>::data_container& data)
	{
		std::sort(data.begin(), data.end());
		typename ConvolutionIterator<Type>::data_iterator p = data.begin();
		typename ConvolutionIterator<Type>::data_container::size_type len = data.size();
		for(typename ConvolutionIterator<Type>::data_container::size_type i=0; i<(len>>2); i++) { p++; }
		if(len%2 == 0)
		{
			double tmp = *(p--);
			return ( (tmp + *p)/2. );
		}
		else {
			return (*p); }
	}

	template<class Type> ConvolutionIterator<Type>::ConvolutionIterator(const Image<Type>* image)
	{
		m_image = image;
		m_tLink = NULL;
		m_mergeFunction = NULL;
		m_unityFunction = NULL;

		m_imageX = 0;
		m_imageY = m_image->height();

		m_templateNegOffsetX = 0;
		m_templatePosOffsetX = 0;
		m_templateNegOffsetY = 0;
		m_templatePosOffsetY = 0;

		m_data = NULL;
	}

	template<class Type> ConvolutionIterator<Type>::ConvolutionIterator(const Image<Type>* image, Template<Type> *tLink,
		merge_function mergeFunction, unity_function unityFunction)
	{
		m_image = image;
		m_tLink = tLink;
		m_mergeFunction = mergeFunction;
		m_unityFunction = unityFunction;

		m_imageX = 0;
		m_imageY = 0;

		m_templateNegOffsetX = (m_tLink->width() - 1)/2;
		m_templatePosOffsetX =  m_tLink->width()/2;
		m_templateNegOffsetY = (m_tLink->height() - 1)/2;
		m_templatePosOffsetY =  m_tLink->height()/2;

		m_data = new data_container( tLink->width() * tLink->height() );
	}

	template<class Type> ConvolutionIterator<Type>::~ConvolutionIterator()
	{
		if(m_data != NULL)
		{
			delete m_data;
			m_data = NULL;
		}
	}

	// ***** CHANGE TYPE(0) TO THE NULL VAR
	template<class Type> void ConvolutionIterator<Type>::clearData(data_container* data)
	{
		data_iterator data_i = data->begin();
		data_iterator data_e = data->end();
		for(; data_i != data_e; ++data_i) {
			*data_i = Type(0); }
	}

	template<class Type> typename ConvolutionIterator<Type>::value_type ConvolutionIterator<Type>::operator*()
	{
		if(m_tLink == NULL) {
			throw ImageException("ConvolutionIterator::operator* [A template must be linked in order to dereference]"); }

		m_tLink->setCenter(m_imageX, m_imageY);

		int xMin = m_imageX - m_templateNegOffsetX;
		int xMax = m_imageX + m_templatePosOffsetX;
		int yMin = m_imageY - m_templateNegOffsetY;
		int yMax = m_imageY + m_templatePosOffsetY;

		clearData(m_data);

		Type imageData, templateData;
		data_iterator data_i = m_data->begin();
		for(int yLoop = yMin; yLoop <= yMax; yLoop++)
		{
			for(int xLoop = xMin; xLoop <= xMax; xLoop++)
			{
				try {
					imageData = m_image->getPixel(xLoop, yLoop); }
				catch(ImageException&) {
					continue; }

				templateData = m_tLink->operator()(xLoop, yLoop);
				*(data_i++) = m_mergeFunction(imageData, templateData);
			}
		}

		return m_unityFunction(*m_data);
	}

	// pre
	template<class Type> ConvolutionIterator<Type>& ConvolutionIterator<Type>::operator++()
	{
		m_imageX++;
		if(m_imageX >= m_image->width())
		{
			m_imageX = 0;
			m_imageY++;
		}
		return *this;
	}

	// post
	template<class Type> ConvolutionIterator<Type> ConvolutionIterator<Type>::operator++(int)
	{
		ConvolutionIterator<Type> tmp = *this;

		m_imageX++;
		if(m_imageX >= m_image->width())
		{
			m_imageX = 0;
			m_imageY++;
		}

		return tmp;
	}
}	//end namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class ConvolutionIterator<char>;
	template class ConvolutionIterator<short>;
	template class ConvolutionIterator<int>;
	template class ConvolutionIterator<long>;
	template class ConvolutionIterator<float>;
	template class ConvolutionIterator<double>;

	template char mf_mul(char&, char&);
	template short mf_mul(short&, short&);
	template int mf_mul(int&, int&);
	template long mf_mul(long&, long&);
	template float mf_mul(float&, float&);
	template double mf_mul(double&, double&);

	template char mf_add(char&, char&);
	template short mf_add(short&, short&);
	template int mf_add(int&, int&);
	template long mf_add(long&, long&);
	template float mf_add(float&, float&);
	template double mf_add(double&, double&);

	template char mf_sub(char&, char&);
	template short mf_sub(short&, short&);
	template int mf_sub(int&, int&);
	template long mf_sub(long&, long&);
	template float mf_sub(float&, float&);
	template double mf_sub(double&, double&);

	template char uf_sum(typename ConvolutionIterator<char>::data_container&);
	template short uf_sum(typename ConvolutionIterator<short>::data_container&);
	template int uf_sum(typename ConvolutionIterator<int>::data_container&);
	template long uf_sum(typename ConvolutionIterator<long>::data_container&);
	template float uf_sum(typename ConvolutionIterator<float>::data_container&);
	template double uf_sum(typename ConvolutionIterator<double>::data_container&);

	template char uf_max(typename ConvolutionIterator<char>::data_container&);
	template short uf_max(typename ConvolutionIterator<short>::data_container&);
	template int uf_max(typename ConvolutionIterator<int>::data_container&);
	template long uf_max(typename ConvolutionIterator<long>::data_container&);
	template float uf_max(typename ConvolutionIterator<float>::data_container&);
	template double uf_max(typename ConvolutionIterator<double>::data_container&);

	template char uf_min(typename ConvolutionIterator<char>::data_container&);
	template short uf_min(typename ConvolutionIterator<short>::data_container&);
	template int uf_min(typename ConvolutionIterator<int>::data_container&);
	template long uf_min(typename ConvolutionIterator<long>::data_container&);
	template float uf_min(typename ConvolutionIterator<float>::data_container&);
	template double uf_min(typename ConvolutionIterator<double>::data_container&);

	template char uf_mean(typename ConvolutionIterator<char>::data_container&);
	template short uf_mean(typename ConvolutionIterator<short>::data_container&);
	template int uf_mean(typename ConvolutionIterator<int>::data_container&);
	template long uf_mean(typename ConvolutionIterator<long>::data_container&);
	template float uf_mean(typename ConvolutionIterator<float>::data_container&);
	template double uf_mean(typename ConvolutionIterator<double>::data_container&);

	template char uf_median(typename ConvolutionIterator<char>::data_container&);
	template short uf_median(typename ConvolutionIterator<short>::data_container&);
	template int uf_median(typename ConvolutionIterator<int>::data_container&);
	template long uf_median(typename ConvolutionIterator<long>::data_container&);
	template float uf_median(typename ConvolutionIterator<float>::data_container&);
	template double uf_median(typename ConvolutionIterator<double>::data_container&);
}
#endif

#endif