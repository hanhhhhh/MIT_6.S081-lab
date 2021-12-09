#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void filecmp(char *init_str,char*target_str){
    // printf("file:%s %s\n",init_str,target_str);
    // printf("file:%s %s\n",fmtname(init_str),target_str);
    if(strcmp(fmtname(init_str),target_str)==0){
        printf("%s\n",init_str);
    }
}

void find(char *init_str,char*target_str){
    // printf("find:%s %s\n",init_str,target_str);
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(init_str, 0)) < 0){
        fprintf(2, "ls: cannot open %s\n", init_str);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", init_str);
        close(fd);
        return;
    }
    switch (st.type) {
    case T_FILE:   //文件 直接比对文件名
        filecmp(init_str,target_str);
        break;
    case T_DIR:
        strcpy(buf, init_str);
        p = buf+strlen(buf);   //p是个指针，指向buf的下一位
        *p++ = '/'; 
        //对于目录下的所有文件
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            if(strcmp(".",de.name)==0 || strcmp("..",de.name)==0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            find(buf,target_str);
        }
            //如果是文件，比较
            //如果是目录，和文件夹下的所有文件比较  
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("too few parameters\n");
        exit(1);
    }
    find(argv[1],argv[2]);
    exit(0);
}
