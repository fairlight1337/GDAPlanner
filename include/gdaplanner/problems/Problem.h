#ifndef __PROBLEM_H__
#define __PROBLEM_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Printable.h>


namespace gdaplanner {
  namespace problems {
    class Problem : public Printable {
    public:
      typedef std::shared_ptr<Problem> Ptr;
      
    private:
    protected:
    public:
      Problem();
      ~Problem();
      
      virtual std::string toString() override;
      
      template<class ... Args>
	static Problem::Ptr create(Args ... args) {
	return std::make_shared<Problem>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __PROBLEM_H__ */
