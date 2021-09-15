#ifndef FUNCTION_H
#define FUNCTION_H

void get_username(char *username);

void get_os(char *os);

void get_directory(char *directory, char *home_directory);

void get_home_directory(char *home_directory);

void get_hostname(char *hostname);

char* read_input();

char** parse_input(char* input, int* command_count);

void execute_commands(char** commands, int commands_count);

char** parse_commands(char* command, int* arguments_count);

#endif