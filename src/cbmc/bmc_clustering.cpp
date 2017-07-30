/*******************************************************************\

Module: Clustering Symbolic Execution of ANSI-C

Author: 

\*******************************************************************/

#include <limits>

#include <goto-symex/slice.h>
#include <util/time_stopping.h>

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

  while(!symex_state.call_stack().empty())
  {

    symex()(
      symex_state,
      goto_functions,
      goto_functions.function_map.at(goto_functions.entry_point()).body);

    if(symex_state.source.pc->type==GOTO)
    {
      // Now, we face GOTO
      if(reachable_if())
        symex().add_goto_if_assumption(symex_state, goto_functions);
      else if(reachable_else())
        symex().add_goto_else_assumption(symex_state, goto_functions);
      else assert(0);
    }

    statistics() << "size of program expression: "
                 << equation.SSA_steps.size()
                 << " steps" << eom;
    std::cout << "tot_vccs: " << symex().total_vccs << "\n";
  }

  prop_conv.set_all_frozen();

  return all_properties(goto_functions, prop_conv);
  decision_proceduret::resultt result=run_decision_procedure(prop_conv);
  if(result==decision_proceduret::resultt::D_SATISFIABLE)
    return safety_checkert::resultt::UNSAFE;
  else if(result==decision_proceduret::resultt::D_UNSATISFIABLE)
    return safety_checkert::resultt::SAFE;
  else
    return safety_checkert::resultt::ERROR;
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
    show_vcc();
    return safety_checkert::resultt::SAFE; // to indicate non-error
  }

  return result;
}

decision_proceduret::resultt bmc_clusteringt::run_and_clear_decision_procedure()
{
  prop_conv.set_all_frozen();

  for(auto & it : equation.SSA_steps)
    it.ignore=false;

  // each time a different solver is created
  prop_convt &prop_conv2=cbmc_solvers.get_solver_local()->prop_conv();

  decision_proceduret::resultt result=run_decision_procedure(prop_conv2);

  //equation.SSA_steps.pop_back();
  return result;
}

bool bmc_clusteringt::reachable_if()
{
  // make a snapshot
  goto_symext::statet backup_state=symex_state;
  auto tmp=equation;

  std::size_t num=equation.SSA_steps.size();
  symex().mock_goto_if_condition(symex_state, goto_functions);
  if(num==equation.SSA_steps.size()) return false;

  std::cout << "\n *** reachable if: " << tmp.count_ignored_SSA_steps() << "\n";
  show_vcc();

  //prop_conv.set_all_frozen();
  //guardt g(symex_state.guard);
  //g.make_not();
  //prop_conv.set_to_true(g);

  decision_proceduret::resultt result=run_and_clear_decision_procedure();

  // recover the analysis
  symex_state=backup_state;
  equation=tmp;

  return (result==decision_proceduret::resultt::D_SATISFIABLE);
}

bool bmc_clusteringt::reachable_else()
{
  // make a snapshot
  goto_symext::statet backup_state=symex_state;
  auto tmp=equation;

  std::size_t num=equation.SSA_steps.size();
  symex().mock_goto_else_condition(symex_state, goto_functions);
  if(num==equation.SSA_steps.size()) return false;

  std::cout << "\n**** reachable else \n";
  show_vcc();

  decision_proceduret::resultt result=run_and_clear_decision_procedure();

  // recover
  symex_state=backup_state;
  equation=tmp;

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
