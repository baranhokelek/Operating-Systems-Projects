#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/stat.h>

static int processID = 1;
module_param(processID, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(processID, "Process ID");

int fileinfo304_init(void)
{
  struct task_struct *task, *temp;
  struct list_head *list;
  int flag = 0;
  printk(KERN_INFO "Inserting Module\n");
  printk(KERN_INFO "Process ID: %d", processID);
  for_each_process(task)
  {
    if (task->pid == processID)
    {
      printk(KERN_INFO "Current Task\nPID: %d\nName: %s\nParent Process:\nPID: %d\nName: %s\n", task->pid, task->comm, task->parent->pid, task->parent->comm);
      list_for_each(list, &task->sibling)
      {
        temp = list_entry(list, struct task_struct, sibling);
        printk(KERN_INFO "Sibling\nPID: %d\nName: %s\n", temp->pid, temp->comm);
      }
      flag = 1;
    }
  }
  if (flag == 0)
    printk(KERN_INFO "No such process is found!");
  return 0;
}

void fileinfo304_exit(void)
{
  printk(KERN_INFO "Removing Module\n");
}

module_init(fileinfo304_init);
module_exit(fileinfo304_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Exercise for COMP304");
MODULE_AUTHOR("Baran Berkay Hökelek");
