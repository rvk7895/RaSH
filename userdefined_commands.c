#include "functions.h"
#include "definitions.h"
#include "builtin_commands.h"
#include "userdefined_commands.h"

void sort_children(struct child* arr, int count) {
    struct child temp;

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count - 1; j++) {
            int comp = strcmp(arr[i].name, arr[j].name);
            if (comp > 0) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

void jobs(struct child* child_list, int child_count, int r_flag, int s_flag) {
    struct child child_list_copy[child_count];

    for (int i = 0; i < child_count; i++) {
        child_list_copy[i] = child_list[i];
    }

    sort_children(child_list_copy, child_count);

    char file_path[MAX_PATH_LENGTH];
    char stat_read[1024];
    FILE* proc_stat;

    for (int i = 0;i < child_count;i++) {
        sprintf(file_path, "/proc/%d/stat", child_list[i].pid);
        proc_stat = fopen(file_path, "r");
        if (proc_stat == NULL) {
            perror("fopen");
            return;
        }

        fgets(stat_read, 1024, proc_stat);
        fclose(proc_stat);

        // getting the running stat 
        // its the 3rd element in the stat file
        char* running_stat = strtok(stat_read, " ");
        for (int j = 0;j < 2;j++) {
            running_stat = strtok(NULL, " ");
        }
        if (r_flag || s_flag) {
            if (strcmp(running_stat, "R") == 0 && r_flag == 1) {
                printf("[%d] Running %s [%d]\n", i + 1, child_list[i].name, (int)child_list[i].pid);
            }

            if (strcmp(running_stat, "S") == 0 && s_flag == 1) {
                printf("[%d] Stopped %s [%d]\n", i + 1, child_list[i].name, (int)child_list[i].pid);
            }
        }

        else {
            printf("[%d] %s %s [%d]\n", i + 1, running_stat[0] == 'S' ? "Stopped" : "Running", child_list[i].name, (int)child_list[i].pid);
        }
    }
    return;
}

void sig(int index, int sig, struct child* child_list, int child_count) {
    struct child child_list_copy[child_count];

    for (int i = 0; i < child_count; i++) {
        child_list_copy[i] = child_list[i];
    }

    sort_children(child_list_copy, child_count);

    if (index > child_count || index < 1) {
        printf("No such job\n");
        return;
    }

    int pid = child_list[index - 1].pid;
    kill(pid, sig);
    return;
}

void fg(int index, struct child* child_list, int child_count) {
    struct child child_list_copy[child_count];

    for (int i = 0; i < child_count; i++) {
        child_list_copy[i] = child_list[i];
    }

    sort_children(child_list_copy, child_count);

    if (index > child_count || index < 1) {
        printf("No such job\n");
        return;
    }

    int pid = child_list[index - 1].pid;
    char process_name[MAX_PATH_LENGTH];
    strcpy(process_name, child_list[index - 1].name);

    delete_child(pid);

    //ignoring signals
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    //setting the foreground process
    int flag = tcsetpgrp(STDIN, getpgid(pid));

    if (flag) {
        printf("Could not give terminal control to the job\n");
        perror("fg:");
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        return;
    }

    if (kill(pid, SIGCONT))
    {
        printf("Unable to resume job %d\n", index);
        perror("fg:");
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        return;
    }

    int status;
    if (waitpid(pid, &status, WUNTRACED) > 0) {
        if (WIFSTOPPED(status)) {
            add_child(pid, process_name);
        }
    }

    if(tcsetpgrp(STDIN, getpgid(0)))
    {
        printf("Could not return terminal controll to the shell. Exitting the shell\n");
        perror("fg:");
    }

    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
}

void bg(int index, struct child* child_list, int child_count) {
    struct child child_list_copy[child_count];

    for (int i = 0; i < child_count; i++) {
        child_list_copy[i] = child_list[i];
    }

    sort_children(child_list_copy, child_count);

    if (index > child_count || index < 1) {
        printf("No such job\n");
        return ;
    }

    int pid = child_list[index - 1].pid;
    char process_name[MAX_PATH_LENGTH];
    strcpy(process_name, child_list[index - 1].name);

    delete_child(pid);

    if (kill(pid, SIGCONT))
    {
        printf("Unable to resume job %d\n", index);
        perror("bg:");
        return ;
    }

    add_child(pid, process_name);
    return ;
}



