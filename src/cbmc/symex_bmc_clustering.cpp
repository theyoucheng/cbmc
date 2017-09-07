/*******************************************************************\

Module: Clustering Bounded Model Checking for ANSI-C

Author:

\*******************************************************************/

#include <limits>

#include <util/source_location.h>

#include "symex_bmc_clustering.h"
#include <analyses/dirty.h>
#include <goto-programs/wp.h>
#include <util/prefix.h>
#include <iostream>

/*******************************************************************\

Function: symex_bmc_incrementalt::symex_bmct

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

int symex_bmc_clusteringt::counter=0;

symex_bmc_clusteringt::symex_bmc_clusteringt(
  const namespacet &_ns,
  symbol_tablet &_new_symbol_table,
  symex_targett &_target):
  symex_bmct(_ns, _new_symbol_table, _target)
{
}

bool symex_bmc_clusteringt::learnt(const symex_targett::sourcet &source)
{
  if(!learning_symex) return false;
  auto it=learnt_map.find(source);
  if(it!=learnt_map.end())
    return !it->second.is_false();
  return false;
  //for(auto it=learnt_map.begin(); it!=learnt_map.end(); ++it)
  //  if(!(it->first.source<source || source<it->first.source))
  //    return !it->second.is_false();
  return false;
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

  assert(!state.id.empty());

  while(!state.call_stack().empty())
  {
    state.locations.push_back(state.source);
    if(learning_symex)
    {
      if(learnt_map.find(state.source)==learnt_map.end())
        learnt_map[state.source]=false_exprt();
    }
    //std::cout << state.source.pc->source_location << "\n";
    //std::cout << "xxxxxxxxxxxx" << (state.source.pc->type==GOTO) << "\n";

    if(state.source.pc->type==GOTO)
    {
      const auto &guard=state.source.pc->guard;
      if(!guard.is_true() && !guard.is_false())
      {
    	merge_gotos(state); // in case there is pending goto
        return;
      }
    }

    if(learnt(state.source))
    {
    //  std::cout << "****learnt: " << state.source.pc->source_location << "\n";
      return;
    }
    //std::cout << "not learnt: " << state.source.pc->source_location << "\n";
    std::cout << "before symex_step\n";
    symex_step(goto_functions, state);
    std::cout << "after symex_step\n";
  }
  std::cout << "out of the loop\n";
  delete state.dirty;
  state.dirty=0;
  std::cout << "///////\n";
}

void symex_bmc_clusteringt::add_learnt_info(
  statet &state,
  const goto_functionst &goto_functions)
{
  for(auto &x: state.locations)
  {
    if(learnt_map[x].is_false()) continue;
    std::cout << "Added learnt info: " << from_expr(learnt_map[x]) << ", "
    		<< state.source.pc->source_location << "\n";
    exprt tmp(learnt_map[x]);
    tmp.make_not();
    clean_expr(tmp, state, false);
    vcc(tmp, "", state);
  }
}

void symex_bmc_clusteringt::print_learnt_map()
{
  std::cout << "\n*** The map learnt ***\n";
  for(auto &x: learnt_map)
  {
    if(x.second.is_false()) continue;
    std::cout << x.first.pc->source_location;
    std::cout << ": ";
    std::cout << from_expr(x.second) << "\n";
  }
}

void symex_bmc_clusteringt::backtrack_learn(statet &state)
{
  exprt learnt_expr=true_exprt();
  std::cout << "map size: " << learnt_map.size() << "\n";
  for(auto it=state.locations.rbegin(); it!=state.locations.rend(); ++it)
  {
    if(it->pc->type==ASSERT)
      learnt_expr=and_exprt(learnt_expr, it->pc->guard);
    else if(it->pc->type==ASSUME)
      learnt_expr=or_exprt(learnt_expr, it->pc->guard);
    else if(it->pc->type==GOTO)
    {
      codet code=it->pc->code;
      code.set_statement(ID_assume);
      code.operands().push_back(it->pc->guard);
      if(it->if_branch) code.op0().make_not();
      learnt_expr=wp(code, learnt_expr, ns);
      exprt tmp(it->pc->guard);
      if(it->if_branch) tmp.make_not();
      learnt_expr=and_exprt(learnt_expr, tmp);
      //learnt_expr=tmp; //implies_exprt(tmp, learnt_expr);
    }
    else if(it->pc->type==ASSIGN)
    {
      learnt_expr=wp(it->pc->code, learnt_expr, ns);
      //std::cout << "\n***4 " << from_expr(learnt_expr)
      //		  << it->pc->source_location << "\n\n";
    }

    //std::cout << "#" << it->pc->incoming_edges.size() << "\n";
    if(it->pc->incoming_edges.size()>1)// && it->pc->type!=GOTO)
    {
      bool backwards_loop=false;
      for(auto &in: it->pc->incoming_edges)
        if(it->pc->location_number<in->location_number)
        {
          backwards_loop=true;
          break;
        }
      if(backwards_loop)
      {
        std::cout << "backwards loop: " << it->pc->source_location << "\n";
        continue;
      }
      std::cout << "Multiple inbound edges: " << it->pc->source_location << "\n";
      std::cout << "***1 " << from_expr(learnt_map[*it]) << "\n";
      std::cout << "***2 " << from_expr(learnt_expr) << "\n";
      learnt_map[*it]=or_exprt(learnt_map[*it], learnt_expr);
      std::cout << "***3 " << from_expr(learnt_expr) << "\n\n";
      //state.rename(learnt_map[*it], ns);
      do_simplify(learnt_map[*it]);
    }
    do_simplify(learnt_expr);
  }
}

void symex_bmc_clusteringt::mock_step(
  statet &state,
  const goto_functionst &goto_functions)
{
  symex_step(goto_functions, state);
}

void symex_bmc_clusteringt::mock_reach(
  statet &state,
  const goto_functionst &goto_functions)
{
  if(!state.guard.is_false())
  {
    //std::string msg=id2string(state.source.pc->source_location.get_comment());
	//if(msg=="")
    //  msg="mock reach: ";
    exprt tmp=false_exprt();

    clean_expr(tmp, state, false);
    vcc(tmp, "mock reach", state);

    //std::cout << "mock reachability: " << state.source.pc->source_location << "\n";
  }

  if(learning_symex)
    add_learnt_info(state, goto_functions);
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

    std::cout << "mock goto-if condition: " << from_expr(tmp) << "\n";
  }

  if(learning_symex)
    add_learnt_info(state, goto_functions);
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
  symex_guard_goto(state, false_exprt());
  assert(state.locations.back().pc->type==GOTO);
  state.locations.back().if_branch=true;
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

    std::cout << "mock goto-else condition: " << from_expr(tmp) << "\n";
  }
  if(learning_symex)
    add_learnt_info(state, goto_functions);
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

  symex_guard_goto(state, true_exprt());
  assert(state.locations.back().pc->type==GOTO);
  state.locations.back().if_branch=false;
}

void symex_bmc_clusteringt::record(statet &state)
{
  std::string id="state"+std::to_string(counter);
  state.id=id;
  ++counter;
}

void symex_bmc_clusteringt::create_a_cluster(statet &state, symex_targett &equation)
{
  record(state);
  //clusters.insert(std::pair<std::string, statet>(state.id, state));
  clusters[state.id]=state;
  std::cout << "***state address: " << state.symex_target << "\n";
  std::cout << "***state address: " << cluster(state).symex_target << "\n";
  //assert(!(&state==&cluster(state)));
  clusters[state.id].symex_target=&equation;
  std::cout << ">>>> state address: " << state.symex_target << "\n";
  std::cout << ">>>> state address: " << cluster(state).symex_target << "\n";
}

goto_symext::statet& symex_bmc_clusteringt::cluster(const std::string &id)
{
  assert(clusters.find(id)!=clusters.end());
  return clusters[id];
}

goto_symext::statet& symex_bmc_clusteringt::cluster(const statet &state)
{
  return cluster(state.id);
}

bool symex_bmc_clusteringt::lotto() const
{
  srand (time(NULL));

  int a=rand()%100+1;

  int b=rand()%a;

  return (b>a/2);
}

void symex_bmc_clusteringt::do_nothing(
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

  //symex_goto(state);
  //symex_goto(cluster(state));
  symex_end_of_function(state);
  symex_transition(state);
}

void symex_bmc_clusteringt::symex_guard_goto(statet &state, const exprt &guard)
{
  const goto_programt::instructiont &instruction=*state.source.pc;
  statet::framet &frame=state.top();

  exprt old_guard=guard;
  clean_expr(old_guard, state, false);

  exprt new_guard=old_guard;
  state.rename(new_guard, ns);
  do_simplify(new_guard);

  if(new_guard.is_false() ||
     state.guard.is_false())
  {
    if(!state.guard.is_false())
    {
      //target.location(state.guard.as_expr(), state.source);
      state.symex_target->location(state.guard.as_expr(), state.source);
    }
    symex_transition(state);
    //goto_programt::const_targett goto_target=
    //  instruction.get_target();
    //symex_transition(state, goto_target, true);


    return; // nothing to do
  }

  //target.goto_instruction(state.guard.as_expr(), new_guard, state.source);
  state.symex_target->goto_instruction(state.guard.as_expr(), new_guard, state.source);

  assert(!instruction.targets.empty());

  // we only do deterministic gotos for now
  if(instruction.targets.size()!=1)
    throw "no support for non-deterministic gotos";

  goto_programt::const_targett goto_target=
    instruction.get_target();

  bool forward=!instruction.is_backwards_goto();
  if(!forward) // backwards?
  {
    // is it label: goto label; or while(cond); - popular in SV-COMP
    if(goto_target==state.source.pc ||
       (instruction.incoming_edges.size()==1 &&
        *instruction.incoming_edges.begin()==goto_target))
    {
      // generate assume(false) or a suitable negation if this
      // instruction is a conditional goto
      exprt negated_cond;

      if(new_guard.is_true())
        negated_cond=false_exprt();
      else
        negated_cond=not_exprt(new_guard);

      symex_assume(state, negated_cond);

      // next instruction
      symex_transition(state);
      //symex_transition(state, goto_target, true);
      return;
    }

    unsigned &unwind=
      frame.loop_iterations[goto_programt::loop_id(state.source.pc)].count;
    unwind++;

    // continue unwinding?
    if(get_unwind(state.source, unwind))
    {
      loop_bound_exceeded(state, new_guard);

      // next instruction
      symex_transition(state);
      //symex_transition(state, goto_target, true);
      return;
    }

    if(new_guard.is_true())
    {
      symex_transition(state, goto_target, true);
      return; // nothing else to do
    }
  }

  goto_programt::const_targett new_state_pc, state_pc;
  symex_targett::sourcet original_source=state.source;

  if(forward)
  {
    new_state_pc=goto_target;
    state_pc=state.source.pc;
    state_pc++;
    //state.state_pc++; //customized pc

    // skip dead instructions
    if(new_guard.is_true())
      while(state_pc!=goto_target && !state_pc->is_target())
      {
        ++state_pc;
        //++state.state_pc; //customized pc
      }

    if(state_pc==goto_target)
    {
      symex_transition(state, goto_target);
      return; // nothing else to do
    }
  }
  else
  {
    new_state_pc=state.source.pc;
    new_state_pc++;
    //state.state_pc++; // customized pc
    state_pc=goto_target;
  }

  // put into state-queue
  statet::goto_state_listt &goto_state_list=
    state.top().goto_state_map[new_state_pc];

  goto_state_list.push_back(statet::goto_statet(state));
  statet::goto_statet &new_state=goto_state_list.back();

  symex_transition(state, state_pc, !forward);
  //symex_transition(state, goto_target, true);

  // adjust guards
  if(new_guard.is_true())
  {
    state.guard.make_false();
  }
  else
  {
    // produce new guard symbol
    exprt guard_expr;

    if(new_guard.id()==ID_symbol ||
       (new_guard.id()==ID_not &&
        new_guard.operands().size()==1 &&
        new_guard.op0().id()==ID_symbol))
      guard_expr=new_guard;
    else
    {
      symbol_exprt guard_symbol_expr=
        symbol_exprt(guard_identifier, bool_typet());
      exprt new_rhs=new_guard;
      new_rhs.make_not();

      ssa_exprt new_lhs(guard_symbol_expr);
      state.rename(new_lhs, ns, goto_symex_statet::L1);
      state.assignment(new_lhs, new_rhs, ns, true, false);

      guardt guard;

      //target.assignment(
      //  guard.as_expr(),
      //  new_lhs, new_lhs, guard_symbol_expr,
      //  new_rhs,
      //  original_source,
      //  symex_targett::assignment_typet::GUARD);
      state.symex_target->assignment(
        guard.as_expr(),
        new_lhs, new_lhs, guard_symbol_expr,
        new_rhs,
        original_source,
        symex_targett::assignment_typet::GUARD);

      guard_expr=guard_symbol_expr;
      guard_expr.make_not();
      state.rename(guard_expr, ns);
    }

    if(forward)
    {
      new_state.guard.add(guard_expr);
      guard_expr.make_not();
      state.guard.add(guard_expr);
    }
    else
    {
      state.guard.add(guard_expr);
      guard_expr.make_not();
      new_state.guard.add(guard_expr);
    }
  }
}
