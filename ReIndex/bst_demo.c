#include<stdio.h>
#include "bst.h"

struct DummyData{
	int dummy1;
	char dummy2[10];
};

int main()
{
	int testarr[8] = {30,25,35,21,26,34,38,25};
	int searcharr[8] = {5353,30,12,25,35,21,38,25};
	int tcount = 8;
	int i, status;
	struct BST_Node *root = NULL;
	struct DummyData dummyData;

	// Assigning a dummy pointer to rec for demonstration purpose
	void *rec = &dummyData;

	for( i=0; i < tcount; i++ ){
		int key = testarr[i];
		printf("Adding key %d\n", key);
		// Note the ampersand & before the root... it is a double pointer
		status = bst_add_node( &root, key, rec );
		if( status != BST_SUCCESS ){
			printf( "Unable to add node key %d... status = %d\n", key, status );
		}
	}
	bst_print( root );
	printf("\n");
	for( i=0; i < tcount; i++ ){
		int key = searcharr[i];
		struct BST_Node *result = bst_search( root, key );
		if( result != NULL )
			printf("Key %d found\n", key);
		else
			printf("Key %d NOT found\n", key);
	}
	bst_free(root);
	return 0;
}


