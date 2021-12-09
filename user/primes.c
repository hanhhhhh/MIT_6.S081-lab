#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

void sys_err(const char *str)
{
    fprintf(2,str);
    exit(1);
}

void child(int ld[]){
    close(ld[WRITE]);    //left neighbor
    int rd[2];   //right neighbor
    if(pipe(rd)==-1) sys_err("error:pipe rd");
    int p;
    if(read(ld[READ], &p, sizeof(p))==0){  //get a number from left neighbor
        exit(0);
    }
    int n;
    printf("prime %d\n",p);
    //为素数 fork新进程
    int pid=fork();
    if(pid<0){
        sys_err("fork err");
    }
    else if(pid==0){
        close(rd[WRITE]);
        close(ld[READ]);
        child(rd);
    }
    else{   //父进程
        close(rd[READ]);
        while(read(ld[READ], &n, sizeof(n))!=0){   //get a number from left neighbor
            if(n%p!=0){
                //send n to right neighbor
                write(rd[WRITE], &n, sizeof(n)); 
            }
        }
        close(ld[READ]);
        close(rd[WRITE]);
        wait(0);
    }
    exit(0);
}

int
main(int argc, char *argv[]){
    if(argc>1) sys_err("input too many arg\n");
    int fd[2];
    if(pipe(fd)==-1) sys_err("pipefd\n");
    int pid=fork();
    if(pid<0){
        sys_err("fork err\n");
    }
    else if(pid==0){   //子进程
        child(fd);
    }
    else{  //父进程
        close(fd[READ]);
        //feed the number 2 through 35 into the pipeline
        for(int i=2;i<=35;i++){
            write(fd[WRITE], &i, sizeof(i)); 
        }
        close(fd[WRITE]);
        wait(0);
    }
    exit(0);
}