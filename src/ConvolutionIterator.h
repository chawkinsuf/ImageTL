#ifndef __CONVOLUTIONITERATOR_H__
#define __CONVOLUTIONITERATOR_H__
/** @file ConvolutionIterator.h
	Contains a class used to hide the implementation of Image access.
	This header contains the declaration of the ImageIterator class, which
	contains methods used to access the image.  The class may be used to access
	the image in a forward and sequencial manor.  A Template that will be used
	to perform convolution must be linked to the iterator.  Linking a Template
	serves to hide the implementation of a generic convolution function.
*/

// Disable warnings about ignoring throws declarations
#pragma warning( disable : 4290 )

namespace ImageTL {	template<class Type> class ConvolutionIterator; }
namespace ImageTL {	template<class Type> class Image; }

#include <vector>
#include <iterator>
#include <algorithm>
#include "ImageException.h"
#include "Template.h"

namespace ImageTL
{
	template<class Type> Type mf_mul(Type&, Type&);				///< Returns the product of the parameters.
	template<class Type> Type mf_add(Type&, Type&);				///< Returns the sum of the parameters.
	template<class Type> Type mf_sub(Type&, Type&);				///< Returns the subtraction of parameter 2 from parameter 1.

	template<class Type> Type uf_sum(typename ConvolutionIterator<Type>::data_container&);		///< Returns the sum of the elements in the list.
	template<class Type> Type uf_max(typename ConvolutionIterator<Type>::data_container&);		///< Returns the maximum of the elements in the list.
	template<class Type> Type uf_min(typename ConvolutionIterator<Type>::data_container&);		///< Returns the minimum of the elements in the list.
	template<class Type> Type uf_mean(typename ConvolutionIterator<Type>::data_container&);		///< Returns the mean of the elements in the list.
	template<class Type> Type uf_median(typename ConvolutionIterator<Type>::data_container&);	///< Returns the median of the elements in the list.

	/** @class ConvolutionIterator
		Used to access the data of the Image class by performing a convolution.
		The class has the ability to parse through the image sequencially,
		while performing a convolution.
		- %Template Convolution Access Mode
			- A Template class must be linked to the ImageIterator via the
			  constructor.
			- Two function pointers must also be provided.
				- The ConvolutionIterator::merge_function is a function that will
				  merge the Template image with the Image data.  That is, each
				  point in Ty(x) is operated with each point in I(x).  For
				  linear convolutions this is simply a function that will
				  multiply two numbers.
				- The ConvolutionIterator::unity_function will then take a list
				  of the result of merging the support of the Template with the
				  Image.  This will be unified to a single value based on the
				  function provided and that value will be the new value of
				  I(x).
			- The starting coordinates of the iterator must also be provided.
				- This is simply (0,0), the default, if the iterator is created
				  at the beginning of the Image.
				- This value must be provided so the iterator may keep track of
				  the x,y coordinates as the iterator is incremented (for use
				  with the Template).
			- The dereferencing operator, operator*(), returns the result of the
			  Template convolution at the current position of the Image.

		@note In order to properly use the ConvolutionIterator class, the
			Image::cbegin() and Image::cend() functions should be used.
	*/
	template<class Type> class ConvolutionIterator
	{
	public:
		typedef std::vector<Type>                 data_container;
		typedef typename data_container::iterator data_iterator;

		typedef std::forward_iterator_tag iterator_category;	///< Specifies that the iterator is a forward iterator.
		typedef Type value_type;								///< The type passed as the template argument.
		typedef value_type& reference;							///< A reference to the type passed as the template argument.
		typedef value_type* pointer;							///< A pointer to the type passed as the template argument.
		typedef int difference_type;							///< Specifies that the array is indexed with integers.

		/** A type to specify the declaration of the function used to merge a
			Template with an Image.
			@note See the detailed description of the ImageIterator class for
				details on using this function.
		*/
		typedef Type (*merge_function)(Type&, Type&);

		/** A type to specify the declaration of the function used to unify the
			result of a Template being merged with an Image.
			@note See the detailed description of the ImageIterator class for
				details on using this function.
		*/
		typedef Type (*unity_function)(data_container&);

		// Makes an iterator at the end of the image
		ConvolutionIterator(const Image<Type>* image);

		virtual ~ConvolutionIterator();

		/** The constructor.
			This constructor will create an iterator at the beginning of image.
			@param image A pointer to image data.
			@param tLink A pointer to a Template that will be used when
				dereferencing the iterator.
			@param mergeFunction A pointer to a function that will merge the
				Template image with the Image data.  This parameter is only
				needed if a Template is being linked to the iterator.
			@param unityFunction A pointer to a function that will unify the
				result of merging the support of the Template with the Image
				into a single value.  This parameter is only needed if a
				Template is being linked to the iterator.

			@note See the detailed description of the ImageIterator class for
				details on using this class.
		*/
		ConvolutionIterator(const Image<Type>* image, Template<Type>* tLink,
					  merge_function mergeFunction, unity_function unityFunction);

		/** Dereferencing operator.
			This function will return the value of the image at the current
			location of the iterator.  This will simply be the value of the
			template convolved with the image at the current point.
			@return The value of the iterator at the current location as
				described above.

			@note Since a reference is not returned no value may be set.
		*/
		virtual value_type operator*();

		/** Pre-increment operator.
			This function will increment the position of the iterator with
			respect to the image array.
			@return A reference to the iterator after the position is incremented.
			@note In general, the pre-increment operator should be used whenever
				possible because it will be faster than the post-increment
				operator.
			@see operator++(int)
		*/
		ConvolutionIterator& operator++();

		/** Post-increment operator.
			This function will increment the position of the iterator with
			respect to the image array.
			@return A copy of the iterator before the position is incremented.
			@note In general, the pre-increment operator should be used whenever
				possible because it will be faster than the post-increment
				operator.
			@see operator++()
		*/
		ConvolutionIterator  operator++(int);

		/** Equality comparison operator.
			This compares the position of the pointer to the image array of the
			calling iterator to that of the passed iterator.
			@param right The iterator to be compared with the calling iterator.
			@retval true If the iterators are at the same position in the
				same image.
			@retval false Otherwise.
			@note The linked image and the position of the iterator are the
				only variables compared.  This means that if two iterators are
				linked to different templates and at the same position in the
				same image, they will evaluate as equal.
			@see operator!=()
		*/
		bool operator==(const ConvolutionIterator& right) const
			{ return (m_image == right.m_image && m_imageX == right.m_imageX && m_imageY == right.m_imageY); }

		/** Inequality comparison operator.
			This compares the position of the pointer to the image array of the
			calling iterator to that of the passed iterator.
			@param right The iterator to be compared with the calling iterator.
			@retval true If the iterators are not at the same position in the
				same image.
			@retval false Otherwise.
			@note The linked image and the position of the iterator are the
				only variables compared.  This means that if two iterators are
				linked to different templates and at the same position in the
				same image, they will evaluate as equal.
			@see operator==()
		*/
		bool operator!=(const ConvolutionIterator& right) const
			{ return (m_image != right.m_image || m_imageX != right.m_imageX || m_imageY != right.m_imageY); }

		void changeImage(const Image<Type>* image)
		{
			m_image = image;
			m_imageX = 0;
			m_imageY = 0;
		}

	protected:
		const Image<Type>* m_image;			///< A pointer to the image.
		Template<Type>* m_tLink;			///< A pointer to the template that is linked to the iterator.
		int m_imageX;						///< The current x position.
		int m_imageY;						///< The current y position.

		int m_templateNegOffsetX;			///< Used to save calculations in the main dereference function.
		int m_templatePosOffsetX;			///< Used to save calculations in the main dereference function.
		int m_templateNegOffsetY;			///< Used to save calculations in the main dereference function.
		int m_templatePosOffsetY;			///< Used to save calculations in the main dereference function.

		 data_container* m_data;			///< Allocated to a specific size and resued in calculation.

		 void clearData(data_container* data);	///< Resets m_data to all null values.

	private:
		merge_function m_mergeFunction;		///< A pointer to the function used to merge the template with the image.
		unity_function m_unityFunction;		///< A pointer to the function used to unify the merged result.
	};
}

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "ConvolutionIterator.cpp"
#endif

#endif