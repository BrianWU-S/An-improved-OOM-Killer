Add myself files:

Check_Condition_caller.c 
	: test Check_Condition_system_call

Check_Condition_caller.h
	: test Check_Condition_system_call

Check_Condition_system_call.c
	: add system call to check the trigger condition

mm_caller.c
	:test MM_limit_system_call

mm_caller.h
	:test MM_limit_system_call

MM_limit_system_call.c
	: add system call to set memory limits for user

prj2_test.c
	: call limit set system call and fork child processes

Design and Implementing my new OOM Killer .pdf
	:project report


/*****************************************/
/*****************************************/
/*****************************************/

Modify kernel files:

init_task.c
	: add global variable definition
	:goldfish/arch/arm/kernel/init_task.c

oom.h
	: add global variable declaration to let them can be used in page_alloc.c
	:goldfish/include/linux/oom.h

oom_kill.c
	: add all the functions of new OOM Killer
	goldfish/mm/oom_kill.c

page_alloc.c
	: add the check trigger condition function for new OOM Killer
	:goldfish/mm/page_alloc.c.

sched.h
	: add new structure definition
	:goldfish/include/linux/sched.h






