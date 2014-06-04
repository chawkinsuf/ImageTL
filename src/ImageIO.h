#ifndef __IMAGEIO_H__
#define __IMAGEIO_H__

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include "Image.h"

namespace ImageTL
{
	// This forces the input to a long double so hopefully it can hanldle any type
	// for which a casting operator is defined to convert Type to a long double.
	// long double is used so that this function will be able to support an image
	// of the largerst data size.
	template<class Type> inline Type ROUND(Type x)
	{
		long double int_part, adjust = (x>0.)?((long double)x+0.5L):((long double)x-0.5L);
		modfl(adjust, &int_part);
		return Type(int_part);
	}

	typedef int depth_handling;
	//- Used for error checking the depth of each pixel in the image.
	//- First the lower limit is adjusted and then the upper limit.
	//- One (and ONLY one) of the lower and first upper options MUST be set.
	//- Upper limit option 1 is for a max pixel value greater than depth.
	//- Upper limit option 2 is for a max pixel value less than depth and is
	//  not required to have an option set if you would like to allow a max
	//  pixel value less than the depth.  It is checked after the first option
	//  and can be set with option 1.  If you would like to set an option for
	//  this only set one.
	//*************************************************************************
	//************** Lower limit options, only set one of these. **************
	//lower_translate - If this is set (and min < 0) each pixel will get
	//            (min<0)?-min:0 added to it
	//lower_truncate - If this is set any pixel < 0 will be set to 0.
	//lower_abs - If this is set any pixel < 0 will be set to -pixel.
	//*************************************************************************
	//************* Upper limit option 1, only set one of these. **************
	//upper_increase - If this is set (and max > depth) the depth of the actual
	//                image will be increased to match the maximum pixel value.
	//                If the depth gets to 65535 the pixels will then be
	//                truncated.
	//upper_scale - If this is set (and max > depth) each pixel will be
	//               muliplied by depth/max.
	//upper_truncate - If this is set any pixel > depth will be set to depth.
	//*************************************************************************
	//************************* Upper limit option 2 **************************
	//upper2_decrease - If this is set (and max < depth) the depth of the image
	//                will be set to max.
	//upper2_stretch - If this is set (and max < depth) each pixel will be
	//                  multiplied by depth/max.
	enum depthHandling
	{
		// *** Lower limit options, only set one of these. ***
		lower_translate = 0x0001,
		lower_truncate = 0x0002,
		lower_abs = 0x0004,

		// *** Upper limit options, only set one of these. ***
		upper_increase = 0x0100,
		upper_scale = 0x0200,
		upper_truncate = 0x0400,

		// *** Upper limit option 2, only set one of these. ***
		upper2_decrease = 0x0800,
		upper2_stretch = 0x1000
	};

	template<class Type> class ImageIO : public Image<Type>
	{
	public:
		//File io and header info
		virtual void readHeader(const char *file) throw(ImageException) = 0;
		void read( const char *file);
		void write(const char *file, int m_depth = 0, const char *comment = "");
		int& depth() { return m_depth; }
		depth_handling& depthHandling() { return m_depth_h; }

		//This allocates and populates the histogram structure in the class.
		//Note that the pixel depth check is forced when this function is called.
		void histogram();

		//Data manipulation functions that DO alter the image
		//This forces the pixel depth and then rounds the image to integers
		void writePrepare();
		//First calls the histogram() function and then uses that to equalize the image
		void histogramEqualize();

		//Data manipulation functions that MIGHT alter the image
		void checkPixelDepth() throw(ImageException);

		//Operator that can be called from derived class's implementations
		ImageIO<Type>& operator=(const ImageIO<Type>& im);

		//Constructors/Destructor
		ImageIO();
		ImageIO(const ImageIO &i, bool copy = true);
		ImageIO(const char *file, depth_handling dh = upper_scale | lower_translate);
		ImageIO(int w, int h, int d, depth_handling dh = upper_scale | lower_translate);
		ImageIO(const Image<Type> &i, bool copy = true, int d = 255, depth_handling dh = upper_scale | lower_translate);
		~ImageIO();

	protected:
		//File io
		virtual void readData(   const char *file) throw(ImageException) = 0;
		virtual void writeHeader(const char *file, const char *comment) throw(ImageException) = 0;
		virtual void writeData(  const char *file) throw(ImageException) = 0;

		//Data members
		int m_depth, *m_hist;

		std::istream::pos_type m_headerLength;
		depth_handling m_depth_h;
	};
}	//End namespace

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "ImageIO.cpp"
#endif

#endif