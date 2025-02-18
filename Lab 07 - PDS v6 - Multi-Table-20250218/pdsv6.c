//IMT2023103
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv6.h"
#include "contact.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
struct PDS_DBInfo repo_handle;
struct Record{
  char name[30];
  int age;
};

// [NEW]
//////////////////////////////////////////////////////////////////////////////
// pds_create_db
//
// Create a new folder named repo_name
// Function to use: mkdir
//
// Create file named repo_name/repo_name.info
//
// Store the name of database into the info file
// Function to use: fwrite
// close the files
// NOTHING need on PDS_DBInfo
//
// Return values:
//
// PDS_FILE_ERROR - if mkdir returns -1
// PDS_SUCCESS - function completes without any error
//
int pds_create_db( char *dbname ){
	int mstatus = mkdir(dbname,0777);
	if(mstatus == -1){
		return PDS_FILE_ERROR;
	}
    char file[100];
    sprintf(file,"%s/%s.info",dbname,dbname);
	FILE *fp = fopen(file,"wb");
	if(fp == NULL){
		perror("ERROR");
		return PDS_FILE_ERROR;
	}
	fwrite(dbname,sizeof(repo_handle.pds_repo_name),1,fp);
	fclose(fp);
}
// [NEW]
//////////////////////////////////////////////////////////////////////////////
// pds_open_db
//
// Change the director to folder named dbname
// Function to use: chdir
//
// Open the file named dbname.info in wb+
// Update the staus of PDS_DBInfo to PDS_REPO_OPEN
//
// Read the name of database from the info file
// Function to use: fread
//
// Compare value read from info file matches the input dbname
//
// Return values:
//
// PDS_DB_ALREADY_OPEN - if open is being attempted on a db that is already open
// PDS_FILE_ERROR - if chdir returns -1
// PDS_FILE_ERROR - if dbname in info does not match with given name
// PDS_SUCCESS - function completes without any error
//
int pds_open_db( char *dbname ){
    int mstatus = chdir(dbname);
    if(mstatus==-1){
        return PDS_FILE_ERROR;
    }
    if(repo_handle.repo_status==PDS_DB_OPEN){
        return PDS_DB_ALREADY_OPEN;
    }
    char name[sizeof(repo_handle.pds_repo_name)];
    sprintf(name,"%s.info",dbname);
    FILE *fp = fopen(name,"rb+");
    fread(&name,sizeof(repo_handle.pds_repo_name),1,fp);
    if(strcmp(name,dbname)){
        return PDS_FILE_ERROR;
    }
    repo_handle.repo_status = PDS_DB_OPEN;
    repo_handle.num_tables=0;
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// helper_get_table_info
//
// Function return the table_info from the array based on name
//
// Return values:
// Pointer to tableinfo - Use & to return the appropriate element from the array
// NULL - if table_name is not found in the tables array of DBInfo
//
struct PDS_TableInfo *helper_get_table_info( char *table_name ){
    for(int i=0;i<repo_handle.num_tables;i++){
        if(strcmp(repo_handle.tables[i].table_name,table_name)==0){
            return &repo_handle.tables[i];
        }
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// pds_create_table
//
// Open the data file and index file in "wb" mode
// Function to use: fopen
//
// Write 0 to index file
// Function to use: fwrite/ 
//
// No updates needed to table needed to PDS_TableInfo
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_FILE_ERROR - if fwrite fails
//
// PDS_SUCCESS - function completes without any error
//
// [OLD PROTOTYPE] int pds_create(char *repo_name);
int pds_create_table( char *table_name ){
    if(repo_handle.repo_status!=PDS_DB_OPEN){
        return PDS_DB_NOT_OPEN;
    }
    char data_file[50],ndx_file[50];
    strcpy(data_file,table_name);
    strcat(data_file,".dat");
    strcpy(ndx_file,table_name);
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
    if(!fwrite(&entries,sizeof(int),1,fp)){
        return PDS_FILE_ERROR;
    }
    fclose(fp);
    return PDS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
// pds_open_table
// Open the data file and index file in rb+ mode
// Function to use: fopen
// Update the fields of PDS_TableInfo appropriately
// Call pds_load_ndx to load the index
// Close only the index file
// Function to use: fclose
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_FILE_ERROR - if fopen returns NULL
// PDS_TABLE_ALREADY_OPEN - if table_status is PDS_TABLE_OPEN
// PDS_SUCCESS - function completes without any error
// int pds_open( char *repo_name, int rec_size );
int pds_open_table( char *table_name, int rec_size ){
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    char data_file[50],ndx_file[50];
    strcpy(data_file,table_name);
    strcat(data_file,".dat");
    strcpy(ndx_file,table_name);
    strcat(ndx_file,".ndx");
    FILE *dat_fp=fopen(data_file,"rb+");
    if(dat_fp==NULL){
        return PDS_FILE_ERROR;
    }
    FILE *ndx_fp = fopen(ndx_file,"rb+");
    if(ndx_fp==NULL){
        return PDS_FILE_ERROR;
    }
    struct PDS_TableInfo *handle = helper_get_table_info(table_name);
    if(handle==NULL){
        char data_file[50],ndx_file[50];
        strcpy(data_file,table_name);
        strcat(data_file,".dat");
        FILE *fp = fopen(data_file,"r");
        if(fp==NULL){
            return PDS_FILE_ERROR;
        }
        struct PDS_TableInfo table_handle;
        strcpy(table_handle.table_name, table_name);
        table_handle.table_status = PDS_TABLE_OPEN;
        table_handle.data_fp = dat_fp;
        table_handle.ndx_fp = ndx_fp;
        table_handle.table_status = PDS_TABLE_OPEN;
        table_handle.rec_size = rec_size;
        fread(&table_handle.rec_count, sizeof(int), 1, ndx_fp);
        pds_load_ndx(&table_handle);
        repo_handle.tables[repo_handle.num_tables++]=table_handle;
        fclose(ndx_fp);
        return PDS_SUCCESS;
    }
    else if(handle->table_status==PDS_TABLE_OPEN){
        return PDS_TABLE_ALREADY_OPEN;
    }
    handle->table_status = PDS_TABLE_OPEN;
    handle->data_fp = dat_fp;
    handle->ndx_fp = ndx_fp;
    handle->table_status = PDS_TABLE_OPEN;
    handle->rec_size = rec_size;
    fread(&handle->rec_count, sizeof(int), 1, ndx_fp);
    pds_load_ndx(handle);
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
//
// [OLD-PROTOTYPE] int pds_load_ndx();
int pds_load_ndx( struct PDS_TableInfo *tablePtr ){
    for (int i = 0; i < tablePtr->rec_count; i++) {
        struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
        fread(ndx_entry, sizeof(struct PDS_NdxInfo), 1, tablePtr->ndx_fp);
        bst_add_node(&tablePtr->ndx_root, ndx_entry->key, (void *)ndx_entry);
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
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_ADD_FAILED - if fwrite fails OR duplicate record is found
// PDS_SUCCESS - function completes without any error
// [OLD PROTOTYPE] int put_rec_by_key( int key, void *rec );
int put_rec_by_key( char *table_name, int key, void *rec ){}


//////////////////////////////////////////////////////////////////////////////
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
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_REC_NOT_FOUND - if the key is not found or is_deleted_flag is REC_DELETED
// PDS_SUCCESS - function completes without any error
// [OLD PROTOTYPE] int get_rec_by_key( int key, void *rec );
int get_rec_by_key( char *table_name, int key, void *rec ){}

//////////////////////////////////////////////////////////////////////////////
// get_rec_by_field
// Brute-force retrieval using an arbitrary search value
// 	search_count = 0
// 	fread key from data file until EOF
//	search_count++
//  fread the record
//  Invoke the matcher using current record and search value
//	if mathcher returns success, return the current record only if not deleted, else continue the loop
// end loop
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_REC_NOT_FOUND - if the record is not found or is_deleted_flag is REC_DELETED
// PDS_SUCCESS - function completes without any error
// comp_count should be the number of comparisons being done in the linear search
// 
// [OLD PROTOTYPE] int get_rec_by_field(void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count);
int get_rec_by_field( char *table_name, void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count){}

//////////////////////////////////////////////////////////////////////////////
// delete_rec_by_key
//
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
// PDS_DB_NOT_OPEN - if database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_REC_NOT_FOUND - if the key is not found or is already marked as REC_DELETED
// PDS_SUCCESS - function completes without any error
// 
// [OLD PROTOTYPE] int delete_rec_by_key( int key );
int delete_rec_by_key( char *table_name, int key ){}


//////////////////////////////////////////////////////////////////////////////
// pds_close
//
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
// PDS_DB_NOT_OPEN - If database is not open
// PDS_TABLE_NOT_OPEN - if table status is not PDS_TABLE_OPEN
// PDS_NDX_SAVE_FAILED - if fopen or fwrite fails
// PDS_SUCCESS - function completes without any error
// [OLD PROTOTYPE] int pds_close( char * repo_name );
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
int pds_close_table( char *table_name ){
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    struct PDS_TableInfo *table_handle = helper_get_table_info(table_name);
    if(table_handle==NULL){
        return PDS_FILE_ERROR;
    }
    if(table_handle->table_status!=PDS_TABLE_OPEN){
        return PDS_TABLE_NOT_OPEN;
    }
    if (table_handle->table_status != PDS_TABLE_OPEN || strcmp(table_handle->table_name, table_name) != 0) {
        return PDS_TABLE_NOT_OPEN;
    }
    char ndx_file[50];
    strcpy(ndx_file, table_name);
    strcat(ndx_file, ".ndx");
    FILE *fp = fopen(ndx_file, "wb");
    fwrite(&table_handle->rec_count, sizeof(int), 1, fp);
    bst_pre_order_traversal(table_handle->ndx_root, fp);
    bst_destroy(table_handle->ndx_root);
    table_handle->ndx_root = NULL;
    fclose(fp);
    fclose(table_handle->data_fp);
    fclose(table_handle->ndx_fp);
    table_handle->data_fp=NULL;
    table_handle->ndx_fp=NULL;
    table_handle->table_status = PDS_TABLE_CLOSED;
    return PDS_SUCCESS;
}

// [NEW]
//////////////////////////////////////////////////////////////////////////////
// pds_close_db
//
// Loop through PDS_TableInfo array
// If the table status is OPEN, then call pds_close_table
// Change the folder to parent folder by calling chdir("..")
int pds_close_db( char *dbname ){
    for(int i=0;i<repo_handle.num_tables;i++){
        if(repo_handle.tables[i].table_status!=PDS_TABLE_CLOSED){
            repo_handle.tables[i].table_status=PDS_TABLE_CLOSED;
        }
    }
    repo_handle.repo_status=PDS_DB_CLOSED;
    chdir("..");
    return PDS_SUCCESS;
}