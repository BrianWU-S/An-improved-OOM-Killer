#include"Check_Condition_caller.h"

int main(int argc, char *argv[]){
    printf("This is my Check_Condition_caller\n");
    int L_index;
    int R_index;
    L_index=0;
    R_index=0;
    long my_test_uid;
    my_test_uid=syscall(383,&L_index,&R_index);
    if(my_test_uid==-1){
        printf("There is no user exceed MM_Limit\n");
        return 0;
    }
    printf("Exceed user is %ld , its position in Limit table is %d, in RSS table is %d \n",my_test_uid,L_index,R_index);
}