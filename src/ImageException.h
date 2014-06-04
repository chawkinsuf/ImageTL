#pragma once

#include <exception>
#include <string>

namespace ImageTL
{
	class ImageException : public std::exception
	{
	public:
		explicit ImageException(std::string m) throw() { m_msg = m; }
		virtual char* what() throw() { return (char*)m_msg.c_str(); }
		~ImageException() throw() {}
	private:
		std::string m_msg;
	};
}