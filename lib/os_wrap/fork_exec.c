#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

char *empty_env[1] = { NULL } ;

int SCPA_fork_exec_wait(char **argv, char *workDir) {

    int pid = fork() ;
    if (pid == -1) return 1 ;

    if (pid == 0) {
        if (chdir(workDir)) {
            exit(EXIT_FAILURE) ;
        }
        execve(argv[0], argv, empty_env) ;
        exit(EXIT_FAILURE) ;
    } else {
        int status ;
        do { wait(&status) ;} while(!WIFEXITED(status)) ;
        if (WEXITSTATUS(status) == EXIT_FAILURE) {
            return 1 ;
        }
        return 0 ;
    }
}