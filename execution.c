#include "lex_analysis.h"
#include "parser.h"
#include "execution.h"
#include "free.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>



void execution(struct list * p, struct list * root) {
    int status = 0;
    exec_ampersand(p, root, &status);
    while(wait(&status) != -1){};
    free_root(root);
    if (WIFEXITED(status)) {
        exit (WEXITSTATUS(status));
    }
    else {
        exit (128 + WTERMSIG(status));
    } 
}

int exec_ampersand(struct list * p, struct list * root, int * status) {
    int pid_1 = exec_conjunction(p, root, status);
    //int status = 0;
    if (p->ampersand) {
        if (p->next != NULL) {
            //процесс для next
            if ((pid_1 = fork()) == 0) {

                do {
                    if (p->ampersand == 0)
                        waitpid(pid_1, status, 0);
                    pid_1 = exec_conjunction(p->next, root, status);
                    p = p->next;
                } while (p->next!= NULL);

                //waitpid(pid_1, status, 0);
                while(wait(status) != -1) {}
                free_root(root);
                if (WIFEXITED(*status)) {
                    exit (WEXITSTATUS(*status));
                }
                else {
                    exit (128 + WTERMSIG(*status));
                } 
            }
        }
    }
    return pid_1;

}

int exec_conjunction(struct list * p, struct list * root, int * status) {
	int pid_1 = exec_conveyor(p, root, status);
    int pid_2 = -1;

    if (p->and != NULL) {
        //процесс конъюнкции
        waitpid(pid_1, status, 0);
        if (!(WIFEXITED(*status) && (WEXITSTATUS(*status) == 0))) {
            return pid_1;
        }
        if ((pid_2 = fork()) == 0) {
            do {
                if (pid_2)
                    waitpid(pid_2, status, 0);
                if (!(WIFEXITED(*status) && (WEXITSTATUS(*status) == 0))) {
                    free_root(root);
                    if (WIFEXITED(*status)) {
                        exit (WEXITSTATUS(*status));
                    }
                    else {
                        exit (128 + WTERMSIG(*status));
                    } 
                }      
                pid_2 = exec_conveyor(p->and, root, status);
                p = p->and;  
            } while(p->and != NULL);
            //waitpid(pid_2, status, 0);
            while(wait(status) != -1) {}
            free_root(root);
            if (WIFEXITED(*status)) {
                exit (WEXITSTATUS(*status));
            }
            else {
                exit (128 + WTERMSIG(*status));
            } 
        }
        return pid_2;
    }
    return pid_1;
}

int exec_conveyor(struct list * p, struct list * root, int * status) {
	p = exec_input_and_output(p, root);
    int pid_1 = -1;
    if (p->pipe != NULL) {  
        //процесс для pipe
        if ((pid_1 = fork()) == 0) {
            int fd1[2], fd2[2];
            fd1[0] = 0; 
            fd1[1] = 1;
            do {
                if (p->pipe != NULL){
                    if (pipe(fd2) == -1) {
                        free_root(root);
                        exit(1);
                    }
                }
                else
                    fd2[1] = 1;
                if (p->dec_output != 1) {
                    if (p->pipe != NULL)
                        close(fd2[1]);
                    fd2[1] = p->dec_output;
                }

                if (p->dec_input != 0) {
                    if (fd1[0] != 0) 
                        close(fd1[0]);
                    fd1[0] = p->dec_input;
                }

                if ((pid_1 = fork()) == 0) {
                    if (fd1[0] == -1 || fd2[1] == -1) {
                        exit(1);
                    }
                    dup2(fd1[0], 0);
                    dup2(fd2[1], 1);
                    if (fd1[0] != 0)
                        close(fd1[0]);
                    if (fd1[1] != 1) 
                        close(fd1[1]);
                    if (p->pipe != NULL){
                        close(fd2[0]);
                        close(fd2[1]);
                    }
                    if (p->argv != NULL) {
                        execvp(p->argv[0], p->argv);
                        free_root(root);
                        exit(127);
                    }
                    if (p->psubcmd != NULL) {
                        if ((pid_1 = fork()) == 0) {
                            execution(p->psubcmd, root);
                            free_root(root);
                            exit(127);
                        }
                        waitpid(pid_1, status, 0);
                        free_root(root);
                        if (WIFEXITED(*status)) {
                            exit (WEXITSTATUS(*status));
                        }
                        else {
                            exit (128 + WTERMSIG(*status));
                        }
                    }
                }
                if (fd1[0] != 0)
                    close(fd1[0]);
                if (fd1[1] != 1) 
                    close(fd1[1]);
                fd1[0] = fd2[0];
                fd1[1] = fd2[1];
                p = p->pipe;
                p = exec_input_and_output(p, root);
        
            } while (p != NULL); 
            if (fd1[0] != 0)
                close(fd1[0]);
            if (fd1[1] != 1) 
                close(fd1[1]);

            waitpid(pid_1, status, 0);
            while(wait(NULL) != -1) {}
            free_root(root);
            if (WIFEXITED(*status)) {
                exit (WEXITSTATUS(*status));
            }
            else {
                exit (128 + WTERMSIG(*status));
            }   
        }
        return pid_1;
    }
    if (p->argv != NULL) {
        if ((pid_1 = fork())== 0) {
            if (p->dec_input == -1 || p->dec_output == -1) {
                free_root(root);
                exit(1);
            }
            dup2(p->dec_input, 0);
            dup2(p->dec_output, 1);
            execvp(p->argv[0], p->argv);
            free_root(root);
            exit(127);
        }
        return pid_1;
    }
    if (p->psubcmd != NULL) {
        if ((pid_1 = fork())== 0) {
            if (p->dec_input == -1 || p->dec_output == -1) {
                free_root(root);
                exit(1);
            }
            dup2(p->dec_input, 0);
            dup2(p->dec_output, 1);
            execution(p->psubcmd, root);
            free_root(root);
            exit(127);
        }
        return pid_1;
    }
    return pid_1;
}

struct list * exec_input_and_output(struct list * p, struct list * root) {
    if (p == NULL)
        return p;
    if (p->input != NULL) 
        p->dec_input = open(p->input, O_RDONLY);
    if (p->output != NULL)
        p->dec_output = creat(p->output, 0666);
    if (p->output_end != NULL)
        p->dec_output = open(p->output_end, O_WRONLY | O_CREAT | O_APPEND, 0666);
    return p;
}
