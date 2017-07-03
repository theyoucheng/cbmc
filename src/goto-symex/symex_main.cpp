/*******************************************************************\

Module: Symbolic Execution

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

/// \file
/// Symbolic Execution

#include <cassert>

#include <util/std_expr.h>
#include <util/rename.h>
#include <util/symbol_table.h>
#include <util/replace_symbol.h>

#include <analyses/dirty.h>

#include "goto_symex.h"
#include <iostream>

void goto_symext::new_name(symbolt &symbol)
{
  get_new_name(symbol, ns);
  new_symbol_table.add(symbol);
}

void goto_symext::vcc(
  const exprt &vcc_expr,
  const std::string &msg,
  statet &state)
{
  total_vccs++;

  exprt expr=vcc_expr;

  // we are willing to re-write some quantified expressions
  rewrite_quantifiers(expr, state);

  // now rename, enables propagation
  state.rename(expr, ns);

  // now try simplifier on it
  do_simplify(expr);

  if(expr.is_true())
    return;

  state.guard.guard_expr(expr);

  remaining_vccs++;
  target.assertion(state.guard.as_expr(), expr, msg, state.source);
}

void goto_symext::symex_assume(statet &state, const exprt &cond)
{
  exprt simplified_cond=cond;

  do_simplify(simplified_cond);

  if(simplified_cond.is_true())
    return;

  if(state.threads.size()==1)
  {
    exprt tmp=simplified_cond;
    state.guard.guard_expr(tmp);
    target.assumption(state.guard.as_expr(), tmp, state.source);
  }
  // symex_target_equationt::convert_assertions would fail to
  // consider assumptions of threads that have a thread-id above that
  // of the thread containing the assertion:
  // T0                     T1
  // x=0;                   assume(x==1);
  // assert(x!=42);         x=42;
  else
    state.guard.add(simplified_cond);

  if(state.atomic_section_id!=0 &&
     state.guard.is_false())
    symex_atomic_end(state);
}

void goto_symext::rewrite_quantifiers(exprt &expr, statet &state)
{
  if(expr.id()==ID_forall)
  {
    // forall X. P -> P
    // we keep the quantified variable unique by means of L2 renaming
    assert(expr.operands().size()==2);
    assert(expr.op0().id()==ID_symbol);
    symbol_exprt tmp0=
      to_symbol_expr(to_ssa_expr(expr.op0()).get_original_expr());
    symex_decl(state, tmp0);
    exprt tmp=expr.op1();
    expr.swap(tmp);
  }
}

/// symex from given state
void goto_symext::operator()(
  statet &state,
  const goto_functionst &goto_functions,
  const goto_programt &goto_program)
{
  assert(!goto_program.instructions.empty());

  state.source=symex_targett::sourcet(goto_program);
  assert(!state.threads.empty());
  assert(!state.call_stack().empty());
  state.top().end_of_function=--goto_program.instructions.end();
  state.top().calling_location.pc=state.top().end_of_function;
  state.symex_target=&target;
  state.dirty=new dirtyt(goto_functions);

  assert(state.top().end_of_function->is_end_function());

  guard_skip=false;
  while(!state.call_stack().empty())
  {
    symex_step(goto_functions, state);
    if(guard_skip)
    {
      std::cout << "&&& " << from_expr(state.guard) << "\n\n";
      lotto_guards.insert(state.guard);
#if 0
      std::cout << "\nCurrent lotto guards\n";
      for(auto &g: lotto_guards)
        std::cout << from_expr(g) << "## ";
      std::cout << "\n";
#endif
    }
#if 1
    guard_skip=false;
    lotto++;
    if(state.source.pc->type==GOTO)
    {
      //if(!state.guard.is_true() && !state.guard.is_false())
      {

        {
          srand(time(NULL));
          int rnd=rand()%lotto;
          if(rnd>lotto/2)
          {
            if(state.source.pc->function=="main")
            {
              if(!lotto_find(state.guard))
              {
                guard_skip=true;
                std::cout << "#### goto " << state.source.pc->source_location
              		  << from_expr(state.guard) << "\n";
              }

            }
          }
        }
      }
    }
#endif
    all_guards.insert(state.guard);

    // is there another thread to execute?
    if(state.call_stack().empty() &&
       state.source.thread_nr+1<state.threads.size())
    {
      unsigned t=state.source.thread_nr+1;
      // std::cout << "********* Now executing thread " << t << '\n';
      state.switch_to_thread(t);
    }
  }

  delete state.dirty;
  state.dirty=0;
}

/// symex starting from given program
void goto_symext::operator()(
  const goto_functionst &goto_functions,
  const goto_programt &goto_program)
{
  statet state;
  operator() (state, goto_functions, goto_program);
}

/// symex from entry point
void goto_symext::operator()(const goto_functionst &goto_functions)
{
  goto_functionst::function_mapt::const_iterator it=
    goto_functions.function_map.find(goto_functionst::entry_point());

  if(it==goto_functions.function_map.end())
    throw "the program has no entry point";

  const goto_programt &body=it->second.body;

  operator()(goto_functions, body);
}

bool goto_symext::lotto_find(const guardt &guard)
{
  for(auto &g: lotto_guards)
  {
	if(g==guard) return true;
    if(guard.operands().size()<g.operands().size())
      continue;
    if(guard.operands().empty() || g.operands().empty())
      continue;
    bool found=true;
    auto it1=g.operands().begin();
    auto it2=guard.operands().begin();
    while(it1!=g.operands().end())
    {
      if(!(*it1==*it2))
      {
        found=false;
        break;
      }
      ++it1;
      ++it2;
    }
    if(found) return true;
  }
  return false;
}

/// do just one step
void goto_symext::symex_step(
  const goto_functionst &goto_functions,
  statet &state)
{
  #if 0
  //if(state.source.pc->type==GOTO)
  {
  std::cout << "\ninstruction type is " << state.source.pc->type << '\n';
  std::cout << "Location: " << state.source.pc->source_location << '\n';
  std::cout << "Guard: " << from_expr(ns, "", state.guard.as_expr()) << '\n';
  std::cout << "Code: " << from_expr(ns, "", state.source.pc->code) << '\n';
  }
  #endif

  assert(!state.threads.empty());
  assert(!state.call_stack().empty());

  const goto_programt::instructiont &instruction=*state.source.pc;

  merge_gotos(state);


  // depth exceeded?
  {
    unsigned max_depth=options.get_unsigned_int_option("depth");
    if(max_depth!=0 && state.depth>max_depth)
      state.guard.add(false_exprt());
    state.depth++;
  }

  if(0) //guard_skip)
  {
    std::cout << "\n@@@@\n";
    std::cout << "Guard: " << from_expr(ns, "", state.guard.as_expr()) << '\n';
	std::cout << "type: " << instruction.type << "\n";
    std::cout << "Location: " << state.source.pc->source_location << "\n\n";
  }


  if(lotto_find(state.guard) && state.source.pc->function=="main")
  {
#if 0
	std::cout << "\n@@@@\n";
	std::cout << "Guard: " << from_expr(ns, "", state.guard.as_expr()) << '\n';
	std::cout << "type: " << instruction.type << "\n";
	std::cout << "Location: " << state.source.pc->source_location << "\n\n";
#endif
	if(instruction.type==GOTO)
	{
	  std::cout << "**chosen goto: " << state.source.pc->source_location
			    << ", " << from_expr(state.guard) << "\n";
      if(!state.guard.is_false())
      {
        exprt tmp=instruction.guard;
        tmp.make_true();
        clean_expr(tmp, state, false);
        state.rename(tmp, ns);
        symex_assume(state, tmp);
      }
      state.source.pc++;
      return;
	}
	else if (instruction.type==ASSIGN)
	{
      std::cout << "**chosen assignment: " << state.source.pc->source_location
    		  << ", " << from_expr(state.guard) << "\n";
	  if(!state.guard.is_false())
	  {
	    exprt tmp=instruction.guard;
	    clean_expr(tmp, state, false);
	    state.rename(tmp, ns);
	    symex_assume(state, tmp);
	  }
	  state.source.pc++;
	  return;
	}
  }
  // actually do instruction
  switch(instruction.type)
  {
  case SKIP:
    if(!state.guard.is_false())
      target.location(state.guard.as_expr(), state.source);
    state.source.pc++;
    break;

  case END_FUNCTION:
    // do even if state.guard.is_false() to clear out frame created
    // in symex_start_thread
    symex_end_of_function(state);
    state.source.pc++;
    break;

  case LOCATION:
    if(!state.guard.is_false())
      target.location(state.guard.as_expr(), state.source);
    state.source.pc++;
    break;

  case GOTO:
    symex_goto(state);
    break;

  case ASSUME:
    if(!state.guard.is_false())
    {
      exprt tmp=instruction.guard;
      clean_expr(tmp, state, false);
      state.rename(tmp, ns);
      symex_assume(state, tmp);
    }

    state.source.pc++;
    break;

  case ASSERT:
    if(!state.guard.is_false())
    {
      std::string msg=id2string(state.source.pc->source_location.get_comment());
      if(msg=="")
        msg="assertion";
      exprt tmp(instruction.guard);
      clean_expr(tmp, state, false);
      vcc(tmp, msg, state);
    }

    state.source.pc++;
    break;

  case RETURN:
    if(!state.guard.is_false())
      return_assignment(state);

    state.source.pc++;
    break;

  case ASSIGN:
    if(!state.guard.is_false())
      symex_assign_rec(state, to_code_assign(instruction.code));

    state.source.pc++;
    break;

  case FUNCTION_CALL:
    if(!state.guard.is_false())
    {
      code_function_callt deref_code=
        to_code_function_call(instruction.code);

      if(deref_code.lhs().is_not_nil())
        clean_expr(deref_code.lhs(), state, true);

      clean_expr(deref_code.function(), state, false);

      Forall_expr(it, deref_code.arguments())
        clean_expr(*it, state, false);

      symex_function_call(goto_functions, state, deref_code);
    }
    else
      state.source.pc++;
    break;

  case OTHER:
    if(!state.guard.is_false())
      symex_other(goto_functions, state);

    state.source.pc++;
    break;

  case DECL:
    if(!state.guard.is_false())
      symex_decl(state);

    state.source.pc++;
    break;

  case DEAD:
    symex_dead(state);
    state.source.pc++;
    break;

  case START_THREAD:
    symex_start_thread(state);
    state.source.pc++;
    break;

  case END_THREAD:
    // behaves like assume(0);
    if(!state.guard.is_false())
      state.guard.add(false_exprt());
    state.source.pc++;
    break;

  case ATOMIC_BEGIN:
    symex_atomic_begin(state);
    state.source.pc++;
    break;

  case ATOMIC_END:
    symex_atomic_end(state);
    state.source.pc++;
    break;

  case CATCH:
    symex_catch(state);
    state.source.pc++;
    break;

  case THROW:
    symex_throw(state);
    state.source.pc++;
    break;

  case NO_INSTRUCTION_TYPE:
    throw "symex got NO_INSTRUCTION";

  default:
    throw "symex got unexpected instruction";
  }
}
