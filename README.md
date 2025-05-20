# Get the physical address of a virtual address of a process.
[�M�׺���](https://staff.csie.ncu.edu.tw/hsufh/COURSES/FALL2024/linux_project_1.html "�M�׺���")

## Environment
```
OS: Ubuntu 22.04
ARCH: X86_64
Source Version: 5.15.137
```

## Linux �� virtual address �� physical address
![01](https://imgur.com/n9PrODm.png)  
![02](https://imgur.com/iPA3kPi.png)  
> �o�|�� page table ���G
> * ������ؿ� (Page Global Directory)(PGD)
> * ���W�ťؿ� (Page Upper Directory)(PUD)
> * �����ťؿ� (Page Middle Directory)(PMD)
> * ���� (Page Table)(PTE)
> �C�@�� table ��3�� key description marco: shift, size, mask�C  
> PGD, PUD, PMD, PTE �|���O�� pgd_t, pud_t, pmd_t, pte_t �Ӱ��y�z�C  

### current ����
<https://elixir.bootlin.com/linux/v5.15.137/source/arch/x86/include/asm/current.h>  
```c
DECLARE_PER_CPU(struct task_struct *, current_task);
static __always_inline struct task_struct *get_current(void)
{
	return this_cpu_read_stable(current_task);
}
#define current get_current()
#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_CURRENT_H */
```
`#define current get_current()`
>current �O�@�� macro�A�L�Q�w�q�b asm/current.h ���Ccurrent ��ڤW�N�O get_current() ��ơC�o�Ө禡�|�^�ǲ{�b CPU �Ұ��� process �� task_struct�C  

>���� macro DECLARE_PER_CPU ���@�άO�N�@���ܼƳ]�w��C�@�� CPU ���C  
>�Ĥ@�檺 DECLARE_PER_CPU ���ڭ̦� struct task_struct* ���A�� current_task �i�H�ΡC  
>get_current() ���ҩI�s�� this_cpu_read_stable(current_task) �禡�A�@�άO�q CPU Ū�� current_task �ܼơC  
>�ڭ̫K����ϥ� current �Ӯ����e process �� task_struct �F�C  

> current ���Ь��x�s task ���U�ظ�ƥΪ��A�ӧڭ̻ݭn���O�̭��� mm �ѼơA�� mm ���c�ХܤF�@�� Process �� Memory �޲z�T���A�]�t�G  
> * page table ��T  
> * Stack, BSS, Data, Code�K�q����T  

## �y�{
1. `pgd_offset`: �ھڥثe�� Virtual Address �M�ثe Process ����Ƶ��c task_struct�A�s���䤤�� mm point�C
2. `mm point` ���x�s�� mm_struct ��m�A�� mm_struct �x�s�� Process ������m��ƪ����c�A�b�ӵ��c���ڭ̴N�i�H��� pgd ����l��m�C�ϥ� pgd_offset �Y�i�s�� pgd page ���� pgd entry�C   
(entry ���e�� pud table �� base address)
3. `pud_offset`: �ھڳz�L pgd_offset �o�쪺 pgd entry �M Virtual Address�A�i�o�� pud entry�C  
(entry ���e�� pmd table �� base address)
4. `pmd_offset`: �ھ� pud entry �����e�M Virtual Address�A�i�o�� pte table �� base address�C
5. `pte_offset`: �ھ� pmd entry �����e�P Virtual Address�A�i�o�� pte �� base address�C
6. �N�q pte �o�쪺 Base Address �P Mask(0xf�Kfff000)�� AND �B��A�Y�i�o�� Page �� Base Physical Address�C
7. Virtual Address �P ~Mask(0x0�K000fff)�� AND �B��o�� offset�A�A�P Page �� base Physical Address �� OR �B��Y�i�o���ഫ�L��B���㪺 Physical Address�C

## ���浲�G - Question 1
![03](https://imgur.com/3poTqPE.jpeg)  

## �[��G
### 1. Before Fork (fork()���e����X)
![04](https://imgur.com/hc5z6dG.jpeg) 
-   **Virtual Address**�G`0x563ab6a04010` �O�����ܼ� `global_a` �� Virtual Address�A�Y Process ���s���� Logical Memory ��m�A�Ѩt�ά����ܼƤ��t�C
-   **Physical Address**�G`0xcf696010` �O `global_a` ������ Physical Address�A�Y Physical Memory ����ڦs�x�a�}�C
-   **PID**�G`pid=4371` �O Parent Process �� pid�A��ܷ�e���b�B�檺 Parent Process�C

�b `fork()` ���e�AParent Process �֦� `global_a` �� Virtual Address �M������ Physical Address�C

### 2. After Fork by parent (fork()���᪺��X)
![05](https://imgur.com/M6fiGjS.jpeg) 
-   **Virtual Address �M Physical Address �S���ܤ�**�GParent Process �A����o `global_a` �� Virtual Address �M Physical Address�A�o��̳��S���ܤơC
-   **PID**�G`pid=4371`�AParent Process �� pid ���M�O `pid=4371`�C

�b `fork()` ����AParent Process ���M�֦��� `global_a` ������A�åB���� Virtual Address �M������ Physical Address �O�����ܡA�N�� Parent Process �èS���� `global_a` �i�����ק�C

### 3. After Fork by child (fork()����Achild process����X)
![06](https://imgur.com/zqVVFJb.jpeg) 
- **Virtual Address �S���ܤ�**�GChild Process ���� `global_a`  �� Virtual Address ���M�O `0x563ab6a04010`�C
- **Physical Address �S���ܤ�**�GChild Process ���� `global_a` ������ Physical Address ���M�O `0xcf696010`�A�P Parent Process �@�ɦP�@�� Physical Address�C
- **PID**�G`pid=4373`�A�o�O Child Process �� pid�C

�b `fork()` ����AChild Process �~�ӤF Parent Process ���O����Ŷ��AParent Process �P Child Process �@�ɬۦP�� Virtual Address �M Physical Address�C�o�O�]���b `fork()` ����A��� Process �Ȯɦ@�ɬۦP�� Physical Memory Page�C

### 4. Test Copy on Write in child (Ĳ�oCoW�ɡAchild process ����X)
![07](https://imgur.com/20LVLdV.jpeg) 
- **Virtual Address �S���ܤ�**�GChild Process ���� `global_a` �� Virtual Address ���M�O`0x563ab6a04010`�C
- **Physical Address �o���ܤ�**�G�� Child Process �ק�F `global_a` ���ȫ�A������ Physical Address �ܦ��F`0x7001d010`�A���Ĳ�o�F **Copy-on-Write�]CoW�^����**�C
- **PID**�G`pid=4373`�A�o�O Child Process �� pid�C

�b Child Process ���A�� `global_a` �Q�ק�ɡALinux �� Copy-on-Write Ĳ�o�AKernel �|�� Child Process ���t�@�ӷs�� Physical Address�A�o�� Parent Process �M Child Process ���A�@�ɦP�@�� Physical Memory Page�C�]�� Child Process �� Physical Address �ܦ��F`0x7001d010`�C  

### What is Copy-on-Write?
-   `fork()` �� Parent Process �M Child Process �@�� Physical Memory�G�b `fork()` �եΤ���AParent Process �M Child Process �@�ɬۦP�� Virtual Address �P Physical Memory �Ӹ`�ٸ귽�A�]���p�G�O����S���Q�ק�A���̥i�H�@�ɬۦP�� Physical Page�C
-   **Ĳ�oCow**�G�� Child Process ���խק�@�ɪ� Memory Page�]�Ҧp `global_a`�^�ɡAKernel �|���t�@�ӷs�� Physical Page�A�o�� Parent Process �M Child Process �N�|���O�֦��ۤv�W�ߪ� Physical Memory�C�o�ӹL�{�s��==�g�J�ɽƻs�]Copy-on-Write, CoW�^==�C�� Virtual Address �O�����ܡC
-   **Virtual Address �O������**�G���ެO Parent Process  �٬O Child Process�A�ק� `global_a` �ɡA���̪� Virtual Address �����|�ܤơC�ܤƪ��O�o�� Virtual Address �ҹ����� Physical Address�C

## ���浲�G - Question 2
![08](https://imgur.com/bmV7MVv.jpeg)

## �[��G
- Virtual Address�G`0x560b703a7040` �O `a[0]` ��Virtual Address�C
- Physical Address�G`0x6bf04040` �O `a[0]` ������Physical Address�C

### The address translation of a[1999999] failed: cannot get physical address.
- Virtual Address�G`0x560b704823c` �O `a[1999999]` ��Virtual Address�C
- Physical Address�G`(nil)` ��ܧA�L�k��� `a[1999999]` �� Physical Address�C

### ������ a[1999999]�|��}����?
- **Lazy Allocation**�G
Linux �t�ά��F�`�ٰO����M�����į�A�|�ϥ� **Lazy Allocation**�C�t�Φb�{�����t�F�O�����A��ڤW�ä��|�ߧY���t Physical Memory�A����o�q�O����u���Q�X�ݡ]�ҦpŪ�g�ާ@�^�APhysical Memory �~�|�Q���t�C
- **������ `a[1999999]` ����**�G
���M���t�F�@�Ӥj�}�C `int a[2000000]`�A���o�ä��N��C�Ӥ������|�ߧY�������� Physical Memory Allocation�C�ھ� Lazy Allocation�A�u����A�u���X�ݬY�Ӥ����ɡA�t�Τ~�|�N�� Virtual Address �M�g�� Physical Address�C

## �p�G���X�� `a[0]` �� `a[1999999]`
```c
    a[0] = 10;

    phy_add=my_get_physical_addresses(&a[0]);
    printf("global element a[0]:\n");  
    printf("Offest of logical address:[%p]   Physical address:[%p]\n", &a[0], phy_add);              
    printf("========================================================================\n");

    a[1999999] = 20;

    phy_add=my_get_physical_addresses(&a[1999999]);
    printf("global element a[1999999]:\n");  
    printf("Offest of logical address:[%p]   Physical address:[%p]\n", &a[1999999], phy_add);              
    printf("========================================================================\n"); 
```
![09](https://imgur.com/a3LRAuW.jpeg)
- Virtual Address�G`0x55f058221040` �O `a[0]` �� Virtual Address�C
- Physical Address�G`0x6956f040` �O `a[0]` ������ Physical Address�C

### The address translation of a[1999999] successed.
- Virtual Address�G`0x55f0589c223c` �O `a[1999999]` �� Virtual Address�C
- Physical Address�G`0x2ffc423c` �O `a[1999999]` ������ Physical Address�C

### ������C������{���ɡAVirtual Address �M Physical Address �|����?
- **��}�Ŷ��պA�H�����J (ASLR - Address Space Layout Randomization)**�G
ASLR �O�@�ب��d�D�O����l�a�|�}�Q�Q�Ϊ��q���w���޳N�C�C���{������ɡA�t�η|�H�����t Virtual Address�A�o�˧����̵L�k�Q�ΩT�w�� Address �Ӷi������C�{�N�@�~�t�Τ@�볣�[�]�o�@����A�H���d�c�N�{����w����}�i�� `Return-to-libc` �����C
�]���N��{�������ܼƦ�m�O�T�w���]�p `a[0]` �M `a[1999999]`�^�A�C������ɥ��̪� Virtual Address ���|���Ҥ��P�C
- **Physical Address**: �ѧ@�~�t�Ϊ� `�D�O����޲z�椸 (MMU)` �t�d���t�A�ھڷ�e�t�Ϊ��i�ΰO���骬�A�ӰʺA�M�w�C�ҥH�Y�� Virtual Address �O�����ܡA���P�ɨ誺 memory allocation ���p�]�|�ɭP�ܼƹ����� Physical Address ���P�C

## �p�G�̧ǥh�X�ݡA�|�b�ƻ�ɭԧ줣�� Physical Address?
```c
    for(int i = 0; i <= 1999999; i++){
        phy_add=my_get_physical_addresses(&a[i]);
        printf("global element a[%d]:\n", i);  
        printf("Offest of logical address:[%p]   Physical address:[%p]\n", &a[i], phy_add);              
        printf("========================================================================\n"); 
        if(phy_add == NULL){
            printf("�b i == %d �ɷ|�줣�� Physical Address\n", i);
            break;
        }
    }
```
![10](https://imgur.com/lsyIftR.jpg)  
�|�b `i == 1008` �ɧ줣�� Physical Address�C

### ������|�b i ���� 1008 �ɧ줣�� Physical Address?
- **�t�� Memory Page �j�p�M int �j�p�����Y:**
Memory Page ���j�p�� 4 KB�]4096 bytes�^�C���]�C�� int �O 4 bytes�A����@�� Page �i�H�e�ǡG
```
4096 / 4 = 1024 �� int
```
�o�N��� 1024 �� int �ƾڭ�n�񺡤@�� Physical Memory Page�C

- Lazy Allocation �M Page Faults:
�@�~�t�ήھ� Lazy Allocation�APhysical Memory Page �u�|�b�ݭn�ɤ~���t�C��{���X�� `a[i]` �ɡA�t�η|�ھڻݭn���t������ Physical Page�C���o�Ǥ��t�O�ھڭ��]Page�^���i�檺�A�Ӥ��O����ܼơC
- ��X�� `a[0]` �� `a[1007]` �ɡA�o�Ǥ����i��w�g�b�P�@�� 4 KB �������A�]���t�ί�������̤��t�æ��\��� Physical Address�C
- ���O��X�ݨ� `a[1008]` �ɡA�����U�@�� Memory Page ���]`a[1008]` ��� 4 KB ���᪺�O����^�o�ɨt�λݭn���t�s�� Physical Memory Page�C�p�G�t�ΥX�{�O���餣���Υ��ন�\���t�ӭ��A�N�|�ɭP�L�k����� Virtual Address �ҹ����� Physical Address�C
- �]���� `i == 1008` �ɧ줣�� Physical Address�A�ܥi��O�]���o�Ӥ�����n���b�Y�� Page ����ɡA�Өt�Φb���t�U�@�� Physical Page �ɥX�{�F���D�C

---
[���O����](https://hackmd.io/@Jyen024/HkcfEMtpkl)