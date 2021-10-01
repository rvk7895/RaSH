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
            return ;
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
    return ;
}