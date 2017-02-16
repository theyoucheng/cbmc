/*******************************************************************\

Module: Coverage Instrumentation

Author: Daniel Kroening

Date: May 2016

\*******************************************************************/

#include <algorithm>
#include <iterator>

#include <util/prefix.h>
#include <util/i2string.h>
#include <util/expr_util.h>
#include <util/ieee_float.h>

#include "cover.h"
#include <iostream>

static double bv_toler=0;
static bool strong_in_type=false;
static bool weakly_strong_in_type=false;


class basic_blockst
{
public:
  explicit basic_blockst(const goto_programt &_goto_program)
  {
    bool next_is_target=true;
    unsigned block_count=0;

    forall_goto_program_instructions(it, _goto_program)
    {
      if(next_is_target || it->is_target())
        block_count++;
        
      block_map[it]=block_count;
      
      if(!it->source_location.is_nil() &&
         source_location_map.find(block_count)==source_location_map.end())
        source_location_map[block_count]=it->source_location;
      
      next_is_target=
        it->is_goto() || it->is_function_call() || it->is_assume();
    }
  }

  // map program locations to block numbers  
  typedef std::map<goto_programt::const_targett, unsigned> block_mapt;
  block_mapt block_map;
  
  // map block numbers to source code locations
  typedef std::map<unsigned, source_locationt> source_location_mapt;
  source_location_mapt source_location_map;
  
  inline unsigned operator[](goto_programt::const_targett t)
  {
    return block_map[t];
  }
  
  void output(std::ostream &out)
  {
    for(block_mapt::const_iterator
        b_it=block_map.begin();
        b_it!=block_map.end();
        b_it++)
      out << b_it->first->source_location
          << " -> " << b_it->second
          << '\n';
  }
};

/*******************************************************************\

Function: as_string

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

const char *as_string(coverage_criteriont c)
{
  switch(c)
  {
  case coverage_criteriont::LOCATION: return "location";
  case coverage_criteriont::BRANCH: return "branch";
  case coverage_criteriont::DECISION: return "decision";
  case coverage_criteriont::CONDITION: return "condition";
  case coverage_criteriont::PATH: return "path";
  case coverage_criteriont::MCDC: return "MC/DC";
  case coverage_criteriont::ASSERTION: return "assertion";
  case coverage_criteriont::COVER: return "cover instructions";
  default: return "";
  }
}

/*******************************************************************\

Function: is_condition

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool is_condition(const exprt &src)
{
  if(src.type().id()!=ID_bool) return false;

  // conditions are 'atomic predicates'
  if(src.id()==ID_and || src.id()==ID_or ||
     src.id()==ID_not || src.id()==ID_implies)
    return false;
  
  return true;
}

/*******************************************************************\

Function: collect_conditions_rec

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void collect_conditions_rec(const exprt &src, std::set<exprt> &dest)
{
  if(src.id()==ID_address_of)
  {
    return;
  }

  for(const auto & op : src.operands())
    collect_conditions_rec(op, dest);

  if(is_condition(src) && !src.is_constant())
    dest.insert(src); 
}

/*******************************************************************\

Function: collect_conditions

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::set<exprt> collect_conditions(const exprt &src)
{
  std::set<exprt> result;
  collect_conditions_rec(src, result);
  return result;
}

/*******************************************************************\

Function: collect_conditions

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::set<exprt> collect_conditions(const goto_programt::const_targett t)
{
  switch(t->type)
  {
  case GOTO:
  case ASSERT:
    return collect_conditions(t->guard);
  
  case ASSIGN:
  case FUNCTION_CALL:
    return collect_conditions(t->code);
    
  default:;
  }
  
  return std::set<exprt>();
}

/*******************************************************************\

Function: collect_operands

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void collect_operands(const exprt &src, std::vector<exprt> &dest)
{
  for(const exprt &op : src.operands())
  {
    if(op.id()==src.id())
      collect_operands(op, dest);
    else
      dest.push_back(op);
  }
}

/*******************************************************************\

Function: collect_mcdc_controlling_rec

  Inputs:

 Outputs:

 Purpose: To recursively collect controlling exprs for
          for mcdc coverage.

\*******************************************************************/

void collect_mcdc_controlling_rec(
  const exprt &src,
  const std::vector<exprt> &conditions,
  std::set<exprt> &result)
{
  // src is conjunction (ID_and) or disjunction (ID_or)
  if(src.id()==ID_and ||
     src.id()==ID_or)
  {
    std::vector<exprt> operands;
    collect_operands(src, operands);

    if(operands.size()==1)
    {
      exprt e=*operands.begin();
      collect_mcdc_controlling_rec(e, conditions, result);
    }
    else if(!operands.empty())
    {
      for(std::size_t i=0; i<operands.size(); i++)
      {
        const exprt op=operands[i];
      
        if(is_condition(op))
        {
          if(src.id()==ID_or)
          {
            std::vector<exprt> others1, others2;
            if(!conditions.empty())
            {
              others1.push_back(conjunction(conditions));
              others2.push_back(conjunction(conditions));
            }
            
            for(std::size_t j=0; j<operands.size(); j++)
            {
              others1.push_back(not_exprt(operands[j]));
              if(i!=j)
                others2.push_back(not_exprt(operands[j]));
              else
                others2.push_back((operands[j]));
            }

            result.insert(conjunction(others1));
            result.insert(conjunction(others2));
            continue;
          }

          std::vector<exprt> o=operands;
        
          // 'o[i]' needs to be true and false
          std::vector<exprt> new_conditions=conditions;
          new_conditions.push_back(conjunction(o));
          result.insert(conjunction(new_conditions));

          o[i].make_not();
          new_conditions.back()=conjunction(o);
          result.insert(conjunction(new_conditions));
        }
        else
        {
          std::vector<exprt> others;
          others.reserve(operands.size()-1);

          for(std::size_t j=0; j<operands.size(); j++)
            if(i!=j)
            {
              if(src.id()==ID_or)
                others.push_back(not_exprt(operands[j]));
              else
                others.push_back(operands[j]);
            }
            
          exprt c=conjunction(others);
          std::vector<exprt> new_conditions=conditions;
          new_conditions.push_back(c);

          collect_mcdc_controlling_rec(op, new_conditions, result);
        }
      }
    }
  }
  else if(src.id()==ID_not)
  {
    exprt e=to_not_expr(src).op();
    if(!is_condition(e))
      collect_mcdc_controlling_rec(e, conditions, result);
    else
    {
      // to store a copy of ''src''
      std::vector<exprt> new_conditions1=conditions;
      new_conditions1.push_back(src);
      result.insert(conjunction(new_conditions1));

      // to store a copy of its negation, i.e., ''e''
      std::vector<exprt> new_conditions2=conditions;
      new_conditions2.push_back(e);
      result.insert(conjunction(new_conditions2));
    }
  }
  else
  {
    /**
     * It may happen that ''is_condition(src)'' is valid,
     * but we ignore this case here as it can be handled
     * by the routine decision/condition detection.
     **/
  }
}

/*******************************************************************\

Function: collect_mcdc_controlling

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::set<exprt> autosac_atomic_expand(const exprt &src);

std::set<exprt> collect_mcdc_controlling(
  const std::set<exprt> &decisions)
{
  std::set<exprt> result;
  
  for(const auto &d : decisions)
  {
    exprt e(d);
    if(e.id()==ID_not) e=e.op0();
    if(e.id()==ID_lt
       or e.id()==ID_le
       or e.id()==ID_equal
       or e.id()==ID_ge
       or e.id()==ID_gt)
    {
      auto res=autosac_atomic_expand(e);
      if(not res.empty())
        result.insert(res.begin(), res.end());
      else result.insert(e);
      e.make_not();
      res=autosac_atomic_expand(e);
      if(not res.empty())
        result.insert(res.begin(), res.end());
      else result.insert(e);
      continue; //return result;
    }
    if(d.operands().size() > 1)
      collect_mcdc_controlling_rec(d, { }, result);
  }

  return result;
}

/*******************************************************************\

Function: replacement_conjunction

  Inputs:

 Outputs:

 Purpose: To replace the i-th expr of ''operands'' with each
          expr inside ''replacement_exprs''.

\*******************************************************************/

std::set<exprt> replacement_conjunction(
  const std::set<exprt> &replacement_exprs,
  const std::vector<exprt> &operands,
  const std::size_t i)
{
  std::set<exprt> result;
  for(auto &y : replacement_exprs)
  {
    std::vector<exprt> others;
    for(std::size_t j=0; j<operands.size(); j++)
      if(i!=j)
        others.push_back(operands[j]);

    others.push_back(y);
    exprt c=conjunction(others);
    result.insert(c);
  }
  return result;
}
        
/*******************************************************************\

Function: collect_mcdc_controlling_nested

  Inputs:

 Outputs:

 Purpose: This nested method iteratively applies 
          ''collect_mcdc_controlling'' to every non-atomic expr
          within a decision

\*******************************************************************/

std::set<exprt> collect_mcdc_controlling_nested(
  const std::set<exprt> &decisions)
{
  // To obtain the 1st-level controlling conditions
  std::set<exprt> controlling = collect_mcdc_controlling(decisions);

  std::set<exprt> result;
  // For each controlling condition, to check if it contains
  // non-atomic exprs
  for(auto &src : controlling)
  {
    std::set<exprt> s1, s2;
    /**
     * The final controlling conditions resulted from ''src''
     * will be stored in ''s1''; ''s2'' is usd to hold the
     * temporary expansion.
     **/
    s1.insert(src);
    
    // dual-loop structure to eliminate complex 
    // non-atomic-conditional terms
    while(true) 
    {
      bool changed=false;
      // the 2nd loop
      for(auto &x : s1)
      {
        // if ''x'' is atomic conditional term, there
        // is no expansion
        if(is_condition(x))
        {
          s2.insert(x);
          continue;
        }
        // otherwise, we apply the ''nested'' method to
        // each of its operands
        std::vector<exprt> operands;
        collect_operands(x, operands);

        for(std::size_t i=0; i<operands.size(); i++)
        {
          std::set<exprt> res;
          /**
           * To expand an operand if it is not atomic,
           * and label the ''changed'' flag; the resulted 
           * expansion of such an operand is stored in ''res''.
           **/
          if(operands[i].id()==ID_not)
          {
            exprt no=operands[i].op0();
            if(!is_condition(no))
            {
              changed=true;
              std::set<exprt> ctrl_no;
              ctrl_no.insert(no);
              res=collect_mcdc_controlling(ctrl_no);
            }
          }
          else if(!is_condition(operands[i]))
          {
            changed=true;
            std::set<exprt> ctrl;
            ctrl.insert(operands[i]);
            res=collect_mcdc_controlling(ctrl);
          }

          // To replace a non-atomic expr with its expansion
          std::set<exprt> co=
            replacement_conjunction(res, operands, i);
          s2.insert(co.begin(), co.end());
          if(res.size() > 0) break;
        }
        // if there is no change x.r.t current operands of ''x'',
        // i.e., they are all atomic, we reserve ''x''
        if(!changed) s2.insert(x);
      }
      // update ''s1'' and check if change happens
      s1=s2;
      if(!changed) {break;}
      s2.clear();
    }

    // The expansions of each ''src'' should be added into
    // the ''result''
    result.insert(s1.begin(), s1.end());
  }
  
  return result;
}

/*******************************************************************\

Function: sign_of_expr

  Inputs: E should be the pre-processed output by
          ''collect_mcdc_controlling_nested''

 Outputs: +1 : not negated
          -1 : negated

 Purpose: The sign of expr ''e'' within the super-expr ''E''

\*******************************************************************/

std::set<signed> sign_of_expr(const exprt &e, const exprt &E)
{
  std::set<signed> signs;

  // At fist, we pre-screen the case such that ''E'' 
  // is an atomic condition
  if(is_condition(E))
  {
    if(e==E)
      signs.insert(+1);
    return signs;
  }

  // or, ''E'' is the negation of ''e''?
  if(E.id()==ID_not)
  {
    if(e==E.op0())
    {
      signs.insert(-1);
      return signs;
    }
  }

  /**
   * In the general case, we analyze each operand of ''E''.
   **/
  std::vector<exprt> ops;
  collect_operands(E, ops);
  for(auto &x : ops)
  {
    exprt y(x);
    if(y == e) signs.insert(+1);
    else if(y.id()==ID_not)
    {
      y.make_not();
      if(y==e) signs.insert(-1);
      if(!is_condition(y))
      {
        std::set<signed> re=sign_of_expr(e, y);
        signs.insert(re.begin(), re.end());
      }
    }
    else if(!is_condition(y))
    {
      std::set<signed> re=sign_of_expr(e, y);
      signs.insert(re.begin(), re.end());
    }
  }

  return signs;
}

/*******************************************************************\

Function: remove_repetition

  Inputs:

 Outputs:

 Purpose: After the ''collect_mcdc_controlling_nested'', there
          can be the recurrence of the same expr in the resulted
          set of exprs, and this method will remove the
          repetitive ones.

\*******************************************************************/

void remove_repetition(std::set<exprt> &exprs)
{
  // to obtain the set of atomic conditions
  std::set<exprt> conditions;
  for(auto &x: exprs)
  {
    std::set<exprt> new_conditions = collect_conditions(x);
    conditions.insert(new_conditions.begin(), new_conditions.end());
  }
  // exprt that contains multiple (inconsistent) signs should
  // be removed
  std::set<exprt> new_exprs;
  for(auto &x : exprs)
  {
    bool kept=true;
    for(auto &c : conditions)
    {
      std::set<signed> signs=sign_of_expr(c, x);
      if(signs.size()>1)
      {
        kept=false;
        break;
      }
    }
    if(kept) new_exprs.insert(x);
  }
  exprs=new_exprs;
  new_exprs.clear();

  for(auto &x: exprs)
  {
    bool red=false;
    /**
     * To check if ''x'' is identical with some
     * expr in ''new_exprs''. Two exprs ''x''
     * and ''y'' are identical iff they have the
     * same sign for every atomic condition ''c''.
     **/
    for(auto &y: new_exprs)
    {
      bool iden = true;
      for(auto &c : conditions)
      {
        std::set<signed> signs1=sign_of_expr(c, x);
        std::set<signed> signs2=sign_of_expr(c, y);
        int s1=signs1.size(), s2=signs2.size();
        // it is easy to check non-equivalence;
        if(s1!=s2)
        {
          iden=false;
          break;
        }
        else
        {
          if(s1==0 && s2==0) continue;
          // it is easy to check non-equivalence
          if(*(signs1.begin())!=*(signs2.begin()))
          {
            iden=false;
            break;
          }
        }
      }
      /**
       * If ''x'' is found identical w.r.t some
       * expr in ''new_conditions, we label it
       * and break.
       **/
      if(iden) 
      {
        red=true;
        break;
      }
    }
    // an expr is added into ''new_exprs''
    // if it is not found repetitive
    if(!red) new_exprs.insert(x);
  }

  // update the original ''exprs''
  exprs = new_exprs;
}

/*******************************************************************\

Function: eval_expr

  Inputs:

 Outputs:

 Purpose: To evaluate the value of expr ''src'', according to
          the atomic expr values

\*******************************************************************/
bool eval_expr(
  const std::map<exprt, signed> &atomic_exprs, 
  const exprt &src)
{
  std::vector<exprt> operands;
  collect_operands(src, operands);
  // src is AND
  if(src.id()==ID_and)
  {
    for(auto &x : operands)
      if(!eval_expr(atomic_exprs, x))
        return false;
    return true;
  }
  // src is OR
  else if(src.id()==ID_or)
  {
    std::size_t fcount=0;

    for(auto &x : operands)
      if(!eval_expr(atomic_exprs, x))
        fcount++;

    if(fcount<operands.size())
      return true;
    else
      return false;
  }
  // src is NOT
  else if(src.id()==ID_not)
  {
    exprt no_op(src);
    no_op.make_not();
    if(is_condition(no_op))
    {
      return false; //atomic_exprs.find(no_op)->second==-1;
    }
    return !eval_expr(atomic_exprs, no_op);
  }
  else //if(is_condition(src))
  {
    // ''src'' should be guaranteed to be consistent
    // with ''atomic_exprs''
    if(atomic_exprs.find(src)->second==+1)
      return true;
    else //if(atomic_exprs.find(src)->second==-1)
      return false;
  }
}

/*******************************************************************\

Function: values_of_atomic_exprs

  Inputs: 

 Outputs:

 Purpose: To obtain values of atomic exprs within the super expr

\*******************************************************************/

std::map<exprt, signed> values_of_atomic_exprs(
  const exprt &e,
  const std::set<exprt> &conditions)
{
  std::map<exprt, signed> atomic_exprs;
  for(auto &c : conditions)
  {
    std::set<signed> signs=sign_of_expr(c, e);
    if(signs.size()==0)
    {
      // ''c'' is not contained in ''e''
      atomic_exprs.insert(std::pair<exprt, signed>(c, 0));
      continue;
    }
    // we do not consider inconsistent expr ''e''
    if(signs.size()!=1) continue;

    atomic_exprs.insert(
      std::pair<exprt, signed>(c, *signs.begin()));
  }
  return atomic_exprs;
}

/*******************************************************************\

Function: is_mcdc_pair

  Inputs: 

 Outputs:

 Purpose: To check if the two input controlling exprs are mcdc
          pairs regarding an atomic expr ''c''. A mcdc pair of
          (e1, e2) regarding ''c'' means that ''e1'' and ''e2''
          result in different ''decision'' values, and this is 
          caused by the different choice of ''c'' value.

\*******************************************************************/

bool is_mcdc_pair(
  const exprt &e1,
  const exprt &e2,
  const exprt &c,
  const std::set<exprt> &conditions,
  const exprt &decision)
{
  // An controlling expr cannot be mcdc pair of itself
  if(e1==e2) return false;
  
  // To obtain values of each atomic condition within ''e1'' 
  // and ''e2''
  std::map<exprt, signed> atomic_exprs_e1=
    values_of_atomic_exprs(e1, conditions);
  std::map<exprt, signed> atomic_exprs_e2=
    values_of_atomic_exprs(e2, conditions);

  // the sign of ''c'' inside ''e1'' and ''e2''
  signed cs1=atomic_exprs_e1.find(c)->second;
  signed cs2=atomic_exprs_e2.find(c)->second;
  // a mcdc pair should both contain ''c'', i.e., sign=+1 or -1
  if(cs1==0||cs2==0)
    return false;
  
  // A mcdc pair regarding an atomic expr ''c''
  // should have different values of ''c''
  if(cs1==cs2)
    return false;

  // A mcdc pair should result in different ''decision''
  if(eval_expr(atomic_exprs_e1, decision)==
     eval_expr(atomic_exprs_e2, decision))
    return false;

  /**
   *  A mcdc pair of controlling exprs regarding ''c''
   *  can have different values for only one atomic
   *  expr, i.e., ''c''. Otherwise, they are not
   *  a mcdc pair.
   **/
  int diff_count=0;
  auto e1_it=atomic_exprs_e1.begin();
  auto e2_it=atomic_exprs_e2.begin();
  while(e1_it!=atomic_exprs_e1.end())
  {
    if(e1_it->second!=e2_it->second)
    diff_count++;
    if(diff_count>1) break;
    e1_it++;
    e2_it++;
  }

  if(diff_count==1) return true;
  else return false;
}

/*******************************************************************\

Function: has_mcdc_pair

  Inputs: 

 Outputs:

 Purpose: To check if we can find the mcdc pair of the 
          input ''expr_set'' regarding the atomic expr ''c''

\*******************************************************************/

bool has_mcdc_pair(
  const exprt &c,
  const std::set<exprt> &expr_set,
  const std::set<exprt> &conditions,
  const exprt &decision)
{
  for(auto y1 : expr_set)
  {
    for(auto y2 : expr_set)
    {
      if(is_mcdc_pair(y1, y2, c, conditions, decision))
      {
        return true;
      }
    }
  }
  return false;
}

/*******************************************************************\

Function: minimize_mcdc_controlling

  Inputs: The input ''controlling'' should have been processed by
          ''collect_mcdc_controlling_nested''
                  and
          ''remove_repetition''

 Outputs:

 Purpose: This method minimizes the controlling conditions for
          mcdc coverage. The minimum is in a sense that by deleting
          any controlling condition in the set, the mcdc coverage
          for the decision will be not complete.

\*******************************************************************/

void minimize_mcdc_controlling(
  std::set<exprt> &controlling,
  const exprt &decision)
{
  // to obtain the set of atomic conditions
  std::set<exprt> conditions;
  for(auto &x: controlling)
  {
    std::set<exprt> new_conditions = collect_conditions(x);
    conditions.insert(new_conditions.begin(), new_conditions.end());
  }

  while(true)
  {
    std::set<exprt> new_controlling;
    bool ctrl_update=false;
    /**
     * Iteratively, we test that after removing an item ''x''
     * from the ''controlling'', can a complete mcdc coverage 
     * over ''decision'' still be reserved?
     *
     * If yes, we update ''controlling'' with the 
     * ''new_controlling'' without ''x''; otherwise, we should 
     * keep ''x'' within ''controlling''.
     *
     * If in the end all elements ''x'' in ''controlling'' are 
     * reserved, this means that current ''controlling'' set is 
     * minimum and the ''while'' loop should be breaked.
     *
     * Note:  implementaion here for the above procedure is 
     *        not (meant to be) optimal.
     **/
    for(auto &x : controlling)
    {
      // To create a new ''controlling'' set without ''x''
      new_controlling.clear();
      for(auto &y : controlling)
        if(y!=x) new_controlling.insert(y);

      bool removing_x=true;
      // For each atomic expr condition ''c'', to check if its is
      // covered by at least a mcdc pair.
      for(auto &c : conditions)
      {
        bool cOK=
          has_mcdc_pair(c, new_controlling, conditions, decision);
        /**
         *  If there is no mcdc pair for an atomic condition ''c'',
         *  then ''x'' should not be removed from the original 
         *  ''controlling'' set
         **/
        if(!cOK)
        {
          removing_x=false;
          break;
        }
      }

      // If ''removing_x'' is valid, it is safe to remove ''x'' 
      // from the original ''controlling''
      if(removing_x)
      {
        ctrl_update=true;
        break;
      }
    }
    // Update ''controlling'' or break the while loop
    if(ctrl_update)
    {
      controlling=new_controlling;
    }
    else break;
  }
}

/*******************************************************************\

Function: expand_ite_expr

  Inputs:

 Outputs:

 Purpose: expand an if-then-else expr

\*******************************************************************/

void expand_ite_expr(exprt &x)
{
  for(exprt & y : x.operands())
    expand_ite_expr(y);

  if(x.id()==ID_if)
  {
    assert(x.operands().size()==3);
    std::vector<exprt> conj_a, conj_b, final_disj;
    x.op0().type().id(ID_bool);
    x.op1().type().id(ID_bool);
    x.op2().type().id(ID_bool);
    conj_a.push_back(x.op0());
    conj_a.push_back(x.op1());
    conj_b.push_back(not_exprt(x.op0()));
    conj_b.push_back(x.op2());
    final_disj.push_back(conjunction(conj_a));
    final_disj.push_back(conjunction(conj_b));
    exprt res=disjunction(final_disj);
    x=res;
  }
}

/*******************************************************************\

Function: collect_decisions_rec

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void collect_decisions_rec(const exprt &src, std::set<exprt> &dest)
{
  if(src.id()==ID_address_of)
  {
    return;
  }

  if(src.type().id()==ID_bool)
  {
    if(src.is_constant())
    {
      // ignore me
    }
    else if(src.id()==ID_not)
    {
      collect_decisions_rec(src.op0(), dest);
    }
    else
    {
      dest.insert(src); 
    }
  }
  //else if(src.id()==ID_if)
  //{
  //  exprt expr(src);
  //  expand_ite_expr(expr); 
  //  dest.insert(expr); 
  //}
  else
  {
    for(const auto & op : src.operands())
      collect_decisions_rec(op, dest);
  }
}

/*******************************************************************\

Function: collect_decisions

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::set<exprt> collect_decisions(const exprt &src)
{
  std::set<exprt> result;
  collect_decisions_rec(src, result);
  return result;
}

/*******************************************************************\

Function: collect_decisions

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

std::set<exprt> collect_decisions(const goto_programt::const_targett t)
{
  switch(t->type)
  {
  case GOTO:
  case ASSERT:
    return collect_decisions(t->guard);
  
  case ASSIGN:
  case FUNCTION_CALL:
    return collect_decisions(t->code);
    
  default:;
  }
  
  return std::set<exprt>();
}

/*******************************************************************\

Function: autosac_atomic_negate

  Inputs:

 Outputs:

 Purpose: negate an expr (<, <=, ==, >=, >) by the AUTOSAC semantic

\*******************************************************************/
static std::set<exprt> bv_with_tolerance(const exprt& src);

std::set<exprt> autosac_atomic_negate(const exprt &src)
{

  if(src.id()==ID_typecast)
    return autosac_atomic_negate(src.op0());

  /**
   * This is a customized approach to "negate" exprs 
   **/
  std::set<exprt> result;

  if( src.id()==ID_equal)
  {
    // the negation of "==" is "<" and ">"
    exprt e1(ID_lt);
    e1.type().id(src.type().id());
    e1.operands().push_back(src.op0());
    e1.operands().push_back(src.op1());

    exprt e2(ID_gt);
    e2.type().id(src.type().id());
    e2.operands().push_back(src.op0());
    e2.operands().push_back(src.op1());

    result.insert(e1);
    result.insert(e2);

    //return result;
  }
  else if(src.id()==ID_lt)
  {
    //the negation of "<" should be "==" and ">"
    exprt e1(ID_equal);
    e1.type().id(src.type().id());
    e1.operands().push_back(src.op0());
    e1.operands().push_back(src.op1());

    exprt e2(ID_gt);
    e2.type().id(src.type().id());
    e2.operands().push_back(src.op0());
    e2.operands().push_back(src.op1());

    result.insert(e1);
    result.insert(e2);

    //return result;
  }
  else if(src.id()==ID_le)
  {
    //the negation of "<=" should be ">"
    exprt e1(ID_gt);
    e1.type().id(src.type().id());
    e1.operands().push_back(src.op0());
    e1.operands().push_back(src.op1());

    result.insert(e1);

    //return result;
  }
  else if(src.id()==ID_ge)
  {
    //the negation of ">=" should be "<"
    exprt e1(ID_lt);
    e1.type().id(src.type().id());
    e1.operands().push_back(src.op0());
    e1.operands().push_back(src.op1());

    result.insert(e1);

    //return result;
  }
  else if(src.id()==ID_gt)
  {
    //the negation of ">" should be "==" and "<"
    exprt e1(ID_equal);
    e1.type().id(src.type().id());
    e1.operands().push_back(src.op0());
    e1.operands().push_back(src.op1());

    exprt e2(ID_lt);
    e2.type().id(src.type().id());
    e2.operands().push_back(src.op0());
    e2.operands().push_back(src.op1());

    result.insert(e1);
    result.insert(e2);

    //return result;
  }
  return result;
}

/*******************************************************************\

Function: autosac_atomic_expand

  Inputs:

 Outputs:

 Purpose: expane an expr (<=, >=) by the AUTOSAC semantic

\*******************************************************************/

static std::set<exprt> bv_with_tolerance(const exprt& src)
{
  assert(bv_toler>=-1 && bv_toler<=1);
  if(bv_toler==0) 
  {
    return {};
  } 

  std::set<exprt> result;
  auto &type=src.op0().type().id();
  if(type==ID_signedbv or type==ID_unsignedbv)
  {
    // +1
    exprt e3(ID_equal);
    e3.type().id(src.type().id());
    e3.operands().push_back(src.op0());

    exprt tole(ID_plus, src.op0().type());
    tole.operands().push_back(src.op1());
    exprt p1= gen_one(src.op0().type());
    tole.operands().push_back(p1);

    e3.operands().push_back(tole);
    result.insert(e3);
  
    // -1
    exprt e4(ID_equal);
    e4.type().id(src.type().id());
    e4.operands().push_back(src.op0());

    exprt tole2(ID_minus, src.op0().type());
    tole2.operands().push_back(src.op1());
    exprt m1= gen_one(src.op0().type());
    tole2.operands().push_back(m1);

    e4.operands().push_back(tole2);
    result.insert(e4);
  }
  else if(type==ID_floatbv)
  {
    {
    // 0.99
    ieee_floatt f1;
    f1.from_float(1-bv_toler);
    ieee_float_spect dest_spec;
    floatbv_typet fbv_type=to_floatbv_type(src.op0().type());
    dest_spec.from_type(fbv_type);
    f1.change_spec(dest_spec);
    exprt p1=f1.to_expr();

    exprt e3(ID_equal);
    e3.type().id(src.type().id());
    e3.operands().push_back(src.op0());

    exprt tole3(ID_mult, src.op0().type());
    tole3.operands().push_back(src.op1());
    tole3.operands().push_back(p1);
    e3.operands().push_back(tole3);

    result.insert(e3);
    }
  
    {
    // 1.01
    ieee_floatt f1;
    f1.from_float(1+bv_toler);
    ieee_float_spect dest_spec;
    floatbv_typet fbv_type=to_floatbv_type(src.op0().type());
    dest_spec.from_type(fbv_type);
    f1.change_spec(dest_spec);
    exprt p1=f1.to_expr();

    exprt e3(ID_equal);
    e3.type().id(src.type().id());
    e3.operands().push_back(src.op0());

    exprt tole3(ID_mult, src.op0().type());
    tole3.operands().push_back(src.op1());
    tole3.operands().push_back(p1);
    e3.operands().push_back(tole3);

    result.insert(e3);
    }
  
  }
  return result;
}

std::set<exprt> autosac_atomic_expand(const exprt &src)
{
  int integer_tolerance_level_a=1;
  int integer_tolerance_level_b=-1;
  float float_tolerance_level_a=0.99;
  float float_tolerance_level_b=1.01;
  /**
   * This is a customized approach to "expand" exprs 
   **/
  std::set<exprt> result;

  if(src.id()==ID_le
     or src.id()==ID_lt)
  {
    exprt e1(ID_lt);
    e1.type().id(src.type().id());
    e1.operands().push_back(src.op0());
    e1.operands().push_back(src.op1());
    result.insert(e1);

    exprt e2(ID_equal);
    e2.type().id(src.type().id());
    e2.operands().push_back(src.op0());
    e2.operands().push_back(src.op1());
    result.insert(e2);

    auto res=bv_with_tolerance(src);
    result.insert(res.begin(), res.end());

    // tolerance level
    //auto &type=src.op0().type().id();
    //if(type==ID_signedbv or type==ID_unsignedbv)
    //{
    //  // +1
    //  exprt e3(ID_equal);
    //  e3.type().id(src.type().id());
    //  e3.operands().push_back(src.op0());

    //  exprt tole(ID_plus, src.op0().type());
    //  tole.operands().push_back(src.op1());
    //  constant_exprt p1(i2string(1), src.op0().type());
    //  tole.operands().push_back(p1);

    //  e3.operands().push_back(tole);
    //  result.insert(e3);
   
    //  // -1
    //  exprt e4(ID_equal);
    //  e4.type().id(src.type().id());
    //  e4.operands().push_back(src.op0());

    //  exprt tole2(ID_minus, src.op0().type());
    //  tole2.operands().push_back(src.op1());
    //  constant_exprt m1(i2string(1), src.op0().type());
    //  tole2.operands().push_back(m1);

    //  e4.operands().push_back(tole2);
    //  result.insert(e4);
    //}
    //else if(type==ID_floatbv)
    //{
    //  {
    //  // 0.99
    //  ieee_floatt f1;
    //  f1.from_float(0.99);
    //  ieee_float_spect dest_spec;
    //  floatbv_typet fbv_type=to_floatbv_type(src.op0().type());
    //  dest_spec.from_type(fbv_type);
    //  f1.change_spec(dest_spec);
    //  exprt p1=f1.to_expr();

    //  exprt e3(ID_equal);
    //  e3.type().id(src.type().id());
    //  e3.operands().push_back(src.op0());

    //  exprt tole3(ID_mult, src.op0().type());
    //  tole3.operands().push_back(src.op1());
    //  tole3.operands().push_back(p1);
    //  e3.operands().push_back(tole3);

    //  result.insert(e3);
    //  }
   
    //  {
    //  // 1.01
    //  ieee_floatt f1;
    //  f1.from_float(1.01);
    //  ieee_float_spect dest_spec;
    //  floatbv_typet fbv_type=to_floatbv_type(src.op0().type());
    //  dest_spec.from_type(fbv_type);
    //  f1.change_spec(dest_spec);
    //  exprt p1=f1.to_expr();

    //  exprt e3(ID_equal);
    //  e3.type().id(src.type().id());
    //  e3.operands().push_back(src.op0());

    //  exprt tole3(ID_mult, src.op0().type());
    //  tole3.operands().push_back(src.op1());
    //  tole3.operands().push_back(p1);
    //  e3.operands().push_back(tole3);

    //  result.insert(e3);
    //  }
   
    //}

    return result;
  }
  if(src.id()==ID_ge
    or src.id()==ID_gt)
  {
    //the expansion of ">=" should be ">" and "=="
    exprt e1(ID_gt);
    e1.type().id(src.type().id());
    e1.operands().push_back(src.op0());
    e1.operands().push_back(src.op1());
    result.insert(e1);

    exprt e2(ID_equal);
    e2.type().id(src.type().id());
    e2.operands().push_back(src.op0());
    e2.operands().push_back(src.op1());
    result.insert(e2);

    auto res=bv_with_tolerance(src);
    result.insert(res.begin(), res.end());

    return result;
  }

  return result;
}


/*******************************************************************\

Function: autosac_expand

  Inputs:

 Outputs:

 Purpose: expand an expr by the AUTOSAC semantic

\*******************************************************************/

std::set<exprt> autosac_expand(const exprt &src)
{
  std::set<exprt> result;

  std::set<exprt> s1, s2;
  if(src.id()!=ID_not)
    s1.insert(src);
  else
  {
    exprt no=src.op0();
    if(no.id()==ID_equal 
      or no.id()==ID_lt 
      or no.id()==ID_le 
      or no.id()==ID_ge 
      or no.id()==ID_gt
      or no.id()==ID_typecast)
    {
      auto res=autosac_atomic_negate(no);
      for(auto &x:res)
        s1.insert(x);
    }
  }


  while(true) // dual-loop structure to expand negations
  {
    bool changed=false;
    for(auto &x : s1)
    {
      std::vector<exprt> operands;
      collect_operands(x, operands);
      for(int i=0; i<operands.size(); i++)
      {
        std::set<exprt> res;
        if(operands[i].id()==ID_not)
        {
          exprt no=operands[i].op0();
          if(no.id()==ID_equal 
             or no.id()==ID_lt 
             or no.id()==ID_le 
             or no.id()==ID_ge 
             or no.id()==ID_gt
             or no.id()==ID_typecast)
          {
            changed=true;
            res=autosac_atomic_negate(no);
          }
        }

        std::set<exprt> co=replacement_conjunction(res, operands, i);
        s2.insert(co.begin(), co.end());
        if(res.size() > 0) break;
      }
      if(not changed) s2.insert(x);
    }
    if(not changed)
      break; //return s1;
    s1=s2;
    s2.clear();
  }

  s2.clear();
  while(true) // dual-loop structure to expand autosac atomics
  {
    bool changed=false;
    for(auto &x : s1)
    {
      std::vector<exprt> operands;
      collect_operands(x, operands);
      for(int i=0; i<operands.size(); i++)
      {
        std::set<exprt> res;
        if(operands[i].id()==ID_le
           or operands[i].id()==ID_ge)
        {
          changed=true;
          res=autosac_atomic_expand(operands[i]);
        }

        std::set<exprt> co=replacement_conjunction(res, operands, i);
        s2.insert(co.begin(), co.end());
        if(res.size() > 0) break;
      }
      if(not changed) s2.insert(x);
    }
    if(not changed)
      break; //return s1;
    s1=s2;
    s2.clear();
  }

  s2.clear();
  //while(true) // dual-loop structure to expand autosac atomics
  {
    bool changed=false;
    for(auto &x : s1)
    {
      std::vector<exprt> operands;
      collect_operands(x, operands);
      for(int i=0; i<operands.size(); i++)
      {
        std::set<exprt> res;
        if(operands[i].id()==ID_lt
           or operands[i].id()==ID_gt)
        {
          changed=true;
          res=autosac_atomic_expand(operands[i]);
        }

        std::set<exprt> co=replacement_conjunction(res, operands, i);
        s2.insert(co.begin(), co.end());
        //if(res.size() > 0) break;
      }
      //if(not changed) s2.insert(x);
    }
    //if(not changed)
    //  break; //return s1;
    s1=s2;
    //s2.clear();
  }

  return s1;

}

/*******************************************************************\

Function: is_autosac_function

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/
bool is_autosac_function(const codet& code)
{
  const code_function_callt &code_function_call=
    to_code_function_call(code);
  std::string func_name=to_symbol_expr(
    code_function_call.function()).get_identifier().c_str();
  return has_prefix(func_name, "__AUTOSAC");
}

bool is_autosac_in_type_function(const codet& code)
{
  const code_function_callt &code_function_call=
    to_code_function_call(code);
  std::string func_name=to_symbol_expr(
    code_function_call.function()).get_identifier().c_str();
  return has_prefix(func_name, "__AUTOSAC_in_type");
}

bool is_autosac_barrier(const codet& code)
{
  const code_function_callt &code_function_call=
    to_code_function_call(code);
  if(code_function_call.arguments().size()!=1) return false;
  auto it=code_function_call.arguments().begin();
  return from_expr(*it)=="\"preconditions\"" || from_expr(*it)=="\"postconditions\"";
}

bool is_autosac_postcondition(const codet& code)
{
  const code_function_callt &code_function_call=
	to_code_function_call(code);
  std::string func_name=to_symbol_expr(
	code_function_call.function()).get_identifier().c_str();
  return has_prefix(func_name, "__AUTOSAC_postcondition");
}

bool function_name_prefix(const codet& code, const std::string& s)
{
  const code_function_callt &code_function_call=
	to_code_function_call(code);
  std::string func_name=to_symbol_expr(
	code_function_call.function()).get_identifier().c_str();
  return has_prefix(func_name, s);
}

std::string autosac_description(const codet& code)
{
  const code_function_callt &code_function_call=
    to_code_function_call(code);
  if(code_function_call.arguments().size()!=2) return "";
  auto it=code_function_call.arguments().begin();
  it++;
  return from_expr(*it);
}


/*******************************************************************\

Function: instrument_cover_goals

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/


bool is_autosac_expression_function(const goto_programt &goto_program)
{

  forall_goto_program_instructions(i_it, goto_program)
  {
    if(!i_it->is_function_call())	continue;
    if(function_name_prefix(i_it->code, std::string("__AUTOSAC_expression_function")))
      return true;
  }
  return false;
}


void instrument_cover_goals(
  const symbol_tablet &symbol_table,
  goto_programt &goto_program,
  coverage_criteriont criterion)
{

bool is_autosac_expr=false;

std::vector<exprt> expression_functions, expression_bodys;


std::vector<std::set<exprt> > autosac_vect;
std::vector<std::string > autosac_words;
std::vector<exprt> autosac_in_types;
std::string autosac_in_type_str="";
std::vector<std::string> autosac_in_type_strs;

  const namespacet ns(symbol_table);
  basic_blockst basic_blocks(goto_program);
  std::set<unsigned> blocks_done;
  // ignore if built-in library
  if(!goto_program.instructions.empty() &&
     has_prefix(id2string(goto_program.instructions.front().source_location.get_file()),
                "<builtin-library-"))
    return;
  
  Forall_goto_program_instructions(i_it, goto_program)
  {

	if(i_it->is_function_call())
	  if(function_name_prefix(i_it->code, std::string("__AUTOSAC_expression_function")))
	  {
		  is_autosac_expr=true;
		  continue;
	  }

    switch(criterion)
    {
    case coverage_criteriont::ASSERTION:
      // turn into 'assert(false)' to avoid simplification
      if(i_it->is_assert())
      {
        i_it->guard=false_exprt();
        i_it->source_location.set_property_class("coverage");
      }
      break;
      
    case coverage_criteriont::COVER:
      // turn __CPROVER_cover(x) into 'assert(!x)'
      if(i_it->is_function_call())
      {
        const code_function_callt &code_function_call=
          to_code_function_call(i_it->code);
        if(code_function_call.function().id()==ID_symbol &&
           to_symbol_expr(code_function_call.function()).get_identifier()==
           "__CPROVER_cover" &&
           code_function_call.arguments().size()==1)
        {
          const exprt c=code_function_call.arguments()[0];
          std::string comment="condition `"+from_expr(ns, "", c)+"'";
          i_it->guard=not_exprt(c);
          i_it->type=ASSERT;
          i_it->code.clear();
          i_it->source_location.set_comment(comment);
          i_it->source_location.set_property_class("coverage");
        }
      }
      else if(i_it->is_assert())
        i_it->make_skip();
      break;
      
    case coverage_criteriont::LOCATION:
      if(i_it->is_assert())
        i_it->make_skip();

      {
        unsigned block_nr=basic_blocks[i_it];
        if(blocks_done.insert(block_nr).second)
        {
          std::string b=i2string(block_nr);
          std::string id=id2string(i_it->function)+"#"+b;
          source_locationt source_location=
            basic_blocks.source_location_map[block_nr];
          
          if(!source_location.get_file().empty() &&
             source_location.get_file()[0]!='<')
          {
            std::string comment="block "+b;
            goto_program.insert_before_swap(i_it);
            i_it->make_assertion(false_exprt());
            i_it->source_location=source_location;
            i_it->source_location.set_comment(comment);
            i_it->source_location.set_property_class("coverage");
            
            i_it++;
          }
        }
      }
      break;
    
    case coverage_criteriont::BRANCH:
      if(i_it->is_assert())
        i_it->make_skip();

      if(i_it==goto_program.instructions.begin())
      {
        // we want branch coverage to imply 'entry point of function'
        // coverage
        std::string comment=
          "function "+id2string(i_it->function)+" entry point";

        source_locationt source_location=i_it->source_location;

        goto_programt::targett t=goto_program.insert_before(i_it);
        t->make_assertion(false_exprt());
        t->source_location=source_location;
        t->source_location.set_comment(comment);
        t->source_location.set_property_class("coverage");
      }
    
      if(i_it->is_goto() && !i_it->guard.is_true())
      {
        std::string b=i2string(basic_blocks[i_it]);
        std::string true_comment=
          "function "+id2string(i_it->function)+" block "+b+" branch true";
        std::string false_comment=
          "function "+id2string(i_it->function)+" block "+b+" branch false";

        exprt guard=i_it->guard;
        source_locationt source_location=i_it->source_location;

        goto_program.insert_before_swap(i_it);
        i_it->make_assertion(not_exprt(guard));
        i_it->source_location=source_location;
        i_it->source_location.set_comment(true_comment);
        i_it->source_location.set_property_class("coverage");

        goto_program.insert_before_swap(i_it);
        i_it->make_assertion(guard);
        i_it->source_location=source_location;
        i_it->source_location.set_comment(false_comment);
        i_it->source_location.set_property_class("coverage");
        
        i_it++;
        i_it++;
      }
      break;
      
    case coverage_criteriont::CONDITION:
      if(i_it->is_assert())
        i_it->make_skip();

      // Conditions are all atomic predicates in the programs.
      {
        const std::set<exprt> conditions=collect_conditions(i_it);

        const source_locationt source_location=i_it->source_location;

        for(const auto & c : conditions)
        {
          const std::string c_string=from_expr(ns, "", c);
        
          const std::string comment_t="condition `"+c_string+"' true";
          goto_program.insert_before_swap(i_it);
          i_it->make_assertion(c);
          i_it->source_location=source_location;
          i_it->source_location.set_comment(comment_t);
          i_it->source_location.set_property_class("coverage");

          const std::string comment_f="condition `"+c_string+"' false";
          goto_program.insert_before_swap(i_it);
          i_it->make_assertion(not_exprt(c));
          i_it->source_location=source_location;
          i_it->source_location.set_comment(comment_f);
          i_it->source_location.set_property_class("coverage");
        }
        
        for(std::size_t i=0; i<conditions.size()*2; i++)
          i_it++;
      }
      break;
    
    case coverage_criteriont::DECISION:
      if(i_it->is_assert())
        i_it->make_skip();

      // Decisions are maximal Boolean combinations of conditions.
      {
        const std::set<exprt> decisions=collect_decisions(i_it);

        const source_locationt source_location=i_it->source_location;

        for(const auto & d : decisions)
        {
          const std::string d_string=from_expr(ns, "", d);
        
          const std::string comment_t="decision `"+d_string+"' true";
          goto_program.insert_before_swap(i_it);
          i_it->make_assertion(d);
          i_it->source_location=source_location;
          i_it->source_location.set_comment(comment_t);
          i_it->source_location.set_property_class("coverage");

          const std::string comment_f="decision `"+d_string+"' false";
          goto_program.insert_before_swap(i_it);
          i_it->make_assertion(not_exprt(d));
          i_it->source_location=source_location;
          i_it->source_location.set_comment(comment_f);
          i_it->source_location.set_property_class("coverage");
        }
        
        for(std::size_t i=0; i<decisions.size()*2; i++)
          i_it++;
      }
      break;
      
    case coverage_criteriont::AUTOSAC:
    case coverage_criteriont::MCDC:
      if(i_it->is_assert())
        i_it->make_skip();

      // 1. Each entry and exit point is invoked
      // 2. Each decision takes every possible outcome
      // 3. Each condition in a decision takes every possible outcome
      // 4. Each condition in a decision is shown to independently
      //    affect the outcome of the decision.
      {
        bool autosac_func_call=false;
        bool autosac_barrier=false;

        bool autosac=(criterion==coverage_criteriont::AUTOSAC);
        if(autosac && i_it->is_function_call())
          autosac_func_call=is_autosac_function(i_it->code);
        if(autosac_func_call)
          autosac_barrier=is_autosac_barrier(i_it->code);

        const std::set<exprt> conditions1=collect_conditions(i_it); //0
        const std::set<exprt> decisions1=collect_decisions(i_it);  //1

        std::vector<std::set<exprt> > cond_dec;
        std::vector<std::string > words;
        if(autosac_func_call and not autosac_barrier)
        {
          /** 
           * AUTOSAC assertions are inserted only when 
           * meeting a barrier.
           */
          std::string desc=autosac_description(i_it->code);
          if((not (strong_in_type or weakly_strong_in_type)) or (not is_autosac_in_type_function(i_it->code)))
          {
            autosac_vect.push_back(conditions1);
            autosac_vect.push_back(decisions1);
            autosac_words.push_back(autosac_description(i_it->code));
          }
          else if(weakly_strong_in_type)
          {
            autosac_in_types.push_back(*decisions1.begin());
            autosac_in_type_strs.push_back(desc);
          }
          else
          {
            autosac_in_types.push_back(*decisions1.begin());
            if(!(autosac_in_type_str==""))
              autosac_in_type_str+=";";
            autosac_in_type_str+=desc;
          }
        }
        else if(autosac_func_call and autosac_barrier)
        {

          if(strong_in_type)
          {
            autosac_vect.push_back({});
            autosac_vect.push_back({conjunction(autosac_in_types)});
            autosac_words.push_back(autosac_in_type_str);
            autosac_in_type_str="";
            autosac_in_types.clear();
          }
          else if(weakly_strong_in_type)
          {
        	if(autosac_in_types.size()==1)
        	{
        		autosac_vect.push_back({});
        		autosac_vect.push_back({autosac_in_types.front()});
                autosac_words.push_back(autosac_in_type_strs[0]);
        	}
        	else
        	{
              for(std::size_t ii=0; ii<autosac_in_types.size(); ii++)
                for(std::size_t jj=ii+1; jj<autosac_in_types.size(); jj++)
                {
                  std::vector<exprt> ve={autosac_in_types[ii], autosac_in_types[jj]};
                  autosac_vect.push_back({});
                  autosac_vect.push_back({conjunction(ve)});
                  autosac_words.push_back(autosac_in_type_strs[ii]+";"+autosac_in_type_strs[jj]);
                }
        	}
            autosac_in_types.clear();
            autosac_in_type_strs.clear();
          }

          cond_dec=autosac_vect;
          words=autosac_words;
          autosac_vect.clear();
          autosac_words.clear();
        }
        else // not AUTOSAC function
        {
          //cond_dec.push_back(conditions1);
          //cond_dec.push_back(decisions1);
          //words.push_back("");
          if(not (conditions1.size()==0 and decisions1.size()==0))
          {
            autosac_vect.push_back(conditions1);
            autosac_vect.push_back(decisions1);
            if(is_autosac_expr)
            {
              autosac_words.push_back(std::string("autosac exprssion function \'") + i_it->function.c_str() + std::string("\': "));
              cond_dec=autosac_vect;
              words=autosac_words;
              autosac_vect.clear();
              autosac_words.clear();
            }
            else
              autosac_words.push_back("");

          }
        }
        
        
        //if(not autosac_func_call)        
        for(int xx=0; xx < cond_dec.size(); xx+=2)
        {
          std::set<exprt> conditions, decisions;
          //if(xx+1<cond_dec.size()) 
          //{
            conditions=cond_dec.at(xx);
            decisions=cond_dec.at(xx+1);
          //}
          //else decisions=cond_dec.at(xx);
          
        
          std::set<exprt> both;
          std::set_union(conditions.begin(), conditions.end(),
                         decisions.begin(), decisions.end(),
                         inserter(both, both.end()));
          both.clear();
        
          const source_locationt source_location=i_it->source_location;
        
          //for(const auto & p : both)
          //{
          //  bool is_decision=decisions.find(p)!=decisions.end();
          //  bool is_condition=conditions.find(p)!=conditions.end();
          //  
          //  std::string description=words.at(xx/2)+": "+
          //    ((is_decision && is_condition)?"decision/condition":
          //    is_decision?"decision":"condition");
          //    
          //  std::string p_string=from_expr(ns, "", p);
          //
          //  std::string comment_t=description+" `"+p_string+"' true";
          //  goto_program.insert_before_swap(i_it);
          //  //i_it->make_assertion(p);
          //  //if(p.id()==ID_forall or p.id()==ID_exists) i_it->make_skip();
          //  //else 
          //  i_it->make_assertion(not_exprt(p));
          //  i_it->source_location=source_location;
          //  i_it->source_location.set_comment(comment_t);
          //  i_it->source_location.set_property_class("coverage");
        

          //  std::string comment_f=description+" `"+p_string+"' false";
          //  goto_program.insert_before_swap(i_it);
          //  //i_it->make_assertion(not_exprt(p));
          //  //if(p.id()==ID_forall or p.id()==ID_exists) i_it->make_skip();
          //  //else 
          //  i_it->make_assertion(p);
          //  i_it->source_location=source_location;
          //  i_it->source_location.set_comment(comment_f);
          //  i_it->source_location.set_property_class("coverage");
          //}
          
          std::set<exprt> controlling;
          for(auto &dec: decisions)
          {
            std::set<exprt> ctrl=collect_mcdc_controlling_nested({dec});
            // collect its controls if there exists
            std::set<exprt> ite_controlling;
            //collect_ite(dec, ite_controlling);
            collect_tenary(dec, ite_controlling);
            ctrl.insert(ite_controlling.begin(), ite_controlling.end());

            remove_repetition(ctrl);
            minimize_mcdc_controlling(ctrl, dec);
            controlling.insert(ctrl.begin(), ctrl.end());
          }
        
          if(autosac)
          {
            std::set<exprt> controlling2;
            for(auto &x: controlling)
            {
              std::set<exprt> res=autosac_expand(x);
              if(res.empty()) 
              {
               if(x.id()==ID_not) 
               {
                 auto tmp_res=autosac_atomic_negate(x.op0());
                 res.insert(tmp_res.begin(), tmp_res.end());
               }
               else res.insert(x);
              }
              for (auto &x: res)
              {
                auto res2=autosac_atomic_expand(x);
                
                res.insert(res2.begin(), res2.end());
              }
              controlling2.insert(res.begin(), res.end());
            }
            controlling=controlling2;
            remove_repetition(controlling);
          }
        
        
          for(const auto & p : controlling)
          {
            
            std::string p_string=from_expr(ns, "", p);
        
            std::string description=
              words.at(xx/2)+": `"+p_string+"'";
              
            goto_program.insert_before_swap(i_it);
            i_it->make_assertion(not_exprt(p));
            //i_it->make_assertion(p);
            i_it->source_location=source_location;
            i_it->source_location.set_comment(description);
            i_it->source_location.set_property_class("coverage");
          }
          
          for(std::size_t i=0; i<both.size()*2+controlling.size(); i++)
            i_it++;
        }
      }
      break;

    case coverage_criteriont::PATH:
      if(i_it->is_assert())
        i_it->make_skip();
      break;
    
    default:;
    }
  }

}

/*******************************************************************\

Function: instrument_cover_goals

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void instrument_cover_goals(
  const symbol_tablet &symbol_table,
  goto_functionst &goto_functions,
  coverage_criteriont criterion,
  const double toler,
  const bool autosac_strong_in_type,
  const bool autosac_weakly_strong_in_type)
{
  assert(!(autosac_strong_in_type and autosac_weakly_strong_in_type));
  bv_toler=toler;
  strong_in_type=autosac_strong_in_type;
  weakly_strong_in_type=autosac_weakly_strong_in_type;

  Forall_goto_functions(f_it, goto_functions)
  {
    if(f_it->first==ID__start ||
       f_it->first=="__CPROVER_initialize")
      continue;

    instrument_cover_goals(symbol_table, f_it->second.body, criterion);
  }
}








/*******************************************************************\

Function: Handler for ite controlling

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/
void collect_ite_rec(const exprt &e, 
                     const exprt &prior_constr,
                     std::set<exprt> &coll)
{

  std::vector<exprt> con_v;

  if(e.id()!=ID_if)
  {
    exprt e_bool(e);
    e_bool.type().id(ID_bool);
    con_v.push_back(prior_constr);
    con_v.push_back(e_bool);
    coll.insert(conjunction(con_v));
    return;
  }  
  
  // if A then B else C
  assert(e.operands().size()==3);
  const exprt &op0=e.op0();
  const exprt &op1=e.op1();
  const exprt &op2=e.op2();
  
  // 1) A && B
  con_v.clear();
  con_v.push_back(prior_constr);
  con_v.push_back(op0);
  collect_ite_rec(op1, conjunction(con_v), coll); 
  // 2) (not A) && C
  con_v.clear();
  con_v.push_back(prior_constr);
  con_v.push_back(not_exprt(op0));
  collect_ite_rec(op2, conjunction(con_v), coll); 
}

void collect_ite(const exprt &src, 
                 std::set<exprt> &coll)
{
  exprt prior;
  prior.make_true();
  if(src.id()==ID_if)
  {
    collect_ite_rec(src, prior, coll);
    return;
  }

  for(auto &op: src.operands())
  {
    if(op.id()==ID_if)
      collect_ite_rec(op, prior, coll);
    if(op.id()==ID_typecast)
      if(op.op0().id()==ID_if)
        collect_ite_rec(op.op0(), prior, coll);
  }

}


/*******************************************************************\

Function: Coverage criterion for tenary operator

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/
void collect_tenary_rec(
  const exprt &e,
  const std::set<exprt> &prior,
  std::set<exprt> &res)
{
  if(e.id() !=ID_if)
  {
    std::set<exprt> e_res=collect_mcdc_controlling_nested({e});
    if(e_res.empty())
    {
      exprt e_bool(e);
      e_bool.type().id(ID_bool);
      //e_res.insert(conjunction({e_bool}));
      e_res.insert(not_exprt(conjunction({e_bool})));
    }

    if(prior.empty())
      res=e_res;
    else
    {
      for(auto &x: prior)
        for(auto &y: e_res)
        res.insert(conjunction({x, y}));
    }

    return;
  }
  // ternary "A?B:C"
  assert(e.operands().size()==3);
  const exprt &A=e.op0();
  const exprt &B=e.op1();
  const exprt &C=e.op2();
  // apply MC/DC on A
  std::set<exprt> Ares=collect_mcdc_controlling_nested({A});
  if(Ares.empty())
  {
    Ares.insert(A);
    Ares.insert(not_exprt(A));
  }

  // Ares needs to be separated into Ares+ and Ares-
  std::set<exprt> Ares_true, Ares_false;
  std::set<exprt> conditions;
  for(auto &x: Ares)
  {
    std::set<exprt> nu_conds=collect_conditions(x);
    conditions.insert(nu_conds.begin(), nu_conds.end());
  }

  for(auto &x: Ares)
  {
    std::map<exprt, signed> atomic_exprs=
      values_of_atomic_exprs(x, conditions);
    if(eval_expr(atomic_exprs, x))
      Ares_true.insert(x);
    else
      Ares_false.insert(x);
  }

  std::set<exprt> nu_prior_true, nu_prior_false;
  // if 'prior' is empty
  if(prior.empty())
  {
    nu_prior_true=Ares_true;
    nu_prior_false=Ares_false;
  }
  else
  {
    for(auto &x: prior)
      for(auto &y: Ares_true)
        nu_prior_true.insert(conjunction({x,y}));

    for(auto &x: prior)
      for(auto &y: Ares_false)
        nu_prior_false.insert(conjunction({x,y}));
  }

  collect_tenary_rec(B, nu_prior_true, res);
  collect_tenary_rec(C, nu_prior_false, res);

}

void collect_tenary(const exprt &src,
                 std::set<exprt> &coll)
{
  std::set<exprt> prior;
  if(src.id()==ID_if)
  {
    collect_tenary_rec(src, prior, coll);
    return;
  }

  for(auto &op: src.operands())
  {
    if(op.id()==ID_if)
      collect_tenary_rec(op, prior, coll);
    if(op.id()==ID_typecast)
      if(op.op0().id()==ID_if)
        collect_tenary_rec(op.op0(), prior, coll);

  }

}













