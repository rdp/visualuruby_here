/*
###################################
#
# larray.c
# Programmed by nyasu <nyasu@osk.3web.ne.jp>
# Copyright 1999-2005  Nishikawa,Yasuhiro
#
# More information at http://vruby.sourceforge.net/index.html
#
###################################
********************************************************
*   linear list
*    : a set of arrays of id and data
*    : non-available 0 or NULL for id and data.
*
*   LArray LArray_new();
*   BOOL LArray_set(LArray,id,data);
*   DATA LArray_get(LArray,id)
*   BOOL LArray_delete(LArray,id,freeflag);
*   void LArray_destruct(LArray);
*/

#include <stdlib.h>
#include "larray.h"

LArrayItem* LArrayItem_new(){
	LArrayItem* p;
	p = malloc(sizeof(LArrayItem));
	if(p){
		/* sentinel if these are not modified */
		p->id=NULL_ID; p->data=0; p->next=NULL;  
	}
	return p;
}

void LArrayItem_delete(LArrayItem* item){
	if(item==NULL) return;
	if(item->id!=NULL_ID || item->next != NULL){ /* while not for sentinel */
		LArrayItem_delete(item->next);
	}
	free(item);
}

LArray* LArray_new(){
	LArray* a=NULL;

	a = malloc(sizeof(LArray));
	if(a){
		a->items = LArrayItem_new();
		a->cached_id = NULL_ID;
	}
	if(a->items==NULL){
		free(a);
		return NULL;
	}
	return a;
}

BOOL LArray_set(LArray* thiss, int id, LArrayDATA data){
	LArrayItem* pt;
	if(thiss==NULL || id==NULL_ID) return 0;  /* wrong id */

	LArray_EACH_START(thiss,pt)
		if(pt->id == id){   /* already set */
			pt->data=data;
			return TRUE;
		}
		if(pt->next==NULL){ /* something wrong */
			return FALSE;
		}
	LArray_EACH_END

	pt->next=LArrayItem_new();
	pt->id=id;
	pt->data=data;

	return TRUE;
}

BOOL LArray_delete(LArray* thiss, int id, BOOL freeflag){
	LArrayItem *pt,*del;
	
	if(thiss==NULL || id==NULL_ID) return 0;  /* wrong id */

	LArray_EACH_START(thiss,pt)
		if(pt->next==NULL) return FALSE; /* something wrong */
		if(pt->id==id){ /* hit */
			/* copy next to pt and free next */
			del=pt->next;
			pt->id   = del->id;
			pt->data = del->data;
			pt->next = del->next;
			if(thiss->cached_id==id){ thiss->cached_id=NULL_ID; }
			if(freeflag) free((void*)del->data);
			free(del);
			return TRUE;
		}
	LArray_EACH_END

	return FALSE;  /* not found */
}

LArrayDATA LArray_get(LArray* thiss, int id){
	LArrayItem* pt;
	
	if(thiss==NULL || id==NULL_ID) return 0;  /* wrong id */
	if(id==thiss->cached_id){  /* cached id */
		return thiss->cached_item->data; 
	}

	LArray_EACH_START(thiss,pt)
		if(pt->id==id){ /* hit */
			thiss->cached_id = id;
			thiss->cached_item = pt;
			return pt->data;
		}
	LArray_EACH_END

	return 0;  /* not found */
}

void LArray_destruct(LArray* thiss){
	if(thiss==NULL) return;
	LArrayItem_delete(thiss->items);
	free(thiss);
}
