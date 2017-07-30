/*******************************************************************\

Module: Clustering Bounded Model Checking for ANSI-C

Author:

\*******************************************************************/

#include <limits>

#include <util/source_location.h>

#include "symex_bmc_clustering.h"
#include <analyses/dirty.h>
#include <iostream>

/*******************************************************************\

Function: symex_bmc_incrementalt::symex_bmct

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

symex_bmc_clusteringt::symex_bmc_clusteringt(
  const namespacet &_ns,
  symbol_tablet &_new_symbol_table,
  symex_targett &_target):
  symex_bmct(_ns, _new_symbol_table, _target)
{
}

void symex_bmc_clusteringt::operator()(
  statet &state,
  const goto_functionst &goto_functions,
  const goto_programt &goto_program)
{
  assert(!goto_program.instructions.empty());

  if(state.symex_target==NULL)
  {
    state.source=symex_targett::sourcet(goto_program);
    assert(!state.threads.empty());
    assert(!state.call_stack().empty());
    state.top().end_of_function=--goto_program.instructions.end();
    state.top().calling_location.pc=state.top().end_of_function;
    state.symex_target=&target;
    state.dirty=new dirtyt(goto_functions);
    symex_transition(state, state.source.pc);
  }

  assert(state.top().end_of_function->is_end_function());

  while(!state.call_stack().empty())
  {
    if(state.source.pc->type==GOTO)
    {
      std::cout << "\n*** GOTO: " << state.source.pc->source_location << "\n";

      const auto &guard=state.source.pc->guard;
      if(!guard.is_true() && !guard.is_false())
      {
    	merge_gotos(state); // in case there is pending goto
        return;
      }
    }

    symex_step(goto_functions, state);

  }

  delete state.dirty;
  state.dirty=0;
}

void symex_bmc_clusteringt::mock_goto_if_condition(
  statet &state,
  const goto_functionst &goto_functions)
{
  if(!state.guard.is_false())
  {
    std::string msg=id2string(state.source.pc->source_location.get_comment());
	if(msg=="")
      msg="__CPROVER_goto_cond: if";
    exprt tmp(state.source.pc->guard);

    clean_expr(tmp, state, false);
    vcc(tmp, msg, state);
  }
}

void symex_bmc_clusteringt::add_goto_if_assumption(
    statet &state,
    const goto_functionst &goto_functions)
{

  assert(!state.threads.empty());
  assert(!state.call_stack().empty());

  // depth exceeded?
  {
    unsigned max_depth=options.get_unsigned_int_option("depth");
    if(max_depth!=0 && state.depth>max_depth)
      state.guard.add(false_exprt());
    state.depth++;
  }

  if(!state.guard.is_false())
  {
    exprt tmp=state.source.pc->guard;
    clean_expr(tmp, state, false);
    tmp.make_not();
    state.rename(tmp, ns);
    symex_assume(state, tmp);
  }

  symex_goto(state);
}

void symex_bmc_clusteringt::mock_goto_else_condition(
  statet &state,
  const goto_functionst &goto_functions)
{

  if(!state.guard.is_false())
  {
    std::string msg=id2string(state.source.pc->source_location.get_comment());
    if(msg=="")
      msg="__CPROVER_goto_cond: else";
    exprt tmp(state.source.pc->guard);
    tmp.make_not();
    clean_expr(tmp, state, false);
    vcc(tmp, msg, state);
  }
}

void symex_bmc_clusteringt::add_goto_else_assumption(
    statet &state,
    const goto_functionst &goto_functions)
{
  #if 0
  std::cout << "\ninstruction type is " << state.source.pc->type << '\n';
  std::cout << "Location: " << state.source.pc->source_location << '\n';
  std::cout << "Guard: " << from_expr(ns, "", state.guard.as_expr()) << '\n';
  std::cout << "Code: " << from_expr(ns, "", state.source.pc->code) << '\n';
  #endif

  assert(!state.threads.empty());
  assert(!state.call_stack().empty());

  // depth exceeded?
  {
    unsigned max_depth=options.get_unsigned_int_option("depth");
    if(max_depth!=0 && state.depth>max_depth)
      state.guard.add(false_exprt());
    state.depth++;
  }

  if(!state.guard.is_false())
  {
    exprt tmp=state.source.pc->guard;
    clean_expr(tmp, state, false);
    state.rename(tmp, ns);
    symex_assume(state, tmp);
  }

  symex_goto(state);
}
