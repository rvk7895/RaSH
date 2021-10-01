#ifndef DEFINTIONS_H
#define DEFINTIONS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h> 
#include <sys/wait.h>
#include <sys/stat.h>
#include <grp.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define MAX_ARG_LENGTH 128
#define MAX_INPUT_LENGTH 1024
#define MAX_USERNAME_LENGTH 32
#define MAX_PATH_LENGTH 256
#define MAX_HOSTNAME_LENGTH 256
#define MAX_ARGUMENTS 1024
#define MAX_FILES_PER_DIR 65535
#define MAX_STRING_LENGTH 1024
#define STAT_COUNT 52
#define MAX_CHILDREN 1024
#define MAX_HISTORY 20

struct child {
    pid_t pid;
    char name[MAX_ARG_LENGTH];
};

extern int errno;

#endif 