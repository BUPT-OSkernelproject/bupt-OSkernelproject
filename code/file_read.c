#include "file_read.h"
#include "file_struct.c"

struct file_struct f_s;

extern struct file_struct f_s;

char *kernel_fget(char* buf, int max_size, struct file *fp){
	/*
	* 参数说明：buf：读取的字符串储存位置；max_size：最大读取的字符数 + 1；fp：内核态flip_open打开文件后获取的文件描述符
	* 函数功能：内核态从fp文件描述符中，读取最大max_size - 1字节的*一行*字符串，并储存在buf中
	* 返回值：正确情况下返回buf指针，否则NULL
	*/
	
	
	int i = 0;
	int read_size;  // vfs_read读取内容的大小
	
	if (0 > max_size){
		printk(KERN_EMERG "param max_size is invalid\n");
		return NULL;
	}

	read_size = kernel_read(fp, buf, max_size, &(fp->f_pos));

	if (1 > read_size){
		return NULL;
	}

	while (buf[i++] != '\n' && i < read_size);
	buf[i - 1] = '\0';  // 到这结束
	fp->f_pos += i - read_size;  // 修改文件指针位置
	
	return buf;
}


int read_file(void){
	char buf1[MAX_BUFFER];  // 读取文件内容
	struct file *fp;  // 储存打开文件的返回值
	
	
	fp = filp_open(FILE_NAME, O_RDWR, 0);

	if (IS_ERR(fp)) {
		printk(KERN_EMERG "open file error\n");
		return -1;
	}
	kernel_fget(buf1, MAX_BUFFER, fp);
	
	/*
	int pid[MAX_SIZE];
	memset(pid, 0, sizeof(pid));
	char* prog[MAX_SIZE];
	memset(prog, 0, sizeof(prog));
	char* file[MAX_SIZE];
	memset(file, 0, sizeof(file));
    */
	//int pid_size;
	//int prog_size;
	//int file_size;
	//char pid[MAX_BUFFER][MAX_SIZE];	
	//char prog[MAX_BUFFER][MAX_SIZE];	
	//char file[MAX_BUFFER][MAX_SIZE];	
	while(1){
		memset(buf1, 0, sizeof(buf1));
		if (NULL == kernel_fget(buf1, MAX_BUFFER, fp)){
			//printk(KERN_EMERG "file end...\n");
			filp_close(fp, NULL);
			break;	
		}
		//printk(KERN_INFO "%s", buf1);
		int count = 0;  // 给pid，prog，file数组计数
		int index;
		char title[MAX_BUFFER];  // 过滤名称
		memset(title, 0, sizeof(title));
		char temp_elem[MAX_BUFFER];  // 过滤名称下面的每一个小项
		memset(temp_elem, 0, sizeof(temp_elem));
		int frag;
		frag = 0;  // 当前在过滤标题还是内容
		for (index = 0; index < MAX_BUFFER; ++index){
			if (buf1[index] == NULL){  // 即使为空，也要把剩下缓冲的加进去
				if (strcmp(title, "pid") == 0){
					//printk(KERN_INFO "%s -- %s", temp_elem, title);
						memset(f_s.pid[count], 0, sizeof(f_s.pid[count]));
						// pid[count] = temp_elem;
						strncpy(f_s.pid[count], temp_elem, MAX_BUFFER);
						count += 1;
						f_s.pid_size = count;
						memset(temp_elem, 0, sizeof(temp_elem));
				}
				else if (strcmp(title, "prog") == 0){
					//printk(KERN_INFO "%s -- %s", temp_elem, title);
						memset(f_s.prog[count], 0, sizeof(f_s.pid[count]));
						// prog[count] = temp_elem;
						strncpy(f_s.prog[count], temp_elem, MAX_BUFFER);
						count += 1;
						f_s.prog_size = count;
						memset(temp_elem, 0, sizeof(temp_elem));		
				}
				else if (strcmp(title, "file") == 0){
					//printk(KERN_INFO "%s -- %s", temp_elem, title);
						memset(f_s.file[count], 0, sizeof(f_s.pid[count]));
						// file[count] = temp_elem;
						strncpy(f_s.file[count], temp_elem, MAX_BUFFER);
						count += 1;
						f_s.file_size = count;
						memset(temp_elem, 0, sizeof(temp_elem));		
				}
				else{
					//printk(KERN_EMERG "error in compare%s", title);
				}	
				break;
			}
			if (frag == 0){
				if (buf1[index] != ':'){
					char temp[] = {buf1[index]};
					temp[1] = '\0'; 	 	 
					strcat(title, temp);
				}
				else {
					frag = 1;
					//printk(KERN_INFO "%s", title); 			      
				}
			} 
			else{
				if (strcmp(title, "pid") == 0){
					if (buf1[index] != ','){
						char temp[] = {buf1[index]};
						temp[1] = '\0';
						// printk(KERN_INFO "%s", temp);
						strcat(temp_elem, temp);
					}
					else{
						//printk(KERN_INFO "%s -- %s", temp_elem, title);
						memset(f_s.pid[count], 0, sizeof(f_s.pid[count]));
						// pid[count] = temp_elem;
						strncpy(f_s.pid[count], temp_elem, MAX_BUFFER);
						count += 1;
						memset(temp_elem, 0, sizeof(temp_elem));				
					}
				}
				else if (strcmp(title, "prog") == 0){
					if (buf1[index] != ','){
						char temp[] = {buf1[index]};
						temp[1] = '\0';
						// printk(KERN_INFO "%s", temp);
						strcat(temp_elem, temp);
					}
					else{
						//printk(KERN_INFO "%s -- %s", temp_elem, title);
						memset(f_s.prog[count], 0, sizeof(f_s.prog[count]));
						// prog[count] = temp_elem;
						strncpy(f_s.prog[count], temp_elem, MAX_BUFFER);
						count += 1;
						memset(temp_elem, 0, sizeof(temp_elem));				
					}
				}
				else if (strcmp(title, "file") == 0){
					if (buf1[index] != ','){
						char temp[] = {buf1[index]};
						temp[1] = '\0';
						// printk(KERN_INFO "%s", temp);
						strcat(temp_elem, temp);
					}
					else{
						//printk(KERN_INFO "%s -- %s", temp_elem, title);
						memset(f_s.file[count], 0, sizeof(f_s.file[count]));
						// file[count] = temp_elem;
						strncpy(f_s.file[count], temp_elem, MAX_BUFFER);
						count += 1;
						memset(temp_elem, 0, sizeof(temp_elem));				
					}
				}
				else{
					//printk(KERN_EMERG "error in compare%s", title);
				}	
			}
			

			// printk(KERN_INFO "%c", buf1[index]);
		}
	}
	
	//printk(KERN_INFO "%d", f_s.pid_size);

	filp_close(fp, NULL);
	
	return 0;
}	
