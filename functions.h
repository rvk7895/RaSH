#ifndef FUNCTION_H
#define FUNCTION_H

void get_username(char *username);

void get_os(char *os);

void get_directory(char *directory, char *home_directory);

void get_home_directory(char *home_directory);

void get_hostname(char *hostname);

void parse_input(char* input, char* current_dir, char* home_dir, char* prev_dir);

void execute_command(char* command,char* current_dir, char* home_dir, char* prev_dir);

char** parse_command(char* command, int* arguments_count);

int foreground_process(char** arguments, int arguments_count);

int background_process(char** arguments, int arguments_count);

void pinfo(char** arguments, int arguments_count);

void delete_child(int pid);

void add_child(int pid, char* name);

void init_child_list();

void initialize_child_process_handler();

void sig_child_handler(int sig);

void handle_pipes(char* command,char* current_dir, char* home_dir, char* prev_dir);

#endif