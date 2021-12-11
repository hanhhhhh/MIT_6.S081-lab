#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


//trace function
uint64
sys_trace(){
  //remembering its argument in a new variable in the proc structure
  int mask;
  if(argint(0, &mask) < 0)     //将p-trapframe->a0的值读出存到mask中
    return -1;
  myproc()->trace_mask=mask;
  return 0;
}

//sysinfo function
//sys_function都是没有形参的
// int sysinfo(struct sysinfo *info){ user中的sysinfo是有参数的，但其系统调用kernel中的sys_sysinfo是没有参数的，参数存在trapframe中，需要从trapfram中取出
extern uint64 get_freemem(void);
extern uint64 get_procnum(void);

int sys_sysinfo(void){
  //dst
  uint64 dst; // user pointer to struct stat
  //sysinfo只有唯一一个参数
  if(argaddr(0, &dst) < 0)
    return -1;
  struct sysinfo tempinfo;
  tempinfo.freemem=get_freemem();
  tempinfo.nproc=get_procnum();
  //Copy from kernel to &info.
  if(copyout(myproc()->pagetable,dst,(char *)&tempinfo,sizeof(tempinfo))){
     return -1;
  }   
  return 0;
}
