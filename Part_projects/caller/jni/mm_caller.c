#include"mm_caller.h"

int main(int argc, char *argv[]){ 
    int flag;
    long i;
    long my_uid=10070;
    printf("Here is the mm_caller, start testing the system call!\n");
    // test the add_entry
    for(i=10070;i<10085;++i){
        flag=syscall(382,i,i-200);
        if(flag==-1){
            printf("Detect some error in system call!\n");
        }
    }

    flag=syscall(382,my_uid,1000834);       // test the update_entry
    if(flag==-1){
        printf("detect update error in system call!\n");
    }
    my_uid=10079;
    flag=syscall(382,my_uid,1000);       // test the update_entry
    if(flag==-1){
        printf("detect update error in system call!\n");
    }
    my_uid=10099;
    flag=syscall(382,my_uid,1000835);       // test the update_entry
    if(flag==-1){
        printf("Here is the end of mm_caller,detect update error in system call!\n");
    }
    
    return 0;
}