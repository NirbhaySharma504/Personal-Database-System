#ifndef CONTACT_H
#define CONTACT_H

struct Contact{
	int contact_id;
	char contact_name[50];
	char phone[50];
};

struct Student{
	int student_id;
	char student_name[50];
	char degree_name[50];
};

////////////////////////////////////////////////////////////////////////////
/* Return 0 if cname matches with contact_name parameter */
/* Return 1 if cname DOES NOT matche with contact_name parameter*/
/* Return > 1 in case of any other error */
int contact_name_matcher( void *rec, void *search_name );

#endif
