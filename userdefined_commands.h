#ifndef USERDEFINED_COMMANDS_H
#define USERDEFINED_COMMANDS_H

void jobs(struct child *child_list, int child_count, int r_flag, int s_flag);

void sig(int index, int sig, struct child *child_list, int child_count);

void fg(int index, struct child *child_list, int child_count);

void bg(int index, struct child *child_list, int child_count);

#endif