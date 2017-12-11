#!/bin/bash
# Terminal echo off

echo -n 'Password: '

# Close echo 
stty -echo

# This section cannot see from the screen 
read password
echo 'hidden section'

# Open echo 
stty echo

echo "The password is: "
echo $password
