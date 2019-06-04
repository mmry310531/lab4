#include <string.h>
#include <stdlib.h>
#include "User.h"
#include "Command.h"

///
/// Allocate new space for User_t
/// The caller should free the allocated space
///
User_t* new_User() {
    User_t *new_user = (User_t*)malloc(sizeof(User_t));
    new_user->id = 0;
    memset(new_user->name, 0, MAX_USER_NAME+1);
    memset(new_user->email, 0, MAX_USER_EMAIL+1);
    new_user->age = 0;
    new_user->output = 0;
    new_user->output2= 0;
    new_user->output3= 0;
    return new_user;
}

///
/// Transform from the input cmd to the User_t
///
User_t* command_to_User(Command_t *cmd) {
    User_t *user = new_User();
    if (!user || !cmd) {
        return NULL;
    }
    if (cmd->args_len != 5) {
        return NULL;
    }


    char string1[MAX_USER_EMAIL+2]="";
    char string2[MAX_USER_EMAIL+2]="";

    user->id = atoi(cmd->args[1]);

    strcat(string1, cmd->args[2]);
    strncpy(user->name, string1, MAX_USER_NAME);
    
    strcat(string2, cmd->args[3]);
    strncpy(user->email, string2, MAX_USER_EMAIL);

    user->age = atoi(cmd->args[4]);

    user->output = 1;
    user->output2= 1;
    user->output3= 1;

    return user;
}

