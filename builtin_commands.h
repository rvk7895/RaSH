#ifndef BUILTIN_COMMANDS_H
#define BUILTIN_COMMANDS_H

void cd(char *dir, char *curr_dir, char *prev_dir);

void pwd(char *dir, char *home_dir);

void echo(char** arguments, int argument_count);

void ls(char** arguments, int argument_count, char* home_dir);

void clear();

void print_file_details(char* path, char* file_name);

int file_type_letter(mode_t mode);

char* ls_perms(mode_t mode);
#endif