#include <stdio.h>
#include <string.h>
#include "fs.h"

int main(){
    FILE *fp;
	int ret;
	struct partition buffer;
    struct dentry *d;
	

	memset(&buffer,0,sizeof(buffer));//buffer �ʱ�ȭ
	fp = fopen("disk.img","rb");
	
	while(1){
		ret = fread(&buffer,sizeof(buffer),1,fp); //������ buffer�� �о�帰��.
		if(ret==0) break;
	}
	fclose(fp);
  
  /* 
  root file�� inode�ȿ� physical disk number�� �ֱ� ������
  inode sturcture(super_block)�� ù��° inode number�� �˾Ƴ��� ���� ����Ҽ� ����.
  */
  printf("File name :%s\n",buffer.s.volume_name);
  printf("First inode number : 0x%x\n", buffer->s.first_inode);
  printf("First disk block number : 0x%x\n", buffer->s.first_data_block);
  printf("block : 0x%x\n",buffer.inode_table[buffer.s.first_inode].mode);
  


	d = (struct dentry*)&buffer.data_blocks[0]; //buffer�� �� ���� ����Ű���� ������ �Ҵ�
 
 //���ʷ� root directory file ���
  while(1){
		if(d->name[0]==0)//null-> break
			break; 
		printf("file name: %s index: %d \n", d->name, d->inode);
		long long addr = (long long)d;
		addr += d->dir_length;// dir_length = 0x2 -> 32byte -> ���� = 16byte -> 2�پ� �̵�
		d = (struct dentry*)addr;//���� inode�� ����Ų��.
	}
	

	return 0;
}
