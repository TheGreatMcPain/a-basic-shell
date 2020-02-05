Author: James McClain
Program Name: shell
Compile: gcc -o shell -O3 shell.c
Run: ./shell

1. Callenges that I ran into:

    One if the first things I ran into had to do with
gcc complaining about ignoring that return value of fgets().
Which on it's own seems pretty harmless, but I wanted to silence
that warning, so the first thing I tried came from
here: (https://stackoverflow.com/questions/11888594/ignoring-return-values-in-c)
which looked like this (void)(fgets(cmd, MAX_LINE, stdin)+1).
I later realized how ugly that solution was, so I decided to put
fgets() in an 'if' statement. 

    When I was tring to figure out how to remove the newline character
from my fgets() string I came across strcspn(), and used it like this
'cmd[strcspn(cmd, "\n")] = 0;' which came from
here: (https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input)
I later figured out that I could just add '\n' to my strtok() arguments like
this 'token = strtok(cmd, " \n")' which will take care of newlines, and
spaces.

    One thing I found is how "slow" clearing an array with memset()
is which added a bit of delay between 'printf("prompt$ ")' statements if
memset() is on the top of the while loop. To get around this delay
I added a function that checks if a string only contains whitespace called
'isEmpty()'. The code for 'isEmpty()' comes from 'filterStr()'
inside of Project 0, but is modified to only check for spaces using 'isspace()'.
Using 'isEmpty()' I check if the string from 'fgets()' is 'empty', and if it is I just
skip the rest of the code and go to the top of the loop. I also moved memset()
below my isEmpty() statement.

2. Other Notes.

    I decided to think ahead a bit, and moved my execvp()/fork() code over
to a separate function in the hopes that it will make life easier when I
implement 'showpid' later on. The arguments from that function,
which is: 'char *const args[]', came from execvp()'s manpage.
