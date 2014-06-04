#include "ComplexImage.h"

namespace ImageTL
{
	void ComplexImage::replaceImage(Image<double> real, Image<double> imag) throw(ImageException)
	{
		if((real.width() != imag.width()) || (real.height() != imag.height())) {
			throw ImageException("ComplexImage::replaceImage [Mismatched dimensions]"); }

		freeImage(m_image);
		m_width  = real.width();
		m_height = real.height();
		m_image  = allocateImage();


		iterator e = end();
		Image<double>::iterator real_i = real.begin();
		Image<double>::iterator imag_i = imag.begin();
		for(iterator i = begin(); i != e; ++i, ++real_i, ++imag_i)
		{
			complex<double> newValue(*real_i, *imag_i);
			*i = newValue;
		}
	}

	void ComplexImage::replaceImage(double real, double imag) throw(ImageException)
	{
		iterator e = end();
		complex<double> newValue(real, imag);
		for(iterator i = begin(); i != e; ++i) {
			*i = newValue; }
	}

	Image<double> ComplexImage::real()
	{
		Image<double> i(m_width, m_height);

		Image<double>::iterator iter = i.begin();
		Image<double>::iterator e = i.end();
		iterator iter_comp = begin();
		for(; iter != e; ++iter, ++iter_comp) {
			*iter = (*iter_comp).real(); }

		return i;
	}

	Image<double> ComplexImage::imag()
	{
		Image<double> i(m_width, m_height);

		Image<double>::iterator iter = i.begin();
		Image<double>::iterator e = i.end();
		iterator iter_comp = begin();
		for(; iter != e; ++iter, ++iter_comp) {
			*iter = (*iter_comp).imag(); }

		return i;
	}

	Image<double> ComplexImage::abs()
	{
		Image<double> i(m_width, m_height);

		Image<double>::iterator iter = i.begin();
		Image<double>::iterator e = i.end();
		iterator iter_comp = begin();
		for(; iter != e; ++iter, ++iter_comp) {
			*iter = std::abs(*iter_comp); }

		return i;
	}

	ComplexImage::ComplexImage(const Image<double> &i) : Image<complex<double> >(i.width(), i.height(), i.edgeHandling())
	{
		Image<double>::iterator iter = i.begin();
		Image<double>::iterator e = i.end();
		iterator iter_comp = begin();
		for(; iter != e; ++iter, ++iter_comp)
		{
			*iter_comp = complex<double>(*iter);
		}
	}

	// Operators
	ComplexImage& ComplexImage::operator=(const Image<complex<double> >& im)
	{
		/*if(m_height != im.height() || m_width != im.width())
		{
			freeImage(m_image);

			m_height = im.height();
			m_width  = im.width();

			m_image = allocateImage();
		}

		Image<complex<double> >::iterator iterCopy = im.begin();
		iterator e = end();
		for(iterator iter = begin(); iter != e; ++iter, ++iterCopy) {
			*iter = *iterCopy; }*/

		Image<complex<double> >::operator=(im);

		return *this;
	}

	ComplexImage& ComplexImage::operator=(const ComplexImage& im)
	{
		/*if(m_height != im.m_height || m_width != im.m_width)
		{
			freeImage(m_image);

			m_height = im.m_height;
			m_width  = im.m_width;

			m_image = allocateImage();
		}

		copyImage(m_image, im.m_image);*/

		Image<complex<double> >::operator=(im);

		return *this;
	}

	ComplexImage& ComplexImage::operator=(complex<double> n)
	{
		/*if(m_image != NULL)
		{
			iterator e = end();
			for(iterator i = begin(); i != e; ++i) {
				*i = n; }
		}
		else {
			throw ImageException("ComplexImage::operator= [No image dimensions on assignment]"); }*/

		Image<complex<double> >::operator=(n);

		return *this;
	}

}	//End namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class Image<complex<double> >;

	template bool operator<( const complex<double>&, const complex<double>&);
	template bool operator<=(const complex<double>&, const complex<double>&);
	template bool operator>( const complex<double>&, const complex<double>&);
	template bool operator>=(const complex<double>&, const complex<double>&);
	template complex<double> operator/(const complex<double>&, const int&);
}
#endif