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
  }

  safety_checkert::resultt operator()();
  safety_checkert::resultt stop_on_fail(bool show_report=true);

  // override bmc_all_propertiest
  virtual void goal_covered(const cover_goalst::goalt &);

protected:
  const goto_functionst &goto_functions;
  bmct &bmc;
  const optionst &options;

  // the failed property
  symex_target_equationt::SSA_stepst::const_iterator failed;

  // the list of localization points up to the failed property
  struct lpointt
  {
    goto_programt::const_targett target;
    double score;
    std::set<source_locationt> lines;
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
  void update_scores(
    lpointst &lpoints,
    const lpoints_valuet& value);

  // localization method: flip each point
  void localize_linear(lpointst &lpoints);

  // localization method: probabilistic fault localization
  void localize_probabilistic(lpointst &lpoints);

  struct pflt
  {
	int max_traces=0;
    literalt activation_literal;
    const literalt &property; // the failed cond
    lpointst &P; // this is the program under analysis
    bmct &bmc;
    const std::string &type;
    pflt(
      lpointst &P_,
	  bmct &bmc_,
	  const literalt &property_,
	  const std::string &type_)
      : property(property_), P(P_), bmc(bmc_), type(type_)
    {
      assert(type=="pfl" || type=="sbo");
    }
    std::vector<lpoints_valuet> failing_traces, passing_traces;

    // The "mc" function  in David Landsberg's probabilistic fault
    // localization algoritam. "mc" tries to find a trace that
    // violates "phi", which does not cover any block in "X"
    // but covers every block in "Y". The resulting trace will be
    // inserted into "traces".
    bool mc(
      const literalt &phi,
      const lpoints_valuet &X,
      const lpoints_valuet &Y,
      std::vector<lpoints_valuet> &traces);

    lpoints_valuet common(const std::vector<lpoints_valuet> &lvs);
    lpoints_valuet complement(
      const lpoints_valuet &v1,
      const lpoints_valuet &v2);
    bool empty(const lpoints_valuet &v);
    void operator()();
    bool get_a_trace(const literalt &assumption_, lpoints_valuet &trace);
    bool get_a_trace(
      const literalt &assumption_,
	  std::vector<lpoints_valuet> &traces);
    literalt trace_literal_and(const lpoints_valuet &trace);
    literalt trace_positive_literal_and(const lpoints_valuet &trace);
    bool trace_equal(const lpoints_valuet &v1, const lpoints_valuet &v2);
    bool in_traces(
      const lpoints_valuet &v,
	  const std::vector<lpoints_valuet> &traces);
    bool lpoint_equal(const lpointt &lp1, const lpointt &lp2);

    // a set of steps to compute the fault probability
    std::vector<int> ef, ep, nf, np;
    std::vector<double> ppv;
    // 1) to remove unnecessary blocks
    void merge_traces();
    void simplify_traces();
    void compute_spectra();
    void measure_sb();
    void compute_ppv();
    void compute_probability();
  };

  symex_target_equationt::SSA_stepst::const_iterator get_failed_property();

  decision_proceduret::resultt
    run_decision_procedure(prop_convt &prop_conv);

  void report(irep_idt goal_id);

  // override bmc_all_propertiest
  virtual void report(const cover_goalst &cover_goals);

  // override bmc_all_propertiest
  virtual void do_before_solving()
  {
    freeze_guards();
  }
};

#endif // CPROVER_CBMC_FAULT_LOCALIZATION_H
