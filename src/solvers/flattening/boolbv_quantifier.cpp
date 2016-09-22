/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <cassert>

#include <util/arith_tools.h>
#include <util/replace_expr.h>

#include "boolbv.h"
#include <util/arith_tools.h>

/*******************************************************************\
  The implementation in this file aims to convert a quantifier 
  expression into literals. Note that if a quantifier forall/exists
  is used at the scope of an assertion, it is handled in the
  procedure of goto-symex and is invisible here.
  At this stage, 

  1) the quantifier has been simplified in within the 
     goto-symex stage;

  2) we further assume that the quantifier is applied upon an array.

  E.g.,
     __CPROVER_assume(__CPROVER_forall{int i; i>=0&&i<=5 ==>
                      a[i]>0&&a[i]<=100});
     or
     __CPROVER_assume(__CPROVER_exists{int i; i>=0&&i<=5 ==>
                      a[i]>0&&a[i]<=100});
     or
     if(__CPROVER_exists{int i; i>=0&&i<=5 ==> a[i]>0&&a[i]<=100});
\*******************************************************************/

/*******************************************************************\

Function: get_index_expr

  Inputs:

 Outputs:

 Purpose: This method fetches the expr that represents the 
          array index.

\*******************************************************************/

exprt get_index_expr(const exprt &forall_rewritten)
{
  exprt res;
  res.make_false();
  for(auto &x : forall_rewritten.operands())
  {
    /**
     * Be careful that such a primitive way to fetch index expr
     * is sensitive to the form of the "simplified quantifier"
     **/
    if(x.id()==ID_ge)
      if(x.op0().id()==ID_symbol and x.op1().id()==ID_constant)
        return x.op0();
  }
  return res;
}

/*******************************************************************\

Function: get_min_index

  Inputs:

 Outputs:

 Purpose: To obtain the min index for the array element in the scope
          of the specified forall/exists operator.

\*******************************************************************/

exprt get_min_index(const exprt &index_expr, const exprt &forall_rewritten)
{
  exprt res;
  res.make_false();
  for(auto &x : forall_rewritten.operands())
  {
    if(x.id()!=ID_not) continue;
    exprt y=x.op0();
    if(y.id()!=ID_ge) continue;
    if(y.op0()==index_expr&&y.op1().id()==ID_constant)
    {
      res=y.op1();
      return res;
    }
  }
  return res;
}

/*******************************************************************\

Function: get_max_index

  Inputs:

 Outputs:

 Purpose: To obtain the max index for the array element in the scope
          of the specified forall/exists operator.

\*******************************************************************/

exprt get_max_index(const exprt &index_expr, const exprt &forall_rewritten)
{
  exprt res;
  res.make_false();
  for(auto &x : forall_rewritten.operands())
  {
    if(x.id()!=ID_ge) continue;
    if(x.op0()==index_expr&&x.op1().id()==ID_constant)
    {
      exprt over_expr=x.op1();
      mp_integer over_i;
      to_integer(over_expr, over_i);
      /**
       * Due to the ''do_simplify'' called along with goto-symex,
       * the ''over_i'' value we obtain here is not the exact
       * maximum index as specified in the original code.
       **/
      over_i-=1;
      res=from_integer(over_i, x.op1().type());
      return res;
    }
  }
  return res;
}

/*******************************************************************\

Function: get_quantifier_body

  Inputs:

 Outputs:

 Purpose: To obtain the body constraint specified within
          of the given forall/exists operator.

\*******************************************************************/

exprt get_quantifier_body(const exprt &re_quantifier)
{
  exprt res;
  res.make_false();
  for(auto &x : re_quantifier.operands())
  {
    if(x.id()==ID_ge)
    {
      if(x.op0().id()==ID_symbol and x.op1().id()==ID_constant)
        continue;
    }
    if(x.id()==ID_not)
    {
      exprt y=x.op0();
      if(y.id()!=ID_ge) continue;
      if(x.op0().id()==ID_symbol and x.op1().id()==ID_constant)
        continue;
    }
    return x;    
  }
  return res;
}

/*******************************************************************\

Function: instantiate(index_expr, index_inst, expr)

  Inputs:

 Outputs:

 Purpose: Replace all "index_expr"s inside "expr" with "index_inst"

\*******************************************************************/
void instantiate(const exprt &index_expr,
                 const mp_integer &index_inst,
                 exprt &expr)
{
  for(auto &x: expr.operands())
  {
    if(x==index_expr)
    {
      x=from_integer(index_inst, x.type()); 
      continue;
    }

    if(x.operands().size()>1)
    {
      instantiate(index_expr, index_inst, x);
      continue;
    }
    if(x.operands().size()==0) continue;
    if(x.id()==ID_not) 
    {
      instantiate(index_expr, index_inst, x.op0());
      continue;
    }
    if(!(x.op0()==index_expr)) continue;
    exprt nu=from_integer(index_inst, x.op0().type()); 
    x.operands().clear(); 
    x.operands().push_back(nu);
  }
}

/*******************************************************************\

Function: boolbvt::convert_quantifier

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

literalt boolbvt::convert_quantifier(const exprt &expr)
{
  assert(expr.operands().size()==2);
  assert(expr.op0().id()==ID_symbol);
  /**
   * We need to rewrite the forall/exists quantifier into
   * an OR expr with 3 operands: min/max indices of array elements
   * accessed and the constraint specified on array elements.
   **/
  exprt re(expr);
  exprt tmp(re.op1());
  re.swap(tmp);
  exprt index_expr=get_index_expr(re);
  exprt min_i=get_min_index(index_expr, re);
  exprt max_i=get_max_index(index_expr, re);
  exprt body_expr=get_quantifier_body(re);
  if(index_expr.is_false() ||
     min_i.is_false() ||
     max_i.is_false() ||
     body_expr.is_false())
    return SUB::convert_rest(expr);
  
  quantifier_list.push_back(quantifiert());

  quantifier_list.back().index_expr=index_expr;
  to_integer(min_i, quantifier_list.back().lb);
  to_integer(max_i, quantifier_list.back().ub);
  quantifier_list.back().id=expr.id();

  literalt l=prop.new_variable();
  quantifier_list.back().l=l;

  // instantiate index-relevant variables in the body if there exists 
  auto it=quantifier_list.end();
  it--;
  if(it->id==ID_exists)
  {
    it->expr=body_expr; 
    return l;
  }
  std::vector<exprt> expr_insts;
  for(mp_integer i=it->lb; i<=it->ub; ++i)
  {
    exprt constraint_expr=body_expr;
    instantiate(it->index_expr, i, constraint_expr);
    expr_insts.push_back(constraint_expr);
  }
  if(it->id==ID_forall)
  {
    it->expr=conjunction(expr_insts);
  }

  return l;
}


/*******************************************************************\

Function: boolbvt::post_process_quantifiers

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void boolbvt::post_process_quantifiers()
{
  std::set<exprt> instances;
  
  if(quantifier_list.empty()) return;

  for(auto it=quantifier_list.begin();
      it!=quantifier_list.end();  
      ++it)
  {
    bvt inst_bv;
    for(mp_integer i=it->lb; i<=it->ub; ++i)
    {
      exprt constraint_expr=it->expr;
      instantiate(it->index_expr, i, constraint_expr);
      inst_bv.push_back(convert_bool(constraint_expr));
    }
    if(it->id==ID_forall)
    {
      prop.set_equal(prop.land(inst_bv), it->l);
    }
    if(it->id==ID_exists)
    {
      prop.set_equal(prop.lor(inst_bv), it->l);
    }
  }

}
