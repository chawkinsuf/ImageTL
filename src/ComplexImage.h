#ifndef __COMPLEXIMAGE_H__
#define __COMPLEXIMAGE_H__

#include <cstring>
#include <complex>
#include "Image.h"

using std::complex;

namespace ImageTL
{
	// Overloaded operators for complex comparisons
	template<class Type> inline bool operator<(const complex<Type>& left, const complex<Type>& right) {
		return (abs(left) < abs(right)); }

	template<class Type> inline bool operator<=(const complex<Type>& left, const complex<Type>& right) {
		return (abs(left) <= abs(right)); }

	template<class Type> inline bool operator>(const complex<Type>& left, const complex<Type>& right) {
		return (abs(left) > abs(right)); }

	template<class Type> inline bool operator>=(const complex<Type>& left, const complex<Type>& right) {
		return (abs(left) >= abs(right)); }

	// This function is needed to take the mean of an image
	template<class Type> inline complex<Type> operator/(const complex<Type>& left, const int& right) {
		return left / Type(right); }

	class ComplexImage : public Image<complex<double> >
	{
	public:
		Image<double> real();
		Image<double> imag();
		Image<double> abs();

		// This frees and allocates memory the size of real (real and imag must be the same size)
		void replaceImage(Image<double> real, Image<double> imag) throw(ImageException);
		void replaceImage(double real, double imag) throw(ImageException);

		ComplexImage(edge_handling eh = edge_skip) : Image<complex<double> >(eh) {}
		ComplexImage(int w, int h, edge_handling eh = edge_skip) : Image<complex<double> >(w, h, eh) {}
		ComplexImage(const Image<complex<double> > &i, bool copy = true) : Image<complex<double> >(i, copy) {}
		ComplexImage(const Image<double> &i);

		// Operators (= operator is not inherited)
		ComplexImage& operator=(const Image<complex<double> >&);
		ComplexImage& operator=(const ComplexImage&);
		ComplexImage& operator=(complex<double>);
	};
}	//End namespace

#endif