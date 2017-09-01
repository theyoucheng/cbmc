 /*******************************************************************\

Module: Clustering Bounded Model Checking

Author: 

 \*******************************************************************/

#ifndef CPROVER_CBMC_BMC_CLUSTERING_H
#define CPROVER_CBMC_BMC_CLUSTERING_H

#include <list>
#include <map>

#include <util/options.h>

#include <solvers/prop/prop_conv.h>
#include <goto-symex/symex_target_equation.h>

#include "symex_bmc_clustering.h"
#include "bv_cbmc.h"
#include "bmc.h"
#include "cbmc_solvers.h"
#include <iostream>

class bmc_clusteringt:public bmct
{
public:
  bmc_clusteringt(
    const optionst &_options,
    const symbol_tablet &_symbol_table,
    message_handlert &_message_handler,
    prop_convt& _prop_conv,
    const goto_functionst &_goto_functions,
	const cbmc_solverst &_cbmc_solvers):
    bmct(
      _options,
      _symbol_table,
      _message_handler,
      _prop_conv),
      cbmc_solvers(_cbmc_solvers),
      goto_functions(_goto_functions),
      symex_clustering(ns, new_symbol_table, equation)
  {
    symex().constant_propagation=options.get_bool_option("propagation");
    equations.push_back(equation);
    symex().create_a_cluster(symex_state, equations[0]);
  }

  virtual ~bmc_clusteringt() { }

  // make public
  virtual resultt run()
  {
    return run(goto_functions);
  }

  virtual resultt step()
  {
    return step(goto_functions);
  }

  cbmc_solverst cbmc_solvers;

  virtual decision_proceduret::resultt
    run_decision_procedure(prop_convt &prop_conv);

  void do_conversion(prop_convt &prop_conv);
protected:
  const goto_functionst &goto_functions;

  // ENHANCE: move this into symex_bmc
  goto_symext::statet symex_state;
  symex_bmc_clusteringt symex_clustering;

  // overload
  virtual resultt run(const goto_functionst &goto_functions);
  virtual resultt step(const goto_functionst &goto_functions);

 private:

  symex_bmc_clusteringt &symex()
  {
    return symex_clustering;
  }

  decision_proceduret::resultt run_and_clear_decision_procedure();
  bool reachable_if();
  bool reachable_else();

  void show_state_vcc(const goto_symext::statet &state);
  void show_state_vcc_plain(const goto_symext::statet &state, std::ostream &out);

  std::vector<symex_target_equationt> equations;

  enum class state_resultt { D_SATISFIABLE, D_UNSATISFIABLE, D_ERROR, D_UNSAFE };

  void setup_clustering_unwind();

  goto_programt::instructiont::targett find_target(
    goto_programt::instructionst::const_iterator target_it,
	goto_functionst &goto_functions);

  void state_dump_c(goto_symext::statet &state);

  void update_goto_functions(goto_functionst &_goto_functions);

  bool is_a_loop(const goto_programt::targett t);

};

#endif
