/*******************************************************************\

Module: Requirement Specification Instrumentation

Author: Daniel Kroening

Date: May 2016

\*******************************************************************/

#ifndef CPROVER_GOTO_INSTRUMENT_RS_INSTRUMENTATION_H
#define CPROVER_GOTO_INSTRUMENT_RS_INSTRUMENTATION_H

#include <goto-programs/goto_model.h>

void instrument_requirements_specification(
  const symbol_tablet &symbol_table,
  goto_functionst &goto_functions);


#endif // CPROVER_GOTO_INSTRUMENT_RS_INSTRUMENTATION_H
