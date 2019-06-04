#include <string.h>
#include <stdlib.h>
#include "Command.h"
#include "SelectState.h"
#include "Table.h"
#include "User.h"

void field_state_handler(Command_t *cmd, size_t arg_idx, Table_t *table) {
    size_t dontcheck = 0;
    char *loc;
    cmd->cmd_args.sel_args.fields = NULL;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
    cmd->cmd_args.sel_args.And=0;
    cmd->cmd_args.sel_args.sum=0;
    cmd->cmd_args.sel_args.avg=0;
    cmd->cmd_args.sel_args.count=0;
    while(arg_idx < cmd->args_len) {
        if(!strncmp(cmd->args[arg_idx], "sum", 3)){
            loc = strstr(cmd->args[arg_idx], "id");
            if(loc!=NULL){
                add_select_field(cmd, "sum_id");
                cmd->cmd_args.sel_args.sum=1;
            } 
            loc = strstr(cmd->args[arg_idx], "age");
            if(loc!=NULL){
                add_select_field(cmd, "sum_age");
                cmd->cmd_args.sel_args.sum=4;
            }
        } else if(!strncmp(cmd->args[arg_idx], "avg", 3)){
            loc = strstr(cmd->args[arg_idx], "id");
            if(loc!=NULL){
                add_select_field(cmd, "avg_id");
                cmd->cmd_args.sel_args.avg=1;
            }  
            loc = strstr(cmd->args[arg_idx], "age");
            if(loc!=NULL){
                add_select_field(cmd, "avg_age");
                cmd->cmd_args.sel_args.avg=4;
            }
        } else if(!strncmp(cmd->args[arg_idx], "count", 5)){
            loc = strstr(cmd->args[arg_idx], "id");
            if(loc!=NULL){
                add_select_field(cmd, "count_id");
                cmd->cmd_args.sel_args.count=1;
            } 
            loc = strstr(cmd->args[arg_idx], "name");
            if(loc!=NULL){
                add_select_field(cmd, "count_name");
                cmd->cmd_args.sel_args.count=2;
            }
            loc = strstr(cmd->args[arg_idx], "email");
            if(loc!=NULL){
                add_select_field(cmd, "count_email");
                cmd->cmd_args.sel_args.count=3;
            }
            loc = strstr(cmd->args[arg_idx], "age");
            if(loc!=NULL){
                add_select_field(cmd, "count_age");
                cmd->cmd_args.sel_args.count=4;
            } else{
                add_select_field(cmd, "count");
                cmd->cmd_args.sel_args.count=5;
            }
        } else if (!strncmp(cmd->args[arg_idx], "*", 1) && dontcheck==0) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "id", 2) && dontcheck==0) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "name", 4) && dontcheck==0) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "email", 5) && dontcheck==0) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "age", 3) && dontcheck==0) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "where", 5) && dontcheck==0) {
            where_state_handler(cmd, arg_idx+1, table);
	    dontcheck = 1;
        } else if (!strncmp(cmd->args[arg_idx], "limit", 5) && dontcheck==0) {
            limit_state_handler(cmd, arg_idx+1);
        } else if (!strncmp(cmd->args[arg_idx], "offset", 6) && dontcheck==0) {
	    offset_state_handler(cmd, arg_idx+1);
	}
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}
/*
void table_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len
            && !strncmp(cmd->args[arg_idx], "table", 5)) {
        arg_idx++;
    }
    if (arg_idx == cmd->args_len) {
        return;
    } else if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
        offset_state_handler(cmd, arg_idx+1);
        return;
    } else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
        limit_state_handler(cmd, arg_idx+1);
        return;
    }
    cmd->type = UNRECOG_CMD;
    return;
}
*/
void offset_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {

            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}
void where_state_handler(Command_t *cmd, size_t arg_idx,Table_t *table) {
    size_t idx;
    size_t check=0;
    char *loc;
    char *temp;	
    char *char_temp;
    char *pre_str;
    char *aft_str;
    char *less="<";
    char *more=">";
    char *equal="=";
    char *nonequal="!=";
    char *less_equal="<=";
    char *more_equal=">=";
    User_t *user_temp;
    while(arg_idx < cmd->args_len){
	if (check==0){
		if(!strncmp(cmd->args[arg_idx], "<", 1) && strncmp(cmd->args[arg_idx], "<=", 2)) {
		    if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id >= atoi(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age >= atof(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id <= atoi(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age <= atof(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    }
		} else if(!strncmp(cmd->args[arg_idx], ">", 1) && strncmp(cmd->args[arg_idx], ">=", 2)) {///////////////////////////////////////////////////////////////////////////////////
		    if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id <= atoi(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age <= atof(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id >= atoi(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age >= atof(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    }    
		} else if(!strncmp(cmd->args[arg_idx], "=", 1)) {///////////////////////////////////////////////////////////////////////////////////////
		    if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id != atoi(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "name", 4)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
			    temp=user_temp->name;
		            if(strncmp(cmd->args[arg_idx+1], temp, 20))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "email", 5)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(strncmp(cmd->args[arg_idx+1], temp, 20))
		                user_temp->output = 0;
		        } 
		    } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age != atof(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id != atoi(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "name", 4)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->name;
		            if(strncmp(cmd->args[arg_idx-1], temp, 20))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "email", 5)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(strncmp(cmd->args[arg_idx-1], temp, 20))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age != atof(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    }
		} else if(!strncmp(cmd->args[arg_idx], "!=", 2)) {///////////////////////////////////////////////////////////////////////////////////////
		    if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id == atoi(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "name", 4)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->name;
		            if(!strncmp(cmd->args[arg_idx+1], temp, 20))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "email", 5)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(!strncmp(cmd->args[arg_idx+1], temp, 20))
		                user_temp->output = 0;
		        } 
		    } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age == atof(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id == atoi(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "name", 4)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->name;
		            if(!strncmp(cmd->args[arg_idx-1], temp, 20))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "email", 5)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(!strncmp(cmd->args[arg_idx-1], temp, 20))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age == atof(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    }
		} else if(!strncmp(cmd->args[arg_idx], "<=", 2)){///////////////////////////////////////////////////////////////////////////////////////
		    if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id > atoi(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age > atof(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id < atoi(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age < atof(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    }
		} else if(!strncmp(cmd->args[arg_idx], ">=", 2)){///////////////////////////////////////////////////////////////////////////////////
		    if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id < atoi(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age < atof(cmd->args[arg_idx+1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id > atoi(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {check=1;
		        for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age > atof(cmd->args[arg_idx-1]))
		                user_temp->output = 0;
		        }
		    }
		} else{
		    loc = strstr(cmd->args[arg_idx], more_equal);
		    if(loc!=NULL){
		        char_temp = cmd->args[arg_idx];
		        loc = strtok(char_temp,more_equal);
		        pre_str = loc;
		        loc = strtok(NULL,more_equal);
		        aft_str = loc;
		        if(!strncmp(pre_str, "id", 2)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id < atoi(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age < atof(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "id", 2)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id > atoi(pre_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age > atof(pre_str))
		                user_temp->output = 0;
		            }
		        }
		    }
		    loc = strstr(cmd->args[arg_idx], less_equal);
		    if(loc!=NULL){
		        char_temp = cmd->args[arg_idx];
		        loc = strtok(char_temp,less_equal);
		        pre_str = loc;
		        loc = strtok(NULL,less_equal);
		        aft_str = loc;
		        if(!strncmp(pre_str, "id", 2)){
		            for(idx=0 ; idx<table->len ; idx++){check=1;
		            user_temp = get_User(table, idx);
		            if(user_temp->id > atoi(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age > atof(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "id", 2)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id < atoi(pre_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age < atof(pre_str))
		                user_temp->output = 0;
		            }
		        }
		    }
		    loc = strstr(cmd->args[arg_idx], nonequal);
		    if(loc!=NULL){
		        char_temp = cmd->args[arg_idx];
		        loc = strtok(char_temp,nonequal);
		        pre_str = loc;
		        loc = strtok(NULL,nonequal);
		        aft_str = loc;
		        if(!strncmp(pre_str, "id", 2)){
		            for(idx=0 ; idx<table->len ; idx++){check=1;
		            user_temp = get_User(table, idx);
		            if(user_temp->id == atoi(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "name", 4)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
			    temp=user_temp->name;
		            if(!strncmp(aft_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "email", 5)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(!strncmp(aft_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age == atof(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "id", 2)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id == atoi(pre_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "name", 4)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->name;
		            if(!strncmp(pre_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "email", 5)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(!strncmp(pre_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age == atof(pre_str))
		                user_temp->output = 0;
		            }
		        }
		    }
		    loc = strstr(cmd->args[arg_idx], equal);
		    if(loc!=NULL){
		        char_temp = cmd->args[arg_idx];
		        loc = strtok(char_temp,equal);
		        pre_str = loc;
		        loc = strtok(NULL,equal);
		        aft_str = loc;
		        if(!strncmp(pre_str, "id", 2)){
		            for(idx=0 ; idx<table->len ; idx++){check=1;
		            user_temp = get_User(table, idx);
		            if(user_temp->id != atoi(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "name", 4)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->name;
		            if(strncmp(aft_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "email", 5)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(strncmp(aft_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age != atof(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "id", 2)){
		            for(idx=0 ; idx<table->len ; idx++){check=1;
		            user_temp = get_User(table, idx);
		            if(user_temp->id != atoi(pre_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "name", 4)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->name;
		            if(strncmp(pre_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "email", 5)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            temp=user_temp->email;
		            if(strncmp(pre_str, temp, 20))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age != atof(pre_str))
		                user_temp->output = 0;
		            }
		        }
		    }
		    loc = strstr(cmd->args[arg_idx], more);
		    if(loc!=NULL){
		        char_temp = cmd->args[arg_idx];
		        loc = strtok(char_temp,more);
		        pre_str = loc;
		        loc = strtok(NULL,more);
		        aft_str = loc;
		        if(!strncmp(pre_str, "id", 2)){
		            for(idx=0 ; idx<table->len ; idx++){check=1;
		            user_temp = get_User(table, idx);
		            if(user_temp->id <= atoi(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age <= atof(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "id", 2)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id >= atoi(pre_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age >= atof(pre_str))
		                user_temp->output = 0;
		            }
		        }
		    }
		    loc = strstr(cmd->args[arg_idx], less);
		    if(loc!=NULL){
		        char_temp = cmd->args[arg_idx];
		        loc = strtok(char_temp,less);
		        pre_str = loc;
		        loc = strtok(NULL,less);
		        aft_str = loc;
		        if(!strncmp(pre_str, "id", 2)){
		            for(idx=0 ; idx<table->len ; idx++){check=1;
		            user_temp = get_User(table, idx);
		            if(user_temp->id >= atoi(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(pre_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age >= atof(aft_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "id", 2)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->id <= atoi(pre_str))
		                user_temp->output = 0;
		            }
		        } else if(!strncmp(aft_str, "age", 3)){check=1;
		            for(idx=0 ; idx<table->len ; idx++){
		            user_temp = get_User(table, idx);
		            if(user_temp->age <= atof(pre_str))
		                user_temp->output = 0;
		            }
		        }
		    }
		}
	} else if(!strncmp(cmd->args[arg_idx], "or", 2)){
            cmd->cmd_args.sel_args.And=1;
            where_2(cmd, arg_idx+1, table);
        } else if(!strncmp(cmd->args[arg_idx], "and", 3)){
            cmd->cmd_args.sel_args.And=2;	
            where_2(cmd, arg_idx+1, table);
        } else if(!strncmp(cmd->args[arg_idx], "limit", 5)){
	       limit_state_handler(cmd, arg_idx+1);
	} else if(!strncmp(cmd->args[arg_idx], "offset", 5)){
	       offset_state_handler(cmd, arg_idx+1);
	}
        arg_idx++;
	
    }
    return;

}

void where_2(Command_t *cmd, size_t arg_idx,Table_t *table) {
    size_t idx;
    char *loc;
    char *temp; 
    char *char_temp;
    char *pre_str;
    char *aft_str;
    char *less="<";
    char *more=">";
    char *equal="=";
    char *nonequal="!=";
    char *less_equal="<=";
    char *more_equal=">=";
    User_t *user_temp;
    while(arg_idx < cmd->args_len){
        if(!strncmp(cmd->args[arg_idx], "<", 1) && strncmp(cmd->args[arg_idx], "<=", 2)) {
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], ">", 1) && strncmp(cmd->args[arg_idx], ">=", 2)) {///////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            }    
        } else if(!strncmp(cmd->args[arg_idx], "=", 1)) {///////////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
            temp=user_temp->name;
                    if(strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output2 = 0;
                } 
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], "!=", 2)) {///////////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(!strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output2 = 0;
                } 
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(!strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], "<=", 2)){///////////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], ">=", 2)){///////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(cmd->args[arg_idx+1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(cmd->args[arg_idx-1]))
                        user_temp->output2 = 0;
                }
            }
        } else{
            loc = strstr(cmd->args[arg_idx], more_equal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,more_equal);
                pre_str = loc;
                loc = strtok(NULL,more_equal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(pre_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(pre_str))
                        user_temp->output2 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], less_equal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,less_equal);
                pre_str = loc;
                loc = strtok(NULL,less_equal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(pre_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(pre_str))
                        user_temp->output2 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], nonequal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,nonequal);
                pre_str = loc;
                loc = strtok(NULL,nonequal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
            temp=user_temp->name;
                    if(!strncmp(aft_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(aft_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(pre_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(!strncmp(pre_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(pre_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(pre_str))
                        user_temp->output2 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], equal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,equal);
                pre_str = loc;
                loc = strtok(NULL,equal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(strncmp(aft_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(aft_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(pre_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(strncmp(pre_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(pre_str, temp, 20))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(pre_str))
                        user_temp->output2 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], more);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,more);
                pre_str = loc;
                loc = strtok(NULL,more);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(pre_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(pre_str))
                        user_temp->output2 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], less);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,less);
                pre_str = loc;
                loc = strtok(NULL,less);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(aft_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(pre_str))
                        user_temp->output2 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(pre_str))
                        user_temp->output2 = 0;
                    }
                }
            }
        }
        arg_idx++;

    }
    return;

}

void where_3(Command_t *cmd, size_t arg_idx,Table_t *table) {
    size_t idx;
    char *loc;
    char *temp; 
    char *char_temp;
    char *pre_str;
    char *aft_str;
    char *less="<";
    char *more=">";
    char *equal="=";
    char *nonequal="!=";
    char *less_equal="<=";
    char *more_equal=">=";
    User_t *user_temp;
    while(arg_idx < cmd->args_len){
        if(!strncmp(cmd->args[arg_idx], "<", 1) && strncmp(cmd->args[arg_idx], "<=", 2)) {
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], ">", 1) && strncmp(cmd->args[arg_idx], ">=", 2)) {///////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            }    
        } else if(!strncmp(cmd->args[arg_idx], "=", 1)) {///////////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
            temp=user_temp->name;
                    if(!strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output3 = 0;
                } 
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(!strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], "!=", 2)) {///////////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(cmd->args[arg_idx+1], temp, 20))
                        user_temp->output3 = 0;
                } 
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "name", 4)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "email", 5)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(cmd->args[arg_idx-1], temp, 20))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], "<=", 2)){///////////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            }
        } else if(!strncmp(cmd->args[arg_idx], ">=", 2)){///////////////////////////////////////////////////////////////////////////////////
            if (!strncmp(cmd->args[arg_idx-1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx-1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(cmd->args[arg_idx+1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "id", 2)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            } else if (!strncmp(cmd->args[arg_idx+1], "age", 3)) {
                for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(cmd->args[arg_idx-1]))
                        user_temp->output3 = 0;
                }
            }
        } else{
            loc = strstr(cmd->args[arg_idx], more_equal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,more_equal);
                pre_str = loc;
                loc = strtok(NULL,more_equal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(pre_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(pre_str))
                        user_temp->output3 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], less_equal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,less_equal);
                pre_str = loc;
                loc = strtok(NULL,less_equal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id <= atoi(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age <= atof(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id >= atoi(pre_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age >= atof(pre_str))
                        user_temp->output3 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], nonequal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,nonequal);
                pre_str = loc;
                loc = strtok(NULL,nonequal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
            temp=user_temp->name;
                    if(strncmp(aft_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(aft_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id != atoi(pre_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(strncmp(pre_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(strncmp(pre_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age != atof(pre_str))
                        user_temp->output3 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], equal);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,equal);
                pre_str = loc;
                loc = strtok(NULL,equal);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(!strncmp(aft_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(aft_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id == atoi(pre_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "name", 4)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->name;
                    if(!strncmp(pre_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "email", 5)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    temp=user_temp->email;
                    if(!strncmp(pre_str, temp, 20))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age == atof(pre_str))
                        user_temp->output3 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], more);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,more);
                pre_str = loc;
                loc = strtok(NULL,more);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(pre_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(pre_str))
                        user_temp->output3 = 0;
                    }
                }
            }
            loc = strstr(cmd->args[arg_idx], less);
            if(loc!=NULL){
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,less);
                pre_str = loc;
                loc = strtok(NULL,less);
                aft_str = loc;
                if(!strncmp(pre_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id < atoi(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(pre_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age < atof(aft_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "id", 2)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->id > atoi(pre_str))
                        user_temp->output3 = 0;
                    }
                } else if(!strncmp(aft_str, "age", 3)){
                    for(idx=0 ; idx<table->len ; idx++){
                    user_temp = get_User(table, idx);
                    if(user_temp->age > atof(pre_str))
                        user_temp->output3 = 0;
                    }
                }
            }
        }
        arg_idx++;

    }
    return;

}
