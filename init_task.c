/*
 *  linux/arch/arm/kernel/init_task.c
 */
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/mqueue.h>
#include <linux/uaccess.h>

#include <asm/pgtable.h>
#include <linux/slab.h>				// kmalloc in this library

static struct signal_struct init_signals = INIT_SIGNALS(init_signals);
static struct sighand_struct init_sighand = INIT_SIGHAND(init_sighand);
/*
 * Initial thread structure.
 *
 * We need to make sure that this is 8192-byte aligned due to the
 * way process stacks are handled. This is done by making sure
 * the linker maps this in the .text segment right after head.S,
 * and making head.S ensure the proper alignment.
 *
 * The things we do for performance..
 */
union thread_union init_thread_union __init_task_data =
	{ INIT_THREAD_INFO(init_task) };

/*
 * Initial task structure.
 *
 * All other task structs will be allocated on slabs in fork.c
 */
struct task_struct init_task = INIT_TASK(init_task);

EXPORT_SYMBOL(init_task);

/*
 * Initial MMLimit here
 * Try whether it successfully init another global variable here
 */
struct MMLimits my_mm_limits;				// store the limit entry for each user
struct MMLimits my_current_RSS;			// store the cunrrent RSS for each user, it's needed in select_bad_process
/*
my_mm_limits.Current_Size=0;
my_mm_limits.Max_Size=500;
my_mm_limits.MMLimit_list= (struct MMLimit_entry*) kmalloc(sizeof(struct MMLimit_entry)* 500,GFP_KERNEL);
*/
EXPORT_SYMBOL(my_mm_limits);
EXPORT_SYMBOL(my_current_RSS);

// define two flag to indicate whether the module is installed, if not, we can't use the new OOM_killer
int MM_Limit_Module_Ready;
int Check_RSS_Module_Ready;
EXPORT_SYMBOL(MM_Limit_Module_Ready);
EXPORT_SYMBOL(Check_RSS_Module_Ready);


