/*******************************************************************\

Module: Fault Localization

Author: Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_CBMC_FAULT_LOCALIZATION_H
#define CPROVER_CBMC_FAULT_LOCALIZATION_H

#include <util/namespace.h>
#include <util/options.h>
#include <util/threeval.h>
#include <langapi/language_ui.h>

#include <goto-symex/symex_target_equation.h>

#include "bmc.h"
#include "all_properties_class.h"

class fault_localizationt:
  public bmc_all_propertiest
{
public:
  explicit fault_localizationt(
    const goto_functionst &_goto_functions,
    bmct &_bmc,
    const optionst &_options)
    :
    bmc_all_propertiest(_goto_functions, _bmc.prop_conv, _bmc),
    goto_functions(_goto_functions),
    bmc(_bmc),
    options(_options)
  {
    set_message_handler(bmc.get_message_handler());
    if(options.get_option("localize-faults-trace-limit")!="")
    {
    	trace_limit=atoi(options.get_option("localize-faults-trace-limit").c_str());
    }
    else trace_limit=-1;
    status() << eom << "<<<<< trace limit: " << trace_limit << eom << eom;
  }

  safety_checkert::resultt operator()();
  safety_checkert::resultt stop_on_fail();

  //override bmc_all_propertiest
  virtual void goal_covered(const cover_goalst::goalt &);

protected:
  const goto_functionst &goto_functions;
  bmct &bmc;
  const optionst &options;

  // the failed property
  symex_target_equationt::SSA_stepst::const_iterator failed;

  // the list of localization points up to the failed property
  struct lpointt {
    goto_programt::const_targett target;
    std::string info;
    double score;
  };
  typedef std::map<literalt, lpointt> lpointst;
  typedef std::map<irep_idt, lpointst> lpoints_mapt;
  lpoints_mapt lpoints_map;

  // this does the actual work
  void run(irep_idt goal_id);

  // this collects the guards as lpoints
  void collect_guards(lpointst &lpoints);
  void freeze_guards();

  // specify an lpoint combination to check
  typedef std::vector<tvt> lpoints_valuet;
  bool check(const lpointst &lpoints, const lpoints_valuet& value);
  void update_scores(lpointst &lpoints,
                     const lpoints_valuet& value);


  // localization method: flip each point
  void localize_linear(lpointst &lpoints);


  std::vector<lpoints_valuet> f_values, p_values, s_values;
  std::vector<lpoints_valuet> f_extra_values, p_extra_values;
  int trace_limit;
  std::vector<lpoints_valuet> F_values, P_values;
  int covered=0;
  irep_idt failed_goal;


  std::vector<int> ef, ep, nf, np;
  void compute_spectra(const lpointst& lpoints);

  lpointst cleaned_lpoints;
  void clean_traces(const lpointst &lpoints);


  // sb: single bug optimal
  std::vector<lpointt> sb_lpoints;
  void measure_sb(const lpointst& lpoints);
  // PFL
  std::vector<double> ppv;
  void compute_ppv(const lpointst &lpoints);
  std::vector<lpointt> pfl_lpoints;
  void pfl(const lpointst& lpoints);



  bool mc(const lpointst &lpoints,
    	  const literalt &property,
    	  const std::vector<lpoints_valuet>& ex,
  		  lpoints_valuet &res);
  bool mc2(const lpointst &lpoints,
    	  const literalt &property,
    	  const std::vector<lpoints_valuet>& ex1,
    	  const std::vector<lpoints_valuet>& ex2,
  		  lpoints_valuet &res);
  bool mc(const lpointst &lpoints,
    	  const literalt &property,
    	  const lpoints_valuet & inc,
  		  lpoints_valuet &res);
  void common(const std::vector<lpoints_valuet> &vs, lpoints_valuet& v);


  // localization method: TBD
  //void localize_TBD(
  //  prop_convt &prop_conv);

  symex_target_equationt::SSA_stepst::const_iterator get_failed_property();

  decision_proceduret::resultt
    run_decision_procedure(prop_convt &prop_conv);

  void report(irep_idt goal_id);

  //override bmc_all_propertiest
  virtual void report(const cover_goalst &cover_goals);

  //override bmc_all_propertiest
  virtual void do_before_solving()
  {
    freeze_guards();
  }
};

#endif // CPROVER_CBMC_FAULT_LOCALIZATION_H
