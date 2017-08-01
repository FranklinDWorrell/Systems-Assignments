## Description
A basic implementation of the ls command with added functionality that allows for (1) default listing of current directory if no location is specified by the user, (2) use of the -a flag, and (3) use of the -r flag. Additionally, output is listed in the same order as the system ls command. 

Written for undergraduate course in Systems Programming Concepts (CSCI 2467) in Spring 2016. 

## To Compile
Requires library provided with Advanced Programming in the Unix Environment, which needs to be downloaded and linked in at the command line. 

In directory containing code:
`gcc -I../../apue.3e/include -L../../apue.3e/lib  myls.c -lapue`

