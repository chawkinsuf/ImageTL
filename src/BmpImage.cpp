#ifndef __BMPIMAGE_CPP__
#define __BMPIMAGE_CPP__

#include "BmpImage.h"
#include <iomanip>

namespace ImageTL
{
	//File io and header info
	template<class Type> void BmpImage<Type>::printVals()
	{
		std::cout << "m_bfh.bfType = " << m_bfh.bfType << std::endl;
		std::cout << "m_bfh.bfSize = " << m_bfh.bfSize << std::endl;
		std::cout << "m_bfh.bfReserved1 = " << m_bfh.bfReserved1 << std::endl;
		std::cout << "m_bfh.bfReserved2 = " << m_bfh.bfReserved2 << std::endl;
		std::cout << "m_bfh.bfOffBits = " << m_bfh.bfOffBits << std::endl;

		std::cout << "m_bih.biSize = " << m_bih.biSize << std::endl;
		std::cout << "m_bih.biWidth = " << m_bih.biWidth << std::endl;
		std::cout << "m_bih.biHeight = " << m_bih.biHeight << std::endl;
		std::cout << "m_bih.biPlanes = " << m_bih.biPlanes << std::endl;
		std::cout << "m_bih.biBitCount = " << m_bih.biBitCount << std::endl;
		std::cout << "m_bih.biSizeImage = " << m_bih.biSizeImage << std::endl;
		std::cout << "m_bih.biXPelsPerMeter = " << m_bih.biXPelsPerMeter << std::endl;
		std::cout << "m_bih.biYPelsPerMeter = " << m_bih.biYPelsPerMeter << std::endl;
		std::cout << "m_bih.biClrUsed = " << m_bih.biClrUsed << std::endl;
		std::cout << "m_bih.biClrImportant = " << m_bih.biClrImportant << std::endl;
	}

	template<class Type> void BmpImage<Type>::createDefaultHeader() throw(ImageException)
	{
		m_bih.biSize			=	40;
		m_bih.biWidth			=	this->m_width;
		m_bih.biHeight			=	this->m_height;
		m_bih.biPlanes			=	1;
		m_bih.biBitCount		=	8;
		m_bih.biCompression		=	0;
		m_bih.biSizeImage		=	this->m_width * this->m_height;
		m_bih.biXPelsPerMeter	=	0;
		m_bih.biYPelsPerMeter	=	0;
		m_bih.biClrUsed			=	256;
		m_bih.biClrImportant	=	0;

		m_bfh.bfType			=	0x4D42;
		m_bfh.bfOffBits			=	(14 + 40 + 1024);//sizeof(m_bfh) + m_bih.biSize + sizeof(g_colorTable);
		m_bfh.bfSize			=	m_bfh.bfOffBits + m_bih.biSizeImage*(m_bih.biBitCount/8);
		m_bfh.bfReserved1		=	0;
		m_bfh.bfReserved2		=	0;
	}

	template<class Type> void BmpImage<Type>::readHeader(const char *file) throw(ImageException)
	{
		std::ifstream fin(file, std::ios::in | std::ios::binary);
		if(!fin) {
			throw ImageException((std::string("BmpImage::readHeader [Error reading header in ") + file) + "]"); }

		//read bitmap file header
		//fin.read((char*)&m_bfh, sizeof(m_bfh));

		//read bitmap info header
		//fin.read((char*)&m_bih, sizeof(m_bih));

		fin.read((char*)&m_bfh.bfType, sizeof(m_bfh.bfType));
		fin.read((char*)&m_bfh.bfSize, sizeof(m_bfh.bfSize));
		fin.read((char*)&m_bfh.bfReserved1, sizeof(m_bfh.bfReserved2));
		fin.read((char*)&m_bfh.bfReserved2, sizeof(m_bfh.bfReserved2));
		fin.read((char*)&m_bfh.bfOffBits, sizeof(m_bfh.bfOffBits));

		fin.read((char*)&m_bih.biSize, sizeof(m_bih.biSize));
		fin.read((char*)&m_bih.biWidth, sizeof(m_bih.biWidth));
		fin.read((char*)&m_bih.biHeight, sizeof(m_bih.biHeight));
		fin.read((char*)&m_bih.biPlanes, sizeof(m_bih.biPlanes));
		fin.read((char*)&m_bih.biBitCount, sizeof(m_bih.biBitCount));
		fin.read((char*)&m_bih.biCompression, sizeof(m_bih.biCompression));
		fin.read((char*)&m_bih.biSizeImage, sizeof(m_bih.biSizeImage));
		fin.read((char*)&m_bih.biXPelsPerMeter, sizeof(m_bih.biXPelsPerMeter));
		fin.read((char*)&m_bih.biYPelsPerMeter, sizeof(m_bih.biYPelsPerMeter));
		fin.read((char*)&m_bih.biClrUsed, sizeof(m_bih.biClrUsed));
		fin.read((char*)&m_bih.biClrImportant, sizeof(m_bih.biClrImportant));

		//throw ImageException("BmpImage::readHeader [Only 8 bit bitmaps supported!]");

		if(m_bih.biBitCount != 8) {
			throw ImageException("BmpImage::readHeader [Only 8 bit bitmaps supported!]"); }

		//set header length, assumed in bytes
		this->m_headerLength = (14 + 40 + 1024);
		this->m_depth  = (1 << m_bih.biBitCount) - 1;
		this->m_height = m_bih.biHeight;
		this->m_width  = m_bih.biWidth;

	}

	template<class Type> void BmpImage<Type>::readData(const char *file) throw(ImageException)
	{
		try
		{
			std::ifstream fin(file, std::ios::in | std::ios::binary);
			if(!fin) {
				throw ImageException((std::string("BmpImage::readData [Error reading data in ") + file) + "]"); }

			// Allocate memory for the image
			this->m_image = this->allocateImage();

			// Skip the header in the bmp file
			fin.seekg(this->m_headerLength);

			// Allocate temporary memory to read the data into
			bool isUpsideDown = true;
			if(this->m_height < 0)
			{
				m_bih.biHeight = -m_bih.biHeight;
				this->m_height = -this->m_height;
				isUpsideDown   = false;
			}

			int paddedWidth = this->m_width;
			if(this->m_width%4 != 0) {
				paddedWidth = this->m_width + 4 - this->m_width%4; }

			int dataLength = (paddedWidth*this->m_height);
			unsigned char* imageData = new unsigned char[dataLength];

			// Read the data
			fin.read((char*)imageData, dataLength);

			// Convert the unsigned chars into the proper data type
			if(isUpsideDown)
			{
				typename Image<Type>::iterator i = this->begin();
				for(int y = this->m_height - 1; y >= 0; y--)
				{
					for(int x = 0; x < paddedWidth; x++)
					{
						if(x >= this->m_width) {
							break; }

						*(i++) = Type( imageData[y*paddedWidth + x] );
					}
				}
			}
			else
			{
				typename Image<Type>::iterator i = this->begin();
				for(int y = 0; y < this->m_height; y++)
				{
					for(int x = 0; x < paddedWidth; x++)
					{
						if(x >= this->m_width) {
							break; }

						*(i++) = Type( imageData[y*paddedWidth + x] );
					}
				}
			}

			delete[] imageData;
		}
		catch(std::bad_alloc &e)
		{
			std::cout<<"BmpImage::readData [Error allocating memory for a "<<this->m_width<<" x "<<this->m_height<<" image:  "<<e.what()<<"]"<<std::endl;
			throw ImageException("BmpImage::readData [Aborting...]");
		}
	}

	template<class Type> void BmpImage<Type>::writeHeader(const char *file, const char *comment) throw(ImageException)
	{

		if(m_bih.biBitCount != 8) {
			throw ImageException("BmpImage::writeHeader [Only 8 bit bitmaps supported!]"); }

		std::ofstream fout(file, std::ios::out | std::ios::binary | std::ios::trunc);
		if(!fout)
		{
			std::stringstream msg_stream;
			msg_stream<<"BmpImage::writeHeader [Error opening " << file << " for writing]<< endl";
			throw ImageException(msg_stream.str());
		}

		//Write bitmap file header
		fout.write((char*)&m_bfh.bfType,		sizeof(m_bfh.bfType));
		fout.write((char*)&m_bfh.bfSize,		sizeof(m_bfh.bfSize));
		fout.write((char*)&m_bfh.bfReserved1,	sizeof(m_bfh.bfReserved2));
		fout.write((char*)&m_bfh.bfReserved2,	sizeof(m_bfh.bfReserved2));
		fout.write((char*)&m_bfh.bfOffBits,		sizeof(m_bfh.bfOffBits));

		fout.write((char*)&m_bih.biSize,		sizeof(m_bih.biSize));
		fout.write((char*)&m_bih.biWidth,		sizeof(m_bih.biWidth));
		fout.write((char*)&m_bih.biHeight,		sizeof(m_bih.biHeight));
		fout.write((char*)&m_bih.biPlanes,		sizeof(m_bih.biPlanes));
		fout.write((char*)&m_bih.biBitCount,	sizeof(m_bih.biBitCount));
		fout.write((char*)&m_bih.biCompression, sizeof(m_bih.biCompression));
		fout.write((char*)&m_bih.biSizeImage,	sizeof(m_bih.biSizeImage));
		fout.write((char*)&m_bih.biXPelsPerMeter, sizeof(m_bih.biXPelsPerMeter));
		fout.write((char*)&m_bih.biYPelsPerMeter, sizeof(m_bih.biYPelsPerMeter));
		fout.write((char*)&m_bih.biClrUsed,		sizeof(m_bih.biClrUsed));
		fout.write((char*)&m_bih.biClrImportant, sizeof(m_bih.biClrImportant));
		fout.write((char*)g_colorTable,			sizeof(g_colorTable));
	}

	template<class Type> void BmpImage<Type>::writeData(const char *file) throw(ImageException)
	{
		if(m_bih.biBitCount != 8) throw ImageException("BmpImage::writeData [Only 8 bit bitmaps allowed!]");

		try
		{
			std::ofstream fout(file, std::ios::out | std::ios::binary | std::ios::app);
			if(!fout)
			{
				std::stringstream msg_stream;
				msg_stream<<"BmpImage::writeData [Error opening "<<file<<" for writing]";
				throw ImageException(msg_stream.str());
			}

			// Allocate temporary memory to read the data into
			int paddedWidth = this->m_width;
			if(this->m_width%4 != 0) {
				paddedWidth = this->m_width + 4 - this->m_width%4; }

			int dataLength = (paddedWidth * this->m_height);
			unsigned char* imageData = new unsigned char[dataLength];

			typename Image<Type>::iterator i = this->begin();
			for(int y = this->m_height - 1; y >= 0; y--)
			{
				for(int x = 0; x < paddedWidth; x++)
				{
					if(x >= this->m_width) {
						imageData[y*paddedWidth + x] = char(0); }
					else {
						imageData[y*paddedWidth + x] = char( *(i++) ); }
				}
			}

			// write the data
			fout.write((char*)imageData, dataLength);

			delete[] imageData;
		}
		catch(std::bad_alloc &e)
		{
			std::stringstream msg_stream;
			msg_stream<<"BmpImage::writeData [Error allocating memory for a "<<this->m_width<<" x "<<this->m_height<<" image:  "<<e.what()<<"]";
			throw ImageException(msg_stream.str());
		}
	}

	// Operators
	template<class Type> BmpImage<Type>& BmpImage<Type>::operator=(const Image<Type>& im)
	{
		Image<Type>::operator=(im);

		createDefaultHeader();

		return *this;
	}

	template<class Type> BmpImage<Type>& BmpImage<Type>::operator=(const ImageIO<Type>& im)
	{

		//if(im.m_depth != 255) {
		//	throw ImageException("BmpImage::operator=(const ImageIO& im) [Only 8 bit bitmaps supported! ]"); }

		ImageIO<Type>::operator=(im);

		createDefaultHeader();

		return *this;
	}

	template<class Type> BmpImage<Type>& BmpImage<Type>::operator=(Type n)
	{
		Image<Type>::operator=(n);

		return *this;
	}
}	//End namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class BmpImage<char>;
	template class BmpImage<short>;
	template class BmpImage<int>;
	template class BmpImage<long>;
	template class BmpImage<float>;
	template class BmpImage<double>;
}
#endif

#endif