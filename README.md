# An-improved-OOM-Killer
This new OOM Killer is designed and implemented by myself to  improve the OOM Killer’s kill process mechanism. That is to say, my work is to make the original Linux OOM Killer smarter.
In Design and Implementing my new OOM Killer .pdf you can view the implementation details and design ideas.
## My work
My work is consisted of four main parts. 
* First part, add a new System Call of setting
MM_max limit for each user and add a new System Call of checking the trigger condition.
* Second part, understand and explain how the original OOM Killer is triggered.
* Third part, design and Implement a new OOM Killer. 
* Fourth part, design a new rule to select the ‘best’
process for OOM Killer and modify the select_badness_function to implement it.
## Stength
* Robustness. The strategy is to help not to replace the original OOM Killer, and two
global flag variables are added in order to ensure each time we use the new OOM Killer we
have gained all the necessary information. So the system will not crash and its robustness is
enhanced due to the help of new OOM Killer’s trigger condition.
* Easy to test. I implement two system_call caller (in mm_caller.c and
Check_Condition_caller.c) to help test the system calls’ correctness. You can just compile
them and execute them in android kernel. (Note that they don’t need any parameters)

