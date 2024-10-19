#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/pstat.h"
#include "user/user.h"

int main (int argc, char *argv[])
{
    struct pstat ps;
    
    if (argc <= 1)
    {
        getpinfo(&ps);
    }

    printf("NAME\tPID\tSTATUS\t\tPRIORITY\n");

    for (int i = 0; i < NPROC; i++)
    {
        if(ps.inuse[i])
        {
            printf("%s\t%d\t", ps.name[i], ps.pid[i]);

            switch(ps.state[i])
            {
                case USED:
                    printf("USED\t\t");
                    break;
                case UNUSED:
                    printf("UNUSED\t\t");
                    break;
                case SLEEPING:
                    printf("SLEEPING\t");
                    break;
                case RUNNABLE:
                    printf("RUNNABLE\t");
                    break;
                case RUNNING:
                    printf("RUNNING\t\t");
                    break;
                case ZOMBIE:
                    printf("ZOMBIE\t\t");
                    break;
            }

            printf("%d\n", ps.effective_priority[i]);
        }
    }
    return 0;
}