#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/dirent.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/delay.h>
#include <linux/pid.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/path.h>
#include <linux/sched.h>
#include <linux/namei.h>
#include <linux/proc_fs.h>
#include <linux/mount.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/inet.h>
#include <net/inet_sock.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include "file_read.h"
#include "file_struct.c"

#define MAX_PATH_LEN 1024
#define MAX_PID_NUM 20
#define MAX_NUM_FILENAMES 10

// Function declarations
void find_processes_with_open_file(const char *target_filename);

MODULE_LICENSE("GPL");

static char *target_filename = "";
module_param(target_filename, charp, 0);
MODULE_PARM_DESC(target_filename, "The target file name to search for");

// Function declarations
void find_processes_with_open_file(const char *target_filename);
void list_open_files(int pid);
static int __init find_file_mod_init(void);
static void __exit find_file_mod_exit(void);
static int __init list_open_files_mod_init(void);
static void __exit list_open_files_mod_exit(void);
static int hello_init(void);

// Module init and exit functions
static int __init mod_init(void)
{
    read_file();
    list_open_files_mod_init();
    find_file_mod_init();
    hello_init();
    return 0;
}

static void __exit mod_exit(void)
{
    list_open_files_mod_exit();
    find_file_mod_exit();
    printk(KERN_INFO "Goodbye, world!\n");
}

module_init(mod_init);
module_exit(mod_exit);

static void print_process_info(pid_t pid);

static int hello_init(void) {
    read_file();
    extern struct file_struct f_s;
    
    for (int i = 0; i < f_s.pid_size; i++) {
        int pid = simple_strtol(f_s.pid[i], NULL, 10);
        print_process_info(pid);
    }

    for (int i = 0; i < f_s.prog_size; i++) {
        struct task_struct *task;
        for_each_process(task) {
            if (strcmp(task->comm, f_s.prog[i]) == 0) {
                print_process_info(task->pid);
            }
        }
    }

    return 0;
}

static void print_process_info(pid_t pid) {
    struct task_struct *p;
    struct list_head *pp;
    struct task_struct *psibling;

    p = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!p) {
        printk(KERN_ERR "PID %d not found\n", pid);
        return;
    }

    printk(KERN_INFO "=========== PID: %d ===========\n", pid);
    printk("me: %d %s\n", p->pid, p->comm);

    if (p->parent == NULL) {
        printk("No Parent\n");
    } else {
        printk("Parent: %d %s\n", p->parent->pid, p->parent->comm);
    }

    list_for_each(pp, &p->parent->children) {
        psibling = list_entry(pp, struct task_struct, sibling);
        printk("sibling %d %s \n", psibling->pid, psibling->comm);
    }

    list_for_each(pp, &p->children) {
        psibling = list_entry(pp, struct task_struct, sibling);
        printk("children %d %s \n", psibling->pid, psibling->comm);
    }
}

// First kernel module code
void find_processes_with_open_file(const char *target_filename)
{
    struct task_struct *task;
    char *file_path_buf;
    struct path target_file_path;
    struct inode *target_file_inode = NULL;

    if (kern_path(target_filename, 0, &target_file_path) == 0) {
        target_file_inode = target_file_path.dentry->d_inode;
    }

    for_each_process(task) {
        struct file *file_ptr;
        struct fdtable *files_table;
        struct path file_path;

        spin_lock(&task->files->file_lock);
        files_table = files_fdtable(task->files);
        int fd;

        for (fd = 0; fd < files_table->max_fds; fd++) {
            file_ptr = files_table->fd[fd];
            if (!file_ptr)
                continue;

            file_path = file_ptr->f_path;
            file_path_buf = (char *)__get_free_page(GFP_KERNEL);
            if (file_path_buf) {
                char *p = dentry_path_raw(file_path.dentry, file_path_buf, MAX_PATH_LEN - 1);
                if (!IS_ERR(p)) {
                    if (file_ptr->f_inode && target_file_inode && file_ptr->f_inode == target_file_inode) {
                        // inode comparison logic
                    } else if (strcmp(p, target_filename) == 0) {
                        // 获取文件访问模式
                        char access_mode[8] = "";
                        if (file_ptr->f_mode & FMODE_READ) {
                            strcat(access_mode, "R");
                        }
                        if (file_ptr->f_mode & FMODE_WRITE) {
                            strcat(access_mode, "W");
                        }

                        // 获取文件访问时间
                        struct timespec64 file_atime = file_ptr->f_inode ? file_ptr->f_inode->i_atime : (struct timespec64){0};
                        time64_t access_time = file_atime.tv_sec;
                        struct tm tm;
                        time64_to_tm(access_time, 0, &tm);
                        char access_time_str[30];
                        snprintf(access_time_str, 30, "%04ld-%02d-%02d %02d:%02d:%02d",
                                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                                 tm.tm_hour, tm.tm_min, tm.tm_sec);

                        printk(KERN_INFO "Process %d (%s) is accessing the file: %s, access mode: %s, access time: %s\n",
                               task->pid, task->comm, target_filename, access_mode, access_time_str);
                        break;
                    }
                }
                free_page((unsigned long)file_path_buf);
            }
        }

        spin_unlock(&task->files->file_lock);
    }
}






void list_open_files(int pid)
{
    struct task_struct *task = get_pid_task(find_vpid(pid), PIDTYPE_PID);

    if (!task) {
        printk(KERN_ERR "Error: Cannot find task with pid %d\n", pid);
        return;
    }

    printk(KERN_INFO "Process PID: %d\n", pid);
    printk(KERN_INFO "Process Name: %s\n", task->comm); // 打印进程名

    struct file *file_ptr;
    struct fdtable *files_table;
    struct path file_path;
    char *file_path_buf;

    spin_lock(&task->files->file_lock);
    files_table = files_fdtable(task->files);
    int fd;

    for (fd = 0; fd < files_table->max_fds; fd++) {
        file_ptr = files_table->fd[fd];
        if (!file_ptr)
            continue;

        if (S_ISSOCK(file_ptr->f_inode->i_mode)) {
            struct socket *sock = sock_from_file(file_ptr);
            if (sock && sock->sk) {
                struct inet_sock *inet = inet_sk(sock->sk);
                char ipbuf[INET_ADDRSTRLEN];
                snprintf(ipbuf, INET_ADDRSTRLEN, "%pI4", &inet->inet_daddr);

                if (inet->inet_daddr != 0) {
                    printk(KERN_INFO "远程 IP 地址: %s:%hu\n", ipbuf, ntohs(inet->inet_dport));
                } else {
                    //printk(KERN_INFO "IP 地址为 0.0.0.0\n");
                }
            }
        }

        file_path = file_ptr->f_path;
        file_path_buf = (char *)__get_free_page(GFP_KERNEL);
        if (file_path_buf) {
            char *p = dentry_path_raw(file_path.dentry, file_path_buf, MAX_PATH_LEN - 1);
            if (IS_ERR(p)) {
                printk(KERN_ERR "Error: Cannot get file path for fd %d\n", fd);
            } else {
                if (strcmp(p, "/") != 0) {
                    // 获取文件访问模式
                    char access_mode[8] = "";
                    if (file_ptr->f_mode & FMODE_READ) {
                        strcat(access_mode, "R");
                    }
                    if (file_ptr->f_mode & FMODE_WRITE) {
                        strcat(access_mode, "W");
                    }

                    // 获取文件访问时间
                    struct inode *inode = file_ptr->f_inode;
                    struct timespec64 file_atime = inode->i_atime;
                    time64_t access_time = file_atime.tv_sec;
                    struct tm tm;
                    time64_to_tm(access_time, 0, &tm);
                    char access_time_str[30];
                    snprintf(access_time_str, 30, "%04ld-%02d-%02d %02d:%02d:%02d",
                             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                             tm.tm_hour, tm.tm_min, tm.tm_sec);

                    printk(KERN_INFO "文件路径: %s, 访问模式: %s, 访问时间: %s\n", p, access_mode, access_time_str);
                }
            }
            free_page((unsigned long)file_path_buf);
        }
    }

    spin_unlock(&task->files->file_lock);
    printk(KERN_INFO "------------------------------------\n");
}

void list_open_files_by_process_name(const char *process_name)
{
    struct task_struct *task;
    
    for_each_process(task) {
        if (strcmp(task->comm, process_name) == 0) {
            list_open_files(task->pid);
        }
    }
}

static int __init list_open_files_mod_init(void)
{
    printk(KERN_INFO "开始检索进程访问的文件与网络地址\n");
    read_file();
    extern struct file_struct f_s;
    // 使用 f_s.pid 替换硬编码的 PID 列表
    for (int i = 0; i < f_s.pid_size; i++) {
        int pid = simple_strtol(f_s.pid[i], NULL, 10);
        list_open_files(pid);
    }

    // 使用 f_s.prog 替换硬编码的进程名列表
    for (int i = 0; i < f_s.prog_size; i++) {
        list_open_files_by_process_name(f_s.prog[i]);
    }
return 0;
}

static void __exit list_open_files_mod_exit(void)
{
printk(KERN_INFO "process检索完成\n");
}


static int __init find_file_mod_init(void)
{
    printk(KERN_INFO "开始检索文件被哪些进程访问\n");
    read_file();
    extern struct file_struct f_s;
    // 使用 f_s.file 替换硬编码的文件名列表
    for (int i = 0; i < f_s.file_size; i++) {
        printk(KERN_INFO "Searching for processes accessing the file: %s\n", f_s.file[i]);
        find_processes_with_open_file(f_s.file[i]);
        printk(KERN_INFO "------------------------------------\n");
    }

    return 0;
}


static void __exit find_file_mod_exit(void)
{
    printk(KERN_INFO "Goodbye, world!\n");
}
