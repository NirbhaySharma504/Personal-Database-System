//IMT2023103
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
// Structure of the index entry
struct PDS_NdxInfo{
	int key;
	int offset;
	int is_deleted;  // Can be REC_DELETED or REC_NOT_DELETED
};
struct Contact{
	int contact_id;
	char contact_name[30];
	char phone[15];
};
int main(int argc, char *argv[]){
    char datfile[50],ndxfile[50];
    strcpy(datfile,argv[1]);
    strcpy(ndxfile,argv[1]);
    strcat(datfile,".dat");
    strcat(ndxfile,".ndx");
    FILE *dat = fopen(datfile,"rb+");
    FILE *fp = fopen(ndxfile,"wb");
    int key,count=0;
    struct Contact rec;
    fwrite(&count,sizeof(int),1,fp);
    while(fread(&key,sizeof(int),1,dat)){
        count++;
        fread(&rec,sizeof(struct Contact),1,dat);
        struct PDS_NdxInfo pds;
        pds.key=key;
        pds.offset=ftell(dat)-sizeof(struct Contact)-sizeof(int);
        pds.is_deleted=0;
        
        fwrite(&pds,sizeof(struct PDS_NdxInfo),1,fp);
    }
    fseek(fp,0,SEEK_SET);
    fwrite(&count,sizeof(int),1,fp);
    fclose(dat);
    fclose(fp);
    return 0;
}
