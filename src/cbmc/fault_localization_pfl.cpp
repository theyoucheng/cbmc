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
  if(phi==const_literal(true)) return false;
  if(empty(X) && empty(Y)) return false;
  bvt assumptions;
  // 1) violation of the "property"
  assumptions.push_back(!phi);
  assumptions.push_back(bmc.equation.current_activation_literal());
  //for(auto &t: passing_traces)
  //{
  //  literalt l=trace_literal_and(t);
  //  if(l==const_literal(true)) return false;
  //  assumptions.push_back(!l);
  //}
  //for(auto &t: failing_traces)
  //{
  //  literalt l=trace_literal_and(t);
  //  if(l==const_literal(true)) return false;
  //  assumptions.push_back(!l);
  //}
  // 2) excluding blocks
  auto it=P.begin();
  if(!empty(X))
  {
    literalt l=trace_positive_literal_and(X);
    if(l==const_literal(true)) return false;
    assumptions.push_back(!l);
  }
  // 3) including blocks
  it=P.begin();
  if(!empty(Y))
  {
    // Existing traces shall be excluded.
    // There is optimization, given that, according to Algorithm 4
    // in David Landsberg's PhD thesis, only "passing traces" need
    // to be considered
#if 0
    for(auto &t: passing_traces)
    {
      literalt l=trace_literal_and(t);
      if(l==const_literal(true)) return false;
      assumptions.push_back(!l);
    }
#endif
#if 0
    for(auto &t: traces)
    {
      literalt l=trace_literal_and(t);
      if(l==const_literal(true)) return false;
      assumptions.push_back(!l);
    }
#endif
    for(auto &y : Y)
    {
      if(y.is_true()) assumptions.push_back(it->first);
      ++it;
    }
  }
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
    return true;
  }
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
  return v;
}

/*******************************************************************\

Function: fault_localizationt::pflt:operator

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::operator()()
{

  pre_merge();
  if(single_trace)
  {
    lpoints_valuet f_trace;
    if(get_a_trace(!property, f_trace))
      failing_traces.push_back(f_trace);
    // simplify_traces();
    compute_spectra();
    measure_sb();
    if(type=="sbo") return;
    compute_ppv();
    compute_probability();
    return;
  }

  // 1) to build the "set" of failing traces
  lpoints_valuet f_trace;
  if(get_a_trace(!property, f_trace))
  {
    if(failing_traces.size()<max_failing_traces)
      failing_traces.push_back(f_trace);
    do
    {
      if(failing_traces.size()>=max_failing_traces)
        break;
      bmc.prop_conv.set_assumptions({});
    } while(mc(property, common(failing_traces), {}, failing_traces));

  }
  // 2) to build the "set" of passing traces
  lpoints_valuet p_trace;
  if(get_a_trace(property, p_trace))
  {
    if(passing_traces.size()<max_passing_traces)
      passing_traces.push_back(p_trace);
    do
    {
      if(passing_traces.size()>=max_passing_traces)
        break;
      bmc.prop_conv.set_assumptions({});
    }
    while(mc(!property, common(passing_traces), {}, passing_traces));
  }

  // 3) to build the "set" of "s" traces
  std::vector<lpoints_valuet> s_traces;
  lpoints_valuet v=complement(common(failing_traces), common(passing_traces));
  for(std::size_t i=0; i<P.size(); ++i)
  {
    if(passing_traces.size()+s_traces.size()>=max_passing_traces)
      break;
    if(!v[i].is_true()) continue;
    lpoints_valuet v_single_element;
    for(std::size_t j=0; j<v.size(); ++j)
    {
      if(i!=j) v_single_element.push_back(tvt(tvt::tv_enumt::TV_UNKNOWN));
      else v_single_element.push_back(tvt(tvt::tv_enumt::TV_TRUE));
    }
    mc(!property, {}, v_single_element, s_traces);
  }

#if 0
  std::cout << "failing traces:\n";
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
#endif
  // merge s_traces into passing_traces
  for(auto &s: s_traces)
  {
    if(!in_traces(s, passing_traces))
      passing_traces.push_back(s);
  }

  // to obtain some extra traces
  // while(failing_traces.size()<max_traces)
  // {
  //   if(!get_a_trace(!property, failing_traces))
  //     break;
  // }

  while(passing_traces.size()<max_traces)
  {
    if(passing_traces.size()>=max_passing_traces)
      break;
    if(!get_a_trace(property, passing_traces))
      break;
  }
#if 0
  std::cout << "\nAfter adding some extra traces\n";
  std::cout << "The set of blocks: \n";
  for(auto &p: P)
  {
	std::cout << "[" << p.first << "] ";
    for(auto &l: p.second.lines)
      std::cout << "##" << l << " ";
    std::cout << "\n";
  }

  std::cout << "\nfailing traces: \n";
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
#endif
  // to compute the probability
  simplify_traces();
  compute_spectra();
  measure_sb();
  if(type=="sbo") return;
  compute_ppv();
  compute_probability();
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
  if(assumption_==const_literal(false))
    return false;
  bvt assumptions;
  assumptions.push_back(assumption_);
  assumptions.push_back(bmc.equation.current_activation_literal());
  //for(auto &t: passing_traces)
  //{
  //  literalt l=trace_literal_and(t);
  //  if(l==const_literal(true)) return false;
  //  assumptions.push_back(!l);
  //}
  //for(auto &t: failing_traces)
  //{
  //  literalt l=trace_literal_and(t);
  //  if(l==const_literal(true)) return false;
  //  assumptions.push_back(!l);
  //}
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

Function: fault_localizationt::pflt:get_a_trace

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::pflt::get_a_trace(
  const literalt &assumption_,
  std::vector<lpoints_valuet> &traces)
{
  if(assumption_==const_literal(false))
    return false;
  bvt assumptions;
  assumptions.push_back(assumption_);
  // existing traces are excluded
  for(auto &t: traces)
  {
    literalt l=trace_literal_and(t);
    if(l==const_literal(true)) return false;
    assumptions.push_back(!l);
  }
  //for(auto &t: passing_traces)
  //{
  //  literalt l=trace_literal_and(t);
  //  if(l==const_literal(true)) return false;
  //  assumptions.push_back(!l);
  //}
  //for(auto &t: failing_traces)
  //{
  //  literalt l=trace_literal_and(t);
  //  if(l==const_literal(true)) return false;
  //  assumptions.push_back(!l);
  //}
  assumptions.push_back(bmc.equation.current_activation_literal());
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
  lpoints_valuet v;
  assert(v1.size()==P.size());
  assert(v2.size()==P.size());
  for(std::size_t i=0; i<P.size() ;++i)
    if(v1[i].is_true() && !v2[i].is_true())
      v.push_back(tvt(tvt::tv_enumt::TV_TRUE));
    else v.push_back(tvt(tvt::tv_enumt::TV_UNKNOWN));
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

Function: fault_localizationt::pflt:lpoint_equal

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::pflt::lpoint_equal(
  const lpointt &lp1,
  const lpointt &lp2)
{
  if(lp1.lines.size()!=lp2.lines.size()) return false;

  auto it1=lp1.lines.begin();
  auto it2=lp1.lines.begin();
  for(std::size_t i=0; i<lp1.lines.size(); ++i)
  {
    if(!(*it1==*it2))
      return false;
    ++it1;
    ++it2;
  }

  return true;
}

/*******************************************************************\

Function: fault_localizationt::pflt:pre_merge

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::pre_merge()
{
  // Because of loop unwinding, there exist multiple copies of
  // the same LOC, and they have to be merged before applying
  // fault localization
  std::vector<std::vector<literalt> > merged_literals;
  merged_literals.resize(P.size());

  std::size_t i=0;
  auto it=P.begin();
  while(it!=P.end())
  {
	merged_literals[i].push_back(it->first);
    auto &lines=it->second.lines;
	auto lt=lines.begin();
	while(lt!=lines.end())
    {
      std::size_t j=0;
      for(auto jt=P.begin(); jt!=it; ++jt)
      {
        if(jt->second.lines.find(*lt)!=jt->second.lines.end())
          break;
        ++j;
      }
      if(j==i)
      {
        lt++;
        continue;
      }
      lt=lines.erase(lt);
      assert(j<i);
      merged_literals[j].push_back(it->first);
    }
	++it;
	++i;
  }
  lpointst P_new;
  i=0;
  for(auto &p: P)
  {
    if(p.second.lines.empty())
    {
      ++i;
      continue;
    }
    std::vector<exprt> bv;
    for(auto &l: merged_literals[i])
      bv.push_back(literal_exprt(l));
    assert(!bv.empty());
    literalt m=bmc.prop_conv.convert(disjunction(bv));
    if(P_new.find(m)!=P_new.end())
      P_new[m].lines.insert(
		p.second.lines.begin(),
		p.second.lines.end());
    else P_new[m]=p.second;
    ++i;
  }
  P.swap(P_new);
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
  std::size_t i=0;
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
    if(all_false)
    {
      for(auto &t: failing_traces)
        t[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
      for(auto &t: passing_traces)
        t[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
    }
    else
      new_P.insert(p);

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
  // remove redundancy
  std::vector<lpoints_valuet> new_failing_traces, new_passing_traces;
  for(auto &t: failing_traces)
    if(!in_traces(t, new_failing_traces))
      new_failing_traces.push_back(t);
  for(auto &t: passing_traces)
    if(!in_traces(t, new_passing_traces))
      new_passing_traces.push_back(t);
  failing_traces=new_failing_traces;
  passing_traces=new_passing_traces;

  std::cout << "\n*** after simplification (blocks) *** \n";
  for(auto &p: P)
  {
    std::cout << "[" << p.first << "] ";
    for(auto &x:p.second.lines)
      std::cout << "##" << x << " ";
    std::cout << "\n";
  }
  std::cout << "\nfailing traces: \n";
  for(auto &x: failing_traces)
    {
      for(auto &y: x)
        std::cout << y.is_true() << " ";
      std::cout << "\n";
    }
    std::cout << "\npassing traces: \n";
    for(auto &x: passing_traces)
    {
      for(auto &y: x)
        std::cout << y.is_true() << " ";
      std::cout << "\n";
    }
}

/*******************************************************************\

Function: fault_localizationt::pflt:compute_spectra

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::compute_spectra()
{
  for(std::size_t i=0; i<P.size(); ++i)
  {
    ef.push_back(0);
    ep.push_back(0);
    nf.push_back(0);
    np.push_back(0);
    for(auto &t: failing_traces)
      if(t[i].is_true())  ef[i]++;
      else nf[i]++;
    for(auto &t: passing_traces)
      if(t[i].is_true())  ep[i]++;
      else np[i]++;
  }
  std::cout << "\n***after compute spectra***\n";
  std::cout << "**ef:\n";
  for(auto &p: ef)
    std::cout << p << " ";
  std::cout << "\n";
  std::cout << "**ep:\n";
  for(auto &p: ep)
    std::cout << p << " ";
  std::cout << "\n";
  std::cout << "**nf:\n";
  for(auto &p: nf)
    std::cout << p << " ";
  std::cout << "\n";
  std::cout << "**np:\n";
  for(auto &p: np)
    std::cout << p << " ";
  std::cout << "\n";
}

/*******************************************************************\

Function: fault_localizationt::pflt:measure_sb

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::measure_sb()
{
  std::size_t i=0;
  for(auto &p: P)
  {
    p.second.score=ef[i]-(ep[i]/(ep[i]+np[i]+1.0));
    ++i;
  }
  std::cout << "\n***after measure_sb***\n";
    for(auto &p: P)
      std::cout << p.second.score << " ";
  std::cout << "\n\n";
}

/*******************************************************************\

Function: fault_localizationt::pflt:compute_ppv

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::compute_ppv()
{
  for(std::size_t i=0; i<ef.size(); ++i)
  {
    double score=0;
    if(ef[i]>0) score=ef[i]/(ef[i]+ep[i]+0.0);
    ppv.push_back(score);
  }
  std::cout << "\n***after compute ppv***\n";
  for(auto &p: ppv)
    std::cout << p << " ";
  std::cout << "\n\n";
}

/*******************************************************************\

Function: fault_localizationt::pflt:compute_probability

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::pflt::compute_probability()
{
  std::vector<double> Prob(P.size(), 0);

  for(auto &t: failing_traces)
  {
    double den=0;
    for(std::size_t i=0; i<t.size(); ++i)
      if(t[i].is_true()) den += ppv[i];

    for(std::size_t i=0; i<t.size(); ++i)
      if(t[i].is_true())
        Prob[i]=(Prob[i]+ppv[i]/den)-Prob[i]*(ppv[i]/den);
  }

  std::size_t i=0;
  for(auto &x: P)
  {
    x.second.score=Prob[i];
    ++i;
  }
  std::cout << "\n***after compute probability***\n";
  for(auto &p: Prob)
    std::cout << p << " ";
  std::cout << "\n\n";
}
