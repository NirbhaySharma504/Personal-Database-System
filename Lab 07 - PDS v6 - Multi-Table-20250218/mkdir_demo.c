#include<stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main( int argc, char *argv[])
{
	if(argc != 3){
		printf("Usage: %s <foldername>\n", argv[0]);
		return 1;
	}
	int mstatus = mkdir(argv[1], 0777);
	if(mstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder created\n");
	}
	int cstatus = chdir(argv[2]);
	if(cstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder changed\n");
	}
	mstatus = mkdir(argv[1], 0777);
	if(mstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder created\n");
	}
	return 0;
}

	
