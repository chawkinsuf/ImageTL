#ifndef __PGMIMAGE_H__
#define __PGMIMAGE_H__

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include "ImageIO.h"

namespace ImageTL
{
	template<class Type> class PgmImage : public ImageIO<Type>
	{
	public:
		// File io
		void readHeader(const char *file) throw(ImageException);

		// Constructors
		PgmImage() : ImageIO<Type>() {}
		PgmImage(const PgmImage &i, bool copy = true) : ImageIO<Type>(i, copy) {}
		PgmImage(const ImageIO<Type> &i, bool copy = true) : ImageIO<Type>(i, copy) {}
		PgmImage(const char *file, depth_handling dh = upper_scale | lower_translate) : ImageIO<Type>(0, 0, 0, dh) { this->read(file); }
		PgmImage(int w, int h, int d, depth_handling dh = upper_scale | lower_translate) : ImageIO<Type>(w, h, d, dh) {}
		PgmImage(const Image<Type> &i, bool copy = true, int d = 255, depth_handling dh = upper_scale | lower_translate) : ImageIO<Type>(i, copy, d, dh) {}

		// Operators (= operator is not inherited)
		PgmImage& operator=(const Image<Type>&);
		PgmImage& operator=(const ImageIO<Type>&);
		PgmImage& operator=(Type);

	protected:
		// File io
		void readData(const char *file) throw(ImageException);
		void writeHeader(const char *file, const char *comment) throw();
		void writeData(const char *file) throw(ImageException);
	};
}	// End namespace

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "PgmImage.cpp"
#endif

#endif