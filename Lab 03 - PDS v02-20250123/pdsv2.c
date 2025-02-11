//IMT2023103
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv2.h"
struct PDS_RepoInfo repo_handle;
struct Record{
  char name[30];
  int age;
};
// pds_create
// Open the data file and index file in "wb" mode
// Function to use: fopen
// Initialize index file by storing "0" to indicate there are zero entries in index file
// Function to use: fwrite
// close the files
//
// Return values:
//
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_SUCCESS - function completes without any error
//

int pds_create(char *repo_name){
    char data_file[50],ndx_file[50];
    strcpy(data_file,repo_name);
    strcat(data_file,".dat");
    strcpy(ndx_file,repo_name);
    strcat(ndx_file,".ndx");
    FILE *fp=fopen(data_file,"wb+");
    if(fp==NULL){
        return PDS_FILE_ERROR;
    }
    fclose(fp);
    fp=fopen(ndx_file,"wb+");
    if(fp==NULL){
        return PDS_FILE_ERROR;
    }
    int entries=0;
    fwrite(&entries,sizeof(int),1,fp);
    fclose(fp);
    return PDS_SUCCESS;
}
// pds_open
// Open the data file and index file in rb+ mode
// Function to use: fopen
// Update the fields of PDS_RepoInfo appropriately
// Read the number of records form the index file
// Function to use: fread
// Load the index into the array and store in ndx_array by reading index entries from the index file
// // Function to use: fread
// Close only the index file
// Function to use: fclose
//
// Return values:
//
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_REPO_ALREADY_OPEN - if repo_status is PDS_REPO_OPEN
// PDS_SUCCESS - function completes without any error
int pds_open( char *repo_name, int rec_size ){
    if (repo_handle.repo_status == PDS_REPO_OPEN) {
        return PDS_REPO_ALREADY_OPEN;
    }
    char data_file[50],ndx_file[50];
    strcpy(data_file,repo_name);
    strcat(data_file,".dat");
    strcpy(ndx_file,repo_name);
    strcat(ndx_file,".ndx");
    FILE *dat_fp=fopen(data_file,"rb+");
    if(dat_fp==NULL){
        return PDS_FILE_ERROR;
    }
    FILE *ndx_fp = fopen(ndx_file,"rb+");
    if(ndx_fp==NULL){
        return PDS_FILE_ERROR;
    }

    strcpy(repo_handle.pds_name, repo_name);
    repo_handle.pds_data_fp = dat_fp;
    repo_handle.pds_ndx_fp = ndx_fp;
    repo_handle.repo_status = PDS_REPO_OPEN;
    repo_handle.rec_size = rec_size;
    fread(&repo_handle.rec_count, sizeof(int), 1, ndx_fp);
    pds_load_ndx();
    fclose(ndx_fp);
    return PDS_SUCCESS;
}

// pds_load_ndx
// Internal function used by pds_open to read index entries into ndx_array
// Function to use: fread
//
// Return values:
//
// PDS_FILE_ERROR - if fread returns less than num_records*8 bytes
// PDS_SUCCESS - function completes without any error
int pds_load_ndx(){
    FILE *fp = repo_handle.pds_ndx_fp;
    fread(repo_handle.ndx_array, sizeof(struct PDS_NdxInfo), repo_handle.rec_count, fp);
    return PDS_SUCCESS;
}

// put_rec_by_key
// Seek to the end of the data file
// Function to use: fseek
// Identify the current location of file pointer
// Function to use: ftell
// Create an index entry with the current data file location using ftell
// Add index entry to ndx_array using offset returned by ftell
// Increment record count
// Write the record at the end of the data file
// Function to use: fwrite
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_ADD_FAILED - if fwrite fails OR duplicate record is found
// PDS_SUCCESS - function completes without any error
int put_rec_by_key( int key, void *rec ){
    if (repo_handle.repo_status == PDS_REPO_CLOSED) {
        return PDS_REPO_NOT_OPEN;
    }
    for(int i=0; i<repo_handle.rec_count; i++){
        if(repo_handle.ndx_array[i].key == key){
            return PDS_ADD_FAILED;
        }
    }
    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    int offset = ftell(repo_handle.pds_data_fp);
    fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp);
    fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
    repo_handle.ndx_array[repo_handle.rec_count].key = key;
    repo_handle.ndx_array[repo_handle.rec_count].offset = offset;
    repo_handle.rec_count++;
    return PDS_SUCCESS;
}


// get_rec_by_key
// Search for index entry in ndx_array
// Seek to the file location based on offset in index entry
// Function to use: fseek
// Read the key at the current location 
// Function to use: fread
// Read the record from the current location
// Function to use: fread
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_REC_NOT_FOUND - if the key is not found
// PDS_SUCCESS - function completes without any error
int get_rec_by_key( int key, void *rec ){
    if (repo_handle.repo_status == PDS_REPO_CLOSED) {
        return PDS_REPO_NOT_OPEN;
    }
    for(int i=0; i<repo_handle.rec_count; i++){
        if(repo_handle.ndx_array[i].key == key){
            fseek(repo_handle.pds_data_fp, repo_handle.ndx_array[i].offset, SEEK_SET);
            fread(&key, sizeof(int), 1, repo_handle.pds_data_fp);
            fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
            return PDS_SUCCESS;
        }
    }
    return PDS_REC_NOT_FOUND;
}

// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Function to use: fopen
// Store the number of records
// Function to use: fwrite
// Unload the ndx_array into the index file (overwrite the entire index file)
// Function to use: fwrite
// Close the index file and data file
// Function to use: fclose
//
// Return values:
//
// PDS_NDX_SAVE_FAILED - if fopen or fwrite fails
// PDS_SUCCESS - function completes without any error
int pds_close( char * repo_name ){
    if (strcmp(repo_name, repo_handle.pds_name) != 0) {
        return PDS_REPO_NOT_OPEN;
    }
    else if(repo_handle.repo_status == PDS_REPO_CLOSED){
        return PDS_REPO_NOT_OPEN;
    }
    char ndx_file[50];
    strcpy(ndx_file, repo_name);
    strcat(ndx_file, ".ndx");
    FILE *fp = fopen(ndx_file, "wb");
    if(fp == NULL){
        return PDS_FILE_ERROR;
    }
    fwrite(&repo_handle.rec_count, sizeof(int), 1, fp);
    fwrite(repo_handle.ndx_array, sizeof(struct PDS_NdxInfo), repo_handle.rec_count, fp);
    fclose(repo_handle.pds_data_fp);
    repo_handle.repo_status = PDS_REPO_CLOSED;
    fclose(fp);
    return PDS_SUCCESS;
}