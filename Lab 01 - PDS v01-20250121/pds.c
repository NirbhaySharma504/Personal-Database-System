//IMT2023103
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pds.h"
struct PDS_RepoInfo repo_handle;
struct Record{
  char name[30];
  int age;
};
// Open data files as per the following convention
// If repo_name is "demo", then data file should be "demo.dat"
// Initialize other members of PDS_RepoInfo global variable
// Open the file in "ab+" mode
// Append: open or create a file for update in binary mode; 
// File read/write location is placed at the end of the file
//////////////////////////////////////////////////////
// Functions to use: 
// fopen in binary append mode
//////////////////////////////////////////////////////
int pds_open( char *repo_name, int rec_size ){
    if (repo_handle.repo_status == PDS_REPO_OPEN) {
        return PDS_REPO_ALREADY_OPEN;
    }
    char file_name[30];
    strcpy(file_name, repo_name);
    strcat(file_name, ".dat");
    FILE *file = fopen(file_name, "ab+");
    if (file == NULL) {
        return PDS_FILE_ERROR; // Error opening file
    }
    strcpy(repo_handle.pds_name, repo_name);
    repo_handle.pds_data_fp = file;
    repo_handle.repo_status = PDS_REPO_OPEN;
    repo_handle.rec_size = rec_size;

    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////
// Store record in the data file
// Seek to the end of the data file using fseek with SEEK_END
// Write the <key,record> pair at the current file location
// Access the necessary fwrite parameters from repo_handle
//////////////////////////////////////////////////////
// Functions to use: Check sample code for usage
// fseek to end of file
// fwrite key
// fwrite record
//////////////////////////////////////////////////////
int put_rec_by_key( int key, void *rec ){
    if (repo_handle.repo_status == PDS_REPO_CLOSED) {
        return PDS_ADD_FAILED;
    }
    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    if(fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp)<1){
        return PDS_ADD_FAILED;
    }
    if(fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp)<1){
        return PDS_ADD_FAILED;
    }
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////
// Read the record based on the given key
// Seek to the beginning of the file using fseek with SEEK_SET
// Set up a loop to read <key, record> pair till the matching key is found
// Functions to use: Check sample code for usage
// fseek to beginning of file
// fread key
// fread record
//////////////////////////////////////////////////////
int get_rec_by_key( int key, void *rec ){
    if (repo_handle.repo_status == PDS_REPO_CLOSED) {
        return PDS_REC_NOT_FOUND;
    }
    fseek(repo_handle.pds_data_fp, 0, SEEK_SET);
    int file_key;
    struct Record file_rec;
    int success=0;
    while(fread(&file_key, sizeof(int), 1, repo_handle.pds_data_fp) > 0){
        fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
        if (file_key == key) {
            success=1;
        }
    }
    if(success==1){
        return PDS_SUCCESS;
    } 
    return PDS_REC_NOT_FOUND;
}

// Close all files and reset repo_handle values 
// // Functions to use: Check sample code for usage
// fclose all file pointers
int pds_close(){
    if (repo_handle.repo_status == PDS_REPO_CLOSED) {
        return PDS_REPO_ALREADY_CLOSED;
    }
    fclose(repo_handle.pds_data_fp);
    repo_handle.repo_status = PDS_REPO_CLOSED;
    return PDS_SUCCESS;
}