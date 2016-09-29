#ifndef __LOADER_H__
#define __LOADER_H__


#include <memory>
#include <iostream>
#include <string>
#include <fstream>

#include <gdaplanner/contexts/Context.h>


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
      
      Context::Ptr readFile(std::string strFilepath);
      
      virtual bool processExpression(Expression exProcess, Context::Ptr ctxContext) = 0;
      virtual Context::Ptr makeContext() = 0;
      
      template<class ... Args>
	static Loader::Ptr create(Args ... args) {
	return std::make_shared<Loader>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __LOADER_H__ */
