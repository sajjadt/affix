#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <algorithm>
#include <opencv2/opencv.hpp>

inline std::string trim(const std::string &s) {
  auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
  auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
  return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

class Formatter {
public:
  Formatter() {}
  ~Formatter() {}

  template <typename Type>
  Formatter & operator << (const Type & value)
  {
    stream_ << value << " ";
    return *this;
  }

  std::string str() const         { return stream_.str(); }
  operator std::string () const   { return stream_.str(); }

  enum ConvertToString 
  {
    to_str
  };
  std::string operator >> (ConvertToString) { return stream_.str(); }

private:
  std::stringstream stream_;

  Formatter(const Formatter &);
  Formatter & operator = (Formatter &);
};

#define CHECK(status, kernel, arg) if(status != CL_SUCCESS) {\
  throw std::runtime_error(Formatter()<< "Failed to set arg"<< kernel << arg);\
}

