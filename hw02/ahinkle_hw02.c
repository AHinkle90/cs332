/*
Name: Alex Hinkle
BlazerId: ahinkle
Project #: 2
To compile: make
To run: ./search [command]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>

#define FTW_F   1   /* file */
#define FTW_D   2   /* directory */
#define FTW_NS  3   /* stat error */

typedef struct {
    int showInfo;   /* -S */
    int useSize;    /* -s */
    long maxSize;
    int usePattern; /* -f */
    char pattern[256];
    int maxDepth;

    int typeFilter; /* 0 = all, 1 = files only, 2 = directories only */
} Options;

/*
Function pointer (referenced figure 4.22). 
This function is called for every file or directory found.
*/
typedef int SearchFunc(const char *pathname,
                       const char *name,
                       const struct stat *statptr,
                       int type,
                       int depth,
                       Options *options);

static int searchTree(const char *pathname, SearchFunc *func, Options *options);
static int doPath(const char *pathname, const char *name, SearchFunc *func,
                  int depth, Options *options);

static int printFile(const char *pathname,
                     const char *name,
                     const struct stat *statptr,
                     int type,
                     int depth,
                     Options *options);

static int shouldPrint(const char *name,
                       const struct stat *statptr,
                       int type,
                       int depth,
                       Options *options);

static void printTabs(int depth);
static void getPermissions(mode_t mode, char *permissions);
static void printUsage(const char *programName);

int main(int argc, char **argv) {
    Options options;
    int opt;
    /* Uses current directory when none is provided */
    char *startDirectory = ".";

    options.showInfo = 0;
    options.useSize = 0;
    options.maxSize = 0;
    options.usePattern = 0;
    options.pattern[0] = '\0';
    options.maxDepth = 0;
    options.typeFilter = 0;
/* Process input commands */ 
    while ((opt = getopt(argc, argv, "Ss:f:t:")) != -1) {
        switch (opt) {
            case 'S':
                options.showInfo = 1;
                break;

            case 's':
                options.useSize = 1;
                options.maxSize = atol(optarg);
                break;

            case 'f':
                options.usePattern = 1;
                strncpy(options.pattern, optarg, sizeof(options.pattern) - 1);
                options.pattern[sizeof(options.pattern) - 1] = '\0';

                if (optind >= argc) {
                    printf("Error: -f requires a pattern and depth.\n");
                    printUsage(argv[0]);
                    return 1;
                }

                options.maxDepth = atoi(argv[optind]);
                optind++;
                break;

            case 't':
                if (strcmp(optarg, "f") == 0) {
                    options.typeFilter = 1;
                } else if (strcmp(optarg, "d") == 0) {
                    options.typeFilter = 2;
                } else {
                    printf("Error: -t must be followed by f or d.\n");
                    return 1;
                }
                break;

            default:
                printUsage(argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        startDirectory = argv[optind];
    }

    searchTree(startDirectory, printFile, &options);

    return 0;
}

static int searchTree(const char *pathname, SearchFunc *func, Options *options) {
    return doPath(pathname, pathname, func, 0, options);
}

static int doPath(const char *pathname, const char *name, SearchFunc *func,
                  int depth, Options *options) {
    struct stat statbuf;
    struct dirent *dirent;
    DIR *dir;
    char childPath[PATH_MAX];
/* Get file info without symbolic links*/
    if (lstat(pathname, &statbuf) < 0) {
        return func(pathname, name, &statbuf, FTW_NS, depth, options);
    }

    if (S_ISDIR(statbuf.st_mode)) {
        func(pathname, name, &statbuf, FTW_D, depth, options);

        dir = opendir(pathname);
/* If dir cannot be opened */
        if (dir == NULL) {
            printf("Error opening directory '%s'\n", pathname);
            return 1;
        }
/* Prevents infinite loop */
        while ((dirent = readdir(dir)) != NULL) {
            if (strcmp(dirent->d_name, ".") == 0 ||
                strcmp(dirent->d_name, "..") == 0) {
                continue;
            }
/* Build complete path */
            snprintf(childPath, sizeof(childPath), "%s/%s",
                     pathname, dirent->d_name);

            doPath(childPath, dirent->d_name, func, depth + 1, options);
        }

        closedir(dir);
    } else {
        func(pathname, name, &statbuf, FTW_F, depth, options);
    }

    return 0;
}

static int printFile(const char *pathname,
                     const char *name,
                     const struct stat *statptr,
                     int type,
                     int depth,
                     Options *options) {
    if (type == FTW_NS) {
        printTabs(depth);
        printf("%s -- stat error\n", name);
        return 0;
    }

    if (!shouldPrint(name, statptr, type, depth, options)) {
        return 0;
    }

    printTabs(depth);
/* Display symbolic link info */
    if (S_ISLNK(statptr->st_mode)) {
        char target[PATH_MAX];
        ssize_t len;

        len = readlink(pathname, target, sizeof(target) - 1);

        if (len != -1) {
            target[len] = '\0';
            printf("%s (%s)", name, target);
        } else {
            printf("%s", name);
        }
    } else {
        printf("%s", name);
    }

    if (options->showInfo) {
        char permissions[11];
        char timeBuffer[64];
        struct tm *timeInfo;
        long size;

        getPermissions(statptr->st_mode, permissions);

        timeInfo = localtime(&(statptr->st_atime));
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);
/* Directories print 0. */
        if (type == FTW_D) {
            size = 0;
        } else {
            size = (long) statptr->st_size;
        }

        printf(" (%ld bytes, %s, %s)", size, permissions, timeBuffer);
    }

    printf("\n");

    return 0;
}
/* Size, depth, string pattern, file/directory. Checks file info 
in statptr against filters from options */
static int shouldPrint(const char *name,
                       const struct stat *statptr,
                       int type,
                       int depth,
                       Options *options) {
    long size;

    if (type == FTW_D) {
        size = 0;
    } else {
        size = (long) statptr->st_size;
    }

    if (options->useSize && size > options->maxSize) {
        return 0;
    }

    if (options->usePattern) {
        if (strstr(name, options->pattern) == NULL) {
            return 0;
        }

        if (depth > options->maxDepth) {
            return 0;
        }
    }

    if (options->typeFilter == 1 && !S_ISREG(statptr->st_mode)) {
        return 0;
    }

    if (options->typeFilter == 2 && !S_ISDIR(statptr->st_mode)) {
        return 0;
    }

    return 1;
}
/* Indents based on directory depth */
static void printTabs(int depth) {
    int i;

    for (i = 0; i < depth; i++) {
        printf("\t");
    }
}
/* Convert permission bits to string */
static void getPermissions(mode_t mode, char *permissions) {
    permissions[0] = S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' : '-';

    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';

    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';

    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';

    permissions[10] = '\0';
}

static void printUsage(const char *programName) {
    printf("Usage:\n");
    printf("  %s [directory]\n", programName);
    printf("  %s -S [directory]\n", programName);
    printf("  %s -s <size> [directory]\n", programName);
    printf("  %s -f <pattern> <depth> [directory]\n", programName);
    printf("  %s -S -s <size> -f <pattern> <depth> [directory]\n", programName);
    printf("  %s -t f [directory]\n", programName);
    printf("  %s -t d [directory]\n", programName);
}