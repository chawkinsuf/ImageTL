#ifndef __IMAGE_CPP__
#define __IMAGE_CPP__
/** @file Image.cpp
	Contains function definitions that are declared in Image.h
*/

#include "Image.h"

namespace ImageTL
{
	//Image definitions
	template<class Type> Image<Type> Image<Type>::subImage(int x, int y, int width, int height) const
	{
		Image<Type> subimage(width, height, m_edgeHandling);
		iterator sub_i = subimage.begin();
		iterator sub_e = subimage.end();

		int y_e = y + height;
		for(int y_i = y; y_i < y_e; y_i++)
		{
			int x_e = x + width;
			for(int x_i = x; x_i < x_e; x_i++, ++sub_i)
			{
				*sub_i = getPixel(x_i, y_i);
			}
		}

		return subimage;
	}

	template<class Type> void Image<Type>::writeToAscii(const char *fileName, int width, int precision)
	{
		std::ofstream fout(fileName);
		if(!fout)
		{
			std::cout<<"Image: Error opening "<<fileName<<" for writing.";
			return;
		}

		iterator iter = begin();
		iterator e = end();
		for(int i=1; iter != e; ++iter, i++)
		{
			fout<<std::setfill(' ')<<std::setw(width)<<std::setprecision(precision)<<*iter;
			if(i == m_width)
			{
				i = 0;
				fout<<std::endl;
			}
			else {
				fout<<" "; }
		}
	}

	template<class Type> Type& Image<Type>::getPixel(int x, int y)
	{
		if(x >= 0 && y >= 0 && x < m_width && y < m_height) {
			return m_image[m_width*y + x]; }

		throw ImageException("Image::getPixel [Out of bounds]");
	}

	template<class Type> Type Image<Type>::getPixel(int x, int y) const
	{
		bool xNeg = (x >= 0), xPos = (x < m_width), yNeg = (y >= 0), yPos = (y < m_height);
		if(xNeg && yNeg && xPos && yPos) {
			return m_image[m_width*y + x]; }
		else if(m_edgeHandling == edge_zero) {
			return Type(0); }
		else if(m_edgeHandling == edge_clamp)
		{
			x = !xNeg?x:0;
			x = !xPos?x:m_width-1;
			y = !yNeg?y:0;
			y = !yPos?y:m_height-1;
			return m_image[m_width*y + x];
		}
		else if(m_edgeHandling == edge_skip) {
			throw ImageException("Image::getPixel [Skip pixel]"); }
		else {
			throw ImageException("Image::getPixel [Invalid edge handling]"); }
	}

	template<class Type> Type& Image<Type>::getPixel(int location)
	{
		if(location<0 || location>=m_width*m_height) {
			throw ImageException("Image::getPixel [Out of bounds]"); }

		return m_image[location];
	}

	template<class Type> Type Image<Type>::getPixel(double xPrime, double yPrime) const
	{
		int x = (int)std::floor(xPrime);
		int y = (int)std::floor(yPrime);

		Type a_xy     = getPixel(x,   y);
		Type a_xPlus  = getPixel(x+1, y);
		Type a_yPlus  = getPixel(x,   y+1);
		Type a_xyPlus = getPixel(x+1, y+1);

		Type xDir =  Type(xPrime - x)*(a_xPlus - a_xy);
		Type yDir =  Type(yPrime - y)*(a_yPlus - a_xy);
		Type xyDir = Type(xPrime - x)*Type(yPrime - y)*(a_xy + a_xyPlus - a_xPlus - a_yPlus);

		return a_xy + xDir + yDir + xyDir;
	}

	template<class Type> Type Image<Type>::max() const
	{
		Type max;
		if(std::numeric_limits<Type>::is_integer) {
			max = std::numeric_limits<Type>::min(); }
		else {
			max = -std::numeric_limits<Type>::max(); }

		iterator e = end();
		for(iterator i = begin(); i != e; ++i) {
			if(*i > max) {
				max = *i; } }
		return max;
	}

	template<class Type> Type Image<Type>::min() const
	{
		Type min = std::numeric_limits<Type>::max();
		iterator e = end();
		for(iterator i = begin(); i != e; ++i) {
			if(*i < min) {
				min = *i; } }
		return min;
	}

	template<class Type> Type Image<Type>::sum() const
	{
		Type sum = Type(0);
		iterator e = end();
		for(iterator i = begin(); i != e; ++i) {
			sum += *i; }
		return sum;
	}

	template<class Type> Type Image<Type>::mean() const
	{
		Type s = sum();
		return s/(m_width*m_height);
	}

	template<class Type> Type Image<Type>::sd() const
	{
		Type sum = Type(0), m = mean(), diff;
		iterator e = end();
		for(iterator i = begin(); i != e; ++i)
		{
			diff = *i - m;
			sum += diff*diff;
		}
		return sqrt(sum/(m_width*m_height));
	}

	template<class Type> Image<Type> Image<Type>::genericUnary(Type (*func)(Type)) const
	{
		Image<Type> temp(*this, false);

		iterator e = end();
		iterator i = begin();
		iterator temp_i = temp.begin();
		for(; i != e; ++i, ++temp_i) {
			*temp_i = (*func)(*i); }

		return temp;
	}

	template<class Type> Image<Type> Image<Type>::genericBinary(const Image<Type> &im, Type (*func)(Type, Type)) const
	{
		Image<Type> temp(*this, false);

		iterator e = end();
		iterator i = begin();
		iterator temp_i = temp.begin();
		iterator im_i   = im.begin();
		for(; i != e; ++i, ++temp_i, ++im_i) {
			*temp_i = (*func)(*i, *im_i); }

		return temp;
	}

	template<class Type> Image<Type> Image<Type>::genericConvolution(Template<Type>& tem,
		typename convolution_iterator::merge_function mergeFunction,
		typename convolution_iterator::unity_function unityFunction) const
	{
		Image<Type> temp(*this, false);

		convolution_iterator i = cbegin(tem, mergeFunction, unityFunction);
		convolution_iterator e = cend();
		iterator temp_i = temp.begin();
		//iterator temp_e = temp.end();
		for(; i != e; ++i, ++temp_i) {
			*temp_i = *i; }

		return temp;
	}

	template<class Type> Image<Type> Image<Type>::genericConvolution(convolution_iterator& i) const
	{
		Image<Type> temp(*this, false);

		i.changeImage(this);

		convolution_iterator e = cend();
		iterator temp_i = temp.begin();
		//iterator temp_e = temp.end();
		for(; i != e; ++i, ++temp_i) {
			*temp_i = *i; }

		return temp;
	}

	//This is the right linear convolution product
	template<class Type> Image<Type> Image<Type>::operator+(Template<Type>& t) const
	{
		Image<Type> iNew(*this, false);

		MulSumIterator<Type> iter(this, &t);
		convolution_iterator iter_end = cend();
		//iterator end = iNew.end();
		for(iterator iterNew = iNew.begin(); iter != iter_end; ++iterNew, ++iter) {
			*iterNew = *iter; }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::domainTransform(void (*func)(double&, double&))
	{
		Image<Type> temp = *this;
		int i = 0;
		double xNew, yNew;
		iterator e = end();
		for(iterator iter = temp.begin(); iter != e; ++iter, i++)
		{
			// ******** Need to confirm this code (xNew and yNew). There was a bug here originally.
			xNew = i%m_width;
			yNew = i/m_height;
			(*func)(xNew, yNew);
			const Image<Type> *that = this;
			*iter = that->getPixel(xNew, yNew);
		}

		return temp;
	}

	template<class Type> Image<Type> Image<Type>::domainTransform(void (*func)(int&, int&))
	{
		Image<Type> temp = *this;
		int xNew, yNew, i = 0;
		iterator e = end();
		for(iterator iter = temp.begin(); iter != e; ++iter, i++)
		{
			xNew = i%m_width;
			yNew = i/m_height;
			(*func)(xNew, yNew);
			*iter = getPixel(xNew, yNew);
		}

		return temp;
	}

	template<class Type> Image<Type>& Image<Type>::resize(int width, int height, bool retain)
	{
		if(!retain || m_image == NULL)
		{
			freeImage(m_image);
			m_width = width;
			m_height = height;
			m_image = allocateImage();
			return (*this);
		}
		else
		{
			const Image<Type>* const_this = this;
			Image<Type> iNew(width, height, m_edgeHandling);
			int newWidthMax  = width - 1,  oldWidthMax  = m_width - 2;
			int newHeightMax = height - 1, oldHeightMax = m_height - 2;
			for(int y=0; y<height; y++)
			{
				for(int x=0; x<width; x++)
				{
					iNew.getPixel(int(x), int(y)) =
					const_this->getPixel( double(x*( oldWidthMax /(double)newWidthMax )),
										  double(y*( oldHeightMax/(double)newHeightMax )) );
				}
			}
			*this = iNew;
			return (*this);
		}
	}

	//Constructors/Destructor
	template<class Type> Image<Type>::Image(edge_handling eh)
	{
		m_width  = 0;
		m_height = 0;
		m_image  = NULL;
		m_edgeHandling = eh;
	}

	template<class Type> Image<Type>::Image(int width, int height, edge_handling eh)
	{
		m_width  = width;
		m_height = height;
		m_image  = allocateImage();
		m_edgeHandling = eh;
	}

	template<class Type> Image<Type>::Image(int width, int height, Type (*func)(int, int), edge_handling eh)
	{
		m_width  = width;
		m_height = height;
		m_image  = allocateImage();
		m_edgeHandling = eh;

		iterator iter = begin();
		iterator e = end();
		for(int i = 0; iter != e; ++iter, i++) {
			*iter = func(i%m_width, i/m_width); }
	}

	template<class Type> Image<Type>::Image(const Image<Type> &i, bool copy)
	{
		m_width  = i.m_width;
		m_height = i.m_height;
		m_image  = allocateImage();
		m_edgeHandling = i.edgeHandling();

		if(copy) {
			copyImage(m_image, i.m_image); }
	}

	template<class Type> Image<Type>::~Image()
	{
		freeImage(m_image);
	}

	//Array memory allocation
	template<class Type> Type* Image<Type>::allocateImage() throw(ImageException)
	{
		Type *im;
		try
		{
			im = new Type[m_width*m_height];
		}
		catch(std::bad_alloc &e)
		{
			std::stringstream msg_stream;

			msg_stream<<"Image::allocateImage [Error allocating memory for a "<<m_width<<"x"<<m_height<<" m_image:  "<<e.what()<<"]"<<std::endl;
			msg_stream<<"Image::allocateImage [Aborting...]";
			throw ImageException(msg_stream.str());
		}
		return im;
	}

	template<class Type> void Image<Type>::freeImage(Type *i)
	{
		if(i != NULL) {
			delete[] i; }
	}

	//This returns true if the image passed is equal to the calling image
	template<class Type> bool Image<Type>::equalTo(const Image<Type> &im) const
	{
		if(m_width == im.m_width && m_height == im.m_height)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				if(m_image[i] != im.m_image[i]) {
					return false; } }
			return true;
		}
		else {
			return false; }
	}

	//This returns true if every point in the image passed is equal to d
	template<class Type> bool Image<Type>::equalTo(const Type& d) const
	{
		for(iterator i = begin(); i != end(); ++i) {
			if(*i != d) {
				return false; } }
		return true;
	}

	//Operators
	template<class Type> Image<Type>& Image<Type>::operator=(const Image<Type> &im)
	{
		if(m_height != im.m_height || m_width != im.m_width)
		{
			freeImage(m_image);

			m_height = im.m_height;
			m_width  = im.m_width;

			m_image = allocateImage();
		}

		copyImage(m_image, im.m_image);

		m_edgeHandling = im.m_edgeHandling;

		return *this;
	}

	template<class Type> Image<Type>& Image<Type>::operator=(const Type& n)
	{
		if(m_image != NULL)
		{
			iterator e = end();
			for(iterator i = begin(); i != e; ++i) {
				*i = n; }
		}
		else {
			throw ImageException("Image::operator= [No image dimensions on assignment]"); }

		return *this;
	}

	template<class Type> Image<Type> Image<Type>::operator-() const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_image != NULL)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = -m_image[i]; }
		}
		else {
			throw ImageException("Image::operator- [No image dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type>& Image<Type>::operator+=(const Image<Type> &im)
	{
		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				m_image[i] += im.m_image[i]; }
		}
		else {
			throw ImageException("Image::operator+= [Unmatched dimensions on assignment]"); }

		return *this;
	}

	template<class Type> Image<Type> Image<Type>::operator+(const Image<Type> &im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = m_image[i] + im.m_image[i]; }
		}
		else {
			throw ImageException("Image::operator+ [Unmatched dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type>& Image<Type>::operator-=(const Image<Type> &im)
	{
		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				m_image[i] -= im.m_image[i]; }
		}
		else {
			throw ImageException("Image::operator-= [Unmatched dimensions on assignment]"); }

		return *this;
	}

	template<class Type> Image<Type> Image<Type>::operator-(const Image<Type> &im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = m_image[i] - im.m_image[i]; }
		}
		else {
			throw ImageException("Image::operator- [Unmatched dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type>& Image<Type>::operator*=(const Image<Type> &im)
	{
		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				m_image[i] *= im.m_image[i]; }
		}
		else {
			throw ImageException("Image::operator*= [Unmatched dimensions on assignment]"); }

		return *this;
	}

	template<class Type> Image<Type> Image<Type>::operator*(const Image<Type> &im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = m_image[i] * im.m_image[i]; }
		}
		else {
			throw ImageException("Image::operator* [Unmatched dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type>& Image<Type>::operator/=(const Image<Type> &im)
	{
		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++)
			{
				if(im.m_image[i] != Type(0)) {
					m_image[i] = m_image[i] / im.m_image[i]; }
				else {
					m_image[i] = 0; }
			}
		}
		else {
			throw ImageException("Image::operator/= [Unmatched dimensions for assignment]"); }

		return *this;
	}

	template<class Type> Image<Type> Image<Type>::operator/(const Image<Type> &im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++)
			{
				if(im.m_image[i] != Type(0)) {
					iNew.m_image[i] = m_image[i] / im.m_image[i]; }
				else {
					iNew.m_image[i] = 0; }
			}
		}
		else {
			throw ImageException("Image::operator/ [Unmatched dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type>& Image<Type>::operator+=(const Type& n)
	{
		iterator e = end();
		for(iterator i = begin(); i != e; ++i) {
			*i += n; }

		return *this;
	}

	template<class Type> Image<Type>& Image<Type>::operator-=(const Type& n)
	{
		iterator e = end();
		for(iterator i = begin(); i != e; ++i) {
			*i -= n; }

		return *this;
	}

	template<class Type> Image<Type>& Image<Type>::operator*=(const Type& n)
	{
		iterator e = end();
		for(iterator i = begin(); i != e; ++i) {
			*i *= n; }

		return *this;
	}

	template<class Type> Image<Type>& Image<Type>::operator/=(const Type& n)
	{
		iterator e = end();
		for(iterator i = begin(); i != e; ++i) {
			*i /= n; }

		return *this;
	}

	template<class Type> Image<Type> Image<Type>::operator+(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = m_image[i] + n; }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator-(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = m_image[i] - n; }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator*(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = m_image[i] * n; }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator/(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = m_image[i] / n; }

		return iNew;
	}

	//NOTE: Returns b(x,y) = (a(x,y) < i(x,y))?1:0
	template<class Type> Image<Type> Image<Type>::operator<(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = Type((m_image[i]<im.m_image[i])?1:0); }
		}
		else {
			throw ImageException("Image::operator< [Unmatched dimensions for operator]"); }

		return iNew;
	}

	//NOTE: Returns b(x,y) = (a(x,y) <= i(x,y))?1:0
	template<class Type> Image<Type> Image<Type>::operator<=(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = Type((m_image[i]<=im.m_image[i])?1:0); }
		}
		else {
			throw ImageException("Image::operator<= [Unmatched dimensions for operator]"); }

		return iNew;
	}

	//NOTE: Returns b(x,y) = (a(x,y) > i(x,y))?1:0
	template<class Type> Image<Type> Image<Type>::operator>(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = Type((m_image[i]>im.m_image[i])?1:0); }
		}
		else {
			throw ImageException("Image::operator> [Unmatched dimensions for operator]"); }

		return iNew;
	}

	//NOTE: Returns b(x,y) = (a(x,y) >= i(x,y))?1:0
	template<class Type> Image<Type> Image<Type>::operator>=(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = Type((m_image[i]>=im.m_image[i])?1:0); }
		}
		else {
			throw ImageException("Image::operator>= [Unmatched dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator==(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = Type((m_image[i]==im.m_image[i])?1:0); }
		}
		else {
			throw ImageException("Image::operator== [Unmatched dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator!=(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = Type((m_image[i]!=im.m_image[i])?1:0); }
		}
		else {
			throw ImageException("Image::operator!= [Unmatched dimensions for operator]"); }

		return iNew;
	}

	//NOTE: Returns a(x,y) = (i(x,y) < n)?1:0
	template<class Type> Image<Type> Image<Type>::operator<(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = Type((m_image[i]<n)?1:0); }

		return iNew;
	}

	//NOTE: Returns a(x,y) = (i(x,y) <= n)?1:0
	template<class Type> Image<Type> Image<Type>::operator<=(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = Type((m_image[i]<=n)?1:0); }

		return iNew;
	}

	//NOTE: Returns a(x,y) = (i(x,y) > n)?1:0
	template<class Type> Image<Type> Image<Type>::operator>(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = Type((m_image[i]>n)?1:0); }

		return iNew;
	}

	//NOTE: Returns a(x,y) = (i(x,y) >= n)?1:0
	template<class Type> Image<Type> Image<Type>::operator>=(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = Type((m_image[i]>=n)?1:0); }

		return iNew;
	}

	//NOTE: Returns a(x,y) = (i(x,y) == n)?1:0
	template<class Type> Image<Type> Image<Type>::operator==(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = Type((m_image[i]==n)?1:0); }

		return iNew;
	}

	//NOTE: Returns a(x,y) = (i(x,y) != n)?1:0
	template<class Type> Image<Type> Image<Type>::operator!=(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = Type((m_image[i]!=n)?1:0); }

		return iNew;
	}

	//These are min and max functions.
	//The resoning behind using these operators is from boolean algrebra
	//  where & is ^ and | is v.  Therefore & is min and | is max.

	//right multiplicative max convolution product
	template<class Type> Image<Type> Image<Type>::operator|(Template<Type> &t) const
	{
		Image<Type> iNew(*this, false);

		MulMaxIterator<Type> iter(this, &t);
		convolution_iterator iter_end = cend();
		//iterator end = iNew.end();
		for(iterator iterNew = iNew.begin(); iter != iter_end; ++iterNew, ++iter) {
			*iterNew = *iter; }

		return iNew;
	}

	//right multiplicative min convolution product
	template<class Type> Image<Type> Image<Type>::operator&(Template<Type> &t) const
	{
		Image<Type> iNew(*this, false);

		MulMinIterator<Type> iter(this, &t);
		convolution_iterator iter_end = cend();
		//iterator end = iNew.end();
		for(iterator iterNew = iNew.begin(); iter != iter_end; ++iterNew, ++iter) {
			*iterNew = *iter; }

		return iNew;
	}

	template<class Type> Image<Type>& Image<Type>::operator|=(const Image<Type>& im)
	{
		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				m_image[i] = (m_image[i] < im.m_image[i])?im.m_image[i]:m_image[i]; }
		}
		else {
			throw ImageException("Image::operator|= [Unmatched dimensions for operator]"); }

		return *this;
	}

	template<class Type> Image<Type>& Image<Type>::operator&=(const Image<Type>& im)
	{
		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				m_image[i] = (m_image[i] > im.m_image[i])?im.m_image[i]:m_image[i]; }
		}
		else {
			throw ImageException("Image::operator&= [Unmatched dimensions for operator]"); }

		return *this;
	}

	template<class Type> Image<Type> Image<Type>::operator|(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
			int loopLength = m_width*m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = (m_image[i] < im.m_image[i])?im.m_image[i]:m_image[i]; }
		}
		else {
			throw ImageException("Image::operator| [Unmatched dimensions for operator]"); }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator|(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = (m_image[i] < n)?n:m_image[i]; }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator&(const Image<Type>& im) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		if(m_height == im.m_height && m_width == im.m_width)
		{
		int loopLength = im.m_width*im.m_height;
			for(int i=0; i<loopLength; i++) {
				iNew.m_image[i] = (m_image[i] > im.m_image[i])?im.m_image[i]:m_image[i]; }
		}
		else {
			throw ImageException("Image::operator& [Unmatched dimensions on & operator]"); }

		return iNew;
	}

	template<class Type> Image<Type> Image<Type>::operator&(const Type& n) const
	{
		Image<Type> iNew(m_width, m_height, m_edgeHandling);

		int loopLength = m_width*m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = (m_image[i] > n)?n:m_image[i]; }

		return iNew;
	}

	//Friendly arithmetic operations
	template<class Type> Image<Type> operator&(const Type& n, const Image<Type>& im)
	{
		Image<Type> iNew(im.m_width, im.m_height, im.m_edgeHandling);

		int loopLength = im.m_width*im.m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = (im.m_image[i] > n)?n:im.m_image[i]; }

		return iNew;
	}

	template<class Type> Image<Type> operator|(const Type& n, const Image<Type>& im)
	{
		Image<Type> iNew(im.m_width, im.m_height, im.m_edgeHandling);

		int loopLength = im.m_width*im.m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = (im.m_image[i] < n)?n:im.m_image[i]; }

		return iNew;
	}

	template<class Type> Image<Type> operator+(const Type& n, const Image<Type>& im)
	{
		Image<Type> iNew(im.m_width, im.m_height, im.m_edgeHandling);

		int loopLength = im.m_width*im.m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = n + im.m_image[i]; }

		return iNew;
	}

	template<class Type> Image<Type> operator-(const Type& n, const Image<Type>& im)
	{
		Image<Type> iNew(im.m_width, im.m_height, im.m_edgeHandling);

		int loopLength = im.m_width*im.m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = n - im.m_image[i]; }

		return iNew;
	}

	template<class Type> Image<Type> operator*(const Type& n, const Image<Type>& im)
	{
		Image<Type> iNew(im.m_width, im.m_height, im.m_edgeHandling);

		int loopLength = im.m_width*im.m_height;
		for(int i=0; i<loopLength; i++) {
			iNew.m_image[i] = n * im.m_image[i]; }

		return iNew;
	}

	template<class Type> Image<Type> operator/(const Type& n, const Image<Type>& im)
	{
		Image<Type> iNew(im.m_width, im.m_height, im.m_edgeHandling);

		int loopLength = im.m_width*im.m_height;
		for(int i=0; i<loopLength; i++)
		{
			if(im.m_image[i] != 0) {
				iNew.m_image[i] = n / im.m_image[i]; }
			else {
				iNew.m_image[i] = 0; }
		}

		return iNew;
	}
}  // end namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class Image<char>;
	template class Image<short>;
	template class Image<int>;
	template class Image<long>;
	template class Image<float>;
	template class Image<double>;

	template Image<char> operator+(const char& left, const Image<char>& right);
	template Image<char> operator-(const char& left, const Image<char>& right);
	template Image<char> operator*(const char& left, const Image<char>& right);
	template Image<char> operator/(const char& left, const Image<char>& right);
	template Image<char> operator|(const char& left, const Image<char>& right);
	template Image<char> operator&(const char& left, const Image<char>& right);

	template Image<short> operator+(const short& left, const Image<short>& right);
	template Image<short> operator-(const short& left, const Image<short>& right);
	template Image<short> operator*(const short& left, const Image<short>& right);
	template Image<short> operator/(const short& left, const Image<short>& right);
	template Image<short> operator|(const short& left, const Image<short>& right);
	template Image<short> operator&(const short& left, const Image<short>& right);

	template Image<int> operator+(const int& left, const Image<int>& right);
	template Image<int> operator-(const int& left, const Image<int>& right);
	template Image<int> operator*(const int& left, const Image<int>& right);
	template Image<int> operator/(const int& left, const Image<int>& right);
	template Image<int> operator|(const int& left, const Image<int>& right);
	template Image<int> operator&(const int& left, const Image<int>& right);

	template Image<long> operator+(const long& left, const Image<long>& right);
	template Image<long> operator-(const long& left, const Image<long>& right);
	template Image<long> operator*(const long& left, const Image<long>& right);
	template Image<long> operator/(const long& left, const Image<long>& right);
	template Image<long> operator|(const long& left, const Image<long>& right);
	template Image<long> operator&(const long& left, const Image<long>& right);

	template Image<float> operator+(const float& left, const Image<float>& right);
	template Image<float> operator-(const float& left, const Image<float>& right);
	template Image<float> operator*(const float& left, const Image<float>& right);
	template Image<float> operator/(const float& left, const Image<float>& right);
	template Image<float> operator|(const float& left, const Image<float>& right);
	template Image<float> operator&(const float& left, const Image<float>& right);

	template Image<double> operator+(const double& left, const Image<double>& right);
	template Image<double> operator-(const double& left, const Image<double>& right);
	template Image<double> operator*(const double& left, const Image<double>& right);
	template Image<double> operator/(const double& left, const Image<double>& right);
	template Image<double> operator|(const double& left, const Image<double>& right);
	template Image<double> operator&(const double& left, const Image<double>& right);
}
#endif

#endif