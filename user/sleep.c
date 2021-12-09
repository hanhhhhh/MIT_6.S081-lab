#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

    if(argc==1){   //用户忘记传参，提示错误信息
        write(1, "error:no argument\n", 18);
        exit(1);
    }
    int time=atoi(argv[1]);
    sleep(time);
    exit(0);
}
