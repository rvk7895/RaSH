// contains functions required for the shell

#include "functions.h"
#include "definitions.h"

void get_username(char* username) {
    uid_t uid = getuid();
    struct passwd* pw = getpwuid(uid);
    if (pw) {
        strcpy(username, pw->pw_name);
    }
}

void get_directory(char* directory, char* home_directory) {
    char* cwd = getcwd(NULL, 0);

    if (strcmp(cwd, home_directory) == 0) {
        strcpy(directory, "~");
    }
    else {
        strcpy(directory, cwd);
    }
    free(cwd);
}

void get_home_directory(char* home_directory) {
    char* cwd = getcwd(NULL, 0);
    strcpy(home_directory, cwd);
    free(cwd);
}

void get_hostname(char* hostname) {
    gethostname(hostname, MAX_HOSTNAME_LENGTH);
}

char* read_input() {
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    if ((read = getline(&line, &len, stdin)) == -1) {
        // exiting the program with status SUCCESS because we received an EOF
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        }
        else {
            perror("getline error: ");
            exit(EXIT_FAILURE);
        }

    }
    return line;
}

char** parse_input(char* input, int* command_count) {

    for (int i = 0;i < strlen(input);i++) {
        if (input[i] == '\t') {
            input[i] = ' ';
        }
        else if (input[i] == '\n') {
            input[i] = ';';
        }

        if(input[i] == ';') {
            (*command_count)++;
        }
    }

    char** commands = malloc(sizeof(char*) * (*command_count));
    *command_count = 0;
    char* command = strtok(input, ";");
    while (command != NULL) {
        (*command_count)++;
        commands[(*command_count) - 1] = command;
        command = strtok(NULL, ";");
    }

    return commands;
}

void execute_commands(char** commands, int commands_count) {
    for (int i = 0;i < commands_count;i++) {
        char* command = commands[i];
        int arguments_count = 0;
        char** arguments = parse_commands(command, &arguments_count);
        printf("Number of arguments %d\n", arguments_count);
    }
}

char** parse_commands(char* input, int* arguments_count) {
    
    char** arguments = malloc(sizeof(char*) * (*arguments_count));
    *arguments_count = 0;
    char* argument = strtok(input, " ");
    while (argument != NULL) {
        (*arguments_count)++;
        arguments[(*arguments_count) - 1] = argument;
        argument = strtok(NULL, " ");
    }

    return arguments;
}