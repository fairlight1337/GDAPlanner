#ifndef __PRINTABLE_H__
#define __PRINTABLE_H__


#include <iostream>
#include <string>
#include <memory>


namespace gdaplanner {
  class Printable {
  public:
    /** Shared pointer to Printable class */
    typedef std::shared_ptr<Printable> Ptr;
    
  private:
  protected:
  public:
    Printable();
    ~Printable();
    
    virtual std::string toString() const = 0;
  };
  
  std::ostream& operator<<(std::ostream& osStream, const Printable::Ptr& prtOut);
  std::ostream& operator<<(std::ostream& osStream, Printable& prtOut);
}


#endif /* __PRINTABLE_H__ */
