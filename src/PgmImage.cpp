#ifndef __PGMIMAGE_CPP__
#define __PGMIMAGE_CPP__

#include "PgmImage.h"

namespace ImageTL
{
	//File io and header info
	template<class Type> void PgmImage<Type>::readHeader(const char *file) throw(ImageException)
	{
		char input[200], cinput;
		size_t commentCount = 0;

		std::ifstream fin(file, std::ios::in);
		if(!fin) {
			throw ImageException((std::string("PgmImage::readHeader [Error reading header in ") + file) + "]"); }

		//Read the magic number
		fin>>input;

		if(std::char_traits<char>::compare(input, "P5", 2) != 0) {
			throw ImageException("PgmImage::readHeader [Invalid file format]"); }

		//Read the m_width
		do
		{
			fin>>input;
			if(std::char_traits<char>::compare(input, "#", 1) != 0)
			{
				if((this->m_width = atoi(input)) <= 0) {
					throw ImageException("PgmImage::readHeader [Invalid file format]"); }
			}
			else
			{
				commentCount += std::char_traits<char>::length(input);
				do {
					fin.get(cinput);
					commentCount++;
				} while(!(std::char_traits<char>::eq(cinput, 10)));
			}
		} while(this->m_width == 0);

		this->m_headerLength = fin.tellg();

		//Read the height
		do
		{
			fin>>input;
			if(std::char_traits<char>::compare(input, "#", 1) != 0)
			{
				if((this->m_height = atoi(input)) <= 0) {
					throw ImageException("PgmImage::readHeader [Invalid file format]"); }
			}
			else
			{
				commentCount += std::char_traits<char>::length(input);
				do {
					fin.get(cinput);
					commentCount++;
				} while(!(std::char_traits<char>::eq(cinput, 10)));
			}
		} while(this->m_height == 0);

		this->m_headerLength = fin.tellg();

		//Read the depth
		do
		{
			fin>>input;
			if(std::char_traits<char>::compare(input, "#", 1) != 0)
			{
				this->m_depth = atoi(input);
				if(this->m_depth <= 0 || this->m_depth > 65535) {
					throw ImageException("PgmImage::readHeader [Invalid file format]"); }
			}
			else
			{
				commentCount += std::char_traits<char>::length(input);
				do {
					fin.get(cinput);
					commentCount++;
				} while(!(std::char_traits<char>::eq(cinput, 10)));
			}
		} while(this->m_depth == 0);

		//Read the final \n
		/*fin.get(cinput);
		if(!(std::char_traits<char>::eq(cinput, '\n') || std::char_traits<char>::eq(cinput, '\r') ||
			std::char_traits<char>::eq(cinput, '\t') || std::char_traits<char>::eq(cinput, '\v') ||
			std::char_traits<char>::eq(cinput, ' '))) { throw ImageException("PgmImage: Invalid file format"); }*/

		//this->m_headerLength = fin.tellg();
		this->m_headerLength = (long)log10((double)this->m_width) +
							   (long)log10((double)this->m_height) +
							   (long)log10((double)this->m_depth) +
							   (long)commentCount + 9;
	}

	template<class Type> void PgmImage<Type>::readData(const char *file) throw(ImageException)
	{
		try
		{
			std::ifstream fin(file, std::ios_base::in | std::ios_base::binary);
			if(!fin) {
				throw ImageException((std::string("PgmImage::readData [Error reading data in ") + file) + "]"); }

			// Allocate memory for the image
			this->m_image = this->allocateImage();

			// Skip the header in the pgm file
			fin.seekg(this->m_headerLength);

			// Allocate temporary memory to read the data into
			int dataLength = ((this->m_depth > 255)?2:1)*(this->m_width*this->m_height);
			unsigned char* imageData = new unsigned char[dataLength];
			unsigned char* data_i = imageData;

			// Read the data
			fin.read((char*)imageData, dataLength);

			// Convert the unsigned chars into the proper data type
			typename Image<Type>::iterator e = this->end();
			for(typename Image<Type>::iterator i = this->begin(); i != e; ++i)
			{
				if(this->m_depth > 255)
				{
					*i = Type(*(data_i++) * 256);
					*i += Type( *(data_i++) );
				}
				else {
					*i = Type( *(data_i++) ); }
			}

			delete[] imageData;
		}
		catch(std::bad_alloc &e)
		{
			std::cout<<"PgmImage::readData [Error allocating memory for a "<<this->m_width<<" x "<<this->m_height<<" image:  "<<e.what()<<"]"<<std::endl;
			throw ImageException("PgmImage::readData [Aborting...]");
		}
	}

	template<class Type> void PgmImage<Type>::writeHeader(const char *file, const char *comment) throw()
	{
		std::ofstream fout(file, std::ios_base::out | std::ios_base::trunc);
		if(!fout)
		{
			std::cout<<"PgmImage::writeHeader [Error opening "<<file<<" for writing]";
			return;
		}

		fout<<"P5 ";
		if(std::char_traits<char>::compare(comment, "", 1) != 0) {
			fout<<"# "<<comment<<"\n"; }
		fout<<this->m_width<<" "<<this->m_height<<" "<<this->m_depth<<" ";
	}

	template<class Type> void PgmImage<Type>::writeData(const char *file) throw(ImageException)
	{
		try
		{
			std::ofstream fout(file, std::ios_base::out | std::ios_base::binary | std::ios_base::app);
			if(!fout)
			{
				std::cout<<"PgmImage::writeData [Error opening "<<file<<" for writing]"<<std::endl;
				return;
			}

			// Allocate temporary memory to read the data into
			int dataLength = ((this->m_depth > 255)?2:1)*(this->m_width*this->m_height);
			unsigned char* imageData = new unsigned char[dataLength];
			unsigned char* data_i = imageData;

			typename Image<Type>::iterator e = this->end();
			for(typename Image<Type>::iterator i = this->begin(); i != e; ++i)
			{
				if(this->m_depth > 255)
				{
					*(data_i++) = (char)(*i/256);
					*(data_i++) = (char)((int)(*i) % 256);
				}
				else {
					*(data_i++) = (char)(*i); }
			}

			// Read the data
			fout.write((char*)imageData, dataLength);

			delete[] imageData;
		}
		catch(std::bad_alloc &e)
		{
			std::cout<<"PgmImage::writeData [Error allocating memory for a "<<this->m_width<<" x "<<this->m_height<<" image:  "<<e.what()<<"]"<<std::endl;
			throw ImageException("PgmImage::writeData [Aborting...]");
		}
	}

	// Operators
	template<class Type> PgmImage<Type>& PgmImage<Type>::operator=(const Image<Type>& im)
	{
		/*if(this->m_height != im.height() || this->m_width != im.width())
		{
			freeImage(this->m_image);

			this->m_height = im.height();
			this->m_width  = im.width();

			this->m_image = this->allocateImage();
		}

		typename Image<Type>::iterator e = this->end();
		typename Image<Type>::iterator iterCopy = im.begin();
		for(typename Image<Type>::iterator iter = this->begin(); iter != e; ++iter, ++iterCopy) {
			*iter = *iterCopy; }*/

		Image<Type>::operator=(im);

		return *this;
	}

	template<class Type> PgmImage<Type>& PgmImage<Type>::operator=(const ImageIO<Type>& im)
	{
		/*if(this->m_height != im.m_height || this->m_width != im.m_width)
		{
			freeImage(this->m_image);

			this->m_height = im.m_height;
			this->m_width  = im.m_width;

			this->m_image = this->allocateImage();
		}

		this->m_depth = im.m_depth;
		this->m_headerLength = 0;
		this->m_depth_h = im.m_depth_h;

		copyImage(this->m_image, im.m_image);*/

		ImageIO<Type>::operator=(im);

		return *this;
	}

	template<class Type> PgmImage<Type>& PgmImage<Type>::operator=(Type n)
	{
		/*if(this->m_image != NULL)
		{
			typename Image<Type>::iterator e = this->end();
			for(typename Image<Type>::iterator i = this->begin(); i != e; ++i) {
				*i = n; }
		}
		else {
			throw ImageException("PgmImage::operator= [No image dimensions on assignment]"); }*/

		Image<Type>::operator=(n);

		return *this;
	}
}	//End namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class PgmImage<char>;
	template class PgmImage<short>;
	template class PgmImage<int>;
	template class PgmImage<long>;
	template class PgmImage<float>;
	template class PgmImage<double>;
}
#endif

#endif