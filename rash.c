#include "definitions.h"
#include "functions.h"

char USERNAME[MAX_USERNAME_LENGTH];
char HOME_DIR[MAX_PATH_LENGTH];
char CURRENT_DIR[MAX_PATH_LENGTH];
char HOSTNAME[MAX_HOSTNAME_LENGTH];
char* INPUT_BUFFER = NULL;
char** COMMANDS = NULL;
int NUMBER_OF_COMMANDS;

int main(int argc, char* argv[]) {

    get_username(USERNAME);
    get_home_directory(HOME_DIR);
    get_directory(CURRENT_DIR, HOME_DIR);
    get_hostname(HOSTNAME);

    // infinite loop for shell
    while (1) {
        printf("<%s@%s:%s>", USERNAME, HOSTNAME, CURRENT_DIR);
        INPUT_BUFFER = read_input();
        COMMANDS = parse_input(INPUT_BUFFER, &NUMBER_OF_COMMANDS);
        // printf("%ld\n", COMMANDS);
        // printf("test\n");
        for (int i = 0;i < NUMBER_OF_COMMANDS;i++) {
            printf("%s\n", COMMANDS[i]);
        }
        execute_commands(COMMANDS, NUMBER_OF_COMMANDS);
    }

}