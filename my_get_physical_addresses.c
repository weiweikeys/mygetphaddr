#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>       // Data structure and functions related to memory management
#include <linux/pid.h>      // To obtain process info
#include <linux/sched.h>    // Definition of process control area
#include <linux/highmem.h>  // Functions related to page table operations
#include <asm/page.h>       // Functions related to page table operations

SYSCALL_DEFINE1(my_get_physical_addresses, void *, vaddr) {
    struct task_struct *task;
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    struct page *page;
    unsigned long physical_address;

    task = current; // Get the current process
    mm = task->mm;  // Get the memory management structure of the process

    // Get PGD (page global directory)
    // pgd_offset to obatin the entry to page-wide directory and checks whether is valid
    // pgd_none to check whether the page doesn't exist
    // pgd_pad to check whether the page directory is damaged
    pgd = pgd_offset(mm, (unsigned long)vaddr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        return (void *)0;
    }
    printk("pgd address: %p\n", pgd);

    // Get P4D (page 4th directory)
    // To obatin the entry to 4th level of the P4D page directory and checks whether is valid
    p4d = p4d_offset(pgd, (unsigned long)vaddr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        return (void *)0;
    }
    printk("p4d address: %p\n", p4d);

    // Get PUD (page parent directory)
    // To obatin the entry to upper-level directory of the PUD page and checks whether is valid
    pud = pud_offset(p4d, (unsigned long)vaddr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        return (void *)0;
    }
    printk("pud address: %p\n", pud);

    // Get PMD (page intermediate directory)
    // To obatin the PMD page intermediate directory and checks whether is valid
    pmd = pmd_offset(pud, (unsigned long)vaddr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        return (void *)0;
    }
    printk("pmd address: %p\n", pmd);

    // Get PTE (page table entry)
    // Get the page entry PTE corresponding to the virtual address and checks whether the page table is valid
    pte = pte_offset_map(pmd, (unsigned long)vaddr);
    if (!pte || pte_none(*pte)) {
        return (void *)0;
    }
    printk("pte address: %p\n", pte);

    // pte_page obtains the corresponding page frame (struct page structure) through the page table entry
    // which is page is physical memory. If the page doesn't exist, return 0
    page = pte_page(*pte);
    if (!page) {
        return (void *)0;
    }
    printk("page address: %p\n", page);

    // page_to_pfn(page) converts the page structure in to a page frame number, which is the page index in physical memory
    // Shift left by PAGE_SHIFT to get underlying physical memory og the page
    // ((unsigned long)vaddr) & ~PAGE_MASK to calculate the offset to the virtual address within page
    // add this offset to the base address of the physical address frame to obtain the final physical address
    physical_address = page_to_pfn(page) << PAGE_SHIFT;
    physical_address |= ((unsigned long)vaddr) & ~PAGE_MASK;

    return (void *)physical_address;

    // 透過 PGD->P4D->PUD->PMD->PTE 解析 Virtual Address，並計算出對應的 Physical Memory. 
    // If the resolution is sucessful, the system call returns the physical addrress corresponding to the virtual address.
}
