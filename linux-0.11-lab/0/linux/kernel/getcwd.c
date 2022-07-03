#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/segment.h>
#include <sys/stat.h>




#define BUF_MAX 256
char * sys_getcwd(char * buf, size_t size)
{	
	char temp[BUF_MAX], buff[BUF_MAX];
	int flag,len;
	struct m_inode *current_inode = current->pwd;
	struct m_inode *root_inode = current->root;
	//超级块
	struct super_block *super_b = (struct super_block *)bread(current_inode->i_dev,1);//超级块
	//首先找到索引节点基址
	struct m_inode *base_inode  = super_b->s_isup;
	//printk("base_inode:%d\n",base_inode->i_num);
	struct m_inode *father_inode;
	struct dir_entry *father_entrys;
	struct dir_entry *current_entrys;
	int i;
	while(1)
	{
		//指向当前目录下的某一目录项
		current_entrys =  (struct dir_entry *)bread(current_inode->i_dev,current_inode->i_zone[0])->b_data;
		int current_entrys_num = current_inode->i_size/sizeof(struct dir_entry);//当前目录的目录项数量
		//在当前目录中找到父目录的目录项
		flag = 0;
		for(i=0;i<current_entrys_num;i++){
			if(!strcmp((current_entrys+i)->name,"..")){//通过**目录项的名字**搜索父目录
				//printk("father_dir:%s\tnode_num%d\n",(current_entrys+i)->name,(current_entrys+i)->inode);
				flag = 1;
				father_inode = iget(current_inode->i_dev,(current_entrys + i)->inode);//父目录i节点
				break;
			}
		}
		if(!flag) {
			printk("Sorry,No ..\n");
			return NULL;
		}
		
		father_entrys = (struct dir_entry *)bread(father_inode->i_dev,father_inode->i_zone[0])->b_data;
		int father_entrys_num = father_inode->i_size/sizeof(struct dir_entry);//父目录的目录项数量
		//在父目录中寻找当前目录的目录项
		flag = 0;
		for(i=0;i<father_entrys_num;i++){
			if((father_entrys+i)->inode == current_inode->i_num){//通过**目录项的i节点序号**搜索子目录
				//printk("current_dir:%s\tnode_num%d\n",(father_entrys+i)->name,(father_entrys+i)->inode);
				memset(temp, 0, sizeof(temp));
				strcat(temp,"/");
				strcat(temp,(father_entrys+i)->name);
				strcat(temp,buff);
				strncpy(buff,temp,BUF_MAX);
				flag = 1;
				break;
			}
		}
		if(!flag){
			printk("Sorry, Current dircory is lost\n");
			return NULL;
		}	
		if(root_inode->i_num == father_inode->i_num)
			break;
		current_inode = father_inode;
	}
	//strcat(buff,'\0');
	//将位于核心段的buff拷贝到位于用户段的buf
	len = 0;
	while(buff[len++]);//计算cwd字符串长度
	//printk("cwd:%s\tlen:%d\n",buff,len);
	for(i=0;i<len;i++){
		put_fs_byte(buff[i],buf+i);
	}
	put_fs_byte('\0',buf+len);
	return buf;
}










/////////////////////////////////////////////////////////////////////////
// typedef struct
// 	{
// 	int	dd_fd;			/* file descriptor */
// 	int	dd_loc;			/* offset in block */
// 	int	dd_size;		/* amount of valid data */
// 	char	*dd_buf;	/* -> directory block */
// 	}	DIR;			    /* stream data from opendir() */

// struct direct {
// 	ino_t d_ino;
// 	char d_name[NAME_MAX];
// };

// static struct linux_dirent result;
// _syscall3(int,read,int,fildes, char *, buf, off_t ,count)
// struct linux_dirent * readdir(DIR * dir)
// {
// 	struct direct * ptr;

// 	if (!dir) {
// 		errno = EBADF;
// 		return NULL;
// 	}
// 	if (!dir->dd_buf)
// 		if (!(dir->dd_buf = malloc(DIRBUF)))
// 			return NULL;
// 		else 
// 			dir->dd_size = dir->dd_loc = 0;
// 	while (1) {
// 		if (dir->dd_size <= dir->dd_loc) {
// 			dir->dd_loc = 0;
// 			dir->dd_size = read(dir->dd_fd,dir->dd_buf,DIRBUF);
// 		}
// 		if (dir->dd_size <= 0)
// 			return NULL;
// 		ptr = (struct direct *) (dir->dd_loc + dir->dd_buf);
// 		dir->dd_loc += sizeof (*ptr);
// 		if (!ptr->d_ino)
// 			continue;
// 		result.d_ino = ptr->d_ino;
// 		strncpy(result.d_name,ptr->d_name,NAME_MAX);
// 		result.d_name[NAME_MAX] = 0;
// 		result.d_reclen = strlen(result.d_name);
// 		return &result;
// 	}
// }

// int closedir(DIR * dir)
// {
// 	int fd;

// 	if (!dir) {
// 		errno = EBADF;
// 		return -1;
// 	}
// 	fd = dir->dd_fd;
// 	free(dir->dd_buf);
// 	free(dir);
// 	return close(fd);
// }

// _syscall2(int ,fstat,int ,fildes, struct stat *,stat_buf)//fstat利用文件句柄(描述符)来获取信息
// DIR * opendir(const char * dirname)
// {
// 	int fd;
// 	struct stat stat_buf;
// 	DIR * ptr;

// 	if ((fd = open(dirname,O_RDONLY))<0)
// 		return NULL;
// 	if (fstat(fd,&stat_buf)<0 ||
// 	    !S_ISDIR(stat_buf.st_mode) ||
// 	    !(ptr=malloc(sizeof(*ptr)))) {
// 		close(fd);
// 		return NULL;
// 	}
// 	memset(ptr,0,sizeof(*ptr));
// 	ptr->dd_fd = fd;
// 	return ptr;
// }

// _syscall2(int ,stat,const char *,filename, struct stat *,stat_buf)//stat利用文件名来获取信息
// _syscall1(int,chdir,const char *, filename)//chdir跳转至目录名
// char * sys_getcwd(char * buf, size_t size){
    
//     char path[BUF_MAX], cwd[BUF_MAX], buff[BUF_MAX];
//     DIR *dirp;
//     struct linux_dirent *dp;
//     struct stat sb, sb_d, sb_1;
//     dev_t dev;
//     ino_t ino;
//     int i;

//     while(1){
//         //获取当前目录的文件信息
//         if(stat(".", &sb) == -1)
//             /*exit("stat");*/
//         dev = sb.st_dev;
//         ino = sb.st_ino;//节点信息
        
//         //获取父目录的对应的目录流和父目录的文件信息
//         if((dirp = opendir("..")) == NULL)
//             /*errExit("opendir");*/
//         if(stat("..", &sb_1) == -1)
//             /*exit("stat");*/

//         //判断当前目录是否与父目录相同
//         if(sb_1.st_dev == dev && sb_1.st_ino == ino)//索引节点号相同 && 设备号相同 ==>目录文件相同
//             break;
        
//         errno = 0;
        
//         //在父目录对应的目录流读取条目
//         while((dp = readdir(dirp)) != NULL){
//            /* snprintf(path, BUF_MAX, "../%s", dp->d_name);*/
//             printk("../%s", dp->d_name);
//             memcpy(path,dp->d_name,BUF_MAX);
//             printk("../%s", "usr");
//             if(stat(path, &sb_d) == -1)
//                /* exit("stat");*/

//             //得到当前目录对应的条目并将目录逐渐完善
//             if(dev == sb_d.st_dev && ino == sb_d.st_ino){
//                 memset(cwd, 0, sizeof(cwd));
//                 if(strcat(cwd, "/") == NULL)
//                    /* exit("strcat");*/
//                 if(strcat(cwd, dp->d_name) == NULL)
//                    /* exit("strcat");*/
//                 if(strcat(cwd, buf) == NULL)
//                    /* exit("strcat");*/

//                 if(strncpy(buf, cwd, BUF_MAX) == NULL)
//                    /* exit("strncpy");*/
//                 break;
//             }
            
//         }

//         if(dp == NULL && errno != 0)
//             /*exit("readdir");*/

//         closedir(dirp);
//         chdir("..");      //改变当前目录
//     }
//     //printk("%s\n",buff);
//     /*
//     for(i=0;i<10;i++)
//     put_fs_byte(*(buff+i), (buf+i));
//     put_fs_byte('\0', (buf+i));
//     */
//     return buf;
// }