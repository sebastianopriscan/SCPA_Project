#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int SCPA_fork_exec_wait(char **argv, char **envp) {

    int pid = fork() ;
    if (pid == -1) return 1 ;

    if (pid == 0) {
        execve(argv[0], argv, envp) ;
        exit(EXIT_FAILURE) ;
    } else {
        int status ;
        do { wait(&status) ;} while(!WIFEXITED(status)) ;
        if (WEXITSTATUS(status) == EXIT_FAILURE) {
            return 1 ;
        }
    }
}