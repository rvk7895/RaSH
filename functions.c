// contains functions required for the shell
#include "functions.h"
#include "definitions.h"
#include "builtin_commands.h"
#include "userdefined_commands.h"

struct child child_list[MAX_CHILDREN];
char* history_list[MAX_HISTORY];

int children_count = 0;
int history_count = 0;

void add_history(char* command) {
    history_list[history_count] = malloc(sizeof(char) * MAX_ARG_LENGTH);
    for (int i = history_count; i > 0; i--) {
        strcpy(history_list[i], history_list[i - 1]);
    }
    strcpy(history_list[0], command);
    if (history_count + 1 != MAX_HISTORY) {
        history_count++;
    }
}

void init_child_list() {
    int i;
    for (i = 0; i < MAX_CHILDREN; i++) {
        child_list[i].pid = -1;
        strcpy(child_list[i].name, "");
    }
}

void add_child(int pid, char* name) {

    if (children_count == MAX_CHILDREN) {
        printf("Too many children!\n");
        return;
    }
    child_list[children_count].pid = pid;
    strcpy(child_list[children_count].name, name);
    children_count++;
}

void delete_child(int pid) {
    char found = 0;
    // If a child process that is in the middle of the array is killed, the processes
    // ahead of it are moved back one space to keep them contigeous
    for (int i = 0; i < children_count; i++)
    {
        if (child_list[i].pid == pid && !found)
            found = 1;
        if (found)
        {
            if (i == children_count - 1)
                child_list[i].pid = -1;
            else
            {
                child_list[i].pid = child_list[i + 1].pid;
                strcpy(child_list[i].name, child_list[i + 1].name);
            }
        }
    }
    children_count--;
}


void initialize_child_process_handler() {
    struct sigaction sigchld_action;
    memset(&sigchld_action, 0, sizeof(sigchld_action));
    sigchld_action.sa_handler = sig_child_handler;
    sigchld_action.sa_flags = SA_RESTART;
    sigemptyset(&sigchld_action.sa_mask);
    sigaction(SIGCHLD, &sigchld_action, NULL);
}

void sig_child_handler(int sig) {
    pid_t pid;
    int status;
    int procKill = 0;
    char output_string[MAX_STRING_LENGTH];

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        status = WIFEXITED(status);
        for (int i = 0; i < MAX_CHILDREN; i++)
            if (child_list[i].pid == pid)
            {
                // Print the termination message and set the flag
                sprintf(output_string, "\n%s with pid %d exited %s\n", child_list[i].name, (int)pid, status != 0 ? "normally" : "abnormally");
                write(STDERR, output_string, strlen(output_string));
                delete_child(child_list[i].pid);
                break;
            }
    }
}


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

    if (strstr(directory, home_directory)) {
        char updated_directory[MAX_PATH_LENGTH];
        strcpy(updated_directory, "~");
        for (int i = strlen(home_directory); i < strlen(directory); i++) {
            updated_directory[i - strlen(home_directory) + 1] = directory[i];
        }
        strcpy(directory, updated_directory);
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

void parse_input(char* input, char* current_dir, char* home_dir, char* prev_dir) {

    int command_count = 0;

    for (int i = 0;i < strlen(input);i++) {
        if (input[i] == '\t') {
            input[i] = ' ';
        }
        else if (input[i] == '\n') {
            input[i] = ';';
        }

        if (input[i] == ';') {
            command_count++;
        }
    }

    char** commands = malloc(sizeof(char*) * command_count);
    command_count = 0;
    char* command = strtok(input, ";");
    while (command != NULL) {
        command_count++;
        commands[command_count - 1] = command;
        command = strtok(NULL, ";");
    }

    for (int i = 0;i < command_count;i++) {
        handle_pipes(commands[i], current_dir, home_dir, prev_dir);
    }

}

void handle_pipes(char* command, char* current_dir, char* home_dir, char* prev_dir) {
    int pipe_count = 0;
    int status = 0;

    for (int i = 0;i < strlen(command);i++) {
        if (command[i] == '|') {
            pipe_count++;
        }
    }

    if (pipe_count == 0) {
        execute_command(command, current_dir, home_dir, prev_dir);
    }

    else {
        char* commands[pipe_count + 1];
        char* depiped_command = strtok(command, "|");
        int number_of_piped_commands = 0;
        while (depiped_command != NULL) {
            commands[number_of_piped_commands] = depiped_command;
            number_of_piped_commands++;
            depiped_command = strtok(NULL, "|");
        }

        for (int i = 0; i < number_of_piped_commands; i++)
        {
            if (commands[i][0] == ' ')
                commands[i]++;
            if (commands[i][strlen(commands[i]) - 1] == ' ')
                commands[i][strlen(commands[i]) - 1] = '\0';
        }

        int pipes[pipe_count][2];
        for (int i = 0; i < pipe_count; i++)
        {
            if (pipe(pipes[i]) == -1)
            {
                perror("pipe");
                return;
            }
        }

        int cpid;

        for (int i = 0; i < pipe_count + 1; i++) {
            cpid = fork();

            if (cpid == -1) {
                perror("fork");
                return;
            }

            if (cpid == 0) {
                if (i < pipe_count) {
                    dup2(pipes[i][1], STDOUT);
                }

                if (i > 0) {
                    dup2(pipes[i - 1][0], STDIN);
                }

                for (int j = 0; j < pipe_count; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                execute_command(commands[i], current_dir, home_dir, prev_dir);
                exit(0);
            }
        }
        for (int i = 0; i < 2 * pipe_count; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < pipe_count + 1; i++)
        {
            if (i == pipe_count)
            {
                wait(&status);
                status = WEXITSTATUS(status);
            }
            else
                wait(NULL);
        }
    }
}

void execute_command(char* command, char* current_dir, char* home_dir, char* prev_dir) {

    int redir = 0;
    // If there is no redirection, execute the command normally
    int readRedir = 0;
    int writeRedir = 0;

    // Will store index of file location in the command
    int infileLoc = -1;
    int outfileLoc = -1;


    // parsing the command
    // printf("%s\n", command);
    char** arguments = malloc(sizeof(char*) * MAX_ARGUMENTS);
    int argument_count = 0;
    char* argument = strtok(command, " ");
    while (argument != NULL) {
        argument_count++;
        arguments[argument_count - 1] = argument;
        argument = strtok(NULL, " ");
    }

    for (int i = 0;i < argument_count; i++) {
        if (arguments[i][0] == '>' || arguments[i][0] == '<') {
            redir = 1;
        }
    }


    add_history(arguments[0]);

    for (int i = 0;i < argument_count;i++) {
        if (strcmp(arguments[i], ">") == 0) {
            writeRedir = 1;
            outfileLoc = i + 1;
        }

        else if (strcmp(arguments[i], "<") == 0) {
            readRedir = 1;
            infileLoc = i + 1;
        }

        else if (strcmp(arguments[i], ">>") == 0) {
            writeRedir = 2;
            outfileLoc = i + 1;
        }
    }

    int stdinBackup;
    int stdoutBackup;
    int ipFd;
    int opFd;
    if (readRedir)
    {
        stdinBackup = dup(STDIN);
        ipFd = open(arguments[infileLoc], O_RDONLY);
        if (ipFd < 0)
        {
            close(stdinBackup);
            printf("Unable to redirect input\n");
            perror("RASH:");
            return;
        }
        if (dup2(ipFd, STDIN) == -1)
        {
            printf("Unable to redirect input\n");
            perror("RASH:");
            return;
        }
    }
    if (writeRedir)
    {
        stdoutBackup = dup(STDOUT);
        int opMethod = writeRedir == 1 ? O_TRUNC : O_APPEND;
        opFd = open(arguments[outfileLoc], O_CREAT | O_WRONLY | opMethod, 0644);
        if (opFd < 0)
        {
            close(stdoutBackup);
            printf("Unable to redirect output\n");
            perror("RASH:");
            return;
        }

        if (dup2(opFd, STDOUT) == -1)
        {
            printf("Unable to redirect output\n");
            perror("RASH:");
            return;
        }
    }
    // implementing cd
    if (strcmp(arguments[0], "cd") == 0) {
        if (argument_count == 1) {
            cd(home_dir, current_dir, prev_dir);
            get_directory(current_dir, home_dir);
        }
        else if (argument_count == 2) {
            if (arguments[1][0] == '~') {
                char dir[MAX_PATH_LENGTH];
                strcpy(dir, home_dir);
                if (strlen(arguments[1]) > 1) {
                    for (int i = 1; i < strlen(arguments[1]); i++) {
                        dir[i - 1 + strlen(home_dir)] = arguments[1][i];
                    }
                }
                cd(dir, current_dir, prev_dir);
            }
            else if (arguments[1][0] == '-') {
                printf("%s\n", prev_dir);

                if (prev_dir[0] == '~') {
                    cd(home_dir, current_dir, prev_dir);
                }
                else {
                    cd(prev_dir, current_dir, prev_dir);
                }
            }
            else {
                cd(arguments[1], current_dir, prev_dir);
            }
            get_directory(current_dir, home_dir);
        }
        else {
            printf("cd: too many arguments\n");
        }
    }

    // implementing pwd
    else if (strcmp(arguments[0], "pwd") == 0) {
        pwd(current_dir, home_dir);
    }

    // implementing echo
    else if (strcmp(arguments[0], "echo") == 0) {
        if (writeRedir != 0)
            arguments[outfileLoc - 1] = NULL;
        if (readRedir != 0)
            arguments[infileLoc - 1] = NULL;
        echo(arguments, argument_count);
    }

    else if (strcmp(arguments[0], "ls") == 0) {
        if (writeRedir != 0)
            arguments[outfileLoc - 1] = NULL;
        if (readRedir != 0)
            arguments[infileLoc - 1] = NULL;
        ls(arguments, argument_count, home_dir);
    }

    else if (strcmp(arguments[0], "exit") == 0) {
        exit(0);
    }

    else if (strcmp(arguments[0], "clear") == 0) {
        clear();
    }

    else if (strcmp(arguments[0], "pinfo") == 0) {
        pinfo(arguments, argument_count);
    }

    else if (strcmp(arguments[0], "repeat") == 0) {
        if (argument_count == 1 || argument_count == 2) {
            printf("repeat: format repeat <number_of_times_to_repeat> <command> <arguments>\n");
        }

        else {
            if (atoi(arguments[1]) == 0) {
                printf("repeat: format repeat <number_of_times_to_repeat > 0> <command> <arguments>\n");
            }

            else {
                int times = atoi(arguments[1]);
                char* new_command = malloc(sizeof(char) * 1024);
                char* command_copy = malloc(sizeof(char) * 1024);
                new_command[0] = '\0';
                for (int i = 2; i < argument_count; i++) {
                    strcat(new_command, arguments[i]);
                    strcat(new_command, " ");
                }

                for (int i = 0;i < times;i++) {
                    strcpy(command_copy, new_command);
                    execute_command(command_copy, current_dir, home_dir, prev_dir);
                }
            }
        }
    }

    else if (strcmp(arguments[0], "history") == 0) {
        if (argument_count == 1) {
            int limit = history_count < 10 ? history_count : 10;

            for (int i = 0;i < limit;i++) {
                printf("%s\n", history_list[i]);
            }
        }

        else if (argument_count == 2) {
            int limit = atoi(arguments[1]);
            if (limit > history_count) {
                printf("history: history limit is %d\n", history_count);
            }

            else if (limit > MAX_HISTORY) {
                printf("history: history limit is %d\n", MAX_HISTORY);
            }

            else {
                for (int i = 0;i < limit;i++) {
                    printf("%s\n", history_list[i]);
                }
            }
        }

        else {
            printf("history: too many arguments\n");
        }
    }

    else if (strcmp(arguments[0], "jobs") == 0) {
        int s_flag = 0;
        int r_flag = 0;

        for (int i = 1;i < argument_count;i++) {
            if (strcmp(arguments[i], "-s") == 0) {
                s_flag = 1;
            }
            else if (strcmp(arguments[i], "-r") == 0) {
                r_flag = 1;
            }
            else if (strcmp(arguments[i], "-rs") == 0 || strcmp(arguments[i], "-sr") == 0) {
                s_flag = 1;
                r_flag = 1;
            }
        }

        jobs(child_list, children_count, r_flag, s_flag);
    }

    else if (strcmp(arguments[0], "sig") == 0) {
        if (argument_count == 1 || argument_count > 3) {
            printf("sig: format sig <job_index> <signal>\n");
        }
        else {
            int job_index = atoi(arguments[1]);
            int signal = atoi(arguments[2]);
            sig(job_index, signal, child_list, children_count);
        }
    }

    else if (strcmp(arguments[0], "fg") == 0) {
        if (argument_count == 1 || argument_count > 2) {
            printf("fg: format fg <job_index>\n");
        }
        else {
            int job_index = atoi(arguments[1]);
            fg(job_index, child_list, children_count);
        }
    }

    else if (strcmp(arguments[0], "bg") == 0) {
        if (argument_count == 1 || argument_count > 2) {
            printf("fg: format fg <job_index>\n");
        }
        else {
            int job_index = atoi(arguments[1]);
            bg(job_index, child_list, children_count);
        }
    }

    else {
        if (arguments[argument_count - 1][0] == '&') {

            if (writeRedir != 0)
                arguments[outfileLoc - 1] = NULL;
            if (readRedir != 0)
                arguments[infileLoc - 1] = NULL;

            background_process(arguments, argument_count);
        }
        else {

            if (writeRedir != 0)
                arguments[outfileLoc - 1] = NULL;
            if (readRedir != 0)
                arguments[infileLoc - 1] = NULL;

            foreground_process(arguments, argument_count);
        }
    }


    free(arguments);

    //resetting STDIN and STDOUT
    if (readRedir) {
        close(ipFd);
        dup2(stdinBackup, STDIN);
    }

    if (writeRedir) {
        close(opFd);
        dup2(stdoutBackup, STDOUT);
    }

    return;
}

int foreground_process(char** arguments, int argument_count) {
    arguments[argument_count] = NULL;

    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        // printf("Entered here\n");
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        setpgid(0, 0);
        if (execvp(arguments[0], arguments) == -1) {
            printf("rash: command not Found: %s\n", arguments[0]);
        }
        // exit(EXIT_FAILURE);
        return -1;
    }
    else if (pid < 0) {
        // Error forking
        perror("Error");
    }
    else {
        // Parent process
        signal(SIGTTOU, SIG_IGN), signal(SIGTTOU, SIG_IGN);
        setpgid(pid, 0);
        tcsetpgrp(STDIN, pid);
        add_child(pid, arguments[0]);
        wpid = waitpid(pid, &status, WUNTRACED);
        tcsetpgrp(STDIN, getpid());
        signal(SIGTTOU, SIG_DFL), signal(SIGTTOU, SIG_DFL);
    }

    return 1;
}

int background_process(char** arguments, int argument_count) {
    arguments[argument_count] = NULL;

    pid_t pid, wpid;

    pid = fork();
    if (pid == 0) {
        setpgid(0, 0);
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        if (execvp(arguments[0], arguments) == -1) {
            printf("rash: command not Found: %s\n", arguments[0]);
        }
        exit(EXIT_FAILURE);
        return -1;
    }
    else if (pid < 0) {
        // Error forking
        perror("Error");
    }
    else {
        // Parent process
        add_child(pid, arguments[0]);
        printf("%d\n", pid);
    }

    return 1;

}

void pinfo(char** arguments, int argument_count) {
    pid_t pid = 0;

    if (argument_count == 1) {
        pid = getpid();
    }
    else if (argument_count == 2) {
        pid = atoi(arguments[1]);
    }

    if (pid == 0) {
        pid = getpid();
    }

    char proc_file_path[MAX_PATH_LENGTH];
    sprintf(proc_file_path, "/proc/%d/stat", pid);
    char proc_file_contents[STAT_COUNT] = { '\0' };

    FILE* proc_file = fopen(proc_file_path, "r");
    if (proc_file == NULL) {
        printf("%s\n\n", strerror(errno));
        return;
    }

    fread(proc_file_contents, 1024, 1, proc_file);
    fclose(proc_file);

    char* attributes[STAT_COUNT] = { NULL };
    char* attribute = strtok(proc_file_contents, " ");
    int attribute_count = 0;
    while (attribute != NULL) {
        attributes[attribute_count] = attribute;
        attribute = strtok(NULL, " ");
        attribute_count++;
    }

    printf("pid -- %d\n", (int)pid);
    printf("Process Status -- %s\n", attributes[2]);
    printf("memory -- %s\n", attributes[22]);

    char exec_file_path[MAX_PATH_LENGTH];
    sprintf(exec_file_path, "/proc/%d/exe", (int)pid);
    char exec_path[MAX_PATH_LENGTH];
    memset(exec_path, 0, MAX_PATH_LENGTH);
    int read_stat = readlink(exec_file_path, exec_path, PATH_MAX);

    printf("Executable Path -- %s\n", read_stat == -1 ? "Doesn't exist" : exec_path);
}