#!/bin/bash

##############################################
# Takes two directory names as arguments and # 
# prints out the names of files that are  	 #
# different between the two directories.  	 #
#                                          	 #
# Assignment: Homework 2                   	 #
# Author: Franklin D. Worrell              	 #
# Date: February 23, 2016                  	 # 
##############################################

##############################################
# Prompt user for directories, read input, 	 #
# store contents of each directory in text 	 #
# file.                                    	 #
##############################################

echo -n "Please enter first directory path: "
read DIRECTORY1
DIREDIT1=$(sed "s|~|$HOME|" <<< "$DIRECTORY1")
ls -p $DIREDIT1 | grep -v / > directory1.txt

echo -n "Please enter the second directory path: " 
read DIRECTORY2
DIREDIT2=$(sed "s|~|$HOME|" <<< "$DIRECTORY2")
ls -p $DIREDIT2 | grep -v / > directory2.txt

##############################################
# Use comm to output only the unique files.  #
##############################################

echo "These files are unique to $DIRECTORY1: " 
comm -23 directory1.txt directory2.txt 

echo

echo "These files are unique to $DIRECTORY2: " 
comm -13 directory1.txt directory2.txt





