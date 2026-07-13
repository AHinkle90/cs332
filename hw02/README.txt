This program allows for searching through a file hierarchy 
and displays files and directories.

The following commands are supported: 

-S

-s <size>

-f <pattern> <depth>

-t f

-t d

These commands can be used together. 

To compile:
make

To run:
./search [options] [directory]

make clean
Removes the executable.  

Testing:
I tested the code using the projects.tar file. I went through and
attempted all the commands with no issues. The executable succesfully allows for searching 
and listing directories. I also created a folder for myself to test with 
called "testdir" with a couple blankfiles inside and it worked just the same as with projects.tar. 
I had an issue where I couldn't get the docx to appear in the search 
with the command ./search -f docx 1 projects but it was due to my depth 
being incorrect. Make clean is used so you can rebuild the executable 
with the newest version of code. Combining commands in one input
worked as well. Ex: ./search -S -s 100 -f docx 2 projects