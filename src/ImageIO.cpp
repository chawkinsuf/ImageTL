#ifndef __IMAGEIO_CPP__
#define __IMAGEIO_CPP__

#include "ImageIO.h"

namespace ImageTL
{
	template<class Type> void ImageIO<Type>::read(const char *file)
	{
		readHeader(file);
		readData(file);
	}

	template<class Type> void ImageIO<Type>::write(const char *file, int d, const char *comment)
	{
		if(d > 0) {
			m_depth = d; }

		std::cout<<std::endl<<file<<": min= "<<this->min()<<", max= "<<this->max()<<std::endl;

		writePrepare();
		writeHeader(file, comment);
		writeData(file);
	}

	template<class Type> void ImageIO<Type>::checkPixelDepth() throw(ImageException)
	{
		Type minValue = this->min();
		//Check the minimum
		if(ROUND(minValue) < 0)
		{
			//Implement lower bound options
			if(m_depth_h & lower_translate)
			{
				*this -= minValue;
				std::cout<<"Lower bound was translated to zero."<<std::endl;
			}
			else if(m_depth_h & lower_truncate)
			{
				typename Image<Type>::iterator e = this->end();
				for(typename Image<Type>::iterator iter = this->begin(); iter != e; ++iter) {
					if(*iter < 0) {
						*iter = 0; } }
				std::cout<<"Lower bound was truncated to zero."<<std::endl;
			}
			else if(m_depth_h & lower_abs)
			{
				typename Image<Type>::iterator e = this->end();
				for(typename Image<Type>::iterator iter = this->begin(); iter != e; ++iter) {
					if(*iter < 0) {
						*iter = -(*iter); } }
				std::cout<<"Absolute value was taken."<<std::endl;
			}
			else {
				throw ImageException("ImageIO::checkPixelDepth [No lower bound pixel depth option is set]"); }
		}

		Type maxValue = this->max();
		//Check the maximum
		if(ROUND(maxValue) > m_depth)
		{
			//Implement upper bound options
			if(m_depth_h & upper_increase)
			{
				if(maxValue <= 65535) {
					m_depth = (int)ROUND(maxValue); }
				else
				{
					m_depth = 65535;
					typename Image<Type>::iterator e = this->end();
					for(typename Image<Type>::iterator iter = this->begin(); iter != e; ++iter) {
						if(*iter > m_depth) {
							*iter = Type(m_depth); } }
				}
				std::cout<<"Pixel depth was increased to "<<m_depth<<"."<<std::endl;
			}
			else if(m_depth_h & upper_scale)
			{
				*this *= m_depth/maxValue;
				std::cout<<"Upper bound was scaled linearly to "<<m_depth<<"."<<std::endl;
			}
			else if(m_depth_h & upper_truncate)
			{
				typename Image<Type>::iterator e = this->end();
				for(typename Image<Type>::iterator iter = this->begin(); iter != e; ++iter) {
					if(*iter > m_depth) {
						*iter = Type(m_depth); } }
				std::cout<<"Upper bound was truncated to "<<m_depth<<"."<<std::endl;
			}
			else {
				throw ImageException("ImageIO::checkPixelDepth [No upper bound pixel depth option is set]"); }
		}
		//Check the maximum again
		else if(ROUND(maxValue) < m_depth)
		{
			//Implement upper bound option 2
			if(m_depth_h & upper2_decrease)
			{
				m_depth = (int)ROUND(maxValue);
				std::cout<<"Pixel m_depth was decreased to "<<m_depth<<"."<<std::endl;
			}
			else if(m_depth_h & upper2_stretch)
			{
				if(maxValue != 0) {
					*this *= m_depth/maxValue; }
				std::cout<<"Upper bound was stretched linearly to "<<m_depth<<"."<<std::endl;
			}
		}
	}

	//Data manipulation functions
	template<class Type> void ImageIO<Type>::writePrepare()
	{
		checkPixelDepth();
		typename Image<Type>::iterator e = this->end();
		for(typename Image<Type>::iterator i = this->begin(); i != e; ++i) {
			*i = ROUND(*i);	}
	}

	template<class Type> void ImageIO<Type>::histogram()
	{
		writePrepare();
		if(m_hist != NULL) {
			delete[] m_hist; }
		m_hist = new int[m_depth + 1];
		memset(m_hist, 0, sizeof(int)*(m_depth + 1));

		typename Image<Type>::iterator e = this->end();
		for(typename Image<Type>::iterator i = this->begin(); i != e; ++i) {
			m_hist[(int)*i]++; }
	}

	template<class Type> void ImageIO<Type>::histogramEqualize()
	{
		histogram();

		double c = (double)m_depth/(this->m_width*this->m_height);
		int *cummHist = new int[m_depth + 1];
		cummHist[0] = m_hist[0];
		for(int i = 1; i<=m_depth; i++) {
			cummHist[i] = cummHist[i - 1] + m_hist[i]; }

		typename Image<Type>::iterator e = this->end();
		for(typename Image<Type>::iterator i = this->begin(); i != e; ++i) {
			*i = Type(c*cummHist[(int)*i]); }

		delete[] cummHist;
	}

	template<class Type> ImageIO<Type>& ImageIO<Type>::operator=(const ImageIO<Type>& im)
	{
		if(this->m_height != im.m_height || this->m_width != im.m_width)
		{
			this->freeImage(this->m_image);

			this->m_height = im.m_height;
			this->m_width  = im.m_width;

			this->m_image = this->allocateImage();
		}

		m_depth = im.m_depth;
		m_headerLength = 0;
		m_depth_h = im.m_depth_h;

		this->copyImage(this->m_image, im.m_image);

		return *this;
	}

	//Constructors
	template<class Type> ImageIO<Type>::ImageIO() : Image<Type>()
	{
		m_depth = 0;
		m_headerLength = 0;
		m_hist = NULL;
		m_depth_h = upper_scale | lower_translate;
	}

	template<class Type> ImageIO<Type>::ImageIO(int w, int h, int d, depth_handling dh) : Image<Type>(w, h)
	{
		m_depth = d;
		m_headerLength = 0;
		m_hist = NULL;
		m_depth_h = dh;
	}

	template<class Type> ImageIO<Type>::ImageIO(const Image<Type> &i, bool copy, int d, depth_handling dh) : Image<Type>(i, copy)
	{
		m_depth = d;
		m_headerLength = 0;
		m_hist = NULL;
		m_depth_h = dh;
	}

	template<class Type> ImageIO<Type>::ImageIO(const ImageIO<Type> &i, bool copy) : Image<Type>(i, copy)
	{
		m_depth = i.m_depth;
		m_headerLength = 0;
		m_hist = NULL;
		m_depth_h = i.m_depth_h;
	}

	template<class Type> ImageIO<Type>::~ImageIO()
	{
		if(m_hist != NULL) {
			delete[] m_hist; }
	}
}	//End namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class ImageIO<char>;
	template class ImageIO<short>;
	template class ImageIO<int>;
	template class ImageIO<long>;
	template class ImageIO<float>;
	template class ImageIO<double>;
}
#endif

#endif