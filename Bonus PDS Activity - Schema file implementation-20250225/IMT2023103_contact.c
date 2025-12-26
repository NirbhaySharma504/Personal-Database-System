//IMT2023103
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "contact.h"

void print_contact( struct Contact *c )
{
	printf("%d,%s,%s\n", c->contact_id,c->contact_name,c->phone);
}

/* Return 0 if phone of the contact matches with phone parameter */
/* Return 1 if phone of the contact does NOT match */
/* Return > 1 in case of any other error */
int contact_name_matcher( void *rec, void *search_name )
{
    struct Contact * rec_ptr = (struct Contact*)rec;
    char * search_ptr = (char *)search_name;
    if(rec_ptr==NULL || search_ptr==NULL){
        return 2;
    }
    if(strcmp(rec_ptr->contact_name,search_ptr)==0){
        return 0;
    }
    return 1;
    
	// Type cast rec to a struct contact pointer
    // Type cast search_name to a char pointer
    // Compare the contact_name in record and search_name using strcmp
    // Return the output of strcmp
}

