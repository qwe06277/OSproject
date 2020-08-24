#include <stdio.h>
#include <string.h>
#include "fs.h"

int main(){
    FILE *fp;
	int ret;
	struct partition buffer;
    struct dentry *d;
	

	memset(&buffer,0,sizeof(buffer));//buffer 초기화
	fp = fopen("disk.img","rb");
	
	while(1){
		ret = fread(&buffer,sizeof(buffer),1,fp); //파일을 buffer로 읽어드린다.
		if(ret==0) break;
	}
	fclose(fp);
  
  /* 
  root file의 inode안에 physical disk number가 있기 때문에
  inode sturcture(super_block)에 첫번째 inode number을 알아내야 값을 출력할수 있음.
  */
  printf("File name :%s\n",buffer.s.volume_name);
  printf("First inode number : 0x%x\n", buffer->s.first_inode);
  printf("First disk block number : 0x%x\n", buffer->s.first_data_block);
  printf("block : 0x%x\n",buffer.inode_table[buffer.s.first_inode].mode);
  


	d = (struct dentry*)&buffer.data_blocks[0]; //buffer의 맨 앞을 가르키도록 포인터 할당
 
 //차례로 root directory file 출력
  while(1){
		if(d->name[0]==0)//null-> break
			break; 
		printf("file name: %s index: %d \n", d->name, d->inode);
		long long addr = (long long)d;
		addr += d->dir_length;// dir_length = 0x2 -> 32byte -> 한줄 = 16byte -> 2줄씩 이동
		d = (struct dentry*)addr;//다음 inode를 가리킨다.
	}
	

	return 0;
}
