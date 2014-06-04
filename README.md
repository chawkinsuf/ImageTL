ImageTL
=======

An image processing template library

There is a makefile included that will compile this source code into a library. Normally, you cannot use template code as a library. However, this library is setup to use explicit instantiation for most common data types you would use with an image.

Here are the specific types that are instantiated:
- char
- short
- int
- long
- float
- double
- std::complex<double> (for the ComplexImage class)

The makefile will pass set the IMAGETL_LIBRARY_COMPILE preprocessor definition when compiling. This will enable a section of code in each cpp file that uses explicit instantiation for all template classes and functions defined in the source file.

If you prefer to not use the library, or need to use a datatype that is not instantiated, simply set the IMAGETL_NO_LIBRARY preprocessor definition. This will incldue function definitions with each header file, as a template normally would.