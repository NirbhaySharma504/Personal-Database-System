//IMT2023103
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv5.h"
#include "contact.h"
struct PDS_RepoInfo repo_handle;
struct Record{
  char name[30];
  int age;
};

//////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////
// pds_open
// Open the data file and index file in rb+ mode
// Function to use: fopen
// Update the fields of PDS_RepoInfo appropriately
// Call pds_load_ndx to load the index
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

//////////////////////////////////////////////////////////////////////////////
// pds_load_ndx
// This is Internal function used by pds_open
// Read the number of records form the index file
// Set up a loop to load the index entries into the BST ndx_root by calling bst_add_node repeatedly for each 
// index entry. Unlike array where you could load entire index, for BST, you add index entries
// one by one by reading the index file one by one in a loop
// (see bst_demo.c how to call bst_add_node in a loop)
// Function to use: fread in a loop
// Function to use: bst_add_node in the same loop
//
// Return values:
//
// PDS_FILE_ERROR - if the number of entries is lesser than the number of records
// PDS_SUCCESS - function completes without any error
int pds_load_ndx(){
    for (int i = 0; i < repo_handle.rec_count; i++) {
        struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
        fread(ndx_entry, sizeof(struct PDS_NdxInfo), 1, repo_handle.pds_ndx_fp);
        bst_add_node(&repo_handle.ndx_root, ndx_entry->key, (void *)ndx_entry);
    }
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// put_rec_by_key
// Seek to the end of the data file
// Function to use: fseek
// Identify the current location of file pointer
// Function to use: ftell
// Create new index entry using the key and ftell value
// [CHANGE] set the is_deleted flag to REC_NOT_DELETED
// Add index entry to BST by calling bst_add_node (see bst_demo.c how to call bst_add_node)
// Function to use: bst_add_node
// Increment record count
// Write the key
// Function to use: fwrite
// Write the record
// Function to use: fwrite
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_ADD_FAILED - if fwrite fails OR duplicate record is found
// PDS_SUCCESS - function completes without any error
int put_rec_by_key( int key, void *rec ){
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }
    if (bst_search(repo_handle.ndx_root, key) != NULL) {
        return PDS_ADD_FAILED;
    }
    fseek(repo_handle.pds_data_fp, 0, SEEK_END);
    long offset = ftell(repo_handle.pds_data_fp);
    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
    ndx_entry->key = key;
    ndx_entry->offset = offset;
    ndx_entry->is_deleted = REC_NOT_DELETED;
    bst_add_node(&repo_handle.ndx_root, key, (void *)ndx_entry);
    repo_handle.rec_count++;
    if (fwrite(&key, sizeof(int), 1, repo_handle.pds_data_fp) < 0 || fwrite(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp) < 0) {
        return PDS_ADD_FAILED;
    }

    return PDS_SUCCESS;
}
// get_rec_by_key
// Search for index entry in BST
// Function to use: bst_search
// 
// Type cast the void *data in the node to struct PDS_NdxInfo *
// [CHANGE] Return PDS_REC_NOT_FOUND if is_deleted_flag is REC_DELETED
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
// [CHANGE] PDS_REC_NOT_FOUND - if the key is not found or is_deleted_flag is REC_DELETED
// PDS_SUCCESS - function completes without any error
int get_rec_by_key( int key, void *rec ){
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }

    struct BST_Node *entry = bst_search(repo_handle.ndx_root, key);

    if (entry == NULL) {
        return PDS_REC_NOT_FOUND;
    }
    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)entry->data;
    if(ndx_entry->is_deleted==REC_DELETED){
        return PDS_REC_NOT_FOUND;
    }
    fseek(repo_handle.pds_data_fp, ndx_entry->offset, SEEK_SET);

    int key_read;
    fread(&key_read, sizeof(int), 1, repo_handle.pds_data_fp);
    fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);

    return PDS_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////////
// get_rec_by_field
// Brute-force retrieval using an arbitrary search value
// 	search_count = 0
// 	fread key from data file until EOF
//	search_count++
//  fread the record
//  Invoke the matcher using current record and search value
//	[CHANGE] if mathcher returns success, return the current record only if not deleted, else continue the loop
// end loop
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_REC_NOT_FOUND - if the record is not found or is_deleted_flag is REC_DELETED
// PDS_SUCCESS - function completes without any error
// comp_count should be the number of comparisons being done in the linear search
// 
int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count){
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }

    fseek(repo_handle.pds_data_fp, 0, SEEK_SET);
    *comp_count = 0;
    while (1) {
        int key;
        if (fread(&key, sizeof(int), 1, repo_handle.pds_data_fp) == 0) {
            break;
        }
        fread(rec, repo_handle.rec_size, 1, repo_handle.pds_data_fp);
        (*comp_count)++;
        if (matcher(rec, searchvalue) == 0) {
            struct BST_Node *entry = bst_search(repo_handle.ndx_root, key);
            struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)entry->data;
            if (ndx_entry->is_deleted == REC_DELETED) {
                continue;
            }
            return PDS_SUCCESS;
        }
    }

    return PDS_REC_NOT_FOUND; 
}
//////////////////////////////////////////////////////////////////////////////
// delete_rec_by_key
// Search for index entry in BST
// Function to use: bst_search
// Type cast the void *data in the node to struct PDS_NdxInfo *
// if is_deleted_flag is REC_NOT_DELETED
// 		Set the is_deleted flag to REC_DELETED
// else
// 		return PDS_REC_NOT_FOUND
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN
// PDS_REC_NOT_FOUND - if the key is not found or is already marked as REC_DELETED
// PDS_SUCCESS - function completes without any error
// 
int delete_rec_by_key( int key ){
    if (repo_handle.repo_status != PDS_REPO_OPEN) {
        return PDS_REPO_NOT_OPEN;
    }

    struct BST_Node *entry = bst_search(repo_handle.ndx_root, key);
    if (entry == NULL) {
        return PDS_REC_NOT_FOUND;
    }
    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)entry->data;
    if (ndx_entry->is_deleted == REC_DELETED) {
        return PDS_REC_NOT_FOUND;
    }
    ndx_entry->is_deleted = REC_DELETED;
    return PDS_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////////
// pds_close
// Open the index file in wb mode (write mode, not append mode)
// Function to use: fopen
// Store the number of records
// Function to use: fwrite
// Unload the index into the index file by traversing the BST in pre-order mode (overwrite the entire index file)
// See function bst_print in bst.c to see how to do tree traversal
// Function to use: fwrite
// Free the BST and set pointer to NULL
// Function to use: bst_free
// Close the index file and data file
// Function to use: fclose
//
// Return values:
//
// PDS_REPO_NOT_OPEN - if repo_status is not PDS_REPO_OPEN or if repo_name does not match with name stored in repo_info
// PDS_NDX_SAVE_FAILED - if fopen or fwrite fails
// PDS_SUCCESS - function completes without any error
int bst_pre_order_traversal(struct BST_Node *root, FILE *fp) {
    if (root == NULL || root->data == NULL || fp == NULL) {
    return PDS_SUCCESS;
}
    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)root->data;
    fwrite(ndx_entry, sizeof(struct PDS_NdxInfo), 1, fp);
    bst_pre_order_traversal(root->left_child, fp);
    bst_pre_order_traversal(root->right_child, fp);
    return PDS_SUCCESS;
}

int pds_close(char *repo_name) {
    if (repo_handle.repo_status != PDS_REPO_OPEN || strcmp(repo_handle.pds_name, repo_name) != 0) {
        return PDS_REPO_NOT_OPEN;
    }
    char ndx_file[50];
    strcpy(ndx_file, repo_name);
    strcat(ndx_file, ".ndx");
    FILE *fp = fopen(ndx_file, "wb");
    fwrite(&repo_handle.rec_count, sizeof(int), 1, fp);
    bst_pre_order_traversal(repo_handle.ndx_root, fp);
    bst_destroy(repo_handle.ndx_root);
    repo_handle.ndx_root = NULL;
    fclose(fp);
    fclose(repo_handle.pds_data_fp);
    fclose(repo_handle.pds_ndx_fp);
    repo_handle.repo_status = PDS_REPO_CLOSED;
    return PDS_SUCCESS;
}
