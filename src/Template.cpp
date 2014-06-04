#ifndef __TEMPLATE_CPP__
#define __TEMPLATE_CPP__

#include "Template.h"

namespace ImageTL
{
	//Template definitions
	template<class Type> bool Template<Type>::verify(int x, int y) const
	{
		if(this->m_xCenter < 0 || this->m_yCenter < 0) {
			throw ImageException("Template::verify [You must specify the center of the template]"); }

		double widthHalf = this->m_width/2., heightHalf = this->m_height/2.;
		if(x<(0.5 - widthHalf)  || x>widthHalf || y<(0.5 - heightHalf) || y>heightHalf) {
			return false; }

		return true;
	}

	template<class Type> Template<Type>::Template(int w, int h)
	{
		this->m_width  = w;
		this->m_height = h;
		this->m_xCenter = -1;
		this->m_yCenter = -1;
	}

	template<class Type> Template<Type>::Template(const Template &t)
	{
		this->m_width  = t.m_width;
		this->m_height = t.m_height;
		this->m_xCenter = t.m_xCenter;
		this->m_yCenter = t.m_yCenter;
	}

	//ConstantTemplate Definitions
	template<class Type> void ConstantTemplate<Type>::rotate(double angle)
	{
	}

	template<class Type> Type ConstantTemplate<Type>::operator()(int x, int y) const
	{
		int xDiff = x - this->m_xCenter, yDiff = y - this->m_yCenter;
		int pos = xDiff + (this->m_width-1)/2 + this->m_width*(yDiff + (this->m_height-1)/2);
		if(this->verify(xDiff, yDiff)) { return m_data[pos]; }
		else { throw ImageException("Template::operator() [Out of Range]"); }
	}

	template<class Type> Type& ConstantTemplate<Type>::operator()(int x, int y)
	{
		int xDiff = x - this->m_xCenter, yDiff = y - this->m_yCenter;
		int pos = xDiff + (this->m_width-1)/2 + this->m_width*(yDiff + (this->m_height-1)/2);
		if(this->verify(xDiff, yDiff)) { return m_data[pos]; }
		else { throw ImageException("Template::operator() [Out of Range]"); }
	}

	template<class Type> Type& ConstantTemplate<Type>::operator()(int index)
	{
		if(index >= 0 && index < this->m_width*this->m_height) {
			return m_data[index]; }

		throw ImageException("Template::operator() [Out of Range]");
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator+=(Type n)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] += n; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator-=(Type n)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] -= n; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator*=(Type n)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] *= n; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator/=(Type n)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] /= n; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator+=(Type* values)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] += values[i]; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator-=(Type* values)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] -= values[i]; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator*=(Type* values)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] *= values[i]; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator/=(Type* values)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] /= values[i]; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator=(Type value)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] = value; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator=(Type* values)
	{
		for(int i = 0; i<this->m_width*this->m_height; i++) {
			m_data[i] = values[i]; }

		return *this;
	}

	template<class Type> ConstantTemplate<Type>& ConstantTemplate<Type>::operator=(const ConstantTemplate &t)
	{
		this->m_width  = t.m_width;
		this->m_height = t.m_height;
		this->m_xCenter = t.m_xCenter;
		this->m_yCenter = t.m_yCenter;

		if(m_data != NULL) {
			delete[] m_data; }

		try
		{
			m_data = new Type[this->m_width * this->m_height];
			for(int i=0; i<this->m_height*this->m_width; i++) {
				m_data[i] = t.m_data[i]; }
		}
		catch(std::bad_alloc &e)
		{
			std::stringstream msg_stream;

			msg_stream<<"ConstantTemplate::operator= [Error allocating memory for a "<<this->m_width<<"x"<<this->m_height<<" image:  "<<e.what()<<"]"<<std::endl;
			msg_stream<<"ConstantTemplate::operator= [Aborting...]";
			throw ImageException(msg_stream.str());
		}

		return *this;
	}

	template<class Type> ConstantTemplate<Type>::ConstantTemplate(const Type tem[], int w, int h) : Template<Type>(w, h)
	{
		try
		{
			m_data = new Type[this->m_width * this->m_height];
			for(int i=0; i<this->m_height*this->m_width; i++) {
				m_data[i] = tem[i]; }
		}
		catch(std::bad_alloc &e)
		{
			std::stringstream msg_stream;

			msg_stream<<"ConstantTemplate::ConstantTemplate [Error allocating memory for a "<<this->m_width<<"x"<<this->m_height<<" image:  "<<e.what()<<"]"<<std::endl;
			msg_stream<<"ConstantTemplate::ConstantTemplate [Aborting...]";
			throw ImageException(msg_stream.str());
		}
	}

	template<class Type> ConstantTemplate<Type>::ConstantTemplate(const Type init, int w, int h) : Template<Type>(w, h)
	{
		try
		{
			m_data = new Type[this->m_width * this->m_height];
			for(int i=0; i<this->m_height*this->m_width; i++) {
				m_data[i] = init; }
		}
		catch(std::bad_alloc &e)
		{
			std::stringstream msg_stream;

			msg_stream<<"ConstantTemplate::ConstantTemplate [Error allocating memory for a "<<this->m_width<<"x"<<this->m_height<<" image:  "<<e.what()<<"]"<<std::endl;
			msg_stream<<"ConstantTemplate::ConstantTemplate [Aborting...]";
			throw ImageException(msg_stream.str());
		}
	}

	template<class Type> ConstantTemplate<Type>::ConstantTemplate(const ConstantTemplate<Type> &t) : Template<Type>(t)
	{
		try
		{
			m_data = new Type[this->m_width * this->m_height];
			for(int i=0; i<this->m_height*this->m_width; i++) {
				m_data[i] = t.m_data[i]; }
		}
		catch(std::bad_alloc &e)
		{
			std::stringstream msg_stream;

			msg_stream<<"ConstantTemplate::ConstantTemplate [Error allocating memory for a "<<this->m_width<<"x"<<this->m_height<<" image:  "<<e.what()<<"]"<<std::endl;
			msg_stream<<"ConstantTemplate::ConstantTemplate [Aborting...]";
			throw ImageException(msg_stream.str());
		}
	}

	template<class Type> ConstantTemplate<Type>::~ConstantTemplate()
	{
		delete[] m_data;
	}

	//FunctionalTemplate Definitions
	template<class Type> Type FunctionalTemplate<Type>::operator()(int x, int y) const
	{
		int xDiff = x - this->m_xCenter, yDiff = y - this->m_yCenter;
		if(this->verify(xDiff, yDiff)) { return m_template_function(*this, x, y, m_functional_parameter); }
		else { throw ImageException("Template::operator() [Out of Range]"); }
	}
}  //end namespace

// Instantiate with common template types for library compilation
#ifdef IMAGETL_LIBRARY_COMPILE
namespace ImageTL
{
	template class Template<char>;
	template class Template<short>;
	template class Template<int>;
	template class Template<long>;
	template class Template<float>;
	template class Template<double>;

	template class ConstantTemplate<char>;
	template class ConstantTemplate<short>;
	template class ConstantTemplate<int>;
	template class ConstantTemplate<long>;
	template class ConstantTemplate<float>;
	template class ConstantTemplate<double>;

	template class FunctionalTemplate<char>;
	template class FunctionalTemplate<short>;
	template class FunctionalTemplate<int>;
	template class FunctionalTemplate<long>;
	template class FunctionalTemplate<float>;
	template class FunctionalTemplate<double>;
}
#endif

#endif