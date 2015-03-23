#!/usr/bin/env python
# Choice (boys)
import sys, random, socket

# handle bad cases
if(len(sys.argv) == 1):
    print("Usage: choice CHOICE1 CHOICE2")
    sys.exit()

# yo ansi boys
UNDERLINE = '\033[4m'
END = '\033[0m'

# don't need the first argument...
sys.argv.pop(0)

# variables
choices = len(sys.argv)
chii = "chii@" + UNDERLINE + socket.gethostname() + END + ">"
choice = sys.argv[random.randrange(0,choices)] + ","

# get rid of whitespace
chii.replace(" ","")
choice.replace(" ","")

# make a choice!
print(chii,choice," obviously!")
