//module definition
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include<linux/string.h>
#include<linux/list.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("Dual BSD/GPL");                             //Usually,MODULE LICENSE(), MODULE DESCRIPTION(), and MODULEAUTHOR()—represent details regarding the software license, descriptionof the module, and author

#define __NR_mmlimitcall 382

extern struct MMLimits my_mm_limits;            // global variable declaration
extern int MM_Limit_Module_Ready;

static int (*oldcall)(void);

// inline function, help find the index of existing MMLimit_entry, if no entry, return -1
int find_uid(struct MMLimits* limit_table,long new_uid){
	int i;
    struct MMLimit_entry* tmp=limit_table->MMLimit_list;
	for(i=0;i<limit_table->Current_Size;++i)
		if(tmp[i].uid==new_uid)
			break;
	if(i==limit_table->Current_Size)
		return -1;
	return i;
}

// update the limit entry if already exists
int update_entry(struct MMLimits* limit_table,int index, unsigned long lmax){
	if(index<0||index>=limit_table->Current_Size)
		return -1;			//index error
    struct MMLimit_entry* tmp=limit_table->MMLimit_list;
	tmp[index].mm_max=lmax;
	return 0;
}

//add new limit entry if not find one 
int add_entry(struct MMLimits* limit_table,unsigned long lmax,long new_uid){
	if(limit_table->Current_Size==limit_table->Max_Size) return -1;    // the MMLimit_list is full          
	struct MMLimit_entry* tmp=limit_table->MMLimit_list;
    int i;
    i=limit_table->Current_Size;
    tmp[i].uid=new_uid;
    tmp[i].mm_max=lmax;
    limit_table->Current_Size+=1;
	return 0;
}

// print the information after add entry
void print_info(struct MMLimits* limit_table){
    int i;
    struct MMLimit_entry* tmp=limit_table->MMLimit_list;
    for(i=0;i< limit_table->Current_Size;++i)
        printk("uid=%ld, mm_max=%ld \n", tmp[i].uid,tmp[i].mm_max);
}

// system call function, set memory limit for specific user
int set_mm_limit(long my_uid, unsigned long mm_max){
    int index;
    int i;
    printk("this is my system set_mm_limit call!\n");
    index=find_uid(&my_mm_limits,my_uid);
    if(index==-1){
        index=add_entry(&my_mm_limits,mm_max,my_uid);   // if add successfully, return 0, else , return -1
        if(index==-1){
            printk("the MMLimit_list is full, add entry failed!\n");
        }
        else{
            print_info(&my_mm_limits);       //traverses and outputs all existing memory limit entries
        }
        return index;       // if set limit successfully, return 0, else , return -1
    }
    i=update_entry(&my_mm_limits,index,mm_max);
    if(i==-1){
        printk("index error, update entry failed!\n");
    }
    else{
        printk("update the %d th entry\n",index);
    }
    return i;
}


//module entry point
static int addsyscall_init(void){
    long*syscall=(long*)0xc000d8c4;
    oldcall=(int(*)(void))(syscall[__NR_mmlimitcall]);
    syscall[__NR_mmlimitcall]=(unsigned long)set_mm_limit;
    printk(KERN_INFO "module load!succeed\n");                  //KERN_INFO  priority flag on which printf output
    
    MM_Limit_Module_Ready=382;
    
    // check whether the global variable my_mm_limits is initialized, if no, init here
    if(my_mm_limits.Current_Size!=0){
        my_mm_limits.Current_Size=0;
        printk(KERN_INFO "my_mm_limits is initialized in module init \n");          // this will not be execute, because Current_Size value is 0,even without init
    }
    if(my_mm_limits.Max_Size!=500){
        my_mm_limits.Max_Size=500;
    }
    if(my_mm_limits.MMLimit_list==NULL){
        printk(KERN_INFO "my_mm_limits's limit table is NULL, initialized in module init \n");
        my_mm_limits.MMLimit_list= (struct MMLimit_entry*) kmalloc(sizeof(struct MMLimit_entry)* my_mm_limits.Max_Size,GFP_KERNEL);
    }
    else{
        printk(KERN_INFO "my_mm_limits's limit table is not NULL, initialized in module init \n");
        my_mm_limits.MMLimit_list= (struct MMLimit_entry*) kmalloc(sizeof(struct MMLimit_entry)* my_mm_limits.Max_Size,GFP_KERNEL);
    }
    return 0;
}
//module exit point
static void addsyscall_exit(void){
    long *syscall =(long*)0xc000d8c4;
    syscall[__NR_mmlimitcall]=(unsigned long) oldcall;
    printk(KERN_INFO "module exit!ending\n");
    // recycle the allocated memory back
    kfree(my_mm_limits.MMLimit_list);
    printk(KERN_INFO "my_mm_limits is recycled! \n");
}
//properties of module
module_init(addsyscall_init);
module_exit(addsyscall_exit);
