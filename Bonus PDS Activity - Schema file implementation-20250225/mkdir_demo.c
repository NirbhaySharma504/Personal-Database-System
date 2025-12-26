#include<stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//
// Usage: ./a.out folder1 folder2
// This program will create folder1 and another folder inside folder1 called folder2
//
int main( int argc, char *argv[])
{
	char *folder1 = argv[1];
	char *folder2 = argv[2];

	if(argc != 3){
		printf("Usage: %s <folder1> <folder2>\n", argv[0]);
		return 1;
	}
	// Create a folder in the current directory as given in the command line argument
	int mstatus = mkdir(folder1, 0777);
	if(mstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder %s created\n", folder1);
	}

	// Change directory to the newly created folder
	int cstatus = chdir(folder1);
	if(cstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder changed to %s\n", folder1);
	}

	// Create folder2 inside folder1
	mstatus = mkdir(folder2, 0777);
	if(mstatus == -1){
		perror("ERROR");
	}
	else{
		printf("Folder %s created\n", folder2);
	}
	return 0;
}

	
