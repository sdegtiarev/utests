#ifndef _ITC_ERROR_H_
#define _ITC_ERROR_H_
#include <errno.h>
#include <string.h>
#include <exception>
#include <sstream>
#include <stdio.h>
namespace itc {

// general purpose error formatter
class error : public std::exception
{
public:
	error() {}
	error(const error& x) { _err<<x._err.str(); }
	virtual ~error() throw() {}
	template<class T> error(const T& t) { _err<<t; }
	template<class T> error& operator<<(const T& t) { _err<<t; return *this; }
	const char* what() const throw() { _holder=_err.str(); return _holder.c_str(); }

protected:
	std::ostringstream _err;
	mutable std::string _holder;
};


// just a name, same functionality as in the base class
//	the template parameter allows to differentiate catch'es
template<int ERROR_TAG>
class usrerr : public error
{
public:
	template<class T> usrerr(const T& t) : error(t) {}
	template<class T> usrerr& operator<<(const T& t) { _err<<t; return *this; }
};

template<typename ERROR_TAG>
class typederr : public error
{
public:
	template<class T> typederr(const T& t) : error(t) {}
	template<class T> typederr& operator<<(const T& t) { _err<<t; return *this; }
};



// system error, allows both explicit and implicit errno grabbing
class syserr : public error
{
public:
	syserr() :_sys(strerror(errno)) {}
	syserr(int err) :_sys(strerror(err)) {}
	template<class T> syserr(const T& t) : error(t),_sys(strerror(errno)) {}
	template<class T> syserr(const T& t, int err) : error(t),_sys(strerror(err)) {}
	template<class T> syserr& operator<<(const T& t) { _err<<t; return *this; }

	~syserr() throw() {}
	const char* what() const throw() { _holder=_err.str()+": "+_sys; return _holder.c_str(); }
private:
	std::string _sys;
};


namespace exception {
// sample error codes
enum {
	NONE
	, EXECUTING
	, BUF_OVERFLOW
	, BUF_SPACE
	, BUF_END
	, BUF_TIMEOUT
};
}//exception


}//itc
#endif

