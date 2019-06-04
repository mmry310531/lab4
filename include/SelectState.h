#ifndef SELECT_STATE_H
#define SELECT_STATE_H

#include "Command.h"
#include "Table.h"
#include "User.h"

void field_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table);
void table_state_handler(Command_t *cmd, size_t arg_idx);
void offset_state_handler(Command_t *cmd, size_t arg_idx);
void limit_state_handler(Command_t *cmd, size_t arg_idx);
void where_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table);
void where_2(Command_t *cmd, size_t arg_idx, Table_t *table);
void where_3(Command_t *cmd, size_t arg_idx, Table_t *table);

#endif
