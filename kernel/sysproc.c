#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "process.h"
#include "pstat.h"

extern struct process_control_block process_table[NPROC]; //array of 64 pcb structs

uint64 sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64 sys_getpid(void)
{
  return myproc()->pid;
}

uint64 sys_fork(void)
{
  return fork();
}

uint64 sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64 sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64 sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (killed(myproc()))
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_setPriority(void)
{
  int pid;
  int priority;
  argint(0, &pid);
  argint(1, &priority);

  if (priority < -20 || priority > 20)
  {
    return -1;
  }

  struct process_control_block *p;
  for (p = process_table; p < &process_table[NPROC]; p++)
  {
    acquire(&p->lock);
    if (p->pid == pid)
    {
      p->real_priority = priority;
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  return -1;
}

uint64 sys_setEffectivePriority(void)
{
  int pid;
  int priority;
  argint(0, &pid);
  argint(1, &priority);

  if (priority < -20 || priority > 20)
  {
    return -1;
  }

  struct process_control_block *p;
  for (p = process_table; p < &process_table[NPROC]; p++)
  {
    acquire(&p->lock);
    if (p->pid == pid)
    {
      p->effective_priority = priority;
      release(&p->lock);
      return 0;
    }
    release(&p->lock);
  }
  return -1;
}

uint64 sys_getpinfo(void)
{
  uint64 dst;
  argaddr(0, &dst);

  struct process_control_block *p;
  struct pstat copied_pstat;
  int i = 0;

  for (p = process_table; p < &process_table[NPROC]; p++)
  {
    acquire(&p->lock);

    copied_pstat.state[i] = p->state;
    //should i check inuse before all this?
    copied_pstat.effective_priority[i] = p->effective_priority;
    copied_pstat.real_priority[i] = p->real_priority;
    copied_pstat.pid[i] = p->pid;
    copied_pstat.ticks[i] = p->ticks;

    release(&p->lock);
    i++;
  }

  //copyout

  return 0;
}