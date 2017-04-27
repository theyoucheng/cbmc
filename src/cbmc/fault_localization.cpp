/*******************************************************************\

Module: Fault Localization

Author: Peter Schrammel

\*******************************************************************/

#include <util/threeval.h>
#include <util/expr_util.h>
#include <util/arith_tools.h>
#include <util/symbol.h>
#include <util/std_expr.h>
#include <util/message.h>
#include <util/time_stopping.h>

#include <solvers/prop/minimize.h>
#include <solvers/prop/literal_expr.h>
#include <solvers/prop/prop.h>

#include <goto-symex/build_goto_trace.h>

#include "fault_localization.h"
#include "counterexample_beautification.h"
#include <util/prefix.h>
#include <string>
#include <algorithm>
#include <iostream>




/*******************************************************************\

Function: fault_localizationt::freeze_guards

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::freeze_guards()
{
  for(const auto &step : bmc.equation.SSA_steps)
  {
    if(!step.guard_literal.is_constant())
      bmc.prop_conv.set_frozen(step.guard_literal);
    if(step.is_assert() &&
       !step.cond_literal.is_constant())
      bmc.prop_conv.set_frozen(step.cond_literal);
  }
}

/*******************************************************************\

Function: fault_localizationt::collect_guards

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::collect_guards(lpointst &lpoints)
{
  //int curr_block_start=-1;
  std::vector<int> block_starts;
  std::vector<int> opens;
  for(symex_target_equationt::SSA_stepst::const_iterator
      it=bmc.equation.SSA_steps.begin();
      it!=bmc.equation.SSA_steps.end(); it++)
  {
	bool dummy_block=false;
    if(it->source.pc->source_location.get_comment()=="__CPROVER_dummy")
    	dummy_block=true;
 /*   {
      std::cout << "\n";
      std::cout << "++++---> line " << it->source.pc->source_location.get_line().c_str() << "\n";
      std::cout << "++++guard literal: " << it->guard_literal << ", cond literal: " << it->cond_literal <<"\n";
      std::cout << "++++guard expr: " << from_expr(it->guard) << ", cond expr: " << from_expr(it->cond_expr) <<"\n";
      std::cout << "\n";
    }
*/
    if(dummy_block or ((it->is_assignment() &&
       it->assignment_type==symex_targett::STATE &&
       !it->ignore)))
    {

      if(dummy_block or !it->guard_literal.is_constant())
      {
        lpoints[it->guard_literal].target=it->source.pc;
        int lnum=atoi(it->source.pc->source_location.get_line().c_str());

        //lnum--;
        if(it->guard_literal.is_constant())
        {
           lpoints[it->guard_literal].info="[" + std::to_string(lnum) + "-" + std::to_string(lnum) + "]";
      	   std::cout << it->guard_literal << ", " << it->identifier  << ", lnum: " << lnum << std::endl;

        }
        else
        {
          // to get the function in which 'it' is
          std::string it_func=it->source.pc->source_location.as_string();
          it_func=it_func.substr(it_func.find("function")+1);
          it_func=it_func.substr(0, it_func.find(","));

          int block_start=lnum, block_end=lnum;
          for(symex_target_equationt::SSA_stepst::const_iterator
        	      jt=bmc.equation.SSA_steps.begin();
        	      jt!=bmc.equation.SSA_steps.end(); jt++)
          {
        	if(jt==it) break;
        	if(jt->guard_literal==it->guard_literal)
        	{
        	  // to get the function in which 'jt' is
        	  std::string jt_func=jt->source.pc->source_location.as_string();
              jt_func=jt_func.substr(jt_func.find("function")+1);
        	  jt_func=jt_func.substr(0, jt_func.find(","));
        	  if(!(jt_func==it_func)) continue;
        	  std::string tmp_c=jt->source.pc->source_location.get_line().c_str();
        	  if(! (tmp_c==""))
        	  {
        	    int tmp=atoi(tmp_c.c_str());
        	    if(tmp<block_start) block_start=tmp;
        	  }
        	}
        	//else block_start=block_end;
          }

          lpoints[it->guard_literal].info="[" + std::to_string(block_start)+"-"+std::to_string(block_end) + "]";
        }

        lpoints[it->guard_literal].score=0;
      }
    }

    if(it==failed)
      break;
  }
  std::cout << "set of lpoints: " << lpoints.size() << "\n";
  for(auto &x: lpoints)
	  std::cout << x.first << ", " << x.second.info << "\n";
}

/*******************************************************************\

Function: fault_localizationt::get_failed_property

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

symex_target_equationt::SSA_stepst::const_iterator
fault_localizationt::get_failed_property()
{
  for(symex_target_equationt::SSA_stepst::const_iterator
      it=bmc.equation.SSA_steps.begin();
      it!=bmc.equation.SSA_steps.end(); it++)
    if(it->is_assert() &&
       bmc.prop_conv.l_get(it->guard_literal).is_true() &&
       bmc.prop_conv.l_get(it->cond_literal).is_false())
      return it;

  assert(false);
  return bmc.equation.SSA_steps.end();
}

/*******************************************************************\

Function: fault_localizationt::check

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool fault_localizationt::check(const lpointst &lpoints,
                                const lpoints_valuet& value)
{
  assert(value.size()==lpoints.size());
  bvt assumptions;
  lpoints_valuet::const_iterator v_it=value.begin();
  for(const auto &l : lpoints)
  {
    if(v_it->is_true())
      assumptions.push_back(l.first);
    else if(v_it->is_false())
      assumptions.push_back(!l.first);
    ++v_it;
  }

  // lock the failed assertion
  assumptions.push_back(!failed->cond_literal);

  //status() << "the guard literal: " << failed->guard_literal << ", sign: " << failed->guard_literal.sign() << eom;

  bmc.prop_conv.set_assumptions(assumptions);

  if(bmc.prop_conv()==decision_proceduret::D_SATISFIABLE)
    return false;

  return true;
}

/*******************************************************************\

Function: fault_localizationt::update_scores

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::update_scores(lpointst &lpoints,
                                        const lpoints_valuet& value)
{
  for(auto &l : lpoints)
  {
    if(bmc.prop_conv.l_get(l.first).is_true())
    {
      l.second.score++;
    }
    else if(bmc.prop_conv.l_get(l.first).is_false() &&
            l.second.score>0)
    {
      l.second.score--;
    }
  }
}

/*******************************************************************\

Function: fault_localizationt::localize_linear

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::localize_linear(lpointst &lpoints)
{
  lpoints_valuet v;
  v.resize(lpoints.size());
  for(size_t i=0; i<lpoints.size(); ++i)
    v[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  for(size_t i=0; i<v.size(); ++i)
  {
    v[i]=tvt(tvt::tv_enumt::TV_TRUE);
    if(!check(lpoints,v)) update_scores(lpoints,v);
    v[i]=tvt(tvt::tv_enumt::TV_FALSE);
    if(!check(lpoints,v)) update_scores(lpoints,v);
    v[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  }
}

/*******************************************************************\

Function: fault_localizationt::pfl

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/
/**
bool fault_localizationt::all_false(const lpoints_valuet& v)
{
  for(auto &x: v)
    if(!x.is_false()) return false;
  return true;
}

bool fault_localizationt::all_true(const lpoints_valuet& v)
{
  for(auto &x: v)
    if(!x.is_true()) return false;
  return true;
}

bool fault_localizationt::equal(const lpoints_valuet& v1, const lpoints_valuet& v2)
{
  if(v1.size()!=v2.size()) return 0;
  for(std::size_t i=0; i<v1.size(); i++)
	  if(!(v1[i]==v2[i])) return false;
  return true;
}

void fault_localizationt::pfl(lpointst &lpoints)
{
  std::vector<lpoints_valuet> f_values, p_values, s_values;
  lpoints_valuet f_value, p_value;
  f_value.resize(lpoints.size());
  p_value.resize(lpoints.size());
  for(size_t i=0; i<lpoints.size(); ++i)
  {
    f_value[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
    p_value[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  }
  // to find failing traces
  while(true)
  {
	lpoints_valuet res;
    if(check(lpoints, failed->guard_literal, f_value, lpoints_valuet(), res))
    {
    	f_values.push_back(res);
    	if(all_false(res) && all_true(f_value)) break;
    	// update the 'f_value' vector
    	for(std::size_t i=0; i<res.size(); ++i)
    	{
    	  if(res[i].get_value()==tvt::tv_enumt::TV_TRUE)
    	  	f_value[i]=tvt(tvt::tv_enumt::TV_TRUE);
    	  status() << res[i] << eom;
    	}
    }
    else break;
  }

}
**/
void fault_localizationt::common(const std::vector<lpoints_valuet> &vs, lpoints_valuet& res)
{
/**	status() << "inside common: " << eom;
	 for(auto &v: vs)
	  {
	    for(auto &x: v)
	      status() << x.is_true() << " ";
	    status () << eom;
	  }
**/
  if(vs.empty()) return;
  for(std::size_t i=0; i<res.size(); ++i)
  {
	bool co=true;
    for(auto &v: vs)
      if(!v[i].is_true()) {co=false; break;}
    if(co) res[i]=tvt(tvt::tv_enumt::TV_TRUE);
  }
/**  status() << "result: " << eom;
  for(auto &x: res)
    status() << x.is_true() << " ";
  status () << eom;**/
}

bool fault_localizationt::mc2(const lpointst &lpoints,
  	  const literalt &property,
  	  const std::vector<lpoints_valuet>& ex1,
  	  const std::vector<lpoints_valuet>& ex2,
	  lpoints_valuet &res)
{
  bvt assumptions;
  //existing traces must be excluded
  for(auto &v: ex1)
  {
	std::vector<literalt> bv;
    lpoints_valuet::const_iterator v_it=v.begin();
	for(const auto &l : lpoints)
	{
	  if(v_it->is_true())
	    bv.push_back(l.first);
	  else if(v_it->is_false())
	 	bv.push_back(!l.first);
	  else assert(0);
	  ++v_it;
    }
	//existing test vector should not be excluded
	if(!bv.empty())
	{
      literalt existing;
      bmc.prop_conv.land(bv, existing);
      assumptions.push_back(!existing);
	}
  }
  for(auto &v: ex2)
  {
	std::vector<literalt> bv;
    lpoints_valuet::const_iterator v_it=v.begin();
	for(const auto &l : lpoints)
	{
	  if(v_it->is_true())
	    bv.push_back(l.first);
	  else if(v_it->is_false())
	 	bv.push_back(!l.first);
	  else assert(0);
	  ++v_it;
    }
	//existing test vector should not be excluded
	if(!bv.empty())
	{
      literalt existing;
      bmc.prop_conv.land(bv, existing);
      assumptions.push_back(!existing);
	}
  }


  assumptions.push_back(property);


  bmc.prop_conv.set_assumptions(assumptions);


  //Running the check
  if(bmc.prop_conv()==decision_proceduret::D_SATISFIABLE)
  {
    res.reserve(lpoints.size());
 	//lpoints_valuet::iterator v_it=res.begin();
 	for(const auto &l : lpoints)
 	{
 	  if(bmc.prop_conv.l_get(l.first).is_true())
 		  res.push_back(tvt(tvt::tv_enumt::TV_TRUE));
 	  else res.push_back(tvt(tvt::tv_enumt::TV_FALSE));
 	}
     return true;
   }

   return false;

}

bool fault_localizationt::mc(const lpointst &lpoints,
  	  const literalt &property,
  	  const std::vector<lpoints_valuet>& ex,
	  lpoints_valuet &res)
{
  bvt assumptions;

  //existing traces must be excluded
  for(auto &v: ex)
  {
	std::vector<literalt> bv;
    lpoints_valuet::const_iterator v_it=v.begin();
	for(const auto &l : lpoints)
	{
	  if(v_it->is_true())
	    bv.push_back(l.first);
	  else if(v_it->is_false())
	 	bv.push_back(!l.first);
	  else assert(0);
	  ++v_it;
    }
	//existing test vector should not be excluded
	if(!bv.empty())
	{
      literalt existing;
      bmc.prop_conv.land(bv, existing);
      assumptions.push_back(!existing);
	}
  }

  // 2. to construct the set of common components
  lpoints_valuet co;
  co.resize(lpoints.size());
  for(size_t i=0; i<lpoints.size(); ++i)
  {
    co[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  }
  common(ex, co);
  std::vector<literalt> bv;
  lpoints_valuet::const_iterator v_it=co.begin();
  for(const auto &l : lpoints)
  {
    if(v_it->is_true())
      bv.push_back(l.first);

  	++v_it;
  }
  //existing test vector should not be excluded
  if(!bv.empty())
  {
    literalt existing;
    bmc.prop_conv.land(bv, existing);
    assumptions.push_back(!existing);
  }

  assumptions.push_back(property);


  bmc.prop_conv.set_assumptions(assumptions);


  //Running the check
  if(bmc.prop_conv()==decision_proceduret::D_SATISFIABLE)
  {
    res.reserve(lpoints.size());
 	//lpoints_valuet::iterator v_it=res.begin();
 	for(const auto &l : lpoints)
 	{
 	  if(bmc.prop_conv.l_get(l.first).is_true())
 		  res.push_back(tvt(tvt::tv_enumt::TV_TRUE));
 	  else res.push_back(tvt(tvt::tv_enumt::TV_FALSE));
 	}
    return true;
   }

   return false;

}


bool fault_localizationt::mc(const lpointst &lpoints,
  	  const literalt &property,
  	  const std::vector<lpoints_valuet>& p_values,
  	  const lpoints_valuet &inc,
		  lpoints_valuet &res)
{
  bvt assumptions;

  //P traces must be excluded
  for(auto &v: p_values)
  {
	std::vector<literalt> bv;
    lpoints_valuet::const_iterator v_it=v.begin();
	for(const auto &l : lpoints)
	{
	  if(v_it->is_true())
	    bv.push_back(l.first);
	  else if(v_it->is_false())
	 	bv.push_back(!l.first);
	  else assert(0);
	  ++v_it;
    }
	//existing test vector should not be excluded
	if(!bv.empty())
	{
      literalt existing;
      bmc.prop_conv.land(bv, existing);
      assumptions.push_back(!existing);
	}
  }

  // 2. to include the single component vector
  lpoints_valuet::const_iterator v_it=inc.begin();
  for(const auto &l : lpoints)
  {
    if(v_it->is_true())
      assumptions.push_back(l.first);
  	++v_it;
  }

  assumptions.push_back(property);


  bmc.prop_conv.set_assumptions(assumptions);


  //Running the check
  if(bmc.prop_conv()==decision_proceduret::D_SATISFIABLE)
  {
    res.reserve(lpoints.size());
 	//lpoints_valuet::iterator v_it=res.begin();
 	for(const auto &l : lpoints)
 	{
 	  if(bmc.prop_conv.l_get(l.first).is_true())
 		  res.push_back(tvt(tvt::tv_enumt::TV_TRUE));
 	  else res.push_back(tvt(tvt::tv_enumt::TV_FALSE));
 	}
     return true;
   }

   return false;

}

/**
bool fault_localizationt::check(const lpointst &lpoints,
			 const literalt &property,
			 const lpoints_valuet& exclusive_v,
			 const lpoints_valuet& inclusive_v,
			 lpoints_valuet &res)
{

  bvt assumptions;
  std::vector<literalt> bv;
  if(!exclusive_v.empty())
  {
    lpoints_valuet::const_iterator v_it=exclusive_v.begin();
    for(const auto &l : lpoints)
    {
      if(v_it->is_true())
      {
    	bv.push_back(l.first);
      }
      ++v_it;
    }
  }

  if(!bv.empty())
  {
    literalt every;
    bmc.prop_conv.land(bv, every);
    assumptions.push_back(!every);
    status() << "not every: " << !every << eom;
  }


  if(!inclusive_v.empty())
  {
    lpoints_valuet::const_iterator v_it=inclusive_v.begin();
    for(const auto &l : lpoints)
    {
      if(v_it->is_true())
      {
        assumptions.push_back(l.first);
      }
      ++v_it;
    }
  }


  // lock the failed assertion
  assumptions.push_back(property);

  status() << "the guard literal: " << failed->guard_literal << ", sign: " << failed->guard_literal.sign() << ", property: " << property << eom;

  bmc.prop_conv.set_assumptions(assumptions);

  if(bmc.prop_conv()==decision_proceduret::D_SATISFIABLE)
  {
	res.reserve(lpoints.size());
	//lpoints_valuet::iterator v_it=res.begin();
	for(const auto &l : lpoints)
	{
	  if(bmc.prop_conv.l_get(l.first).is_true())
		  res.push_back(tvt(tvt::tv_enumt::TV_TRUE));
	  else res.push_back(tvt(tvt::tv_enumt::TV_FALSE));
	}
    return true;
  }

  return false;
}
**/

/*******************************************************************\

Function: fault_localizationt::run

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::run(irep_idt goal_id)
{
 // find failed property
  failed=get_failed_property();
  assert(failed!=bmc.equation.SSA_steps.end());

  if(goal_id==ID_nil)
    goal_id=failed->source.pc->source_location.get_property_id();
  lpointst &lpoints = lpoints_map[goal_id];

  // collect lpoints
  collect_guards(lpoints);

  if(lpoints.empty())
    return;

  status() << "Localizing fault" << eom;

  // pick localization method
  //  if(options.get_option("localize-faults-method")=="TBD")
  localize_linear(lpoints);
  //pfl(lpoints);

  //clear assumptions
  bvt assumptions;
  bmc.prop_conv.set_assumptions(assumptions);
}

/*******************************************************************\

Function: fault_localizationt::report()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::report(irep_idt goal_id)
{
  if(goal_id==ID_nil)
	goal_id=failed->source.pc->source_location.get_property_id();

  if(dict_goals.find(goal_id)==dict_goals.end()) return;

  std::string desc=goal_map[goal_id].description;
  if(desc.find("passing")!=std::string::npos) return;

  pfl_framework(goal_id);
  desc=dict_goals[goal_id];
  pfl_coret &core=dict_cores[desc];
  status() << eom << "[" + desc +  "] " + "fails" << "\n" << eom;
  status() << "Most likely fault location (single bug optimal fault localization):\n";
  for(int i=0; i<core.sb_lpoints.size(); i++)
  {
  	if(i==result_list) break;
    status() << core.sb_lpoints[i].info << ", function " << core.sb_lpoints[i].target->function << ": " << core.sb_lpoints[i].score << eom;
  }

  status() << eom;
  status() << "Most likely fault location (PFL):\n";
  for(int i=0; i<core.pfl_lpoints.size(); i++)
  {
    if(i==result_list) break;
    status() << core.pfl_lpoints[i].info << ", function " << core.pfl_lpoints[i].target->function << ": " << core.pfl_lpoints[i].score << eom;
  }

  status() << eom;
/**
  if(goal_id==ID_nil)
    goal_id=failed->source.pc->source_location.get_property_id();

  lpointst &lpoints = lpoints_map[goal_id];

  std::string info=goal_map[goal_id].description;
  //status() << "info: " << info << eom;
  if(has_prefix(info, "__CPROVER_fault passing traces")) return;

  if(lpoints.empty())
  {
    status() << "["+id2string(goal_id)+"]: \n"
                   << "   unable to localize fault"
                   << eom;
    return;
  }
  **/
/**
  debug() << "Fault localization scores:" << eom;
  lpointt &max=lpoints.begin()->second;
  for(auto &l : lpoints)
  {

    debug() << l.second.target->source_location
            << "\n  score: " << l.second.score << eom;
    if(max.score<l.second.score)
      max=l.second;
  }

  info=info.replace(0, 37, "");
  std::string info2=id2string(goal_id);
  std::size_t pos=info2.find("assertion.");
  info2=info2.substr(0, pos+9);
  std::string info3=max.target->source_location.as_string();
  std::size_t pos3=info3.find("line ");
  info3=info3.replace(0, pos3, "");
  //status() << "["+id2string(goal_id)+"]: \n"
  status() << "[" + info2 +  " at line "+ info +"] fails \n"
                   //<< "  " << info3 //max.target->source_location
                   << eom;
  status() << "Most likely fault location (single bug optimal fault localization):\n";**/
/**  for(int i=0; i<sb_lpoints.size(); i++)
  {
	if(i==result_list) break;
    status() << sb_lpoints[i].info << ", function " << sb_lpoints[i].target->function << ": " << sb_lpoints[i].score << eom;
  }
  status() << eom;
  status() << "Most likely fault location (PFL):\n";
  for(int i=0; i<pfl_lpoints.size(); i++)
  {
    if(i==result_list) break;
    status() << pfl_lpoints[i].info << ", function " << pfl_lpoints[i].target->function << ": " << pfl_lpoints[i].score << eom;
  }

  status() << eom;
**/
}

/*******************************************************************\

Function: fault_localizationt::operator()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

safety_checkert::resultt fault_localizationt::operator()()
{
  if(options.get_bool_option("stop-on-fail"))
    return stop_on_fail();
  else
    return bmc_all_propertiest::operator()();
}

/*******************************************************************\

Function: fault_localizationt::run_decision_procedure

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

decision_proceduret::resultt
fault_localizationt::run_decision_procedure(prop_convt &prop_conv)
{
  status() << "Passing problem to "
               << prop_conv.decision_procedure_text() << eom;

  prop_conv.set_message_handler(bmc.get_message_handler());

  // stop the time
  absolute_timet sat_start=current_time();

  bmc.do_conversion();

  freeze_guards();

  status() << "Running " << prop_conv.decision_procedure_text()
               << eom;

  decision_proceduret::resultt dec_result=prop_conv.dec_solve();
  // output runtime

  {
    absolute_timet sat_stop=current_time();
    status() << "Runtime decision procedure: "
             << (sat_stop-sat_start) << "s" << eom;
  }

  return dec_result;
}

/*******************************************************************\

Function: fault_localizationt::stop_on_fail

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

safety_checkert::resultt fault_localizationt::stop_on_fail()
{
  switch(run_decision_procedure(bmc.prop_conv))
  {
  case decision_proceduret::D_UNSATISFIABLE:
    bmc.report_success();
    return safety_checkert::SAFE;

  case decision_proceduret::D_SATISFIABLE:
    if(options.get_bool_option("trace"))
    {
      if(options.get_bool_option("beautify"))
        counterexample_beautificationt()(
          dynamic_cast<bv_cbmct &>(bmc.prop_conv), bmc.equation, bmc.ns);

      bmc.error_trace();
    }

    //localize faults
    run(ID_nil);
    status() << "\n** Most likely fault location:" << eom;
    report(ID_nil);

    bmc.report_failure();
    return safety_checkert::UNSAFE;

  default:
    error() << "decision procedure failed" << eom;

    return safety_checkert::ERROR;
  }
}

/*******************************************************************\

Function: fault_localizationt::goal_covered

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::goal_covered(
  const cover_goalst::goalt &)
{
  covered++;
  //status() << "covered is " << covered << ", goal_map size " << goal_map.size() << eom ;
  for(auto &g : goal_map)
  {
    // failed already?
    if(g.second.status==goalt::statust::FAILURE) continue;

    // check whether failed
    for(auto &c : g.second.instances)
    {
      literalt cond=c->cond_literal;

      if(solver.l_get(cond).is_false())
      {

        g.second.status=goalt::statust::FAILURE;
        symex_target_equationt::SSA_stepst::iterator next=c;
        next++; // include the assertion
        build_goto_trace(bmc.equation, next, solver, bmc.ns,
                         g.second.goto_trace);


          //localize faults
        failed=get_failed_property();
        assert(failed!=bmc.equation.SSA_steps.end());
        irep_idt goal_id=g.first;
        failed_goal=goal_id;
        if(goal_id==ID_nil)
          goal_id=failed->source.pc->source_location.get_property_id();
        lpointst &lpoints = lpoints_map[goal_id];

        // collect lpoints
        collect_guards(lpoints);

        if(lpoints.empty())
          return;
        debug() << eom << "source location: " << failed->source.pc->source_location << eom << eom;


        // We assume that a failing trace must contains the key world '__CPROVER_fault failing'
        std::string desc=goal_map[goal_id].description;
        if(desc.find("failing")!=std::string::npos)
        {
           desc.replace(desc.find("failing"), 7, "");

           v_lpoints_valuet failing;
           while(true)
           {
        	 bvt assumptions;
        	 bmc.prop_conv.set_assumptions(assumptions);
             lpoints_valuet res;
        	 //if(mc(lpoints, failed->guard_literal, failing, res))
             if(mc(lpoints, !failed->cond_literal, failing, res))
        	 {
               failing.push_back(res);
               for(auto &r: res)
            	   std::cout << r.is_true() << " ";
               std::cout << "\n";
        	 }
        	 else break;
           }


           // extra failing traces
           int c=failing.size();
           while(c++<trace_limit)
           {
             bvt assumptions;
             bmc.prop_conv.set_assumptions(assumptions);
             lpoints_valuet res;
        	 if(mc2(lpoints, !failed->cond_literal, failing, {}, res))
        	 {
               failing.push_back(res);
        	 }
        	 else break;
           }

           dict_f_values[desc]=failing;
           //dict_lpoints[desc]=lpoints;
           pfl_coret core;
           core.lpoints=lpoints;
           dict_cores[desc]=core;
           dict_goals[g.first]=desc;


           bvt assumptions;
           bmc.prop_conv.set_assumptions(assumptions);

        }
        else if(desc.find("passing")!=std::string::npos)
        {
          desc.replace(desc.find("passing"), 7, "");
          v_lpoints_valuet passing;
          std::cout <<"this is a passing trace " << desc << std::endl;
          while(true)
          {
       		lpoints_valuet res;
       		if(mc(lpoints, !failed->cond_literal, passing, res))
       	    {
              passing.push_back(res);
       	    }
       	    else break;
          }
          bvt assumptions;
          bmc.prop_conv.set_assumptions(assumptions);

          //debug() << "<<<Generating S>>>" << eom;
          // simple if we encounter failing traces after failing traces
          bool after_failing=(dict_f_values.find(desc)!=dict_f_values.end());
          lpoints_valuet cf, cp;
  	      cf.resize(lpoints.size());
  	      cp.resize(lpoints.size());
  	      for(size_t i=0; i<lpoints.size(); ++i)
  	      {
  	        cf[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  	        cp[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  	      }
  	      if(after_failing)
  	      {
  	        v_lpoints_valuet failing=dict_f_values[desc];
  	        common(failing, cf);
  	      }
  	      else
  	      {
  	        // we create a 'fake' cf
  	  	    for(size_t i=0; i<lpoints.size(); ++i)
  	  	    {
  	  	      cf[i]=tvt(tvt::tv_enumt::TV_TRUE);
  	  	    }
  	      }
  	      common(passing, cp);
          for(std::size_t i=0; i<lpoints.size(); ++i)
          {
        	if(!(cf[i].is_true() && !cp[i].is_true())) //must be failing but not passing
        	  continue;
            lpoints_valuet v=cf;
            for(std::size_t j=0; j<lpoints.size(); ++j)
            {
              if(i!=j) v[j]=tvt(tvt::tv_enumt::TV_UNKNOWN);
            }

       		lpoints_valuet res;
       	    if(mc(lpoints, !failed->guard_literal, passing, v, res))
       	    	passing.push_back(res);
          }

          // extra passing traces
          int c=passing.size(); //+sing.size();


          while(c++<trace_limit)
          {

            lpoints_valuet res;
       	    if(mc2(lpoints, !failed->cond_literal, passing, {}, res))
       	    {
              passing.push_back(res);
       	    }
       	    else break;

          }

          dict_p_values[desc]=passing;

          bmc.prop_conv.set_assumptions(assumptions);




        }

      }

    }
  }


}


void fault_localizationt::pfl_framework(const irep_idt goal)
{

  if(dict_goals.find(goal)==dict_goals.end())
    return;
  std::string desc=dict_goals[goal];
  pfl_coret & core=dict_cores[desc];

  core.F_values=dict_f_values[desc];


  if(dict_goals.find(goal)==dict_goals.end())
    core.P_values=v_lpoints_valuet(); // it may happen that there is no passing trace
  else
    core.P_values=dict_p_values[desc];

  clean_traces(core);
  compute_spectra(core);
  measure_sb(core);
  compute_ppv(core);
  pfl(core);

  std::cout << "----------------------\n";
       for(auto &l: core.cleaned_lpoints)
       {
          debug() << l.second.info << " ";
       }
       debug() << eom;

       for(auto &v: core.F_values)
       {
         for(auto &x: v)
           debug() << x.is_true() << " ";
         debug () << "-" << eom;
       }
       for(auto &v: core.P_values)
       {
         for(auto &x: v)
           debug() << x.is_true() << " ";
         debug () << "+" << eom;
       }

}


/*******************************************************************\

Function: fault_localizationt::report

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void fault_localizationt::report(
  const cover_goalst &cover_goals)
{
  bmc_all_propertiest::report(cover_goals);

  switch(bmc.ui)
  {
  case ui_message_handlert::PLAIN:
    if(cover_goals.number_covered()>0)
    {
      status() << "\n** Results and failure diagnosis:\n" << eom;
      for(auto &g : goal_map)
      {
        if(g.second.status!=goalt::statust::FAILURE) continue;
        report(g.first);

      }
    }
    break;
  case ui_message_handlert::XML_UI:
    break;
  case ui_message_handlert::JSON_UI:
    break;
  }
}




void fault_localizationt::compute_spectra(pfl_coret &core)
{
  auto & F_values=core.F_values;
  auto & P_values=core.P_values;
  auto & lpoints=core.cleaned_lpoints;
  //auto & cleaned_lpoints=core.cleaned_lpoints;
  auto & sb_lpoints=core.sb_lpoints;
  auto & pfl_lpoints=core.pfl_lpoints;
  auto & ppv=core.ppv;
  auto & ef = core.ef;
  auto & ep = core.ep;
  auto & nf = core.nf;
  auto & np = core.np;



  // ther number of blocks
  int num=lpoints.size();

  for(int i=0; i<num; ++i)
  {
	ef.push_back(0);
	ep.push_back(0);
	nf.push_back(0);
	np.push_back(0);

	for(auto &pv: F_values)
	{
	  if(pv[i].is_true()) ef[i]++;
	  else nf[i]++;
	}

	for(auto &pv: P_values)
	{
	  if(pv[i].is_true()) ep[i]++;
	  else np[i]++;
	}
  }
}

void fault_localizationt::measure_sb(pfl_coret &core)
{
	  auto & F_values=core.F_values;
	  auto & P_values=core.P_values;
	  auto & lpoints=core.cleaned_lpoints;
	  //auto & cleaned_lpoints=core.cleaned_lpoints;
	  auto & sb_lpoints=core.sb_lpoints;
	  auto & pfl_lpoints=core.pfl_lpoints;
	  auto &ppv=core.ppv;
	  auto & ef = core.ef;
	  auto & ep = core.ep;
	  auto & nf = core.nf;
	  auto & np = core.np;

  debug() << "ef: \n";
  for(auto &x: ef)
	debug() << x << ", ";
  debug() << "\n";
  debug() << "nf: \n";
  for(auto &x: nf)
	debug() << x << ", ";
  debug() << "\n";
  debug() << "ep: \n";
  for(auto &x: ep)
	debug() << x << ", ";
  debug() << "\n";
  debug() << "np: \n";
  for(auto &x: np)
	debug() << x << ", ";
  debug() << "\n";


  for(auto&x : lpoints)
  {
    sb_lpoints.push_back(x.second);
  }

  for(int i=0; i<lpoints.size(); i++)
  {
    double score=ef[i]-(ep[i]/(ep[i]+np[i]+1.0));
    sb_lpoints[i].score=score;
  }
  sort(sb_lpoints.begin(),
	   sb_lpoints.end(),
	   [](const lpointt& a, const lpointt& b)
	   {return a.score>b.score;});
}

void fault_localizationt::compute_ppv(pfl_coret &core)
{
	  auto & F_values=core.F_values;
	  auto & P_values=core.P_values;
	  auto & lpoints=core.cleaned_lpoints;
	  //auto & cleaned_lpoints=core.cleaned_lpoints;
	  auto & sb_lpoints=core.sb_lpoints;
	  auto & pfl_lpoints=core.pfl_lpoints;
	  auto &ppv=core.ppv;
	  auto & ef = core.ef;
	  auto & ep = core.ep;
	  auto & nf = core.nf;
	  auto & np = core.np;

  // ther number of blocks
  int num=lpoints.size();
 // for(int i=0; i<num; ++i)
 // {
    for(int i=0; i<ef.size(); ++i)
    {
    	double score=0;
    	if(ef[i]>0)
    	{
    		score=(ef[i]/(ef[i]+ep[i]+0.0));
    		//status() << ef[i] << ", " << ep[i] << ", " << score << eom;
    	}

    	ppv.push_back(score);
    }
 // }
    debug() << "ppv: " <<eom;
    for(auto &x:ppv)
    	debug() << x << " ";
    debug() << eom;
}

void fault_localizationt::pfl(pfl_coret &core)
{
	  auto & F_values=core.F_values;
	  auto & P_values=core.P_values;
	  auto & lpoints=core.cleaned_lpoints;
	  //auto & cleaned_lpoints=core.cleaned_lpoints;
	  auto & sb_lpoints=core.sb_lpoints;
	  auto & pfl_lpoints=core.pfl_lpoints;
	  auto &ppv=core.ppv;
	  auto & ef = core.ef;
	  auto & ep = core.ep;
	  auto & nf = core.nf;
	  auto & np = core.np;


  for(auto&x : lpoints)
  {
    pfl_lpoints.push_back(x.second);
  }
  std::vector<double> P(pfl_lpoints.size(), 0);


  for(int i=0; i<F_values.size(); i++)
  {
    double den=0;
    for(int c=0; c<F_values[i].size(); c++)
    {
      if(F_values[i][c].is_true())
    	  den += ppv[c];
    }
    for(int c=0; c<F_values[i].size(); c++)
    {
        if(F_values[i][c].is_true())
          P[c] = ((P[c]+(ppv[c]/den))-(P[c]*(ppv[c]/den)));
    }
  }
  for(int i=0; i<lpoints.size(); i++)
	  pfl_lpoints[i].score=P[i];
  sort(pfl_lpoints.begin(),
	   pfl_lpoints.end(),
	   [](const lpointt& a, const lpointt& b)
	   {return a.score>b.score;});
}


void fault_localizationt::clean_traces(pfl_coret &core)
{
	  auto & F_values=core.F_values;
	  auto & P_values=core.P_values;
	  auto & lpoints=core.lpoints;
	  auto & cleaned_lpoints=core.cleaned_lpoints;
	  auto & sb_lpoints=core.sb_lpoints;
	  auto & pfl_lpoints=core.pfl_lpoints;
	  auto & ppv=core.ppv;
	  auto & ef = core.ef;
	  auto & ep = core.ep;
	  auto & nf = core.nf;
	  auto & np = core.np;


  int i=-1;
  for(auto &v: lpoints)
  {
	i++;
	//if(i==lpoints.size()-1) continue;
	bool all_false=true;
    for(auto &x: F_values)
    {
      if(x[i].is_true()) {all_false=false; break;}
    }
	bool all_true=true;
    for(auto &x: P_values)
    {
      if(x[i].is_false()) {all_true=false; break;}
    }
    //if(i==lpoints.size()-1 or not (all_false or all_true))
    //if(i!=lpoints.size()-1 and
    //if (i!=lpoints.size()-1 and (all_false or all_true))
    //if ((all_false or all_true))
    if(all_false)
    {
        for(auto &x: F_values)
       	  x[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
         for(auto &x: P_values)
          x[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
    }
    else
    {
      cleaned_lpoints[v.first]=v.second;
    }

  }

  for(auto &v: F_values)
  {
    auto it=v.begin();
    while(it!=v.end())
    {
      if(it->is_unknown())
    	  it=v.erase(it);
      else it++;
    }
  }

  for(auto &v: P_values)
  {
    auto it=v.begin();
    while(it!=v.end())
    {
      if(it->is_unknown())
    	  it=v.erase(it);
      else it++;
    }
  }


  // this is for merging unwound bodies of the same block
  merge(core);

}

void fault_localizationt::merge(pfl_coret &core)
{
	  auto & F_values=core.F_values;
	  auto & P_values=core.P_values;
	  auto & cleaned_lpoints=core.cleaned_lpoints;
	  auto & sb_lpoints=core.sb_lpoints;
	  auto & pfl_lpoints=core.pfl_lpoints;
	  auto &ppv=core.ppv;
	  auto & ef = core.ef;
	  auto & ep = core.ep;
	  auto & nf = core.nf;
	  auto & np = core.np;
  lpointst lpoints; //=cleaned_lpoints;

  int i=-1;
  for(auto &v: cleaned_lpoints)
  {
	  i++;
	  int k=locate_first(v.second, cleaned_lpoints);
	  if(k==i)
	    lpoints[v.first]=v.second;
	  else
	  {
		  for(auto &v: F_values)
		  {
			if(v[i].is_true())
				v[k]= tvt(tvt::tv_enumt::TV_TRUE);
	       	 v[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
		  }
		  for(auto &v: P_values)
		  {
			if(v[i].is_true())
				v[k]= tvt(tvt::tv_enumt::TV_TRUE);
	       	v[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
		  }
	  }
  }

  cleaned_lpoints=lpoints;
  for(auto &v: F_values)
  {
    auto it=v.begin();
    while(it!=v.end())
    {
      if(it->is_unknown())
    	  it=v.erase(it);
      else it++;
    }
  }

  for(auto &v: P_values)
  {
    auto it=v.begin();
    while(it!=v.end())
    {
      if(it->is_unknown())
    	  it=v.erase(it);
      else it++;
    }
  }


}


int fault_localizationt::locate_first(const lpointt& lp, const lpointst& lps)
{
	int i=-1;
	for(auto &v: lps)
	{
	  i++;
	  //status() << "--> " << v.second.info << ", " << lp.info << ": " << (v.second.info==lp.info) << eom << eom;
	  if(v.second.info==lp.info) return i;
	}

	return ++i;
}





