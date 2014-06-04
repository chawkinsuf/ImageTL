#ifndef __IMAGEITERATOR_H__
#define __IMAGEITERATOR_H__
/** @file ImageIterator.h
	Contains a class used to hide the implementation of Image access.
	This header contains the declaration of the ImageIterator class, which
	contains methods used to access the image.  The class may be used to access
	the image in a forward and sequencial manor.
*/

// Disable warnings about ignoring throws declarations
#pragma warning( disable : 4290 )

#include <iterator>
#include "ImageException.h"

namespace ImageTL
{
	/** @class ImageIterator
		Used to access the data of the Image class.
		The class has the ability to parse through the image sequencially.
		- Direct Access Mode
			- If the image is accessed directly, the ImageIterator class is
			  nothing but a simple iterator for the Image array.
			- When the iterator is incremented, operator++(), the pointer to the
			  array in memory is simply incremented.
			- The dereferencing operator, operator*(), returns a reference to
			  this position in the array.
				- This allows for the read and write of image data using the
				  dereferencing operator.

		@note In order to properly use the ImageIterator class, the
			Image::begin() and Image::end() functions should be used.
	*/
	template<class Type> class ImageIterator
	{
	public:
		typedef std::forward_iterator_tag iterator_category;	///< Specifies that the iterator is a forward iterator.
		typedef Type value_type;								///< The type passed as the template argument.
		typedef value_type& reference;							///< A reference to the type passed as the template argument.
		typedef value_type* pointer;							///< A pointer to the type passed as the template argument.
		typedef int difference_type;							///< Specifies that the array is indexed with integers.

		/** The constructor.
			This is the only constructor for the ImageIterator class because a
			pointer to image data to iterate through is needed to use this class.
			@param image A pointer to image data.
		*/
		ImageIterator(pointer image = NULL) { m_image  = image; }

		/** Dereferencing operator.
			This function will return the value of the image at the current
			location of the iterator.  This will simply be the value at
			the current location of the image.
			@return The value of the iterator at the current location as
				described above.

			@note Since a reference is returned, the value may be altered.
		*/
		reference operator*() { return *m_image; }

		/** Pre-increment operator.
			This function will increment the position of the iterator with
			respect to the image array.
			@return A reference to the iterator after the position is incremented.
			@note In general, the pre-increment operator should be used whenever
				possible because it will be faster than the post-increment
				operator.
			@see operator++(int)
		*/
		ImageIterator& operator++() { m_image++; return *this; }

		/** Post-increment operator.
			This function will increment the position of the iterator with
			respect to the image array.
			@return A copy of the iterator before the position is incremented.
			@note In general, the pre-increment operator should be used whenever
				possible because it will be faster than the post-increment
				operator.
			@see operator++()
		*/
		ImageIterator  operator++(int);

		/** Equality comparison operator.
			This compares the position of the pointer to the image array of the
			calling iterator to that of the passed iterator.
			@param right The iterator to be compared with the calling iterator.
			@retval true If the iterators are at the same position in the image
				data.
			@retval false Otherwise.

			@see operator!=()
		*/
		bool operator==(const ImageIterator& right) const { return (m_image == right.m_image); }

		/** Inequality comparison operator.
			This compares the position of the pointer to the image array of the
			calling iterator to that of the passed iterator.
			@param right The iterator to be compared with the calling iterator.
			@retval true If the iterators are not at the same position in the
				image data.
			@retval false Otherwise.

			@see operator==()
		*/
		bool operator!=(const ImageIterator& right) const { return (m_image != right.m_image); }

	private:
		pointer m_image;					///< A pointer to the current position of the image.
	};
}

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "ImageIterator.cpp"
#endif

#endif