/*******************************************************************\

Module: Clustering Bounded Model Checking

Author: 

\*******************************************************************/

#ifndef CPROVER_CBMC_SYMEX_BMC_CLUSTERING_H
#define CPROVER_CBMC_SYMEX_BMC_CLUSTERING_H

#include "symex_bmc.h"
#include <goto-symex/symex_target.h>

class symex_bmc_clusteringt:
  public symex_bmct
{
 public:
  symex_bmc_clusteringt(
    const namespacet &_ns,
    symbol_tablet &_new_symbol_table,
    symex_targett &_target);

  virtual void operator()(
    statet &state,
    const goto_functionst &goto_functions,
    const goto_programt &goto_program);

  virtual void mock_goto_if_condition(
    statet &state,
    const goto_functionst &goto_functions);

  virtual void mock_step(
    statet &state,
    const goto_functionst &goto_functions);

  virtual void mock_reach(
    statet &state,
    const goto_functionst &goto_functions);

  virtual void backtrack_learn(statet &state);

  virtual void print_learnt_map();


  virtual bool learnt(const symex_targett::sourcet &source);

  virtual void add_learnt_info(
    statet &state,
    const goto_functionst &goto_functions);

  virtual void add_goto_if_assumption(
      statet &state,
      const goto_functionst &goto_functions);

  virtual void mock_goto_else_condition(
    statet &state,
    const goto_functionst &goto_functions);

  virtual void add_goto_else_assumption(
      statet &state,
      const goto_functionst &goto_functions);

  virtual void do_nothing(
      statet &state,
      const goto_functionst &goto_functions);

  std::vector<statet> states;

  static int counter;

  void record(statet &state);

  void create_a_cluster(statet &state, symex_targett &equation);

  std::map<std::string, statet> clusters;

  statet &cluster(const std::string &id);

  statet &cluster(const statet &state);

  bool lotto() const;

  virtual void symex_guard_goto(statet &state, const exprt &guard);

protected:

  bool learning_symex=true;
  typedef std::map<symex_targett::sourcet, exprt> learnt_mapt;
  learnt_mapt learnt_map;


};

#endif
