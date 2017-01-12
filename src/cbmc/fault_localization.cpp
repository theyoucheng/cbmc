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
  int curr_block_start=-1;
  std::vector<int> opens;
  for(symex_target_equationt::SSA_stepst::const_iterator
      it=bmc.equation.SSA_steps.begin();
      it!=bmc.equation.SSA_steps.end(); it++)
  {

	if(it->is_goto())
		curr_block_start=atoi(it->source.pc->source_location.get_line().c_str())-1;


    if(it->is_assignment() &&
       it->assignment_type==symex_targett::STATE &&
       !it->ignore)
    {
        std::string lhs=from_expr(it->cond_expr.op0()).c_str();
        if (has_prefix(lhs, "__CPROVER_fault")) continue;

      //if(!it->guard_literal.is_constant())
      {
        lpoints[it->guard_literal].target=it->source.pc;
        int lnum=atoi(it->source.pc->source_location.get_line().c_str());
        lnum--;
        if(lnum==curr_block_start or curr_block_start<0)
            lpoints[it->guard_literal].info="line " + std::to_string(lnum);
        else
         lpoints[it->guard_literal].info="line from " + std::to_string(curr_block_start)+" to "+std::to_string(lnum);

        curr_block_start=lnum+1;
        //lpoints[it->guard_literal].info=it->source.pc->source_location.get_line().c_str();


        lpoints[it->guard_literal].score=0;
      }
      status() << it->source.pc->source_location << eom;
    }

    if(it==failed)
      break;
  }
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
	status() << "inside common: " << eom;
	 for(auto &v: vs)
	  {
	    for(auto &x: v)
	      status() << x.is_true() << " ";
	    status () << eom;
	  }

  if(vs.empty()) return;
  for(std::size_t i=0; i<res.size(); ++i)
  {
	bool co=true;
    for(auto &v: vs)
      if(!v[i].is_true()) {co=false; break;}
    if(co) res[i]=tvt(tvt::tv_enumt::TV_TRUE);
  }
  status() << "result: " << eom;
  for(auto &x: res)
    status() << x.is_true() << " ";
  status () << eom;
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

  //status() << "the guard literal: " << failed->guard_literal << ", sign: " << failed->guard_literal.sign() << ", property: " << property << eom;

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
  status() << "[" + info2 +  " at line "+ info +"]: \n"
                   //<< "  " << info3 //max.target->source_location
                   << eom;
  status() << "According to single bug optimal fault localization:\n";
  for(int i=0; i<sb_lpoints.size(); i++)
  {
	if(i==10) break;
    status() << sb_lpoints[i].info << ", function " << sb_lpoints[i].target->function << ": " << sb_lpoints[i].score << eom;
  }
  status() << eom;
  status() << "According to PFL:\n";
  for(int i=0; i<pfl_lpoints.size(); i++)
  {
    if(i==10) break;
    status() << pfl_lpoints[i].info << ", function " << pfl_lpoints[i].target->function << ": " << pfl_lpoints[i].score << eom;
  }

  status() << eom;

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
        if(goal_id==ID_nil)
          goal_id=failed->source.pc->source_location.get_property_id();
        lpointst &lpoints = lpoints_map[goal_id];

        // collect lpoints
        collect_guards(lpoints);
        status() << "the collection of guards: " << lpoints.size() << eom;
        for(auto &l: lpoints)
        	status() << l.first << " ";
        status() << eom << eom;

        if(lpoints.empty())
          return;
        status() << eom << "source location: " << failed->source.pc->source_location << eom << eom;

        /**
         * 1. the failing trace (F) must be built first
         * 2. then the passing trace (P)
         * 3. finally it is S
         *
         */
        if(f_values.empty())
        {
            status() << "<<<Generating failing traces>>>" << eom;
            status() << goal_map[goal_id].description << eom;
           assert(has_prefix(goal_map[goal_id].description, "__CPROVER_fault failing traces"));


           status() << c->source.pc->source_location << ": " << c->guard_literal << ", " << c->guard_literal.sign() << eom;
           while(true)
           {
        	 bvt assumptions;
        	 bmc.prop_conv.set_assumptions(assumptions);
             lpoints_valuet res;
        	 if(mc(lpoints, failed->guard_literal, f_values, res))
        	 {
               f_values.push_back(res);
        	 }
        	 else break;
           }
          // bvt assumptions;
          // bmc.prop_conv.set_assumptions(assumptions);

           // extra failing traces
           int c=f_values.size();
           while(c++<trace_limit)
           {
        	 status() << "<<<<< c is " << c << eom;
             bvt assumptions;
             bmc.prop_conv.set_assumptions(assumptions);
             lpoints_valuet res;
        	 if(mc2(lpoints, failed->guard_literal, f_values, f_extra_values, res))
        	 {
               f_extra_values.push_back(res);
        	 }
        	 else break;
           }


           bvt assumptions;
           bmc.prop_conv.set_assumptions(assumptions);

        }
        else //if(!f_values.empty())
        {
          status() << "<<<Generating passing traces>>>" << eom;
          status() << goal_map[goal_id].description << eom;

          assert(has_prefix(goal_map[goal_id].description, "__CPROVER_fault passing traces"));
          //status() << "<<<Generating passing traces>>>" << eom;
          status() << c->source.pc->source_location << ": " << c->guard_literal << ", " << c->guard_literal.sign() << eom;
          while(true)
          {
       		lpoints_valuet res;
       		if(mc(lpoints, failed->guard_literal, p_values, res))
       	    {
              p_values.push_back(res);
       	    }
       	    else break;
          }
          bvt assumptions;
          bmc.prop_conv.set_assumptions(assumptions);

          status() << "<<<Generating S>>>" << eom;
          lpoints_valuet cf, cp;
  	      cf.resize(lpoints.size());
  	      cp.resize(lpoints.size());
  	      for(size_t i=0; i<lpoints.size(); ++i)
  	      {
  	        cf[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  	        cp[i]=tvt(tvt::tv_enumt::TV_UNKNOWN);
  	      }
  	      common(f_values, cf);
  	      common(p_values, cp);
          for(std::size_t i=0; i<lpoints.size(); ++i)
          {
        	if(!(cf[i].is_true() && !cp[i].is_true()))
        	  continue;
            lpoints_valuet v=cf;
            for(std::size_t j=0; j<lpoints.size(); ++j)
            {
              if(i!=j) v[j]=tvt(tvt::tv_enumt::TV_UNKNOWN);
            }

       		lpoints_valuet res;
       	    if(mc(lpoints, failed->guard_literal, v, res))
       	    	s_values.push_back(res);
          }

          // extra passing traces
          int c=p_values.size()+s_values.size();

          std::vector<lpoints_valuet> ps;
          ps.insert(ps.end(), p_values.begin(), p_values.end());
          ps.insert(ps.end(), s_values.begin(), s_values.end());

          while(c++<trace_limit)
          {

            lpoints_valuet res;
       	    if(mc2(lpoints, failed->guard_literal, ps, p_extra_values, res))
       	    {
              p_extra_values.push_back(res);
       	    }
       	    else break;

          }

        }


        status() << "\nThe coverage matrix" << eom;

        if(F_values.empty())
        {
          F_values.insert(F_values.end(), f_values.begin(), f_values.end());
          F_values.insert(F_values.end(), f_extra_values.begin(), f_extra_values.end());
        }
        P_values.insert(P_values.end(), p_values.begin(), p_values.end());
        P_values.insert(P_values.end(), s_values.begin(), s_values.end());
        P_values.insert(P_values.end(), p_extra_values.begin(), p_extra_values.end());


        if(!F_values.empty() and !P_values.empty())
        {
        	clean_traces(lpoints);
            compute_spectra(cleaned_lpoints);
            measure_sb(cleaned_lpoints);
            compute_ppv(cleaned_lpoints);
            pfl(cleaned_lpoints);
        }


        for(auto &l: lpoints)
        {
           status() << l.second.info << " ";
        }
        status() << eom;
        for(auto &v: F_values)
        {
          for(auto &x: v)
            status() << x.is_true() << " ";
          status () << "-" << eom;
        }
        for(auto &v: P_values)
        {
          for(auto &x: v)
            status() << x.is_true() << " ";
          status () << "+" << eom;
        }

/**
        for(auto &v: f_values)
        {
          for(auto &x: v)
            status() << x.is_true() << " ";
          status () << "-" << eom;
        }
        for(auto &v: f_extra_values)
        {
          for(auto &x: v)
            status() << x.is_true() << " ";
          status () << "-" << eom;
        }/**
        //status() << eom << "The set of passing traces (P)" << eom;
        for(auto &v: p_values)
        {
          for(auto &x: v)
            status() << x.is_true() << " ";
          status () << "+" << eom;
        }
        //status() << eom << "The S set" << eom;
        for(auto &v: s_values)
        {
          for(auto &x: v)
            status() << x.is_true() << " ";
          status () << "+" << eom;
        }
        for(auto &v: p_extra_values)
        {
          for(auto &x: v)
            status() << x.is_true() << " ";
          status () << "+" << eom;
        }**/
        status () << eom;
        // f_values.clear(); p_values.clear(); s_values.clear();

      }
    }
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
      status() << "\n** Most likely fault location:" << eom;
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




void fault_localizationt::compute_spectra(const lpointst& lpoints)
{
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

void fault_localizationt::measure_sb(const lpointst& lpoints)
{

  status() << "ef: \n";
  for(auto &x: ef)
	status() << x << ", ";
  status() << "\n";
  status() << "nf: \n";
  for(auto &x: nf)
	status() << x << ", ";
  status() << "\n";
  status() << "ep: \n";
  for(auto &x: ep)
	status() << x << ", ";
  status() << "\n";
  status() << "np: \n";
  for(auto &x: np)
	status() << x << ", ";
  status() << "\n";


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

void fault_localizationt::compute_ppv(const lpointst& lpoints)
{
  // ther number of blocks
  int num=lpoints.size();
  for(int i=0; i<num; ++i)
  {
    for(int i=0; i<ef.size(); ++i)
    {
    	double score=0;
    	if(ef[i]>0)
    		score=(ef[i]/(ef[i]+ep[i]+0.0));
    	ppv.push_back(score);
    }
  }
}

void fault_localizationt::pfl(const lpointst& lpoints)
{
  for(auto&x : lpoints)
  {
    pfl_lpoints.push_back(x.second);
  }
  std::vector<double> P(pfl_lpoints.size(), 0);

  for(int i=0; i<F_values.size(); i++)
  {
    double den=0;
    for(int c=0; c<F_values[i].size()-1; c++)
    {
      if(F_values[i][c].is_true())
    	  den += ppv[c];
    }
    for(int c=0; c<F_values[i].size()-1; c++)
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


void fault_localizationt::clean_traces(const lpointst &lpoints)
{

 // cleaned_lpoints=lpoints;
 // return;

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


}









