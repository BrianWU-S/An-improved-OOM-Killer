# An-improved-OOM-Killer
This new OOM Killer is designed and implemented by myself to  improve the OOM Killer’s kill process mechanism. That is to say, my work is to make the original Linux OOM Killer smarter.
In Design and Implementing my new OOM Killer .pdf you can view the implementation details and design ideas.
# My work
My work is consisted of four main parts. 
* First part, add a new System Call of setting
MM_max limit for each user and add a new System Call of checking the trigger condition.
* Second part, understand and explain how the original OOM Killer is triggered.
* Third part, design and Implement a new OOM Killer. 
* Fourth part, design a new rule to select the ‘best’
process for OOM Killer and modify the select_badness_function to implement it.

