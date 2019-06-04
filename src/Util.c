#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"
#include "User.h"

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
	    for (idx = 0; idx < sel_args->fields_len; idx++) {
		if (!strncmp(sel_args->fields[idx], "*", 1)) {
		    printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
		} else{
		    if (idx > 0) printf(", ");

		    if (!strncmp(sel_args->fields[idx], "id", 2)) {
		        printf("%d", user->id);
		    } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
		        printf("%s", user->name);
		    } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
		        printf("%s", user->email);
		    } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
		        printf("%d", user->age);
		    }
		}
	    }
    printf(")\n");
}

///
/// Print the users for given offset and limit restriction
///
void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd) {
    size_t idx;
    size_t idx2;
    size_t check;
    User_t *user_temp;
    char *temp_table_name="yoman";
    Table_t *temp_table;
    temp_table = new_Table(temp_table_name);
    int ret;
    for(idx=0;idx<table->len;idx++){
    	user_temp = get_User(table,idx);
    	if(user_temp->output3){
	    	check = user_temp->output + user_temp->output2;
	    	if(cmd->cmd_args.sel_args.And==0){
	    	    if(check==2){
	    		ret = add_User(temp_table, user_temp);
	    	    }
	    	}
	    	else if((cmd->cmd_args.sel_args.And)==1){
	    	    if(check>=1){
	    	    	ret = add_User(temp_table, user_temp);
	    	    }
	    	    
	    	}
	    	else if((cmd->cmd_args.sel_args.And)==2){
	    	    if(check==2){
	    	    	ret = add_User(temp_table, user_temp);
	     	    }
	    	}
	    }
    }
    ret = ret+0;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;
    if (offset == -1) {
        offset = 0;
    }
///    printf("sum:%d\n",cmd->cmd_args.sel_args.sum);
///    printf("avg:%d\n",cmd->cmd_args.sel_args.avg);
///    printf("count:%d\n",cmd->cmd_args.sel_args.count);
    if(cmd->cmd_args.sel_args.sum || cmd->cmd_args.sel_args.avg || cmd->cmd_args.sel_args.count){
        //char *temp_table_name2="temp_table2";
        //Table_t *temp_table2;
        //temp_table2 = new_Table(temp_table_name2);
        if(offset>0 || limit!=-1 || limit==0 ){
        }
        else{
            printf("(");
            for(idx=0;idx<cmd->cmd_args.sel_args.fields_len;idx++){
                if (idx > 0) printf(", ");
                if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "sum_id", 6)){
                    int sum=0;
                    for(idx2=0;idx2<temp_table->len;idx2++){
                        user_temp = get_User(temp_table, idx2);
                        sum += user_temp->id;
                    }
                    printf("%d", sum);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "sum_age", 7)){
                    int sum=0;
                    for(idx2=0;idx2<temp_table->len;idx2++){
                        user_temp = get_User(temp_table, idx2);
                        sum += user_temp->age;
                    }
                    printf("%d", sum);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "avg_id", 6)){
                    int sum=0;
                    for(idx2=0;idx2<temp_table->len;idx2++){
                        user_temp = get_User(temp_table, idx2);
                        sum += user_temp->id;
                    }
                    double avg=sum/temp_table->len;
                    printf("%.3lf", avg);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "avg_age", 7)){
                    int sum=0;
                    for(idx2=0;idx2<temp_table->len;idx2++){
                        user_temp = get_User(temp_table, idx2);
                        sum += user_temp->age;
                    }
                    double avg=sum/temp_table->len;
                    printf("%.3lf", avg);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "count_id", 8)){
                    printf("%ld", temp_table->len);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "count_name", 10)){
                    printf("%ld", temp_table->len);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "count_email", 11)){
                    printf("%ld", temp_table->len);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "count_age", 9)){
                    printf("%ld", temp_table->len);
                } else if(!strncmp(cmd->cmd_args.sel_args.fields[idx], "count", 5)){
                    printf("%ld", temp_table->len);
                }
            }
            printf(")\n");
        }
    }


    else{
	    if (idxList) {
		for(idx = offset; idx < idxListLen; idx++){
		    if(limit != 1 && (idx - offset) >= limit) {
			break;
	 	    }
		    print_user(get_User(temp_table, idxList[idx]), &(cmd->cmd_args.sel_args));
		}
	    } else {
		for (idx = offset; idx < temp_table->len; idx++) {
		    if (limit != -1 && (idx - offset) >= limit) { //  && (idx - offset) >= limit
		    	break;
		    }
		    print_user(get_User(temp_table,idx), &(cmd->cmd_args.sel_args));
		}
	    }
	}
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table, cmd);
        return SELECT_CMD;
    } else if(!strncmp(cmd->args[0], "update", 6)) {
        handle_update_cmd(table, cmd);
		return UPDATE_CMD;
   } else if(!strncmp(cmd->args[0], "delete", 6)) {
        handle_delete_cmd(table, cmd);
        return DELETE_CMD;
    } else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    User_t *user_temp;
    size_t idx;
    cmd->cmd_args.sel_args.And=0;
    field_state_handler(cmd, 1, table);
    print_users(table, NULL, 0, cmd);
////	for(idx=0;idx<table->len;idx++){
////	user_temp = get_User(table, idx);
////	printf("output1:%d ; output2:%d\n", user_temp->output, user_temp->output2);
////}
    for(idx=0;idx<table->len;idx++){
    	user_temp = get_User(table, idx);
    	user_temp->output = 1;
    	user_temp->output2= 1;
    }
    return table->len;
}
int handle_update_cmd(Table_t *table, Command_t *cmd){
    cmd->type = UPDATE_CMD;

    size_t arg_idx=1;
    size_t idx;
    size_t dontcheck=0;
    cmd->cmd_args.sel_args.update_name=0;
    cmd->cmd_args.sel_args.update_email=0;
    cmd->cmd_args.sel_args.update_age=0;
    cmd->cmd_args.sel_args.And=0;
    char keep_str[50];
    char *loc;
    char *char_temp;
    char *pre_str;
    char *aft_str;
    User_t *user_temp;

    while(arg_idx < cmd->args_len) {
        if(!strncmp(cmd->args[arg_idx], "set", 3) && !dontcheck){
            arg_idx++;
            loc = strstr(cmd->args[arg_idx], "=");
            if(loc==NULL){
                if(!strncmp(cmd->args[arg_idx], "name", 4)){
                    cmd->cmd_args.sel_args.update_name=1;
                    arg_idx += 2;
                    strncpy(keep_str, cmd->args[arg_idx], MAX_USER_EMAIL);//123
                } else if(!strncmp(cmd->args[arg_idx], "email", 5)){
                    cmd->cmd_args.sel_args.update_email=1;
                    arg_idx += 2;
                    strncpy(keep_str, cmd->args[arg_idx], 50);
                } else if(!strncmp(cmd->args[arg_idx], "age", 3)){
                    cmd->cmd_args.sel_args.update_age=1;
                    arg_idx += 2;
                    strncpy(keep_str, cmd->args[arg_idx], 50);
                } else{
                    strncpy(keep_str, cmd->args[arg_idx], 50);
                    arg_idx += 2;
                    if(!strncmp(cmd->args[arg_idx], "name", 4)){
                        cmd->cmd_args.sel_args.update_name=1;
                    } else if(!strncmp(cmd->args[arg_idx], "email", 5)){
                        cmd->cmd_args.sel_args.update_email=1;
                    } else if(!strncmp(cmd->args[arg_idx], "age", 3)){
                        cmd->cmd_args.sel_args.update_age=1;
		    }
                }
                
            } else{
                char_temp = cmd->args[arg_idx];
                loc = strtok(char_temp,"=");
                pre_str = loc;
                loc = strtok(NULL,"=");
                aft_str = loc;
                if(!strncmp(pre_str, "name", 4)){   
                    cmd->cmd_args.sel_args.update_name=1;
                    strncpy(keep_str, aft_str, 50);
                } else if(!strncmp(pre_str, "email", 5)){
                    cmd->cmd_args.sel_args.update_email=1;
                    strncpy(keep_str, aft_str, 50);
                } else if(!strncmp(pre_str, "age", 3)){
                    cmd->cmd_args.sel_args.update_age=1;
                    strncpy(keep_str, aft_str, 50);
                } else if(!strncmp(aft_str, "name", 4)){
                    cmd->cmd_args.sel_args.update_name=1;
                    strncpy(keep_str, aft_str, 50);
                } else if(!strncmp(aft_str, "email", 5)){
                    cmd->cmd_args.sel_args.update_email=1;
                    strncpy(keep_str, aft_str, 50);
                } else if(!strncmp(aft_str, "age", 3)){
                    cmd->cmd_args.sel_args.update_age=1;
                    strncpy(keep_str, aft_str, 50);
                }
                /*loc = strstr(cmd->args[arg_idx], "name");
                if(loc!=NULL){
                    char_temp = cmd->args[arg_idx];
                    loc = strtok(char_temp,"=");
                    pre_str = loc;
                    loc = strtok(NULL,"=");
                    aft_str = loc;
                } 
                loc = strstr(cmd->args[arg_idx], "email");
                if(loc!=NULL){
                    har_temp = cmd->args[arg_idx];
                    loc = strtok(char_temp,"=");
                    pre_str = loc;
                    loc = strtok(NULL,"=");
                    aft_str = loc;
                }
                loc = strstr(cmd->args[arg_idx], "age");
                if(loc!=NULL){
                    har_temp = cmd->args[arg_idx];
                    loc = strtok(char_temp,"=");
                    pre_str = loc;
                    loc = strtok(NULL,"=");
                    aft_str = loc;
                }*/
            }
        } else if (!strncmp(cmd->args[arg_idx], "where", 5)){
	    dontcheck=1;
            where_state_handler(cmd, arg_idx+1, table);
        }
        arg_idx++;
    }
    /////////////////////////////////////////////////////////////////////update
    char string[MAX_USER_EMAIL+2]="";           
    strcat(string,keep_str);
    for(idx=0;idx<table->len;idx++){
        user_temp = get_User(table, idx);
        if(cmd->cmd_args.sel_args.And==0){
            if(user_temp->output==1){
                if(cmd->cmd_args.sel_args.update_name==1){
                    strncpy(user_temp->name, string, MAX_USER_NAME);
                } else if(cmd->cmd_args.sel_args.update_email==1){   
                    strncpy(user_temp->email, string, MAX_USER_EMAIL);
                } else if(cmd->cmd_args.sel_args.update_age==1){
                    user_temp->age = atoi(string);
                }
            }
        } else if(cmd->cmd_args.sel_args.And==1){
            if(user_temp->output==1 || user_temp->output2==1){
                if(cmd->cmd_args.sel_args.update_name==1){
                    strncpy(user_temp->name, string, MAX_USER_NAME);
                } else if(cmd->cmd_args.sel_args.update_email==1){   
                    strncpy(user_temp->email, string, MAX_USER_EMAIL);
                } else if(cmd->cmd_args.sel_args.update_age==1){
                    user_temp->age = atoi(string);
                }
            }
        } else if(cmd->cmd_args.sel_args.And==2){
            if(user_temp->output==1 && user_temp->output2==1){
                if(cmd->cmd_args.sel_args.update_name==1){
                    strncpy(user_temp->name, string, MAX_USER_NAME);
                } else if(cmd->cmd_args.sel_args.update_email==1){   
                    strncpy(user_temp->email, string, MAX_USER_EMAIL);
                } else if(cmd->cmd_args.sel_args.update_age==1){
                    user_temp->age = atoi(string);
                }
            }
        }
    }
    for(idx=0;idx<table->len;idx++){ 
    	user_temp = get_User(table, idx);
    	user_temp->output = 1;
    	user_temp->output2= 1;
    }

    return 1;
}
///
/// Show the help messages
///
int handle_delete_cmd(Table_t *table, Command_t *cmd){
	cmd->type = DELETE_CMD;
	User_t *user_temp;
	size_t dontcheck=0;
	size_t idx;
	size_t arg_idx=1;
	while(arg_idx < cmd->args_len) {
		if(!strncmp(cmd->args[arg_idx], "where", 3)){
			dontcheck=1;
			where_3(cmd, arg_idx, table);
		}
	arg_idx++;
	}
	if(!dontcheck){
		for(idx=0;idx<table->len;idx++){
			user_temp = get_User(table, idx);
			user_temp->output3 = 0;
		}
	}
	return 1;
}
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}

