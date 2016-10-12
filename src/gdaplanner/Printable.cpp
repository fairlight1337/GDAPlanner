#include <gdaplanner/Printable.h>
#include <iostream>


namespace gdaplanner {
  Printable::Printable() {
  }
  
  Printable::~Printable() {
  }
  
  std::ostream& operator<<(std::ostream& osStream, const Printable::Ptr& prtOut) {
    osStream << prtOut->toString();
    
    return osStream;
  }
  
  std::ostream& operator<<(std::ostream& osStream, Printable& prtOut) {
    osStream << prtOut.toString();
    
    return osStream;
  }
}
