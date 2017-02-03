/*******************************************************************\

Module: Coverage Instrumentation

Author: Daniel Kroening

Date: May 2016

\*******************************************************************/

#ifndef CPROVER_COVER_H
#define CPROVER_COVER_H

#include <goto-programs/goto_model.h>

enum class coverage_criteriont {
  LOCATION, BRANCH, DECISION, CONDITION,
  PATH, MCDC, ASSERTION, COVER, AUTOSAC, BOUNDARY};

void instrument_cover_goals(
  const symbol_tablet &symbol_table,
  goto_programt &goto_program,
  coverage_criteriont);

void instrument_cover_goals(
  const symbol_tablet &symbol_table,
  goto_functionst &goto_functions,
  coverage_criteriont, const double toler=0,
  const bool autosac_strong_in_type=false,
  const bool weakly_autosac_strong_in_type=false
  );

void collect_ite_rec(const exprt &e, 
                     const exprt &prior_constr,
                     std::set<exprt> &coll);

void collect_ite(const exprt &src, 
                 std::set<exprt> &coll);

void collect_tenary(const exprt &src,
                 std::set<exprt> &coll);

#endif
