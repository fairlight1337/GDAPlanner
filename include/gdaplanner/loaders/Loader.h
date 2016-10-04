#ifndef __LOADER_H__
#define __LOADER_H__


#include <memory>
#include <iostream>
#include <string>
#include <fstream>

#include <gdaplanner/contexts/Context.h>
#include <gdaplanner/problems/Problem.h>


namespace gdaplanner {
  namespace loaders {
    class Loader {
    public:
      typedef std::shared_ptr<Loader> Ptr;
      
    private:
    protected:
    public:
      Loader();
      ~Loader();
      
      contexts::Context::Ptr readContextFile(std::string strFilepath);
      virtual contexts::Context::Ptr makeContext() = 0;
      virtual bool processExpression(Expression exProcess, contexts::Context::Ptr ctxContext) = 0;
      
      problems::Problem::Ptr readProblemFile(std::string strFilepath);
      virtual problems::Problem::Ptr makeProblem() = 0;
      virtual bool processExpression(Expression exProcess, problems::Problem::Ptr prbContext) = 0;
      
      template<class ... Args>
	static Loader::Ptr create(Args ... args) {
	return std::make_shared<Loader>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __LOADER_H__ */
