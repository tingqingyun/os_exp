#include "linux/init.h"
#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/moduleparam.h"
#include "linux/pid.h"
#include "linux/list.h"
#include "linux/sched.h"

MODULE_LICENSE("GPL");
static int pid;
module_param(pid, int, 0644);

static int __init show_family_init(void)
{
	struct pid *ppid;
	struct task_struct *p;
	struct task_struct *pos;
	char *ptype[4] = {"[I]","[P]","[S]","[C]"};
	
	ppid = find_get_pid(pid);
	if(ppid == NULL)
	{
		printk("[showtaskfamily] error,PID not exists.\n");
		return -1;
	}
	p = pid_task(ppid, PIDTYPE_PID);
	
	printk("%-10s%-20s%-6s%-6s\n","type","name","pid","state");
	printk("---------------------------------------\n");
	
	printk("%-10s%-20s%-6d%-6d\n",ptype[0],p->comm,p->pid,p->state);
	
	printk("%-10s%-20s%-6d%-6d\n",ptype[1],p->real_parent->comm,p->real_parent->pid,p->real_parent->state);
	
	list_for_each_entry(pos,&(p->real_parent->children),sibling)
	{
		if(pos->pid == pid)
			continue;
		printk("%-10s%-20s%-6d%-6d\n",ptype[2],pos->comm,pos->pid,pos->state);
	}
	
	list_for_each_entry(pos,&(p->children),sibling)
	{
		printk("%-10s%-20s%-6d%-6d\n",ptype[3],pos->comm,pos->pid,pos->state);
	}
	
	return 0;
}

static void __exit show_family_exit(void)
{
	printk("[showtaskfamily] module uninstalled\n");
}

module_init(show_family_init);
module_exit(show_family_exit);
