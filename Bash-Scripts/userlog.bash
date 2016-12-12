#!/bin/bash

##############################################
# Takes a username as input and prints out	 #
# how many times that user is logged into 	 #
# the system.                              	 #
#                                          	 #
# Assignment: Homework 2                   	 #
# Author: Franklin D. Worrell              	 #
# Date: February 23, 2016                  	 # 
##############################################

##############################################
# Print prompt for user name, read input.  	 #
##############################################

echo -n "Please enter the user's username: " 
read USERNAME 

##############################################
# Get list of logged-in users from who, pipe #
# who's output into grep searching for the 	 #
# entered username, then count the lines   	 #
# that grep outputs.                       	 #
##############################################

who | grep "$USERNAME" | wc -l 