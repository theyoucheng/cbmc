/*******************************************************************\

Module: Probabilistic Fault Localization

Author: Youcheng Sun

\*******************************************************************/

#include "fault_localization.h"
#include <iostream>

/*******************************************************************\

Function: fault_localizationt::pflt::mc

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::pflt::mc(
  const literalt &phi,
  const lpoints_valuet &X,
  const lpoints_valuet &Y,
  std::vector<lpoints_valuet> &traces)
{
  if(empty(X) && empty(Y)) return false;
  std::cout << "***here we are inside mc...\n";
  std::cout << "**X: \n";
  for(auto &x: X)
	  std::cout << x.is_true() << " ";
  std::cout << "\n";
  std::cout << "**Y: \n";
  for(auto &x: Y)
	  std::cout << x.is_true() << " ";
  std::cout << "\n";
  bvt assumptions;
  // 1) violation of the "property"
  assumptions.push_back(!phi);
  assumptions.push_back(bmc.equation.current_activation_literal());
  std::cout << "**cond: " << !phi << "\n";
  // 2) excluding blocks
  auto it=P.begin();
  if(!empty(X))
  {
	assumptions.push_back(!trace_positive_literal_and(X));
  }
  // 3) including blocks
  it=P.begin();
  if(!empty(Y))
  {
    // Existing traces shall be excluded.
	// There is optimization, given that, according to Algorithm 4
	// in David Landsberg's PhD thesis, only "passing traces" need
	// to be considered
	// for(auto &t: passing_traces)
	// {
    //  literalt l=trace_literal_and(t);
    //  std::cout << "***trave literal and: " << l << std::endl;
    //  assumptions.push_back(!l); break;
	// }
    for(auto &y : Y)
    {
      if(y.is_true()) assumptions.push_back(it->first);
      ++it;
    }
  }
  std::cout << "***assumptions: \n";
  for(auto &a: assumptions)
    std::cout << a << " \n";
  std::cout << "\n";
  bmc.prop_conv.set_assumptions(assumptions);
  if(bmc.prop_conv()==decision_proceduret::D_SATISFIABLE)
  {
    lpoints_valuet trace;
    for(auto &p: P)
    {
      if(bmc.prop_conv.l_get(p.first).is_true())
        trace.push_back(tvt(tvt::tv_enumt::TV_TRUE));
      else trace.push_back(tvt(tvt::tv_enumt::TV_FALSE));
    }
    traces.push_back(trace);
    std::cout << "the mc trace returned\n";
    for(auto &t: trace)
    	std::cout << t.is_true() << " ";
    std::cout << "\n";
    return true;
  }
  std::cout << "-----> mc not satisfied\n";
  return false;
}

/*******************************************************************\

Function: fault_localizationt::pflt:empty

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::pflt::empty(const lpoints_valuet &v)
{
  if(v.empty()) return true;
  assert(v.size()==P.size());
  for(auto v_it=v.begin(); v_it!=v.end(); ++v_it)
  {
    if(v_it->is_true()) return false;
  }
  return true;
}

/*******************************************************************\

Function: fault_localizationt::pflt:common

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

fault_localizationt::lpoints_valuet fault_localizationt::pflt::common(
  const std::vector<lpoints_valuet> &lvs)
{
  std::cout << "we are in common\n";
  for(auto &v: lvs)
  {
    for(auto &x: v)
      std::cout << x.is_true() << " ";
    std::cout << "\n";
  }
  lpoints_valuet v;
  for(auto &lv: lvs)
    assert(lv.size()==P.size());
  if(lvs.empty())
  {
    for(std::size_t i=0; i<P.size(); ++i)
      v.push_back(tvt(tvt::tv_enumt::TV_UNKNOWN));
    return v;
  }

  for(std::size_t i=0; i<P.size(); ++i)
  {
	bool co=true;
    for(auto &lv : lvs)
    {
      if(!lv[i].is_true())
      {
        co=false;
        break;
      }
    }
    if(co) v.push_back(tvt(tvt::tv_enumt::TV_TRUE));
    else v.push_back(tvt(tvt::tv_enumt::TV_UNKNOWN));
  }
  std::cout << "**result\n";
  for(auto &x: v)
    std::cout << x.is_true() << " ";
  std::cout << "\n";
  return v;
}

/*******************************************************************\

Function: fault_localizationt::pflt:operator

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::operator()()
{std::cout << "probabilistic fault localization\n";
  // 1) to build the "set" of failing traces
  lpoints_valuet f_trace;
  if(get_a_trace(!property, f_trace))
  {
    failing_traces.push_back(f_trace);
    std::cout << "the initial failing trace: \n";
    for(auto &f: f_trace)
    	std::cout << f.is_true() << " ";
    //std::cout << trace_literal(f_trace) << " ";
    std::cout << "\n";
    do
    {
      bmc.prop_conv.set_assumptions({});
    } while(mc(property, common(failing_traces), {}, failing_traces));

  }std::cout << "***size of failing traces: " << failing_traces.size() << "\n";
  // 2) to build the "set" of passing traces
  lpoints_valuet p_trace;
  if(get_a_trace(property, p_trace))
  {
    passing_traces.push_back(p_trace);
    std::cout << "**the initial passing trace: \n";
    for(auto &f: p_trace)
    	std::cout << f.is_true() << " ";
    std::cout << "\n";
    do
    {
      bmc.prop_conv.set_assumptions({});
    }
    while(mc(!property, common(passing_traces), {}, passing_traces));
  }std::cout << "***size of passing traces: " << passing_traces.size() << "\n";
  // 3) to build the "set" of "s" traces
  std::vector<lpoints_valuet> s_traces;
  lpoints_valuet v=complement(common(failing_traces), common(passing_traces));
  for(std::size_t i=0; i<P.size(); ++i)
  {
    if(!v[i].is_true()) continue;
    lpoints_valuet v_single_element;
    for(std::size_t j=0; j<v.size(); ++j)
      if(i!=j) v_single_element.push_back(tvt(tvt::tv_enumt::TV_UNKNOWN));
      else v_single_element.push_back(tvt(tvt::tv_enumt::TV_TRUE));

    mc(!property, {}, v_single_element, s_traces);
  }

  std::cout << "failing traces: \n";
  for(auto &x: failing_traces)
  {
    for(auto &y: x)
      std::cout << y.is_true() << " ";
    std::cout << "\n";
  }
  std::cout << "passing traces: \n";
  for(auto &x: passing_traces)
  {
    for(auto &y: x)
      std::cout << y.is_true() << " ";
    std::cout << "\n";
  }
  std::cout << "s traces: \n";
  for(auto &x: s_traces)
  {
    for(auto &y: x)
      std::cout << y.is_true() << " ";
    std::cout << "\n";
  }
  // merge s_traces into passing_traces
  for(auto &s: s_traces)
  {
    if(!in_traces(s, passing_traces))
      passing_traces.push_back(s);
  }

  simplify_traces();
}

/*******************************************************************\

Function: fault_localizationt::pflt:get_a_trace

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::pflt::get_a_trace(
  const literalt &assumption_,
  lpoints_valuet &trace)
{
  bvt assumptions;
  assumptions.push_back(assumption_);
  //bmc.equation.new_activation_literal(bmc.prop_conv);
  assumptions.push_back(bmc.equation.current_activation_literal());
  bmc.prop_conv.set_assumptions(assumptions);
  if(bmc.prop_conv()==decision_proceduret::D_SATISFIABLE)
  {
    trace.clear();
    for(auto &p: P)
    {
      if(bmc.prop_conv.l_get(p.first).is_true())
        trace.push_back(tvt(tvt::tv_enumt::TV_TRUE));
      else trace.push_back(tvt(tvt::tv_enumt::TV_FALSE));
    }
    return true;
  }
  return false;
}

/*******************************************************************\

Function: fault_localizationt::pflt:complement

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

fault_localizationt::lpoints_valuet fault_localizationt::pflt::complement(
  const lpoints_valuet &v1,
  const lpoints_valuet &v2)
{
  std::cout << "we are in complement\n";
  for(auto &x: v1)
	  std::cout << x.is_true() << " ";
  std::cout << "\n";
  for(auto &x: v2)
	  std::cout << x.is_true() << " ";
  std::cout << "\n";
  lpoints_valuet v;
  assert(v1.size()==P.size());
  assert(v2.size()==P.size());
  for(std::size_t i=0; i<P.size() ;++i)
    if(v1[i].is_true() && !v2[i].is_true())
      v.push_back(tvt(tvt::tv_enumt::TV_TRUE));
    else v.push_back(tvt(tvt::tv_enumt::TV_UNKNOWN));
  std::cout << "**result: \n";
  for(auto &x: v)
	  std::cout << x.is_true() << " ";
  std::cout << "\n";
  return v;
}

/*******************************************************************\

Function: fault_localizationt::pflt:trace_literal_and

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

literalt fault_localizationt::pflt::trace_literal_and(
  const lpoints_valuet &trace)
{
  assert(trace.size()==P.size());
  if(empty(trace))
  {
    literalt l;
    l.make_true();
    return l;
  }
  std::vector<exprt> bv;
  auto it=trace.begin();
  for(auto &x: P)
  {
	literal_exprt expr(x.first);
	if(it->is_false())
	  expr.make_not();
	++it;
    bv.push_back(expr);
  }

  std::cout << "trace_literal_and expr: " << from_expr(conjunction(bv)) << "\n";
  return bmc.prop_conv.convert(conjunction(bv));
}

/*******************************************************************\

Function: fault_localizationt::pflt:trace_positive_literal_and

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/
literalt fault_localizationt::pflt::trace_positive_literal_and(
  const lpoints_valuet &trace)
{
  assert(trace.size()==P.size());
  if(empty(trace))
  {
    literalt l;
    l.make_true();
    return l;
  }
  std::vector<exprt> bv;
  auto it=trace.begin();
  for(auto &x: P)
  {
	if(it->is_true())
      bv.push_back(literal_exprt(x.first));
	++it;
  }

  return bmc.prop_conv.convert(conjunction(bv));
}

/*******************************************************************\

Function: fault_localizationt::pflt:trace_equal

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::pflt::trace_equal(
  const lpoints_valuet &v1,
  const lpoints_valuet &v2)
{
  assert(v1.size()==P.size() && v2.size()==P.size());
  for(std::size_t i=0; i<P.size(); ++i)
    if(v1[i]!=v2[i]) return false;
  return true;
}

/*******************************************************************\

Function: fault_localizationt::pflt:in_traces

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::pflt::in_traces(
  const lpoints_valuet &v,
  const std::vector<lpoints_valuet> &traces)
{
  for(auto &t: traces)
    if(trace_equal(t, v)) return true;
  return false;
}

/*******************************************************************\

Function: fault_localizationt::pflt:simplify_traces

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::simplify_traces()
{
  lpointst new_P;
  int i=0;
  for(auto &p: P)
  {
    bool all_false=true;
    for(auto &t: failing_traces)
    {
      if(t[i].is_true())
      {
        all_false=false;
        break;
      }
    }
    bool all_true=true;
    for(auto &t: passing_traces)
    {
      if(t[i].is_false())
      {
        all_true=false;
        break;
      }
    }
    std::cout << "**i=" << i << ", " << all_false << ", " << all_true << "\n";
    if(all_false || all_true)
    {
      for(auto &t: failing_traces)
        t[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
      for(auto &t: passing_traces)
        t[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
    }
    else new_P.insert(p);

    ++i;
  }
  // to update "traces" and "lpoints"
  for(auto &t: failing_traces)
  {
    auto it=t.begin();
    while(it!=t.end())
    {
      if(it->is_unknown())
    	  it=t.erase(it);
      else it++;
    }
  }
  for(auto &t: passing_traces)
  {
    auto it=t.begin();
    while(it!=t.end())
    {
      if(it->is_unknown())
    	  it=t.erase(it);
      else it++;
    }
  }
  P=new_P;
  std::cout << "**after simplification\n";
  for(auto &p: P)
  {
    std::cout << p.first << ", " << p.second.target->source_location << "\n";
  }
  std::cout << "passing traces: \n";
  for(auto &x: failing_traces)
    {
      for(auto &y: x)
        std::cout << y.is_true() << " ";
      std::cout << "\n";
    }
    std::cout << "passing traces: \n";
    for(auto &x: passing_traces)
    {
      for(auto &y: x)
        std::cout << y.is_true() << " ";
      std::cout << "\n";
    }
}
