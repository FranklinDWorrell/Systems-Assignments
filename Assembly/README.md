## Description
This directory contains programs written for a course in Assembly Language (CSCI 2450) taken in Spring 2016. The language is 32-bit Microsoft Macro Assembler (MASM). Two programs, Fibonacci.asm and Encryption.asm make use of the the Irvine32 library provided with Kip Irvine's textbook "Assembly Language for x86 Processors"--the textbook used for the course. 

1. Fibonacci.asm is designed to illustrate recursion in MASM and computes and prints the third Fibonacci number. 

1. Encryption.asm takes an input file name, an encryption key string, and an output file name and uses the key to encrypt the input file and write it to the output file. 

1. Virus.asm, was the final project for the course. It is a virus. IT IS POSTED HERE ONLY FOR ACADEMIC/ILLUSTRATIVE PURPOSES AND IS NOT MEANT TO BE COMPILED OR RUN ON ANY SYSTEM. It makes use of the MASM32 library, and instructions for finding and acquiring that library are given in the documentation in that .asm file. The virus creates a copy of its executable code and stores it at a specified location in the file system. It adds that executable to startup registry key. Next, it creates a new user account with administrative privileges on the system. It finally creates a text file in a specified directory that mocks the infected user. Information reguarding the environment in which the program was developed can be found in that file. 

