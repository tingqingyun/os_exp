#include "linux/init.h"
#include "linux/module.h"
#include "linux/kernel.h"
#include "linux/sched/signal.h"
#include "linux/sched.h"

MODULE_LICENSE("GPL");

static int __init show_all_init(void)
{
	struct task_struct *p;
	printk("%-20s%-6s%-6s%-6s%-6s\n", "Name", "PID", "State", "Prio", "PPID");
	printk("--------------------------------------------\n");
	//for_each_process(p) from 0 to all process
	for_each_process(p)
	{
		if(p->mm == NULL)
		{
			printk("%-20s%-6d%-6d%-6d%-6d\n",p->comm, p->pid, p->state, p->prio, p->parent->pid);
		}
	}
	
	return 0;
}

static void __exit show_all_exit(void)
{
	printk("[show_all] Module Uninstalled.");
}

module_init(show_all_init);
module_exit(show_all_exit);

