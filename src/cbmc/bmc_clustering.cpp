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
#include <goto-programs/remove_returns.h>
#include <goto-programs/show_goto_functions.h>
#include <goto-instrument/dump_c.h>
#include <goto-instrument/unwind.h>


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

  unsigned c=0;

  while(!symex_state.call_stack().empty())
  {

    symex()(
      symex_state,
      goto_functions,
      goto_functions.function_map.at(goto_functions.entry_point()).body);

    if(symex_state.source.pc->type==GOTO)
    {
      //std::cout << "A: " << from_expr(symex_state.state_pc->guard);
      symex_state.state_pc=find_target(
        symex_state.source.pc,
	    symex_state.state_goto_functions);
      //std::cout << "B: " << from_expr(symex_state.state_pc->guard);
      bool is_loop=is_a_loop(symex_state.state_pc);
#if 0
      if(is_loop)
      {
        symex_state.state_goto_functions.update();
        show_goto_functions(
          ns,
          ui_message_handlert::uit::PLAIN,
	      symex_state.state_goto_functions);
        goto_functionst::function_mapt::iterator func_it=
          symex_state.state_goto_functions.function_map.find(symex_state.state_pc->function);
        goto_unwindt goto_unwind;
        goto_unwind.unwind(
          func_it->second.body,
          symex_state.state_pc,
          symex_state.state_pc->get_target(),
          1,
          goto_unwindt::unwind_strategyt::CONTINUE);
        std::cout << "after unwinding...\n";
        symex_state.state_pc->make_skip();
        symex_state.state_goto_functions.update();
        show_goto_functions(
          ns,
          ui_message_handlert::uit::PLAIN,
	      symex_state.state_goto_functions);
      }
#endif
      if(c++==10)
      {
        state_dump_c(symex_state);
      }

      if(reachable_if())
      {
        //std::cout << "C: " << from_expr(symex_state.state_pc->guard) << "\n";
        symex().add_goto_if_assumption(symex_state, goto_functions);
#if 0
        goto_programt::instructiont::targett i_it=symex_state.state_pc;
        goto_programt &state_goto_program=
          symex_state.state_goto_functions.function_map.find(
            symex_state.state_pc->function)->second.body;
        state_goto_program.insert_after(i_it);
        ++i_it;
        i_it->make_assumption(not_exprt(symex_state.state_pc->guard));
        i_it->source_location=symex_state.state_pc->source_location;
        i_it->source_location.set_comment("if branch: "+from_expr(i_it->guard));
#endif
        if(is_loop) //else
        {
          // unwind 1 more step
          symex_state.state_goto_functions.update();
          show_goto_functions(
            ns,
            ui_message_handlert::uit::PLAIN,
    	    symex_state.state_goto_functions);
          goto_functionst::function_mapt::iterator func_it=
            symex_state.state_goto_functions.function_map.find(symex_state.state_pc->function);
          goto_unwindt goto_unwind;
          goto_unwind.unwind(
            func_it->second.body,
            symex_state.state_pc,
            symex_state.state_pc->get_target(),
            1,
            goto_unwindt::unwind_strategyt::CONTINUE);
          //std::cout << "after unwinding...\n";
          //symex_state.state_pc->make_skip();
          //symex_state.state_goto_functions.update();
          //show_goto_functions(
          //  ns,
          //  ui_message_handlert::uit::PLAIN,
    	  //  symex_state.state_goto_functions);
        }
        goto_programt::instructiont::targett i_it=symex_state.state_pc;
        goto_programt &state_goto_program=
          symex_state.state_goto_functions.function_map.find(
            symex_state.state_pc->function)->second.body;
        state_goto_program.insert_after(i_it);
        ++i_it;
        i_it->make_assumption(not_exprt(symex_state.state_pc->guard));
        i_it->source_location=symex_state.state_pc->source_location;
        i_it->source_location.set_comment("if branch: "+from_expr(i_it->guard));
        symex_state.state_pc->make_skip();
        symex_state.state_goto_functions.update();
      }
      else if(reachable_else())
      {
        //symex_state.state_pc->guard=true_exprt();
        symex().add_goto_else_assumption(symex_state, goto_functions);
#if 1
        goto_programt::instructiont::targett i_it=symex_state.state_pc;
        goto_programt &state_goto_program=
          symex_state.state_goto_functions.function_map.find(
            symex_state.state_pc->function)->second.body;
        state_goto_program.insert_after(i_it);
        ++i_it;
        i_it->make_assumption(symex_state.state_pc->guard);
        i_it->source_location=symex_state.state_pc->source_location;
        i_it->source_location.set_comment("else branch: "+from_expr(i_it->guard));
        symex_state.state_pc->guard=true_exprt();
        i_it->swap(*symex_state.state_pc);
#endif
      }
      else assert(0);

    }

    statistics() << "size of program expression: "
                 << equation.SSA_steps.size()
                 << " steps" << eom;
    std::cout << "tot_vccs: " << symex().total_vccs << "\n";
  }

  //state_dump_c(symex_state);

  prop_conv.set_all_frozen();

  //symex_state=symex().cluster(symex_state);
  //equation=*(dynamic_cast<symex_target_equationt*>(symex().cluster(symex_state).symex_target));

  return all_properties(goto_functions, prop_conv);
#if 0
  decision_proceduret::resultt result=run_decision_procedure(prop_conv);
  if(result==decision_proceduret::resultt::D_SATISFIABLE)
    return safety_checkert::resultt::UNSAFE;
  else if(result==decision_proceduret::resultt::D_UNSATISFIABLE)
    return safety_checkert::resultt::SAFE;
  else
    return safety_checkert::resultt::ERROR;
#endif
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

  symex_state.initialize(goto_functions);
  update_goto_functions(symex_state.state_goto_functions);

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

bool bmc_clusteringt::reachable_if()
{

  std::cout << "\n++++++++++++++++++++ reachable if +++++++++++++++++\n";
  std::cout << symex_state.source.pc->source_location << "\n";
  std::cout << from_expr(symex_state.source.pc->code) << "\n";

  // make a snapshot
  goto_symext::statet backup_state=symex_state;
  auto tmp=equation;

  std::size_t num=equation.SSA_steps.size();
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

goto_programt::instructiont::targett bmc_clusteringt::find_target(
  goto_programt::instructionst::const_iterator target_it,
  goto_functionst &goto_functions)
{
  std::cout << "*** find target: " << target_it->source_location << "\n";
  //for(auto &x : goto_functions.function_map)
  //  std::cout << "++ " << x.first << "\n";
  goto_functionst::function_mapt::iterator func_it=
    goto_functions.function_map.find(target_it->function);
  //std::cout << "the function: " << func_it->first << "\n";

  goto_programt::instructiont ins;
  Forall_goto_program_instructions(it, func_it->second.body)
  //auto it=symex_state.state_pc;
  //++it;
  //for(; it!=func_it->second.body.instructions.end(); ++it)
  {
	//std::cout << from_expr(it->guard) << ", " << from_expr(target_it->guard) << "\n";
    if(target_it->code==it->code &&
      target_it->source_location==it->source_location &&
      target_it->type==it->type &&
      target_it->guard==it->guard)
    {
      return it;
      break;
    }
  }
  assert(0);
}

void bmc_clusteringt::state_dump_c(goto_symext::statet &state)
{
  std::cout << "\n***to dump c***\n";
  //const bool is_cpp=false; //cmdline.isset("dump-cpp");
  //const bool h_libc=true; //!cmdline.isset("no-system-headers");
  //const bool h_all=false; //cmdline.isset("use-all-headers");
  //namespacet ns(symbol_table);

  // restore RETURN instructions in case remove_returns had been
  // applied
  symbol_tablet stable=ns.get_symbol_table();
  //goto_functionst gfunctions=state.state_goto_functions; //state.state_goto_functions;
  //restore_returns(stable, gfunctions);
  //update_goto_functions(gfunctions);
  //std::cout << "dumped c" << "\n";
  show_goto_functions(
              ns, //namespacet(symbol_table),
              ui_message_handlert::uit::PLAIN,
  			state.state_goto_functions);


  //goto_modelt goto_model;
  //goto_model.goto_functions=state.state_goto_functions;
  //update_goto_functions(goto_model.goto_functions);
  //goto_model.symbol_table=stable;
  //remove_returns(goto_model);

  std::ofstream out("state_dump_c.c");
  if(!out)
  {
    error() << "failed to write to `" << "state_dump_c.c" << "'";
    return; //throw 10; //return 10;
  }
  (dump_c)(state.state_goto_functions, false, false, ns, out);
  //  else
  //    (is_cpp ? dump_cpp : dump_c)(
  //      goto_functions, h_libc, h_all, ns, std::cout);
  std::cout << "\n***after dump c***\n";

}

void bmc_clusteringt::update_goto_functions(goto_functionst &_goto_functions)
{
  for(auto &x: _goto_functions.function_map)
  {
    Forall_goto_program_instructions(it, x.second.body)
	{
	  if(it->targets.empty()) continue;
	  if(!it->is_goto()) continue;
	  // there is only one target
	  bool found=false;
	  goto_programt::instructionst::iterator target=it->targets.front();
	  Forall_goto_program_instructions(jt, x.second.body)
	  {
	    if(target->code==jt->code &&
	      target->source_location==jt->source_location &&
		  target->type==jt->type &&
		  target->guard==jt->guard)
	    {
	      it->set_target(jt);
	      found=true;
	      break;
	    }
	  }
	  if(!found)
	  {
		std::cout << "***Not found...\n";
	    it->make_skip();
	  }
	}
  }
  //_goto_functions.update();
}
#
bool bmc_clusteringt::is_a_loop(const goto_programt::targett t1)
{
  if(!t1->is_goto()) return false;
  goto_functionst::function_mapt::const_iterator func_it=
      goto_functions.function_map.find(t1->function);
  forall_goto_program_instructions(it, func_it->second.body)
  {
    if(it->is_backwards_goto())
    {
      std::cout << "There is one backwards loop\n";
      std::cout << it->source_location << "\n";
      std::cout << it->get_target()->source_location << "\n";

      std::cout << t1->source_location << "\n";

      if(it->get_target()->source_location==t1->source_location)
        return true;
    }
  }
  return false;
}

