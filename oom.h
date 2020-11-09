#ifndef __INCLUDE_LINUX_OOM_H
#define __INCLUDE_LINUX_OOM_H

/*
 * /proc/<pid>/oom_adj is deprecated, see
 * Documentation/feature-removal-schedule.txt.
 *
 * /proc/<pid>/oom_adj set to -17 protects from the oom-killer
 */
#define OOM_DISABLE (-17)
/* inclusive */
#define OOM_ADJUST_MIN (-16)
#define OOM_ADJUST_MAX 15

/*
 * /proc/<pid>/oom_score_adj set to OOM_SCORE_ADJ_MIN disables oom killing for
 * pid.
 */
#define OOM_SCORE_ADJ_MIN	(-1000)
#define OOM_SCORE_ADJ_MAX	1000

#ifdef __KERNEL__

#include <linux/sched.h>
#include <linux/types.h>
#include <linux/nodemask.h>

struct zonelist;
struct notifier_block;
struct mem_cgroup;
struct task_struct;
struct MMLimits;
struct MMLimit_entry;

/*
 * Types of limitations to the nodes from which allocations may occur
 */
enum oom_constraint {
	CONSTRAINT_NONE,
	CONSTRAINT_CPUSET,
	CONSTRAINT_MEMORY_POLICY,
	CONSTRAINT_MEMCG,
};

extern void compare_swap_oom_score_adj(int old_val, int new_val);
extern int test_set_oom_score_adj(int new_val);

extern unsigned int oom_badness(struct task_struct *p, struct mem_cgroup *memcg,
			const nodemask_t *nodemask, unsigned long totalpages);
extern int try_set_zonelist_oom(struct zonelist *zonelist, gfp_t gfp_flags);
extern void clear_zonelist_oom(struct zonelist *zonelist, gfp_t gfp_flags);

extern void out_of_memory(struct zonelist *zonelist, gfp_t gfp_mask,
		int order, nodemask_t *mask, bool force_kill);

extern void my_out_of_memory(struct zonelist *zonelist, gfp_t gfp_mask,
		int order, nodemask_t *nodemask, bool force_kill,long chosen_uid, 
		int limit_entry_index, int RSS_entry_index);

extern int register_oom_notifier(struct notifier_block *nb);
extern int unregister_oom_notifier(struct notifier_block *nb);

extern bool oom_killer_disabled;

static inline void oom_killer_disable(void)
{
	oom_killer_disabled = true;
}

static inline void oom_killer_enable(void)
{
	oom_killer_disabled = false;
}

extern struct task_struct *find_lock_task_mm(struct task_struct *p);

/* sysctls */
extern int sysctl_oom_dump_tasks;
extern int sysctl_oom_kill_allocating_task;
extern int sysctl_panic_on_oom;
// two flag variable declaration here
extern int MM_Limit_Module_Ready;
extern int Check_RSS_Module_Ready;

// global variable declaration
extern struct MMLimits my_current_RSS;
extern struct MMLimits my_mm_limits;            // note that here the my_mm_limits should be initialized, so that this requires we install MM_limit_system_call first.
#endif /* __KERNEL__*/
#endif /* _INCLUDE_LINUX_OOM_H */
