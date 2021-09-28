#include "definitions.h"
#include "functions.h"

char USERNAME[MAX_USERNAME_LENGTH];
char HOME_DIR[MAX_PATH_LENGTH];
char CURRENT_DIR[MAX_PATH_LENGTH];
char HOSTNAME[MAX_HOSTNAME_LENGTH];
char PREV_DIR[MAX_PATH_LENGTH];
char INPUT_BUFFER[MAX_INPUT_LENGTH];
int NUMBER_OF_COMMANDS;

int main(int argc, char* argv[]) {

    get_username(USERNAME);
    get_home_directory(HOME_DIR);
    get_directory(CURRENT_DIR, HOME_DIR);
    get_hostname(HOSTNAME);
    strcpy(PREV_DIR, CURRENT_DIR);

    init_child_list();
    initialize_child_process_handler();

    // infinite loop for shell
    while (1) {
        printf("<%s@%s:%s>", USERNAME, HOSTNAME, CURRENT_DIR);
        //taking input
        if(!fgets(INPUT_BUFFER, MAX_INPUT_LENGTH, stdin)) {
            break;
        }
        parse_input(INPUT_BUFFER, CURRENT_DIR, HOME_DIR, PREV_DIR);
    }

}