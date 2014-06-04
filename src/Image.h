#ifndef __IMAGE_H__
#define __IMAGE_H__
/** @file Image.h
	The core of the %Image Processing Library.
	This header contains the declaration of the Image class, which contains the
	core image algebra operations needed to work with images.
*/
/** @mainpage
	<center><h2>An Image Processing Library</h2></center>
	@author Chris Hawkins
	@version 2.0
	@date ©2002-2003
*/
/** @example Example_Image_Simple.cpp
	This is an example for the basic use of ImageTL::Image.
*/

#pragma warning( disable : 4290 )

namespace ImageTL {	template<class Type> class Image; }

#include <limits>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "ImageException.h"
#include "Template.h"
#include "ImageIterator.h"
#include "ConvolutionIterator.h"
#include "CommonIterators.h"

/** @namespace ImageTL
	Contains all the classes and functions of the %Image Processing Library.
*/
namespace ImageTL
{
	template<class Type> class Image;
	template<class Type> Image<Type> operator+(const Type& left, const Image<Type>& right);
	template<class Type> Image<Type> operator-(const Type& left, const Image<Type>& right);
	template<class Type> Image<Type> operator*(const Type& left, const Image<Type>& right);
	template<class Type> Image<Type> operator/(const Type& left, const Image<Type>& right);
	template<class Type> Image<Type> operator|(const Type& left, const Image<Type>& right);
	template<class Type> Image<Type> operator&(const Type& left, const Image<Type>& right);

	/** Definitions used to specify the edge handling options.
		Used by getPixel() to determine how to handle memory access that goes
		beyond the image boundary.  This is mostly important for functions that
		calculate the convolution of the image and a template.

		@note Only the const getPixel() will use the edge_handling since a
			reference cannot be returned if the value is not inside the image
			boundary.
		@see getPixel()
	*/
	enum edge_handling
	{
		edge_skip,	/*!< This will simply make getPixel() thorw an
					ImageException when attempting to access out of range
					values.*/
		edge_clamp,	/*!< This will replace any out of bounds access with the
					closest pixel inside the image boundry.*/
		edge_zero	/*!< This will simply replace any out of bounds access with a
					zero.*/
	};

	/** @class Image
		The core of the %Image Processing Library.
		The purpose of this class is to hide the implementation of every
		possible image algebra operation.  The class contains overloaded
		operators for pixel-wise operations and template convolutions.  It has
		built-in interfaces that are designed to be used with the Template and
		ImageIterator classes.  It also has generic operations to which function
		pointers can be passed.  All of this together allows the class to handle
		any	conceivable	functionality in a smooth, convenient, and elegant
		fashion.

		@note The resoning behind the minimum and maximum operators is the
			relationship between | and & with boolean algebra, where | is a
			symbol for union (v) and & is a symbol for intersection (^).

		@section image_type Template Type Restrictions
		There are a few restrictions to the type that is allowed to be used with
		the Image class and all its derived classes.
		-# The type must be castable (or constructable) from an integer.
			- For example, the statement <tt>Type(0)</tt> must be valid for Type.
			- <tt>Type(0)</tt> should also yield a result similar in
			  functionality as zero is to real numbers.
	    -# If the type is a class it must have a copy contructor defined, e.g.
			<tt>Type(const Type&)</tt>.
		-# If the type is a class it must have an equals operator defined, e.g.
			<tt>operator=(const Type&)</tt>.
		-# The type must have all comparison operators defined.
			- <tt> bool operator< (const Type&, const Type&) </tt>
			- <tt> bool operator<=(const Type&, const Type&) </tt>
			- <tt> bool operator> (const Type&, const Type&) </tt>
			- <tt> bool operator>=(const Type&, const Type&) </tt>
			- <tt> bool operator==(const Type&, const Type&) </tt>
			- <tt> bool operator!=(const Type&, const Type&) </tt>
	*/
	template<class Type> class Image
	{
	public:
		typedef Type value_type;					///< The type passed as the template argument.
		typedef ImageIterator<Type> iterator;		///< The iterator type to be used with this class.
		typedef ConvolutionIterator<Type> convolution_iterator;	///< The convolution iterator type to be used with this class

		Image(edge_handling eh = edge_clamp);		///< The default constructor.

		/** A constructor that allocates memory for a
			<i>width</i> x <i>height</i> image array.
			@param width The width of the image.
			@param height The height of the image.
		*/
		Image(int width, int height, edge_handling eh = edge_clamp);

		/** A constructor that allocates memory for a <i>width</i> x <i>height</i>
			image array and then initializes it with the function given.
			@param width The width of the image.
			@param height The height of the image.
			@param func A function pointer to a function that will be used to initialize the image based on the coordinates of the pixel.

			The function, <i>func</i>, must have the form <tt>Type initialize(int x, int y)</tt>.
			- <i>x</i> The x-coordinate.
			- <i>y</i> The y-coordinate.
			- <i>Returns:</i> The value at pixel (x,y).
		*/
		Image(int width, int height, Type (*func)(int, int), edge_handling eh = edge_clamp);

		/** Copy constructor
			@param im An image used to initialize the new image.
			@param copy If copy is true the image data will be copied from
				<i>im</i> to the new image.  Otherwise the new image will be
				initialized to the same size as <i>im</i>.
		*/
		Image(const Image &im, bool copy = true);

		/** Destructor.
			This function deallocates the memory allocated for the image array.
		*/
		~Image();

		/** Returns a reference to the current edge handling settings.
			@see edge_handling
		*/
		edge_handling& edgeHandling() { return m_edgeHandling; }

		/** Returns a the current edge handling settings.
			@see edge_handling
		*/
		edge_handling edgeHandling() const { return m_edgeHandling; }

		/** Returns the height of the image.
			@see width()
		*/
		int height() const { return m_height; }

		/** Returns the width of the image.
			@see height()
		*/
		int width()  const { return m_width; }

		/** Writes the contents of the image to an ASCII file.
			@param fileName The name of the file.
			@param width The number of characters for each pixel of the output.
			@param precision The number of digits for each pixel of the output.
			@see ImageIO::write()
		*/
		void writeToAscii(const char* fileName, int width = 14, int precision = 8);

		/** Returns an ConvolutionIterator positioned at the beginning of the image.
			The result of convolving the template with the image at the current
			pixel will be returned based on the merge and unity functions
			passed.

			@param tLink A template that will be linked with the iterator.
			@param mergeFunction A function pointer to a function that will be
				used to "merge" the template with the image.
			@param unityFunction A function pointer to a function that will be
				used to "unify" the result of merging the template with the
				image to a single value.
			@return An iterator positioned on the first pixel of the image.

			@see cend(), ConvolutionIterator
		*/
		convolution_iterator cbegin(Template<Type>& tLink,
			typename convolution_iterator::merge_function mergeFunction,
			typename convolution_iterator::unity_function unityFunction) const
			{ return convolution_iterator(this, &tLink, mergeFunction, unityFunction); }

		/** Returns a ConvolutionIterator positioned at the end of the image.
			The result of this function should only be used to detect when an
			iterator has reached the end of the image.

			@return An iterator positioned one pixel past the end of the image.
			@note The equality comparison operators in ConvolutionIterator only
				compare the	position of the image (not the linked template).
			@see cbegin(), ConvolutionIterator
		*/
		convolution_iterator cend()	const
			{ return convolution_iterator(this); }

		/** Returns an ImageIterator positioned at the beginning of the image.
			The iterator will progress through the image pixel by pixel.

			@return An iterator positioned on the first pixel of the image.
			@see end(), ImageIterator
		*/
		iterator begin() const { return iterator(m_image); }

		/** Returns an ImageIterator positioned at the end of the image.
			The result of this function should only be used to detect when an
			iterator has reached the end of the image.
			@return An iterator positioned one pixel past the end of the image.
			@see begin(), ImageIterator
		*/
		iterator end() const { return iterator( m_image + m_width*m_height ); }

		/** Returns the maximum pixel value in the image.
			@see min(), mean(), sd(), sum()
		*/
		Type  max() const;

		/** Returns the minimum pixel value in the image.
			@see max(), mean(), sd(), sum()
		*/
		Type  min() const;

		/** Pixel-wise equality comparison for two images.
			@param image The image to be compared with the calling image.
			@return Returns true if every pixel in <i>image</i> is equal to
				every pixel in the calling image.
			@note The overloaded comparison operators are characteristic
				functions.  Only use this function for an actual comparison.

			@see equalTo(const Type&) const
		*/
		bool  equalTo(const Image &image) const;

		/** Pixel-wise equality comparison for an image and a value.
			@param value The value to be compared with the calling image.
			@return Returns true if every pixel in the image passed is equal to
				<i>value</i>.
			@note The overloaded comparison operators are characteristic
				functions.  Only use this function for an actual comparison.

			@see equalTo(const Image&) const
		*/
		bool  equalTo(const Type& value) const;

		/** Returns the pixel value at (<i>x</i>,<i>y</i>).
			This function returns a reference to the pixel value, therefore the
			pixel value can also be set using this function. If (x,y) is not in
			the domain of the image the function returns zero.  For
			compatibility reasons you can set the value of a pixel not in the
			domain of the image, but this action would be meaningless.

			@param x The x-coordinate of the pixel to be accessed.
			@param y The y-coordinate of the pixel to be accessed.
			@return A reference to the pixel at (<i>x</i>,<i>y</i>) in the
				image.

			@see getPixel(double,double) const
		*/
		Type& getPixel(int x, int y);
		Type  getPixel(int x, int y) const;

		/** Returns the pixel value at (<i>location%width</i>,<i>location/width</i>).
			This function returns a reference to the pixel value, therefore the
			pixel value can also be set using this function. If <i>location</i>
			is not in the domain of the image the function throws an exception.

			@param location The 1-D mapped pixel location.
			@return A reference to the pixel at
				(<i>location%width</i>,<i>location/width</i>) in the image.

			@see getPixel(double,double) const
		*/
		Type& getPixel(int location);

		/** Returns the pixel value at (<i>x</i>,<i>y</i>) based on a bilinear
			approximation.

			@param x The x-coordinate of the pixel to be accessed.
			@param y The y-coordinate of the pixel to be accessed.
			@return The value of the pixel at (<i>x</i>,<i>y</i>) in the image
				based on a continuous bilinear approximation.

			@see getPixel(int,int)
		*/
		Type  getPixel(double x, double y) const;

		/** Returns the subimage with the upper left coordinate at
			(<i>x</i>,<i>y</i>) and with the specified <i>height</i> and
			<i>width</i>.

			@param x The x-coordinate of the upper left pixel.
			@param y The y-coordinate of the upper left pixel.
			@param width The width of the subimage.
			@param height The height of the subimage.
			@return The subimage.
		*/
		Image<Type> subImage(int x, int y, int width, int height) const;

		/** Returns the sum of the pixels in the image.
			@see max(), min(), mean(), sd()
		*/
		Type  sum()  const;

		/** Returns the mean of the pixels in the image.
			@see max(), min(), sd(), sum()
		*/
		Type  mean() const;

		/** Returns the standard deviation of the pixels in the image.
			@see max(), min(), mean(), sum()
		*/
		Type  sd()   const;

		/** Applies <i>func</i> to each pixel in the image, replacing the image
			with the result.
			@param func A function pointer to the function that is to be applied
				to every pixel.
			@return The new image.

			@see genericBinary(), genericConvolution()
		*/
		Image genericUnary(Type (*func)(Type)) const;

		/** Applies <i>func</i> for each pixel in the image and <i>im</i>,
			replacing the image with the result.
			This function replaces each pixel with func(image(x,y), im(x,y)).
			For example, if <i>func</i> were defined to return the sum of two
			numbers, the result would be image+<i>im</i>.

			@param im An image to be applied to the calling image using
				<i>func</i>.
			@param func A function pointer to the function that is to be applied
				to each pixel.
			@return The new image.

			@see genericUnary(), genericConvolution()
		*/
		Image genericBinary(const Image &im, Type (*func)(Type, Type)) const;

		/** Calculates the convolution of the image and <i>tem</i> based on the
			merge and unity functions passed.
			@param tem A reference to the template that will be used to
				calculate the convolution.
			@param mergeFunction A function pointer to a function that will be
				used to "merge" the template with the image.
			@param unityFunction A function pointer to a function that will be
				used to "unify" the result of merging the template with the
				image to a single value.
			@return The new image.

			@see genericUnary(), genericBinary()
		*/
		Image genericConvolution(Template<Type>& tem,
								 typename convolution_iterator::merge_function mergeFunction,
								 typename convolution_iterator::unity_function unityFunction) const;

		/** Calculates the convolution of the image using a given
			ConvolutionIterator.
			The purpose of this function is to allow the use of classes derived
			from ConvolutionIterator that have hard-coded operations for
			efficiency.
			@param c_iter A reference to the ConvolationIterator that will be used
				to calculate the convolution.
			@return The new image.

			@see genericUnary(), genericBinary()
		*/
		Image genericConvolution(convolution_iterator& c_iter) const;

		/** Applies a domain transform based on a bilinear approximation of the
			image.
			For each pixel in the image, func(x,y) is called.  The <i>func</i>
			function replaces the coordinate passed to it (x,y) with a new
			coordinate (x',y').  Then image(x,y) is replaced with image(x',y').
			@param func A function pointer to the function that will calculate
				the transform.
			@return The new image.

			@see getPixel(double,double) const
		*/
		Image domainTransform(void (*func)(double&, double&)); //const

		/** Applies a domain transform to the image.
			For each pixel in the image, func(x,y) is called.  The <i>func</i>
			function replaces the coordinate passed to it (x,y) with a new
			coordinate (x',y').  Then image(x,y) is replaced with image(x',y').
			@param func A function pointer to the function that will calculate
				the transform.
			@return The new image.

			@see getPixel(int,int)
		*/
		Image domainTransform(void (*func)(int&, int&)); //const

		/** Resizes the image to the new dimensions provided.
			@param width The new width of the image.
			@param height The new height of the image.
			@param retain If this is true the image data will be retained and
				resized to match the new dimensions using a bilinear
				approximation.  If this is false the image will be resized and
				<b>all data will be lost</b>!
			@return A reference to the altered image.
			@note This function <b>will</b> alter the calling image.

			@see getPixel(double,double) const
		*/
		Image& resize(int width, int height, bool retain = true);

		// Cast operator to let you cast a Image<T1> to an Image<T2>
		/*operator Image<float>()
		{
			Image<float> newImage(m_width, m_height);
			Image<float>::iterator iterNew = newImage.begin();
			for(iterator iter = begin(); !iter.end(); ++iter, ++iterNew) {
				*iterNew = float(*iter); }
			return newImage;
		}*/

		// Friendly Operators
		friend Image operator|<Type>(const Type& left, const Image& right);		///< Pixel-wise maximun.
		friend Image operator&<Type>(const Type& left, const Image& right);		///< Pixel-wise minimum.
		friend Image operator+<Type>(const Type& left, const Image& right);		/*!< Pixel-wise addition. */
		friend Image operator-<Type>(const Type& left, const Image& right);		/*!< Pixel-wise subtraction. */
		friend Image operator*<Type>(const Type& left, const Image& right);		/*!< Pixel-wise multiplication. */
		friend Image operator/<Type>(const Type& left, const Image& right);		/*!< Pixel-wise division. */

		// Template Convolution Operators
		Image operator+(Template<Type>& right) const;			///< Right linear convolution product.
		Image operator|(Template<Type>& right) const;			///< Right multiplicative maximum convolution product.
		Image operator&(Template<Type>& right) const;			///< Right multiplicative minimun convolution product.

		//These are min and max functions.
		//The resoning behind using these operators is from boolean algrebra
		//  where & is ^ and | is v.  Therefore & is min and | is max.
		Image  operator| (const Type&  right) const;							///< Pixel-wise maximun.
		Image  operator| (const Image& right) const;							///< Pixel-wise maximun.
		Image& operator|=(const Image& right);									///< Pixel-wise maximun.
		Image  operator& (const Type&  right) const;							///< Pixel-wise minimum.
		Image  operator& (const Image& right) const;							///< Pixel-wise minimum.
		Image& operator&=(const Image& right);									///< Pixel-wise minimum.

		//The following are defined as the characteristic function.
		//Note that they do NOT return a boolean.
		/** Less than characteristic function.
			@return @code newImage(x,y) = (image(x,y) < right)?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Image&) const
		*/
		Image operator< (const Type& right) const;
		/** Less than characteristic function.
			@return @code newImage(x,y) = (image(x,y) < right(x,y))?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Image&) const
		*/
		Image operator< (const Image& right) const;
		/** Less than or equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) <= right)?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Type&) const
		*/
		Image operator<=(const Type& right) const;
		/** Less than or equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) <= right(x,y))?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Image&) const
		*/
		Image operator<=(const Image& right) const;
		/** Greater than characteristic function.
			@return @code newImage(x,y) = (image(x,y) > right)?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Type&) const
		*/
		Image operator> (const Type& right) const;
		/** Greater than characteristic function.
			@return @code newImage(x,y) = (image(x,y) > right(x,y))?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Image&) const
		*/
		Image operator> (const Image& right) const;
		/** Greater than or equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) >= right)?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Type&) const
		*/
		Image operator>=(const Type& right) const;
		/** Greater than or equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) >= right(x,y))?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Image&) const
		*/
		Image operator>=(const Image& right) const;
		/** Equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) == right)?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Type&) const
		*/
		Image operator==(const Type& right) const;
		/** Equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) == right(x,y))?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Image&) const
		*/
		Image operator==(const Image& right) const;
		/** Not equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) != right)?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Type&) const
		*/
		Image operator!=(const Type& right) const;
		/** Not equal to characteristic function.
			@return @code newImage(x,y) = (image(x,y) != right(x,y))?1:0 @endcode
			@warning This is <b>not</b> a comparison operator!
			@see equalTo(const Image&) const
		*/
		Image operator!=(const Image& right) const;

		// Aritmetic Operators
		Image operator+(const Type&  right) const;								/*!< Pixel-wise addition. */
		Image operator+(const Image& right) const;								/*!< Pixel-wise addition. */
		Image operator-(const Type&  right) const;								/*!< Pixel-wise subtraction. */
		Image operator-(const Image& right) const;								/*!< Pixel-wise subtraction. */
		Image operator*(const Type&  right) const;								/*!< Pixel-wise multiplication. */
		Image operator*(const Image& right) const;								/*!< Pixel-wise multiplication. */
		Image operator/(const Type&  right) const;								/*!< Pixel-wise division. */
		Image operator/(const Image& right) const;								/*!< Pixel-wise division. */
		Image operator-() const;												/*!< Pixel-wise negation. */

		// Assignment Operators that DO overwrite the calling Image
		Image& operator= (const Image& right);									/*!< Pixel-wise equality. */
		Image& operator= (const Type&  right);									/*!< Pixel-wise equality. */
		Image& operator+=(const Image& right);									/*!< Pixel-wise addition. */
		Image& operator+=(const Type&  right);									/*!< Pixel-wise addition. */
		Image& operator-=(const Image& right);									/*!< Pixel-wise subtraction. */
		Image& operator-=(const Type&  right);									/*!< Pixel-wise subtraction. */
		Image& operator*=(const Image& right);									/*!< Pixel-wise multiplication. */
		Image& operator*=(const Type&  right);									/*!< Pixel-wise multiplication. */
		Image& operator/=(const Image& right);									/*!< Pixel-wise division. */
		Image& operator/=(const Type&  right);									/*!< Pixel-wise division. */

	protected:
		/** %Image array memory allocation.
			This function allocates an m_width x m_height array.  It is a
			virtual	function, so if a derived class requires a different form of
			allocation it can override this function.

			@return A pointer to the newly allocated memory.

			@see freeImage(), copyImage()
		*/
		virtual Type* allocateImage() throw(ImageException);

		/** Memory deallocation.
			This function deallocates an array of memory pointed to by
			<i>im.</i>.  It is a virtual function, so if a derived class uses a
			different form of allocation it can override this function to match.

			@param im A pointer to an image array.

			@see allocateImage(), copyImage()
		*/
		virtual void  freeImage(Type *im);

		/** %Image array data copy.
			This function copies m_width x m_height x sizeof(Type) bytes from
			array <i>from</i> to array <i>to</i>.  It is a virtual function so
			that another implementation may be used in a derived class.

			@param to A pointer to the array to be overwritten by the data in
				<i>from</i>.
			@param from A pointer to the array used to overwrite <i>to</i>.

			@see allocateImage(), freeImage()
		*/
		virtual void  copyImage(Type *to, const Type *from) { memcpy(to, from, sizeof(Type)*m_height*m_width); }

		//Data members
		int   m_height;								///< The height of the image.
		int   m_width;								///< The width of the image.
		Type* m_image;								///< An m_width x m_height array used to store the image data.
		edge_handling m_edgeHandling;				///< The edge handling settings. @note This property is not inherited with the equals operator.
	};
}	// end namespace

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "Image.cpp"
#endif

#endif