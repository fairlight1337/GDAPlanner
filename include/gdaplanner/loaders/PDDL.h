#ifndef __LOADERS_PDDL_H__
#define __LOADERS_PDDL_H__


#include <memory>
#include <iostream>

#include <gdaplanner/loaders/Loader.h>
#include <gdaplanner/contexts/PDDL.h>


namespace gdaplanner {
  namespace loaders {
    class PDDL : public Loader {
    public:
      typedef std::shared_ptr<PDDL> Ptr;
      
    private:
    protected:
    public:
      PDDL();
      ~PDDL();
      
      virtual bool processExpression(Expression exProcess, Context::Ptr ctxContext) override;
      virtual Context::Ptr makeContext() override;
      
      template<class ... Args>
	static PDDL::Ptr create(Args ... args) {
	return std::make_shared<PDDL>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __LOADERS_PDDL_H__ */
