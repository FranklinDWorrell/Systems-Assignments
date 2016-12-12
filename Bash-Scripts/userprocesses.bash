#!/bin/bash 

##############################################
# Polls running processes every 2 seconds 	 #
# and outputs the processes belonging to the #
# current user, clearing the screen with  	 #
# each poll to the processes.              	 #
#                                          	 #
# Assignment: Homework 2                   	 #
# Author: Franklin D. Worrell              	 #
# Date: February 23, 2016                  	 # 
##############################################

##############################################
# Get and store user name of current user.   #
##############################################

CURRENT_USER=$(whoami)

##############################################
# Create infinite loop--user must use ^C to  #
# terminate process--output user's processes #
# and sleep for 2 seconds before repeating.	 #
##############################################

while [ 0 ]; do
	clear
	ps -cu $CURRENT_USER
	sleep 2
done
