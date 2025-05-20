#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>         // used to make system call
#include <sys/syscall.h>    // used to make system call
#include <sys/types.h>
#include <sys/wait.h>

#define MY_SYSCALL_NUM 449

void *my_get_physical_addresses(void *addr){

    void *paddr;

    paddr = (void *)syscall(MY_SYSCALL_NUM, addr);

    if (paddr == 0) {
        printf("failure: cannot get physical address\n");
        return NULL;
    } else {
        printf("The virtual address %p ---> The physical address: %p \n", addr, paddr);
    }

    return paddr;
}


int global_a = 123;

void hello(void) {
    printf("======================================================================================================\n");
}

int main() {
    int loc_a;
    void *parent_use, *child_use;

    printf("=========================== Before Fork ==================================\n");
    parent_use = my_get_physical_addresses(&global_a);
    printf("pid=%d: global variable global_a:\n", getpid());
    printf("Offset of logical address:[%p]   Physical address:[%p]\n", &global_a, parent_use);
    printf("========================================================================\n");
    printf("\n");

    if (fork()) {  
        /* parent code */
        printf("vvvvvvvvvvvvvvvvvvvvvvvvvv  After Fork by parent  vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
        parent_use = my_get_physical_addresses(&global_a);
        printf("pid=%d: global variable global_a:\n", getpid());
        printf("******* Offset of logical address:[%p]   Physical address:[%p]\n", &global_a, parent_use);
        printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n");
        printf("\n");
        wait(NULL); 
    } else {  
        /* child code */
        printf("llllllllllllllllllllllllll  After Fork by child  llllllllllllllllllllllllllllllll\n");
        child_use = my_get_physical_addresses(&global_a);
        printf("******* pid=%d: global variable global_a:\n", getpid());
        printf("******* Offset of logical address:[%p]   Physical address:[%p]\n", &global_a, child_use);
        printf("llllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll\n");
        printf("____________________________________________________________________________\n");
        printf("\n");

        /* trigger CoW (Copy on Write) */
        global_a = 789;

        printf("iiiiiiiiiiiiiiiiiiiiiiiiii  Test copy on write in child  iiiiiiiiiiiiiiiiiiiiiiii\n");
        child_use = my_get_physical_addresses(&global_a);
        printf("******* pid=%d: global variable global_a:\n", getpid());
        printf("******* Offset of logical address:[%p]   Physical address:[%p]\n", &global_a, child_use);
        printf("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n");
        printf("____________________________________________________________________________\n");
        printf("\n");
        sleep(1000);
    }
    return 0;
}
