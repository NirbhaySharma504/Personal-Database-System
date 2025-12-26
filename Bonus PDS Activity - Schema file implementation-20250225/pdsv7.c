#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pdsv7.h"
#include "contact.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct PDS_DBInfo repo_handle;

struct Record{
  char name[30];
  int age;
};

// [MODIFIED]
//////////////////////////////////////////////////////////////////////////////
// pds_create_db
//
// Create a new folder named repo_name
// Create file named repo_name/repo_name.info
// Store the name of database and initialize table count to 0 into the info file
//
// Return values:
//
// PDS_FILE_ERROR - if mkdir returns -1 or if file operations fail
// PDS_SUCCESS - function completes without any error
//
int pds_create_db(char *dbname) {
    int mstatus = mkdir(dbname, 0777);
    if(mstatus == -1) {
        return PDS_FILE_ERROR;
    }
    
    char file[100];
    sprintf(file, "%s/%s.info", dbname, dbname);
    
    FILE *fp = fopen(file, "wb");
    if(fp == NULL) {
        perror("ERROR");
        return PDS_FILE_ERROR;
    }
    
    // Write database name
    fwrite(dbname, sizeof(repo_handle.pds_repo_name), 1, fp);
    
    // Initialize table count to 0
    int table_count = 0;
    fwrite(&table_count, sizeof(int), 1, fp);
    
    fclose(fp);
    return PDS_SUCCESS;
}

// [MODIFIED]
//////////////////////////////////////////////////////////////////////////////
// pds_open_db
//
// Change the directory to folder named dbname
// Open the file named dbname.info in rb+
// Read database name and table information from the info file
// Update the repo_handle with information read from the info file
//
// Return values:
//
// PDS_DB_ALREADY_OPEN - if open is being attempted on a db that is already open
// PDS_FILE_ERROR - if chdir returns -1 or if file operations fail
// PDS_FILE_ERROR - if dbname in info does not match with given name
// PDS_SUCCESS - function completes without any error
//
int pds_open_db(char *dbname) {
    int mstatus = chdir(dbname);
    if(mstatus == -1) {
        return PDS_FILE_ERROR;
    }
    
    if(repo_handle.repo_status == PDS_DB_OPEN) {
        return PDS_DB_ALREADY_OPEN;
    }
    
    char info_filename[100];
    sprintf(info_filename, "%s.info", dbname);
    
    FILE *fp = fopen(info_filename, "rb+");
    if(fp == NULL) {
        return PDS_FILE_ERROR;
    }
    
    // Read database name
    char db_name_read[sizeof(repo_handle.pds_repo_name)];
    if(fread(db_name_read, sizeof(repo_handle.pds_repo_name), 1, fp) != 1) {
        fclose(fp);
        return PDS_FILE_ERROR;
    }
    
    if(strcmp(db_name_read, dbname) != 0) {
        fclose(fp);
        return PDS_FILE_ERROR;
    }
    
    // Read number of tables
    int num_tables;
    if(fread(&num_tables, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return PDS_FILE_ERROR;
    }
    
    // Initialize repo_handle
    strcpy(repo_handle.pds_repo_name, dbname);
    repo_handle.repo_status = PDS_DB_OPEN;
    repo_handle.num_tables = num_tables;
    
    // Read table information if tables exist
    for(int i = 0; i < num_tables; i++) {
        struct PDS_TableInfo table_info;
        
        // Read table name
        if(fread(table_info.table_name, sizeof(table_info.table_name), 1, fp) != 1) {
            fclose(fp);
            return PDS_FILE_ERROR;
        }
        
        // Read record size
        if(fread(&table_info.rec_size, sizeof(int), 1, fp) != 1) {
            fclose(fp);
            return PDS_FILE_ERROR;
        }
        repo_handle.tables[i] = table_info;
        
        pds_open_table(table_info.table_name,table_info.rec_size);
    }
    
    fclose(fp);
    return PDS_SUCCESS;
}

// [NO CHANGE]
//////////////////////////////////////////////////////////////////////////////
// helper_get_table_info
//
// Function return the table_info from the array based on name
//
struct PDS_TableInfo *helper_get_table_info(char *table_name) {
    for(int i = 0; i < repo_handle.num_tables; i++) {
        if(strcmp(repo_handle.tables[i].table_name, table_name) == 0) {
            return &repo_handle.tables[i];
        }
    }
    return NULL;
}

// [MODIFIED]
//////////////////////////////////////////////////////////////////////////////
// pds_create_table
//
// Open the data file and index file in "wb" mode
// Create table files and update repo_handle with new table info
// Update the info file to include the new table
//
// Return values:
//
// PDS_DB_NOT_OPEN - if database is not open
// PDS_FILE_ERROR - if file operations fail
// PDS_SUCCESS - function completes without any error
//
int pds_create_table(char *table_name, int rec_size) {
    if(repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    // Check if table already exists
    if(helper_get_table_info(table_name) != NULL) {
        return PDS_SUCCESS;
    }
    
    char data_file[50], ndx_file[50];
    strcpy(data_file, table_name);
    strcat(data_file, ".dat");
    strcpy(ndx_file, table_name);
    strcat(ndx_file, ".ndx");
    
    // Create data file
    FILE *fp = fopen(data_file, "wb+");
    if(fp == NULL) {
        return PDS_FILE_ERROR;
    }
    fclose(fp);
    
    // Create index file
    fp = fopen(ndx_file, "wb+");
    if(fp == NULL) {
        return PDS_FILE_ERROR;
    }
    
    int entries = 0;
    if(!fwrite(&entries, sizeof(int), 1, fp)) {
        fclose(fp);
        return PDS_FILE_ERROR;
    }
    fclose(fp);
    
    // Add table info to repo_handle
    struct PDS_TableInfo table_info;
    strcpy(table_info.table_name, table_name);
    table_info.table_status = PDS_TABLE_CLOSED;
    table_info.rec_size = rec_size;
    table_info.data_fp = NULL;
    table_info.ndx_fp = NULL;
    table_info.ndx_root = NULL;
    table_info.rec_count = 0;
    
    repo_handle.tables[repo_handle.num_tables] = table_info;
    repo_handle.num_tables++;
    
    // Update info file with new table information
    char info_filename[100];
    sprintf(info_filename, "%s.info", repo_handle.pds_repo_name);
    
    fp = fopen(info_filename, "rb+");
    if(fp == NULL) {
        return PDS_FILE_ERROR;
    }
    
    // Update number of tables
    fseek(fp, sizeof(repo_handle.pds_repo_name), SEEK_SET);
    fwrite(&repo_handle.num_tables, sizeof(int), 1, fp);
    
    // Append new table information at the end
    fseek(fp, 0, SEEK_END);
    fwrite(table_info.table_name, sizeof(table_info.table_name), 1, fp);
    fwrite(&table_info.rec_size, sizeof(int), 1, fp);
    
    fclose(fp);
    pds_open_table(table_name,rec_size);
    return PDS_SUCCESS;
}

// [NO MAJOR CHANGE - Minor updates to handle existing tables]
//////////////////////////////////////////////////////////////////////////////
// pds_open_table
// Open the data file and index file in rb+ mode
// Update the fields of PDS_TableInfo appropriately
//
int pds_open_table(char *table_name, int rec_size) {
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    char data_file[50], ndx_file[50];
    strcpy(data_file, table_name);
    strcat(data_file, ".dat");
    strcpy(ndx_file, table_name);
    strcat(ndx_file, ".ndx");
    
    FILE *dat_fp = fopen(data_file, "rb+");
    if(dat_fp == NULL) {
        return PDS_FILE_ERROR;
    }
    
    FILE *ndx_fp = fopen(ndx_file, "rb+");
    if(ndx_fp == NULL) {
        fclose(dat_fp);
        return PDS_FILE_ERROR;
    }
    
    struct PDS_TableInfo *handle = helper_get_table_info(table_name);
    if(handle == NULL) {
        // This should not happen if we properly loaded tables from info file
        fclose(dat_fp);
        fclose(ndx_fp);
        return PDS_TABLE_NOT_OPEN;
    }
    
    if(handle->table_status == PDS_TABLE_OPEN) {
        fclose(dat_fp);
        fclose(ndx_fp);
        return PDS_TABLE_ALREADY_OPEN;
    }
    
    handle->table_status = PDS_TABLE_OPEN;
    handle->data_fp = dat_fp;
    handle->ndx_fp = ndx_fp;
    handle->rec_size = rec_size;
    
    // Read record count from index file
    fread(&handle->rec_count, sizeof(int), 1, ndx_fp);
    
    // Load index into BST
    pds_load_ndx(handle);
    
    fclose(ndx_fp);
    handle->ndx_fp = NULL;  // Set to NULL as we've closed it
    
    return PDS_SUCCESS;
}

// [NO CHANGE]
//////////////////////////////////////////////////////////////////////////////
// pds_load_ndx
// Load index entries into BST
//
int pds_load_ndx(struct PDS_TableInfo *tablePtr) {
    for (int i = 0; i < tablePtr->rec_count; i++) {
        struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
        fread(ndx_entry, sizeof(struct PDS_NdxInfo), 1, tablePtr->ndx_fp);
        bst_add_node(&tablePtr->ndx_root, ndx_entry->key, (void *)ndx_entry);
    }
    return PDS_SUCCESS;
}

// [NO CHANGE]
//////////////////////////////////////////////////////////////////////////////
// put_rec_by_key
// Add a record with the given key
//
int put_rec_by_key(char *table_name, int key, void *rec) {
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    struct PDS_TableInfo *handle = helper_get_table_info(table_name);
    if(handle == NULL) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    if (handle->table_status != PDS_TABLE_OPEN || strcmp(handle->table_name, table_name) != 0) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    fseek(handle->data_fp, 0, SEEK_END);
    long offset = ftell(handle->data_fp);
    
    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)malloc(sizeof(struct PDS_NdxInfo));
    ndx_entry->key = key;
    ndx_entry->offset = offset;
    ndx_entry->is_deleted = REC_NOT_DELETED;
    
    bst_add_node(&handle->ndx_root, key, (void *)ndx_entry);
    handle->rec_count++;
    
    if (fwrite(&key, sizeof(int), 1, handle->data_fp) < 0 || 
        fwrite(rec, handle->rec_size, 1, handle->data_fp) < 0) {
        return PDS_ADD_FAILED;
    }

    return PDS_SUCCESS;
}

// [NO CHANGE]
//////////////////////////////////////////////////////////////////////////////
// get_rec_by_key
// Retrieve a record by key
//
int get_rec_by_key(char *table_name, int key, void *rec) {
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    struct PDS_TableInfo *handle = helper_get_table_info(table_name);
    if(handle == NULL) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    if (handle->table_status != PDS_TABLE_OPEN || strcmp(handle->table_name, table_name) != 0) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    struct BST_Node *entry = bst_search(handle->ndx_root, key);
    if (entry == NULL) {
        return PDS_REC_NOT_FOUND;
    }
    
    struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)entry->data;
    if(ndx_entry->is_deleted == REC_DELETED) {
        return PDS_REC_NOT_FOUND;
    }
    
    fseek(handle->data_fp, ndx_entry->offset, SEEK_SET);
    
    int key_read;
    fread(&key_read, sizeof(int), 1, handle->data_fp);
    fread(rec, handle->rec_size, 1, handle->data_fp);
    
    return PDS_SUCCESS;
}

// [NO CHANGE]
//////////////////////////////////////////////////////////////////////////////
// get_rec_by_field
// Retrieve a record by field using a matcher function
//
int get_rec_by_field(char *table_name, void *searchvalue, void *rec, int (*matcher)(void *rec, void *searchvalue), int *comp_count) {
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    struct PDS_TableInfo *handle = helper_get_table_info(table_name);
    if(handle == NULL) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    if (handle->table_status != PDS_TABLE_OPEN || strcmp(handle->table_name, table_name) != 0) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    fseek(handle->data_fp, 0, SEEK_SET);
    *comp_count = 0;
    
    while (1) {
        int key;
        if (fread(&key, sizeof(int), 1, handle->data_fp) == 0) {
            break;
        }
        
        fread(rec, handle->rec_size, 1, handle->data_fp);
        (*comp_count)++;
        
        if (matcher(rec, searchvalue) == 0) {
            struct BST_Node *entry = bst_search(handle->ndx_root, key);
            struct PDS_NdxInfo *ndx_entry = (struct PDS_NdxInfo *)entry->data;
            
            if (ndx_entry->is_deleted == REC_DELETED) {
                continue;
            }
            
            return PDS_SUCCESS;
        }
    }
    
    return PDS_REC_NOT_FOUND; 
}

// [NO CHANGE]
//////////////////////////////////////////////////////////////////////////////
// delete_rec_by_key
// Mark a record as deleted
//
int delete_rec_by_key(char *table_name, int key) {
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    struct PDS_TableInfo *handle = helper_get_table_info(table_name);
    if(handle == NULL) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    if (handle->table_status != PDS_TABLE_OPEN || strcmp(handle->table_name, table_name) != 0) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    struct BST_Node *entry = bst_search(handle->ndx_root, key);
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

// BST traversal helper function
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

// [NO CHANGE]
//////////////////////////////////////////////////////////////////////////////
// pds_close_table
// Close a table and save its index
//
int pds_close_table(char *table_name) {
    if (repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    struct PDS_TableInfo *table_handle = helper_get_table_info(table_name);
    if(table_handle == NULL) {
        return PDS_FILE_ERROR;
    }
    
    if (table_handle->table_status != PDS_TABLE_OPEN || strcmp(table_handle->table_name, table_name) != 0) {
        return PDS_TABLE_NOT_OPEN;
    }
    
    char ndx_file[50];
    strcpy(ndx_file, table_name);
    strcat(ndx_file, ".ndx");
    
    FILE *fp = fopen(ndx_file, "wb");
    if (fp == NULL) {
        return PDS_FILE_ERROR;
    }
    
    fwrite(&table_handle->rec_count, sizeof(int), 1, fp);
    bst_pre_order_traversal(table_handle->ndx_root, fp);
    
    bst_destroy(table_handle->ndx_root);
    table_handle->ndx_root = NULL;
    
    fclose(fp);
    fclose(table_handle->data_fp);
    
    table_handle->data_fp = NULL;
    table_handle->ndx_fp = NULL;
    table_handle->table_status = PDS_TABLE_CLOSED;
    
    return PDS_SUCCESS;
}

// [MODIFIED]
//////////////////////////////////////////////////////////////////////////////
// pds_close_db
// Close all open tables and update the info file with current table information
//
int pds_close_db(char *dbname) {
    if(repo_handle.repo_status != PDS_DB_OPEN) {
        return PDS_DB_NOT_OPEN;
    }
    
    // Close all open tables
    for(int i = 0; i < repo_handle.num_tables; i++) {
        if(repo_handle.tables[i].table_status == PDS_TABLE_OPEN) {
            pds_close_table(repo_handle.tables[i].table_name);
        }
    }
    
    // Update info file with current table information
    char info_filename[100];
    sprintf(info_filename, "%s.info", dbname);
    
    FILE *fp = fopen(info_filename, "wb");
    if(fp == NULL) {
        return PDS_FILE_ERROR;
    }
    
    // Write database name
    fwrite(dbname, sizeof(repo_handle.pds_repo_name), 1, fp);
    
    // Write number of tables
    fwrite(&repo_handle.num_tables, sizeof(int), 1, fp);
    
    // Write table information
    for(int i = 0; i < repo_handle.num_tables; i++) {
        fwrite(repo_handle.tables[i].table_name, sizeof(repo_handle.tables[i].table_name), 1, fp);
        fwrite(&repo_handle.tables[i].rec_size, sizeof(int), 1, fp);
    }
    
    fclose(fp);
    
    // Mark database as closed
    repo_handle.repo_status = PDS_DB_CLOSED;
    
    // Change to parent directory
    chdir("..");
    
    return PDS_SUCCESS;
}