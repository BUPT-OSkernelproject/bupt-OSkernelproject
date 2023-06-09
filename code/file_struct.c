#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#define MAX_BUFFER 100
#define MAX_SIZE 100

struct file_struct{
        int pid_size;
        int prog_size;
        int file_size;
        char pid[MAX_BUFFER][MAX_SIZE];
        char prog[MAX_BUFFER][MAX_SIZE];
        char file[MAX_BUFFER][MAX_SIZE];
};



