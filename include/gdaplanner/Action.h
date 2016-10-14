#pragma once
#ifndef __ACTION_H__
#define __ACTION_H__


#include <memory>
#include <iosfwd>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Predicate.h>
#include <gdaplanner/Printable.h>


extern template class std::vector<gdaplanner::Expression>;


namespace gdaplanner {
  /** \brief Class representing a first-order Action
      
      \since 0.1.0 */
  class Action : public Printable {
  public:
    /** Shared pointer to Action class */
    typedef std::shared_ptr<Action> Ptr;
    
  private:
    Predicate::Ptr m_pdPredicate;
    Expression m_exPreconditions;
    Expression m_exEffects;

    Action matchAction(std::vector<Action> const& prototypes, Expression const& step, int idx = -1);
    
  protected:
  public:
    Action(Predicate::Ptr pdPredicate, Expression exPreconditions, Expression exEffects);
    ~Action();

    Action parametrize(std::map<std::string, Expression> const& params) const;
    void initFromSequence(std::string const& name, std::vector<Action> const& prototypes, std::vector<Expression> const& steps);

    Expression const& predicateExpression() const
    {
        return m_pdPredicate->expression();
    }
    Expression const& preconditions() const
    {
        return m_exPreconditions;
    }
    Expression const& effects() const
    {
        return m_exEffects;
    }

    virtual std::string toString() const override;
    
    Expression expression();
    
    Predicate::Ptr predicate();
    
    template<class ... Args>
      static Action::Ptr create(Args ... args) {
      return std::make_shared<Action>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __ACTION_H__ */
