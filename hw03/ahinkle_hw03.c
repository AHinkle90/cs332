/*
Name: Alex Hinkle
BlazerId: ahinkle
Project #: 3
To compile: make
To run: ./hw3 <directory>
        ./hw3 -u <username> <directory>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#include <limits.h>
#include <errno.h>

static int isTextFile(const char *fileName);
static long countWords(const char *pathname);
static void printFile(const char *pathname, const char *fileName,
                      const struct stat *statbuf, int showOwner);
static void printUsage(const char *programName);

int main(int argc, char **argv) {
char *directoryName;
char *username = NULL;
struct passwd *userInfo = NULL;
DIR *directory;
struct dirent *entry;
int showOwner = 0;
int childCount = 0;
int status;

/* Process command-line arguments */
if (argc == 2) {
directoryName = argv[1];
} else if (argc == 4 && strcmp(argv[1], "-u") == 0) {
showOwner = 1;
username = argv[2];
directoryName = argv[3];
userInfo = getpwnam(username);
if (userInfo == NULL) {
printf("Error: User not found.\n");
return 1;
}
} else {
printUsage(argv[0]);
return 1;
}

/* Open directory */
directory = opendir(directoryName);
if (directory == NULL) {
printf("Error: Directory not found.\n");
return 1;
}

/* Read each entry in the directory */
while ((entry = readdir(directory)) != NULL) {
char pathname[PATH_MAX];
struct stat statbuf;
pid_t pid;

/* Skip current and parent directories */
if (strcmp(entry->d_name, ".") == 0 ||
strcmp(entry->d_name, "..") == 0) {
continue;
}

/* Build complete path */
if (snprintf(pathname, sizeof(pathname), "%s/%s",
directoryName, entry->d_name) >= (int) sizeof(pathname)) {
printf("Error: Path is too long.\n");
continue;
}

/* Get file information */
if (lstat(pathname, &statbuf) < 0) {
printf("Error: Cannot access file %s.\n", entry->d_name);
continue;
}

/* Only process regular files */
if (!S_ISREG(statbuf.st_mode)) {
continue;
}

/* Skip files not owned by the requested user */
if (userInfo != NULL && statbuf.st_uid != userInfo->pw_uid) {
continue;
}

/* Create one child process for each file */
pid = fork();
if (pid < 0) {
printf("Error: fork failed.\n");
closedir(directory);
while (childCount > 0) {
wait(NULL);
childCount--;
}
return 1;
}

if (pid == 0) {
closedir(directory);
printFile(pathname, entry->d_name, &statbuf, showOwner);
exit(0);
}

childCount++;
}

closedir(directory);

/* Parent waits for all children */
while (childCount > 0) {
if (wait(&status) < 0) {
if (errno == EINTR) {
continue;
}
printf("Error: wait failed.\n");
return 1;
}
childCount--;
}

return 0;
}

/* Returns 1 if the file ends in .txt */
static int isTextFile(const char *fileName) {
const char *extension = strrchr(fileName, '.');
if (extension != NULL && strcmp(extension, ".txt") == 0) {
return 1;
}
return 0;
}

/* Counts words in a text file */
static long countWords(const char *pathname) {
FILE *file;
char word[256];
long words = 0;

file = fopen(pathname, "r");
if (file == NULL) {
return -1;
}

while (fscanf(file, "%255s", word) == 1) {
words++;
}

if (ferror(file)) {
fclose(file);
return -1;
}

fclose(file);
return words;
}

/* Prints file size and word count from the child process */
static void printFile(const char *pathname, const char *fileName,
                      const struct stat *statbuf, int showOwner) {
long words;
struct passwd *owner;

printf("File: %s | Size: %ld bytes", fileName, (long) statbuf->st_size);

if (isTextFile(fileName)) {
words = countWords(pathname);
if (words < 0) {
printf(" | Error: File could not be read");
} else {
printf(" | Words: %ld", words);
}
} else {
printf(" | Words: N/A");
}

if (showOwner) {
owner = getpwuid(statbuf->st_uid);
if (owner != NULL) {
printf(" | Owner: %s", owner->pw_name);
} else {
printf(" | Owner: unknown");
}
}

printf("\n");
}

static void printUsage(const char *programName) {
printf("Usage: %s <directory_name>\n", programName);
printf("       %s -u <username> <directory_name>\n", programName);
}
