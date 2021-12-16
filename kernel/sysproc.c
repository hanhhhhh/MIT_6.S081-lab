#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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


#ifdef LAB_PGTBL
extern pte_t * walk(pagetable_t,uint64,int); // trampoline.S
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  //arg1:要检查的第一个用户页的起始虚地址
  //arg2:要检查的页面number
  //arg3:指向缓存的用户地址，来把结果存到一个bitmask，每页使用一位的数据结构，其中第一页对应于最低有效位
  uint64 start_va;  
  if(argaddr(0, &start_va) < 0)
    return -1;
  int number;
  if(argint(1, &number) < 0)
    return -1;
  uint64 ua;
  if(argaddr(2, &ua) < 0)
    return -1;
  uint64 resultmask=1;
  uint64 bitmask=0;
  uint64 PTE_A_mask=~PTE_A;
  if(number>64){
    printf("panic! number of page over the upper limit");
    return -1;
  }
  uint64 va=start_va;
  pte_t *pte;
  for(int i=0;i<number;i++){
    pte=walk(myproc()->pagetable,va,0);
        //判断是否访问过  
    if((*pte)&PTE_A){    //访问过
      // printf("hello\n");
      *pte=*pte & PTE_A_mask;   //访问过的clear PTE_A
      bitmask=bitmask|(resultmask<<i);  
      // printf("bit:%p\n",bitmask); 
    }
    va+=PGSIZE;
  }
  copyout(myproc()->pagetable,ua,(char *)&bitmask,sizeof(uint64));
  return 0;
}
#endif

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
