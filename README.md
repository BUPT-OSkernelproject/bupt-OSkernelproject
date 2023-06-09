# bupt-OSkernelproject
北邮操作系统课程设计——基于linux的内核模块编写，本模块由本组共同完成，供开源参考

具体任务要求如下：
开发一个内核模块或组件，完成如下功能:

读取 ~/targets 文件。格式如下，pid: 100,234prog: programanother programfile: a filename; another filename记录 pid 或 prog 进程访问了哪些文件和 IP 地址，记录 file 文件被哪些进程所访问。记录内容至少包括进程 pid 和程序名、日期时间访问模式。

当进程或文件数量不大于 5人，展示它们的关系，例如，进程之间的父子关系，某个文件被哪此进程并发访问。

支持记录最多 20 个进程，给出进程分别为 5,10,20 个情况下的模块性能，包括 CPU 和内存使用情况。
