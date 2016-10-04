#ifndef __PDDL_H__
#define __PDDL_H__


#include <memory>
#include <iostream>

#include <gdaplanner/problems/Problem.h>


namespace gdaplanner {
  namespace problems {
    class PDDL : public Problem {
    public:
      typedef std::shared_ptr<PDDL> Ptr;
      
    private:
    protected:
    public:
      PDDL();
      ~PDDL();
      
      template<class ... Args>
	static PDDL::Ptr create(Args ... args) {
	return std::make_shared<PDDL>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __PDDL_H__ */
