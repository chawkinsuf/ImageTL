#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#include <list>
#include <sstream>
#include "ImageException.h"

namespace ImageTL
{
	/** @class Template
		Provides a consistant interface for computing convolutions.
		A pure virtual class that can be derived from in order to provide a
		consistant interface for computing convolutions of an Image with a
		Template.  Although this class cannot be instantiated, constructors
		are provided that can be called from derived classes.

		@note The Template class and all derived classes must be linked to an
			ImageIterator in order for the edge handling to function properly.
		@par
		@note The dereferencing operator does not return a reference and is
			therefore read-only.
	*/
	template<class Type> class Template
	{
	public:
		int  xCenter() const   { return m_xCenter; }	///< Returns the x-coordinate of the center of the Template.
		int  yCenter() const   { return m_yCenter; }	///< Returns the y-coordinate of the center of the Template.
		int  width()   const   { return m_width; }		///< Returns the width of the Template.
		int  height()  const   { return m_height; }		///< Returns the height of the Template.
		int  size()    const   { return m_width*m_height; }	///< Returns the width times the height of the Template.

		/** Set the center of the Template to the specified coordinates.
			@param x The new x-coordinate of the center.
			@param y The new y-coordinate of the center.

			@note The coordinates should be specified in the image space.
		*/
		void setCenter(int x, int y) { m_xCenter = x; m_yCenter = y; }

		/** Verifies that the specified coordinates are in the template boundary.
			This function simply ensures safe memory access and can give a
				clean exit if the template is being accessed incorrectly.
			@param x The x-coordinate translated into the template space, where
				the center is (0, 0).  That is, the x-coordinate in the image
				space minus the x-coordinate of the center of the template.
			@param y The y-coordinate translated into the template space, where
				the center is (0, 0).  That is, the y-coordinate in the image
				space minus the y-coordinate of the center of the template.
			@retval true If the coordinates are inside the template boundary.
			@retval false If the coordinates are outside the template boundary.

			@note In the current implementation, this function must be called
				by the dereferenceing operator as it is defined in any derived
				classes.

			@see Template::operator()()
		*/
		bool verify(int x, int y) const;

		/** %Template dereferencing operator.
			Since this is a pure virtual function, any derived classes must
				provide the	implementation based this description.
			@param x The x-coordinate in the image space.
			@param y The y-coordinate in the image space.
			@return The value of the template at the specified coordinates,
				relative to the center of the template.

			@note In the current implementation, this function must call the
				verify() function in order to ensure the coordinates passed are
				valid for the template based on the current center.  This
				function should also throw the necessary exception if the
				coordinates are not valid.

			@see Template::verify()
		*/
		virtual Type operator()(int x, int y) const = 0;

		/** A constructor that sets all of the Template parameters.
			@param width The width of the template.
			@param height The height of the template.
			@param antiSupport The template value that is considered outside of
				the support.
			@param eh The edge handling settings.

			@note The center in the template space is defined as
				<tt>( (m_width-1)/2, (m_height-1)/2 )</tt>.

			@see edgeHandling
			@see edge_handling
		*/
		Template(int width, int height);

		Template(const Template &t);			///< Copy constructor.

	protected:
		int m_xCenter;							///< The x-coordinate of the center of the template.
		int m_yCenter;							///< The y-coordinate of the center of the template.
		int m_width;							///< The width of the template.
		int m_height;							///< The height of the template.
	};

	template<class Type> class ConstantTemplate: public Template<Type>
	{
	public:
		Type  operator()(int x, int y) const;
		Type& operator()(int x, int y);
		Type& operator()(int index);

		ConstantTemplate& operator+=(Type value);
		ConstantTemplate& operator-=(Type value);
		ConstantTemplate& operator*=(Type value);
		ConstantTemplate& operator/=(Type value);

		ConstantTemplate& operator+=(Type* values);
		ConstantTemplate& operator-=(Type* values);
		ConstantTemplate& operator*=(Type* values);
		ConstantTemplate& operator/=(Type* values);

		ConstantTemplate& operator=(Type  value);
		ConstantTemplate& operator=(Type* values);

		void rotate(double angle);

		ConstantTemplate& operator=(const ConstantTemplate &t);

		//Constant template constructors
		ConstantTemplate(const Type tem[], int width, int height);

		ConstantTemplate(const Type init, int width, int height);

		ConstantTemplate(const ConstantTemplate &t);

		~ConstantTemplate();
	protected:
		Type *m_data;
	};

	template<class Type> class FunctionalTemplate: public Template<Type>
	{
	public:
		Type getParam() const { return m_functional_parameter; }
		Type operator()(int x, int y) const;
		void setWidth (int width)  { this->m_width  = width; }
		void setHeight(int height) { this->m_height = height; }
		FunctionalTemplate& operator()(Type (*_tf)(const FunctionalTemplate<Type>&, int, int, const Type&), const Type& param)
			{ m_template_function = _tf; m_functional_parameter = param; return *this; }

		//Functional template constuctor; must define m_template_function()
		FunctionalTemplate(int width, int height)
			: Template<Type>(width, height) {}

		FunctionalTemplate(const FunctionalTemplate &t) : Template<Type>(t)
		{
			m_functional_parameter = t.m_functional_parameter;
			m_template_function    = t.m_template_function;
		}

	protected:
		Type m_functional_parameter;
		Type (*m_template_function)(const FunctionalTemplate<Type>&, int, int, const Type&);
	};

	//Common Template definitions
	//const double array[m_height * m_width]
	const int moore_h = 3;
	const int moore_w = 3;
	const double moore[moore_h * moore_w] = {
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	};
	const int von_neumann_h = 3;
	const int von_neumann_w = 3;
	const double von_neumann[von_neumann_w * von_neumann_h] = {
		0, 1, 0,
		1, 1, 1,
		0, 1, 0
	};
	const int laplacian_h = 3;
	const int laplacian_w = 3;
	const double laplacian[laplacian_w * laplacian_h] = {
		0,  1, 0,
		1, -4, 1,
		0,  1, 0
	};
	//ConstantTemplate<double> M(Image::moore, Image::moore_w, Image::moore_h);
}

// Include the function definitions in the header if we aren't using a compiled library
#ifdef IMAGETL_NO_LIBRARY
#include "Template.cpp"
#endif

#endif