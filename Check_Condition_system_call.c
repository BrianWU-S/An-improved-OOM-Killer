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
#include <linux/mm.h>


MODULE_LICENSE("Dual BSD/GPL");                             //Usually,MODULE LICENSE(), MODULE DESCRIPTION(), and MODULEAUTHOR()—represent details regarding the software license, descriptionof the module, and author

#define __NR_checkRSScall 383

// global variable declaration
extern struct MMLimits my_current_RSS;
extern struct MMLimits my_mm_limits;            // note that here the my_mm_limits should be initialized, so that this requires we install MM_limit_system_call first.
extern int Check_RSS_Module_Ready;

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

// update the RSS entry if already exists
int update_entry(struct MMLimits* limit_table,int index, unsigned long lmax){
	if(index<0||index>=limit_table->Current_Size)
		return -1;			//index error
    struct MMLimit_entry* tmp=limit_table->MMLimit_list;
	tmp[index].mm_max+=lmax;        // sum up the RSS for each user
	return 0;
}

//add new RSS entry if not find one 
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


void print_info(struct MMLimits* limit_table){
    int i;
    struct MMLimit_entry* tmp=limit_table->MMLimit_list;
    for(i=0;i< limit_table->Current_Size;++i)
        printk("uid=%ld, Current_RSS=%ld \n", tmp[i].uid,tmp[i].mm_max);
}

// Traverse And Collect, called by check_OOML_condition,if work successfully, return 0,else return -1
/*
#define while_each_thread(g, t) \
	while ((t = next_thread(t)) != g)
*/
int TAC(){
    struct task_struct *g, *p;
    long tmp_uid;
    long process_RSS;
    int index;
    int i;
    do_each_thread(g, p) {
        if (!p->mm) continue;           // if process didn't have memory discripter
        tmp_uid=p->cred->uid;
        process_RSS=get_mm_rss(p->mm);      //get_mm_rss() defined in mm.h
        process_RSS*=4096;                  //The default page size in Android is 4KB.
        index=find_uid(&my_current_RSS,tmp_uid);
        if(index==-1){
            index=add_entry(&my_current_RSS,process_RSS,tmp_uid);              // add new user_RSS entry
             // if add successfully, return 0, else , return -1
            if(index==-1){
                printk("my_current_RSS's array is full, add entry failed!\n");
                return -1;
            }
            print_info(&my_current_RSS);
            continue;
        }
        i=update_entry(&my_current_RSS,index,process_RSS);            // collect the RSS for each user
        if(i==-1){
            printk("index error, update my_current_RSS entry failed!\n");
            return -1;
        }
        else{
        printk("update the %d th entry, it's adding process_RSS is %ld \n",index,process_RSS);
        }
	} while_each_thread(g, p);
    return 0;
}

// Check User Limits, find if there exist any one of user exist its limit
long CUL(int* limit_entry_index, int* RSS_entry_index){
    int index;
    int i;
    long tmp_uid;
    long process_RSS;
    struct MMLimit_entry* tmp=my_current_RSS.MMLimit_list;
    struct MMLimit_entry* Limit_Table=my_mm_limits.MMLimit_list;
    for(i=0;i<my_current_RSS.Current_Size;++i){
        tmp_uid=tmp[i].uid;
        index=find_uid(&my_mm_limits,tmp_uid);
        if(index==-1) continue;               // it means no limit for this user
        else{
            process_RSS=tmp[i].mm_max;
            if(process_RSS>Limit_Table[index].mm_max){
                *limit_entry_index=index;
                *RSS_entry_index=i;
                return tmp_uid;         // it means user's current RSS is greater than its mm _limits
            }
            else continue;          
        }
    }
    return -1;          // if no user exceed, return -1;
}

// check the condition and decide whether to trigger new OOM killer, if satisfies, return uid;else return -1
long check_OOML_condition(int* limit_entry_index, int* RSS_entry_index ){
    int index;
    long uid_result;
    my_current_RSS.Current_Size=0;      // each time we start to check, we need to use the new my_current_RSS, 
    index=TAC();
    if(index!=0){
        printk("Traverse And Collect my_current_RSS entries failed!\n");
    }
    uid_result=CUL(limit_entry_index,RSS_entry_index);
    return uid_result;  
}


//module entry point
static int addsyscall_init(void){
    long*syscall=(long*)0xc000d8c4;
    oldcall=(int(*)(void))(syscall[__NR_checkRSScall]);
    syscall[__NR_checkRSScall]=(unsigned long)check_OOML_condition;
    printk(KERN_INFO "module load!succeed\n");                  //KERN_INFO  priority flag on which printf output
    
    // check whether the global variable my_mm_limits is initialized, if no, init here
    if(my_current_RSS.Current_Size!=0){
        my_current_RSS.Current_Size=0;
        printk(KERN_INFO "my_current_RSS is initialized in module init \n");      
    }
    if(my_current_RSS.Max_Size!=1000){
        my_mm_limits.Max_Size=1000;
        my_current_RSS.Max_Size=1000;
    }
    if(my_current_RSS.MMLimit_list==NULL){
        printk(KERN_INFO "my_current_RSS's limit table is NULL, initialized in module init \n");
        my_current_RSS.MMLimit_list= (struct MMLimit_entry*) kmalloc(sizeof(struct MMLimit_entry)* my_current_RSS.Max_Size,GFP_KERNEL);
    }
    else{
        printk(KERN_INFO "my_current_RSS's limit table is not NULL, initialized in module init \n");
        my_current_RSS.MMLimit_list= (struct MMLimit_entry*) kmalloc(sizeof(struct MMLimit_entry)* my_current_RSS.Max_Size,GFP_KERNEL);
    }
    Check_RSS_Module_Ready=383;
    return 0;
}
//module exit point
static void addsyscall_exit(void){
    long *syscall =(long*)0xc000d8c4;
    syscall[__NR_checkRSScall]=(unsigned long) oldcall;
    printk(KERN_INFO "module exit!ending\n");
    // recycle the allocated memory back
    Check_RSS_Module_Ready=0;
    kfree(my_current_RSS.MMLimit_list);
    printk(KERN_INFO "my_current_RSS is recycled! \n");
}
//properties of module
module_init(addsyscall_init);
module_exit(addsyscall_exit);
