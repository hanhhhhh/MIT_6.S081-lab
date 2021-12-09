/*
父进程：
向子进程发送一个字节  close 读

子进程：close 写 print"<pid>: received ping"，pid是他的进程ID，
向朝向他父进程的pipe写一个字节，
然后exit

父进程读子进程发送的字节，print "<pid>: received pong"，然后退出
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ 0
#define WRITE 1
// #include "string.h"
void sys_err(const char *str)
{
    fprintf(2,str);
    exit(1);
    
}
void main(void)
{
    int pid;
    char buf[1024];
    int fd1[2];   //父进程向子进程发送数据
    int fd2[2];   //子进程向父进程发送数据
    char *p1 = "ping\n";
    char *p2 = "pong\n";
    
    if (pipe(fd1) == -1){    ///fd[0] → r； fd[1] → w
        sys_err("pipe1");
    }
    if (pipe(fd2) == -1) {  
        sys_err("pipe2");
        
        // sys_err("pipe");
    }
    
    pid = fork();
    if (pid < 0) {
        sys_err("fork err");
        // write(1,"fork err\n",9);
    } else if (pid == 0) {   //子进程
        close(fd1[WRITE]);   
        close(fd2[READ]);

        //read from fd1
        read(fd1[READ], buf, sizeof(buf));   //子进程从pipe中读出ping
        printf("%d: received ping\n",getpid());
        close(fd1[READ]);
        
        //write to fd2
        write(fd2[WRITE], p2, strlen(p2)); 
        close(fd2[WRITE]);
        exit(0);
    } else {   //父进程
        close(fd1[READ]);
        close(fd2[WRITE]);

        //write to fd1
        write(fd1[WRITE], p1, strlen(p1));   //父进程向子进程发送ping，写入pipe
        wait(0);
        close(fd1[WRITE]);

        //read from fd2
        read(fd2[READ], buf, sizeof(buf));
        printf("%d: received pong\n",getpid());
        close(fd2[READ]);
    }
    exit(0);
}