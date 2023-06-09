#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#define MAX_BUFFER 100
#define MAX_SIZE 100
#define FILE_NAME "/home/plm/os/targets.txt"

char *kernel_fget(char* buf, int max_size, struct file *fp);

int read_file(void);
