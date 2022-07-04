#define __LIBRARY__
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/segment.h>
#include <sys/stat.h>




#define NAME_MAX     14    //必须为14，因为一个目录项16个字节，前两个字节是索引节点号

/////////////////////////sys_getdents//////////////////////////
struct linux_dirent {                         //专为输出的目录项结构
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[NAME_MAX+1];
};

int sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
    //初始化
    struct file         *file;          //文件结构
    struct m_inode      *inode;         //索引节点结构inode
    struct buffer_head  *bufhd;         //缓冲块头结构
    struct dir_entry 	*d_entry;     //目录项结构(内存)

    int linux_dirent_size   = sizeof(struct linux_dirent);
    int dir_entry_size      = sizeof(struct dir_entry);
    int l = 0;//l为已读linux_dirent总字节数
    int d = 0;//d为已读页表项的总字节数
	int i;

    //检查参数合法性
    if(fd >= NR_OPEN)   return -1;          //目录的文件描述符错误
    if(count < linux_dirent_size) return -1;//写入的字节数不足一个linux_dirent
    file = current->filp[fd];
    if(!file) return -1;                    //没读到目录文件

    //读目录
    inode = file->f_inode;              //目录的索引节点
    struct linux_dirent *dirp_tmp;      //专为输出的目录项结构,为dirp的赋值而开
    dirp_tmp = (struct linux_dirent *)malloc(linux_dirent_size);
    char * buf;                         //为dirp的赋值而开，大小为一个linux_dirent
    buf = (char*)malloc(linux_dirent_size);

    bufhd = bread(inode->i_dev , inode->i_zone[0]);//读出目录索引节点中的第一个数据块的内容

    for( ; d < inode->i_size ; d += dir_entry_size)
    {
		//printk("***\n");
        if (l >= count - linux_dirent_size) break;//dirp已经满了，返回读取的字节数     
        
        //bufhd->b_data 是char*类型的指针(char为一字节)，因此+d可以到这个目录项
        d_entry = (struct dir_entry *)(bufhd->b_data + d);
        //printk("&&&\n");
        //读到了目录的最后一项，则返回0
        if(!d_entry->inode) continue;
        
        //将目录项信息从dir_entry转移到dirp_tmp						 //d_entry(dir_entry)----->dirp_tmp(linux_dirent)
        dirp_tmp->d_ino = d_entry->inode;
        dirp_tmp->d_off = 0; 
        dirp_tmp->d_reclen = linux_dirent_size;
        strcpy(dirp_tmp->d_name,d_entry->name);

        //将dirp_tmp的信息转化成char类型并存到buf                     //dirp_tmp(linux_dirent)--->buf(char)
        memcpy(buf,dirp_tmp,linux_dirent_size);
        //利用put_fs_byte将buf(在核心数据段)逐字节存入dirp(在用户数据段，段基址在fs，即fs:[addr])   
        for(i=0 ; i < linux_dirent_size ; i++){						//buf(char)--->dirp数组(linux_dirent)
            put_fs_byte(*(buf+i), ((char*)dirp)+l+i);
        }
        l += linux_dirent_size;
    }
	return l;
}