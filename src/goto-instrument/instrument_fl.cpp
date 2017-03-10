/*******************************************************************\

Module: Coverage Instrumentation

Author: Daniel Kroening

Date: May 2016

\*******************************************************************/

#include <algorithm>
#include <iterator>

#include <util/prefix.h>
#include <util/i2string.h>
#include <util/expr_util.h>

#include "instrument_fl.h"
#include "cover.h"
#include <iostream>

void instrument_dummy_block(
  const symbol_tablet &symbol_table,
  goto_programt &goto_program)
{
  const namespacet ns(symbol_table);

  // ignore if built-in library
  if(!goto_program.instructions.empty() &&
     has_prefix(id2string(goto_program.instructions.front().source_location.get_file()),
                "<builtin-library-"))
    return;

  Forall_goto_program_instructions(i_it, goto_program)
  {
	  std::cout << "i_it-->source-location: " << i_it->source_location << "\n\n";

goto_programt::instructiont w=*i_it;
goto_program.insert_before_swap(i_it);
//i_it->make_assignment();
i_it->make_skip();
i_it->source_location=w.source_location;
i_it->source_location.set_comment("__CPROVER_dummy");


std::cout << "w-->source-location: " << i_it->source_location << ", " << i_it->source_location.get_comment() << "\n\n";

	  break;
  }
}

void collect_decisions_rec2(const exprt &src, std::set<exprt> &dest)
{
  if(src.id()==ID_address_of)
  {
    return;
  }

  if(src.type().id()==ID_bool)
  {
    if(src.is_constant())
    {
      // ignore me
    }
    else if(src.id()==ID_not)
    {
      collect_decisions_rec2(src.op0(), dest);
    }
    else
    {
      dest.insert(src);
    }
  }
  else
  {
    for(const auto & op : src.operands())
      collect_decisions_rec2(op, dest);
  }
}

void collect_decisions_rec2(
  const goto_programt::const_targett t,
  std::set<exprt> &dest)
{
  if(t->is_assert()) collect_decisions_rec2(t->guard, dest);
  else if(t->is_function_call()) collect_decisions_rec2(t->code, dest);
  else assert(0);
}

void instrument_fl(
  const symbol_tablet &symbol_table,
  goto_programt &goto_program,
  const bool svcomp)
{
  const namespacet ns(symbol_table);

  // ignore if built-in library
  if(!goto_program.instructions.empty() &&
     has_prefix(id2string(goto_program.instructions.front().source_location.get_file()),
                "<builtin-library-"))
    return;

  Forall_goto_program_instructions(i_it, goto_program)
  {
	  if(i_it->is_function_call() && svcomp)
	  {
		  const code_function_callt &code_function_call=
		    to_code_function_call(i_it->code);
		  std::string func_name=to_symbol_expr(
		    code_function_call.function()).get_identifier().c_str();
		  if (!(func_name==std::string("__VERIFIER_assert")))
			  continue;
	  }
	  else if (i_it->is_assert())
	  {/**
		  if(i_it->function==std::string("__VERIFIER_assert")&&svcomp)
		  {
		    i_it->make_skip();
		    continue;
		  }
		  **/
	  }
	  else continue;

	  std::set<exprt> dest;
	  collect_decisions_rec2(i_it, dest);
	  if(dest.empty()) continue;
	  exprt e=*dest.begin();
	  std::string desc=i_it->source_location.as_string();
	  // add the failing assertion
      goto_program.insert_before_swap(i_it);
      i_it->make_assertion(e);
      i_it->source_location.set_comment("failing assertion: " + desc);

      // add the passing assertion
      goto_program.insert_before_swap(i_it);
      i_it->make_assertion(not_exprt(e));
      i_it->source_location.set_comment("passing assertion: " + desc);
      i_it++;
      i_it++;
      i_it->make_skip();
  }
}

void instrument_fl(
  const symbol_tablet &symbol_table,
  goto_functionst &goto_functions,
  const bool svcomp)
{
  Forall_goto_functions(f_it, goto_functions)
  {
    if(f_it->first==ID__start ||
       f_it->first=="__CPROVER_initialize")
      continue;

    instrument_fl(symbol_table, f_it->second.body, svcomp);
    if(f_it->first=="main")
        instrument_dummy_block(symbol_table, f_it->second.body);

  }
}
