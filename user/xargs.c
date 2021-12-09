#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

void sys_err(const char *str)
{
    fprintf(2,str);
    exit(1);
     
}
int main(int argc, char *argv[]){
    char buf[512];
    if( argc<2) sys_err("xargs <command>");

    //将参数传入argv
    char *params[MAXARG];
    for(int i=0;i<argc-1;i++){
        params[i]=argv[i+1];
    }
    //params[0]为command
    //params[1]开始是参数
    //params[argc]是null
    //由于argv[0]=xargs,params中有效项一共有argc-1个
    //params[argc-1]用来存放从标准输入读入的参数
    char *cmd=argv[1];   //command
    //一个char一个char地读
    int i=0;
    int n=0;
    while(1){   //read lines
        i=0;
        //each line
        while((n=read(0,&buf[i],1))!=0){
            // printf("%c\n",buf[i]);
            //n==0表示所有参数都读完
            //buf[i]=='\n'表示该行读完
            if(n==0 || buf[i]=='\n'){
                break;
            }
            i++;
        }
        if(i==0){   //没有读出
            break;
        }
        buf[i]=0;
        params[argc-1]=buf;
        // printf("%s\n",params[0]);
        if(fork()==0){
            exec(cmd,params);
            exit(0);
        }
        else{
            wait(0);
        }
    }
    exit(0);
}