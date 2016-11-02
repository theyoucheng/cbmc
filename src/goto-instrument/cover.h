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
  PATH, MCDC, ASSERTION, COVER };

void instrument_cover_goals(
  const symbol_tablet &symbol_table,
  goto_programt &goto_program,
  coverage_criteriont);

void instrument_cover_goals(
  const symbol_tablet &symbol_table,
  goto_functionst &goto_functions,
  coverage_criteriont);

// functions for boundary values analysis 
std::set<exprt> non_ordered_expr_expansion(const exprt &src);
std::set<exprt> decision_expansion(const exprt &dec);
std::set<exprt> non_ordered_predicate_expansion(const exprt &src);
std::set<exprt> ordered_negation(const exprt &src);
bool is_arithmetic_predicate(const exprt &src);

// a set of help methods
void collect_operands(const exprt &src, std::vector<exprt> &dest);
std::set<exprt> replacement_and_conjunction(
  const std::set<exprt> &replacement_exprs,
  const std::vector<exprt> &operands,
  const std::size_t i);

#endif
