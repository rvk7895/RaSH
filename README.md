# RaSH

By: Ritvik Aryan Kalra\
Roll number: 2019115002

## File Description

`rash.c` - Contains the main loop for the shell and calls for the initialization funcitons.

`builtin_commands.c` - Contains the functions and their helper functions for the builtin commands of the shell like `ls`, `cd` and so on.

`functions.c` - Contains the functions that are required for the functioning of the shell like parsing the input and executing the commands.

## Executing

```
make
```

## Functionalities of the shell

* Can run foreground and background processes
* Has builtin commands like `ls` `cd` `echo` `pwd`
* Displays notification when a background process has exited
* Has the command `pinfo` to check for information of processes in shell.
* `history` command to check for history. (Doesn't store history across multiple sessions)
