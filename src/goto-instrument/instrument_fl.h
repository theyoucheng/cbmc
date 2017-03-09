#ifndef CPROVER_GOTO_INSTRUMENT_FL_H
#define CPROVER_GOTO_INSTRUMENT_FL_H

#include <goto-programs/goto_model.h>


void instrument_fl(
  const symbol_tablet &symbol_table,
  goto_programt &goto_program,
  const bool svcomp);

void instrument_fl(
  const symbol_tablet &symbol_table,
  goto_functionst &goto_functions,
  const bool svcomp);

#endif // CPROVER_GOTO_INSTRUMENT_COVER_H
