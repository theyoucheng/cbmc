/*******************************************************************\

Module: Clustering Symbolic Execution of ANSI-C

Author: 

\*******************************************************************/

#include <limits>

#include <goto-symex/slice.h>
#include <util/time_stopping.h>
#include <util/string2int.h>
#include <util/source_location.h>
#include <util/string_utils.h>
#include <util/time_stopping.h>
#include <util/message.h>
#include <util/json.h>
#include <util/cprover_prefix.h>

#include <langapi/mode.h>
#include <langapi/languages.h>
#include <langapi/language_util.h>

#include <ansi-c/ansi_c_language.h>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "bmc_clustering.h"
#include <iostream>

/*******************************************************************\

Function: bmc_clusteringt::step

  Inputs:

 Outputs:

 Purpose: run incremental BMC loop

\*******************************************************************/

safety_checkert::resultt bmc_clusteringt::step(
  const goto_functionst &goto_functions)
{
  symex().total_vccs=0;
  symex().remaining_vccs=0;

  setup_clustering_unwind();

  while(!symex_state.call_stack().empty())
  {
    std::cout << "xxxxxxxxxxxxxxxxxx\n";
    symex()(
      symex_state,
      goto_functions,
      goto_functions.function_map.at(goto_functions.entry_point()).body);

    bool ended=symex_state.call_stack().empty();

    //std::cout << "**before reachable: " << symex_state.source.pc->source_location << "\n";
    if(ended)
    {
      symex().backtrack_learn(symex_state);
      symex().print_learnt_map();
    }

    if(!reachable())
    {
      std::cout << "backtrack: \n";
      throw(0); //continue;
    }

    if(symex_state.source.pc->type!=GOTO)
    {
      symex().mock_step(symex_state, goto_functions);
      continue;
    }
    else //if(symex_state.source.pc->type==GOTO)
    {
      // Now, we face GOTO

      std::cout << "####  " << (symex_state.source.pc)->source_location << "\n";
      std::cout << "#### loop id?: "
    		    << symex_state.top().loop_iterations[goto_programt::loop_id(symex_state.source.pc)].count
				<< ", "
    		    << symex_state.top().loop_iterations[goto_programt::loop_id(symex_state.source.pc)].is_recursion
				<< ", "
				<< options.get_unsigned_int_option("unwind")
				<< "\n";

      if(reachable_if())
      {
        symex().add_goto_if_assumption(symex_state, goto_functions);
      }
      else if(reachable_else())
      {
        symex().add_goto_else_assumption(symex_state, goto_functions);
      }
      else assert(0);
    }

    statistics() << "size of program expression: "
                 << equation.SSA_steps.size()
                 << " steps" << eom;
    std::cout << "tot_vccs: " << symex().total_vccs << "\n";
  }

  prop_conv.set_all_frozen();

  return all_properties(goto_functions, prop_conv);
}

/*******************************************************************\

Function: bmc_incrementalt::run

  Inputs:

 Outputs:

 Purpose: run incremental BMC loop

\*******************************************************************/

safety_checkert::resultt bmc_clusteringt::run(
  const goto_functionst &goto_functions)
{
  safety_checkert::resultt result;

  result=step(goto_functions);

  if(options.get_bool_option("show-vcc"))
  {

    std::cout << "***symex state vcc: \n";
    show_state_vcc(symex_state);

    show_state_vcc(symex().cluster(symex_state));

    return safety_checkert::resultt::SAFE; // to indicate non-error
  }

  return result;
}

decision_proceduret::resultt bmc_clusteringt::run_and_clear_decision_procedure()
{
  prop_conv.set_all_frozen();

  //for(auto & it : equation.SSA_steps)
  //  it.ignore=false;
  auto last=equation.SSA_steps.end();
  --last;
  for(auto it=equation.SSA_steps.begin(); it!=last; it++)
    if(it->is_assert()) it->ignore=true;

  // each time a different solver is created
  prop_convt &prop_conv2=cbmc_solvers.get_solver_local()->prop_conv();

  decision_proceduret::resultt result=run_decision_procedure(prop_conv2);

  //equation.SSA_steps.pop_back();
  return result;
}

bool bmc_clusteringt::reachable_assert()
{

  std::cout << "\n++++++++++++++++++++ reachable assert +++++++++++++++++\n";
  std::cout << symex_state.source.pc->source_location << "\n";
  std::cout << from_expr(symex_state.source.pc->code) << "\n";

  // make a snapshot
  goto_symext::statet backup_state=symex_state;
  auto tmp=equation;

  std::size_t num=equation.SSA_steps.size();
  symex().mock_step(symex_state, goto_functions);

  if(num==equation.SSA_steps.size()) return false;
  show_vcc();
  decision_proceduret::resultt result=run_and_clear_decision_procedure();

  // recover the analysis
  symex_state=backup_state;
  equation=tmp;

  --symex().total_vccs;
  --symex().remaining_vccs;

  return (result==decision_proceduret::resultt::D_SATISFIABLE);
}

void bmc_clusteringt::clear(symex_target_equationt &equation)
{
  for(auto &x: equation.SSA_steps)
    if(x.is_assert()) x.ignore=true;
}


bool bmc_clusteringt::reachable()
{

  std::cout << "\n++++++++++++++++++++ reachability analysis +++++++++++++++++\n";
  //std::cout << symex_state.source.pc->source_location << "\n";
  //std::cout << from_expr(symex_state.source.pc->code) << "\n";

  // make a snapshot
  goto_symext::statet backup_state=symex_state;
  auto tmp=equation;

  std::size_t num=equation.SSA_steps.size();
  std::cout << "num: " << num << "\n";
  clear(equation);
  symex().mock_reach(symex_state, goto_functions);
  std::cout << "--------\n";

  if(num==equation.SSA_steps.size()) return false;
  show_vcc();
  decision_proceduret::resultt result=run_and_clear_decision_procedure();

  // recover the analysis
  symex_state=backup_state;
  equation=tmp;

  --symex().total_vccs;
  --symex().remaining_vccs;

  return (result==decision_proceduret::resultt::D_SATISFIABLE);
}

bool bmc_clusteringt::reachable_if()
{

  std::cout << "\n++++++++++++++++++++ reachable if +++++++++++++++++\n";
  std::cout << symex_state.source.pc->source_location << "\n";
  std::cout << from_expr(symex_state.source.pc->code) << "\n";

  // make a snapshot
  goto_symext::statet backup_state=symex_state;
  auto tmp=equation;

  std::size_t num=equation.SSA_steps.size();
  clear(equation);
  symex().mock_goto_if_condition(symex_state, goto_functions);

  if(num==equation.SSA_steps.size()) return false;
  show_vcc();
  decision_proceduret::resultt result=run_and_clear_decision_procedure();

  // recover the analysis
  symex_state=backup_state;
  equation=tmp;

  --symex().total_vccs;
  --symex().remaining_vccs;

  return (result==decision_proceduret::resultt::D_SATISFIABLE);
}

bool bmc_clusteringt::reachable_else()
{
  std::cout << "\n++++++++++++++++++++ reachable else +++++++++++++++++\n";
  std::cout << symex_state.source.pc->source_location << "\n";
  std::cout << from_expr(symex_state.source.pc->code) << "\n";

  // make a snapshot
  goto_symext::statet backup_state=symex_state;
  auto tmp=equation;

  std::size_t num=equation.SSA_steps.size();
  clear(equation);
  symex().mock_goto_else_condition(symex_state, goto_functions);
  show_vcc();
  if(num==equation.SSA_steps.size()) return false;
  decision_proceduret::resultt result=run_and_clear_decision_procedure();

  // recover
  symex_state=backup_state;
  equation=tmp;

  --symex().total_vccs;
  --symex().remaining_vccs;

  return (result==decision_proceduret::resultt::D_SATISFIABLE);
}

decision_proceduret::resultt
bmc_clusteringt::run_decision_procedure(prop_convt &prop_conv)
{
  status() << "Passing problem to "
           << prop_conv.decision_procedure_text() << eom;

  prop_conv.set_message_handler(get_message_handler());

  // stop the time
  absolute_timet sat_start=current_time();

  do_conversion(prop_conv);

  status() << "Running " << prop_conv.decision_procedure_text() << eom;


  decision_proceduret::resultt dec_result=prop_conv.dec_solve();
  // output runtime

  {
    absolute_timet sat_stop=current_time();
    status() << "Runtime decision procedure: "
             << (sat_stop-sat_start) << "s" << eom;
  }

  return dec_result;
}

void bmc_clusteringt::do_conversion(prop_convt &prop_conv)
{
  // convert HDL (hook for hw-cbmc)
  do_unwind_module();

  status() << "converting SSA" << eom;

  // convert SSA
  equation.convert(prop_conv);

  // the 'extra constraints'
  if(!bmc_constraints.empty())
  {
    status() << "converting constraints" << eom;

    forall_expr_list(it, bmc_constraints)
      prop_conv.set_to_true(*it);
  }
}

void bmc_clusteringt::show_state_vcc(const goto_symext::statet &state)
{
  const std::string &filename=options.get_option("outfile");
  bool have_file=!filename.empty() && filename!="-";

  std::ofstream of;

  if(have_file)
  {
    of.open(filename);
    if(!of)
      throw "failed to open file "+filename;
  }

  std::ostream &out=have_file?of:std::cout;

  show_state_vcc_plain(state, out);

  if(have_file)
    of.close();
}

void bmc_clusteringt::show_state_vcc_plain(
  const goto_symext::statet &state,
  std::ostream &out)
{
  out << "\n" << "VERIFICATION CONDITIONS:" << "\n" << "\n";

  languagest languages(ns, new_ansi_c_language());

  bool has_threads=equation.has_threads();

  symex_target_equationt *symex_target_equation=
    dynamic_cast<symex_target_equationt*>(state.symex_target);

  for(symex_target_equationt::SSA_stepst::iterator
      s_it=symex_target_equation->SSA_steps.begin();
      s_it!=symex_target_equation->SSA_steps.end();
      s_it++)
  {
    if(!s_it->is_assert())
      continue;

    if(s_it->source.pc->source_location.is_not_nil())
      out << s_it->source.pc->source_location << "\n";

    if(s_it->comment!="")
      out << s_it->comment << "\n";

    symex_target_equationt::SSA_stepst::const_iterator
      p_it=symex_target_equation->SSA_steps.begin();

    // we show everything in case there are threads
    symex_target_equationt::SSA_stepst::const_iterator
      last_it=has_threads?symex_target_equation->SSA_steps.end():s_it;

    for(unsigned count=1; p_it!=last_it; p_it++)
      if(p_it->is_assume() || p_it->is_assignment() || p_it->is_constraint())
      {
        if(!p_it->ignore)
        {
          std::string string_value;
          languages.from_expr(p_it->cond_expr, string_value);
          out << "{-" << count << "} " << string_value << "\n";

          #if 0
          languages.from_expr(p_it->guard_expr, string_value);
          out << "GUARD: " << string_value << "\n";
          out << "\n";
          #endif

          count++;
        }
      }

    out << "|--------------------------" << "\n";

    std::string string_value;
    languages.from_expr(s_it->cond_expr, string_value);
    out << "{" << 1 << "} " << string_value << "\n";

    out << "\n";
  }
}

void bmc_clusteringt::setup_clustering_unwind()
{
  const std::string &set=options.get_option("unwindset");
  std::vector<std::string> unwindset_loops;
  split_string(set, ',', unwindset_loops, true, true);

  for(auto &val : unwindset_loops)
  {
    unsigned thread_nr=0;
    bool thread_nr_set=false;

    if(!val.empty() &&
       isdigit(val[0]) &&
       val.find(":")!=std::string::npos)
    {
      std::string nr=val.substr(0, val.find(":"));
      thread_nr=unsafe_string2unsigned(nr);
      thread_nr_set=true;
      val.erase(0, nr.size()+1);
    }

    if(val.rfind(":")!=std::string::npos)
    {
      std::string id=val.substr(0, val.rfind(":"));
      long uw=unsafe_string2int(val.substr(val.rfind(":")+1));

      if(thread_nr_set)
        symex().set_unwind_thread_loop_limit(thread_nr, id, uw);
      else
        symex().set_unwind_loop_limit(id, uw);
    }
  }

  if(options.get_option("unwind")!="")
    symex().set_unwind_limit(options.get_unsigned_int_option("unwind"));
}

