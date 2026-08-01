This program accepts a directory as a command-line argument and
traverses the directory to list all files, excluding subdirectories. A child process is created for each file. The parent process waits for all child processes to finish before exiting. The program also includes error handling for edge cases and when files can't be opened or read. 

For each file, the program displays:

- File name
- File size in bytes
- Word count

The following command-line option is also supported:
-u <username>

The -u option filters the results and only displays files owned by the specified user. It also displays the username. 

To compile:
make

To run:
./hw3 <directory>

Ex:
./hw3 testdir

To run with username:
./hw3 -u codespace testdir

To remove the executable:
make clean

Testing:

I created a folder called "testdir" with two .txt files, an empty .txt file, a file that was not a .txt file (.jpg), and a subdirectory. The program accurately displayed the file names, sizes, and word counts. It did not display the subdirectory. The .jpg file returned N/A for word count. The empty .txt file returned zero words and zero bytes. 

I also tested the program without a directory and with an
incorrect directory name. The program displayed the usage message when no argument was provided and displayed an error when the directory did not exist. An empty directory ended the program but did not return an error. 

I tested the username option using:
./hw3 -u codespace testdir

The program displayed all files owned by the entered user and printed the username along with the file name, size, and word count. 