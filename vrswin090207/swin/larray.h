/********************************************************
*
*
*   linear list
*    : a set of arrays of id and data
*    : non-available 0 or NULL for data
*   struct LArrayItem { int id; DATA data; LArrayItem* next; };
*   typedef LArrayItem* LArray
*
*   LArray LArray_new();
*   BOOL LArray_set(LArray,id,data);
*   DATA LArray_get(LArray,id)
*   BOOL LArray_delete(LArray,id,freeflag);  if freeflag is true, free(data)
*   void LArray_destruct(LArray);
*/

#define NULL_ID 0

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif


#define LArrayDATA long

typedef struct _LArrayItem {
	int id; LArrayDATA data; struct _LArrayItem *next; 
} LArrayItem;

typedef struct _LArray {
	LArrayItem* items;

	/* cached_item is valid while cached_id is not NULL_ID */
	int cached_id; LArrayItem* cached_item;  
} LArray;

LArray* LArray_new();
BOOL LArray_set(LArray* thiss, int id, LArrayDATA data);
BOOL LArray_delete(LArray* thiss, int id, BOOL);
LArrayDATA LArray_get(LArray* thiss, int id);
void LArray_destruct(LArray* thiss);

#define LArray_EACH_START(list,pt) \
	pt=list->items;\
	while(pt->id!=NULL_ID){

#define LArray_EACH_END \
		pt = pt->next;\
	}

