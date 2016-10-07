#include <gdaplanner/problems/Problem.h>


namespace gdaplanner {
  namespace problems {
    Problem::Problem() {
    }
    
    Problem::~Problem() {
    }
    
    std::string Problem::toString() const {
      return "Empty base problem definition";
    }
  }
}
