## To Compile 
`gcc subprocesses.c`

## Description
Sample output is provided below for running the program with each of the command line options. The program is meant to demonstrate what happens to the parent ID of a process when its parent terminates before it does. Additionally, the program answers the question of what happens to the parent ID of a grandchild process when the child (its parent) finishes executing before the parent (its grandparent) finishes--i.e., does the grandchild inherit the parent as ppid? 

When the user enters "1" as a command-line argument, the order of termination is grandchild, child, parent. In this case, no parent IDs are altered, because in each case the children finish executing before their respective parents. When the user enters "2" as a command-line argument, the order of termination is child, parent, grandchild. This cases answers the question posed above: when the child terminates, the grandchild does NOT inherit the parent's process ID as its own parent process ID. Instead, the grandchild is adopted by the init function, process ID 1. In the third case, where the user enters "3" as a command-line argument, the parent finishes first, then the child, then the grandchild. Here, the output is as expected. Once it is orphaned, the child's parent ID becomes 1. After the grandchild is orphaned, its parent ID also becomes 1. In each case, as the parent dies, the child is adopted by init with process ID 1. 

Written for an undergraduate Systems Programming Concepts course at UNO (CSCI 2467) in Spring 2016. 

