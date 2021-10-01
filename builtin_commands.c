#include "functions.h"
#include "definitions.h"
#include "builtin_commands.h"

int isDirectory(char* path) {
    DIR* dir = opendir(path);
    if (dir) {
        return 1;
    }
    else {
        return 0;
    }
}

int file_type_letter(mode_t mode) {
    char c;
    if (S_ISREG(mode))
        c = '-';
    else if (S_ISDIR(mode))
        c = 'd';
    else if (S_ISBLK(mode))
        c = 'b';
    else if (S_ISCHR(mode))
        c = 'c';
    else if (S_ISFIFO(mode))
        c = 'p';
    else if (S_ISLNK(mode))
        c = 'l';
    else if (S_ISSOCK(mode))
        c = 's';
    else {
        /* Unknown type -- possibly a regular file? */
        c = '?';
    }
    return (c);
}

char* permissions(mode_t mode) {
    char* rwx[] = { "---", "--x", "-w-", "-wx",
                                "r--", "r-x", "rw-", "rwx" };
    static char bits[11];

    bits[0] = file_type_letter(mode);
    strcpy(&bits[1], rwx[(mode >> 6) & 7]);
    strcpy(&bits[4], rwx[(mode >> 3) & 7]);
    strcpy(&bits[7], rwx[(mode & 7)]);
    if (mode & S_ISUID)
        bits[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID)
        bits[6] = (mode & S_IXGRP) ? 's' : 'l';
    if (mode & S_ISVTX)
        bits[9] = (mode & S_IXOTH) ? 't' : 'T';
    bits[10] = '\0';
    return (bits);
}

void print_file_details(char* path, char* filename) {
    struct stat statbuf;

    stat(path, &statbuf);

    printf("%s ", permissions(statbuf.st_mode));
    printf("%4.ld ", statbuf.st_nlink);
    printf("%s ", getpwuid(statbuf.st_uid)->pw_name);
    printf("%s ", getgrgid(statbuf.st_gid)->gr_name);
    printf("%12.ld ", statbuf.st_size);

    char date[21];
    strftime(date, 20, "%b %d %H:%M", localtime(&(statbuf.st_ctime)));
    printf("%s ", date);
    printf("%s\n", filename);
}

int file_filter(const struct dirent* entry) {
    if (entry->d_name[0] == '.') {
        return 0;
    }
    return 1;
}

void cd(char* dir, char* curr_dir, char* prev_dir) {
    int change_dir = chdir(dir);
    strcpy(prev_dir, curr_dir);
    strcpy(curr_dir, dir);


    if (change_dir == -1) {
        printf("%s\n", strerror(errno));
    }
}

void pwd(char* dir, char* home_dir) {
    if (dir[0] == '~') {
        printf("%s\n", home_dir);
    }
    else {
        printf("%s\n", dir);
    }
}

void echo(char** arguments, int argument_count) {
    for (int i = 1; arguments[i] != NULL;i++) {
        printf("%s", arguments[i]);
    }
    printf("\n");
}

void ls(char** arguments, int argument_count, char* home_dir) {
    struct dirent** name_list;
    char* final_list[MAX_FILES_PER_DIR];
    int n;
    int a_flag = 0;
    int l_flag = 0;
    int directory_name_indexes[MAX_ARGUMENTS];
    int number_of_directories = 0;

    if (argument_count == 1) {
        n = scandir(".", &name_list, file_filter, alphasort);
        for (int i = 0;i < n;i++) {
            {
                printf("%s\n", name_list[i]->d_name);
                free(name_list[i]);
            }
        }
        return;
    }

    else {
        for (int i = 1;i < argument_count;i++) {
            if (strcmp(arguments[i], "-a") == 0) {
                a_flag = 1;
            }
            else if (strcmp(arguments[i], "-l") == 0) {
                l_flag = 1;
            }
            else if (strcmp(arguments[i], "-al") == 0 || strcmp(arguments[i], "-la") == 0) {
                a_flag = 1;
                l_flag = 1;
            }
            else {
                directory_name_indexes[number_of_directories] = i;
                number_of_directories++;
            }
        }
    }

    if (number_of_directories == 0) {
        n = scandir(".", &name_list, a_flag ? NULL : file_filter, alphasort);
        if (n == -1) {
            printf("%s\n", strerror(errno));
            return;
        }
        for (int i = 0;i < n;i++) {
            final_list[i] = malloc(strlen(name_list[i]->d_name) + 1);
            strcpy(final_list[i], name_list[i]->d_name);
            free(name_list[i]);
        }

        if (l_flag) {
            for (int i = 0;i < n;i++) {
                char filepath[MAX_PATH_LENGTH];
                char filename[MAX_PATH_LENGTH];
                strcpy(filepath, final_list[i]);
                strcpy(filename, final_list[i]);
                print_file_details(filepath, filename);
                free(final_list[i]);
            }
        }
        else {
            for (int i = 0;i < n;i++) {
                printf("%s\n", final_list[i]);
            }
        }
    }
    else {
        for (int i = 0;i < number_of_directories;i++) {
            if (!isDirectory(arguments[directory_name_indexes[i]])) {
                if (l_flag) {
                    print_file_details(arguments[directory_name_indexes[i]], arguments[directory_name_indexes[i]]);
                }
                else {
                    printf("%s\n", arguments[directory_name_indexes[i]]);
                }
                printf("\n");
                continue;
            }

            char dir[MAX_PATH_LENGTH];
            strcpy(dir, arguments[directory_name_indexes[i]]);
            if (dir[0] == '~') {
                strcpy(dir, home_dir);
                for (int j = 0;j < strlen(arguments[directory_name_indexes[i]]);j++) {
                    dir[j + strlen(home_dir)] = arguments[directory_name_indexes[i]][j];
                }
            }
            if (number_of_directories > 1) {
                printf("%s:\n", arguments[directory_name_indexes[i]]);
            }
            n = scandir(dir, &name_list, a_flag ? NULL : file_filter, alphasort);
            if (n == -1) {
                printf("%s\n\n", strerror(errno));
                continue;
            }
            for (int j = 0;j < n;j++) {
                final_list[j] = malloc(strlen(name_list[j]->d_name) + 1);
                strcpy(final_list[j], name_list[j]->d_name);
                free(name_list[j]);
            }

            if (l_flag) {
                for (int j = 0;j < n;j++) {
                    char filepath[MAX_PATH_LENGTH];
                    char filename[MAX_PATH_LENGTH];
                    strcpy(filepath, dir);
                    strcat(filepath, "/");
                    strcat(filepath, final_list[j]);
                    strcpy(filename, final_list[j]);
                    print_file_details(filepath, filename);
                    free(final_list[j]);
                }
            }
            else {
                for (int j = 0;j < n;j++) {
                    printf("%s\n", final_list[j]);
                }
            }
            printf("\n");
        }

    }

}

void clear() {
    printf("\033[2J\033[1;1H");
}

