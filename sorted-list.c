#include "mapred.h"

SortedListPtr SLCreate(CompareFuncT cf){
	SortedListPtr start=(SortedListPtr)malloc(sizeof(struct SortedList));
	/*if malloc somehow fails, we return NULL*/
	if(start==NULL){
		return NULL;

	}
	start->head=NULL;
	start->comp=cf;
	return start;
}
SortedListIteratorPtr SLCreateIterator(SortedListPtr list){
	if(list->head==NULL){
		return NULL;
	}
	SortedListIteratorPtr iter=(SortedListIteratorPtr)malloc(sizeof(SortedListIteratorPtr));
	/*if malloc somehow fails, we return NULL*/
	if(iter==NULL){
		return NULL;
	}
	iter->curr=list->head;
	iter->curr->count++;
	return iter;
}
int SLInsert(SortedListPtr list, void *newObj){
	if(list==NULL){
		return 0;

	}
	node temp=(node)malloc(sizeof(struct node_list));
	/*if malloc somehow fails, we return 0*/
	if(temp==NULL){
		return 0;
	}
	node prev;
	node curr;
	temp->obj=newObj;
	temp->next=NULL;
	temp->count=1;
	curr=list->head;
	int comparison;
	if((list->head==NULL)){
		list->head=temp;
		return 1;
	}
	/* -1 if first is smaller
	*  0 if equal
	*  1 if second is smaller aka first is bigger
	*/

	/*if it's bigger than head*/
	if((comparison = list->comp(newObj, curr->obj)) == 0)/*if the objects are equal do nothing (for indexer pa3, allows compare function to handle this case)*/
    {
        free(temp->obj);
        free(temp);
        return 1;
    }
	if((comparison > 0)){
		temp->next=list->head;
		list->head=temp;
		return 1;
	}
	/*used this for only one node in list AND it's smaller than current.
	*for traversing the list easier in while loop*/
	if((curr->next == NULL) && (comparison < 0)){
		curr->next=temp;
		return 1;
	}
	prev=curr;
	curr=curr->next;
	while(curr!=NULL){
		if((comparison = list->comp(newObj, curr->obj)) >= 0){ /*if it equals curr or newobj is greater, we insert, and return*/
			temp->next=prev->next;
			prev->next=temp;
			return 1;
		}
		prev=curr;							/*else we simply move along the list*/
		curr=curr->next;
	}
	/*I can assume it will be inserted in the end if curr is null*/
	prev->next=temp;
	return 1;
}
int SLRemove(SortedListPtr list, void *newObj){
	if(list==NULL){
		return 0;

	}
	if(list->head==NULL){
		return 0;
	}
	node prev;
	node curr;
	curr=list->head;
	/*if it's the head*/
	if(list->comp(curr->obj, newObj)==0){
		list->head=curr->next;
		curr->count--;
		/*free(curr->obj);*/
		if(curr->count>=1){
			if(curr->next!=NULL){
					curr->next->count++;}
			curr->deLink=1;
				return 1;
			}
			else{
				free(curr);}
		return 1;
	}
	/*everything else*/
	prev=curr;
	curr=curr->next;
	while(curr!=NULL){
		if(list->comp(curr->obj, newObj)==0){
			prev->next=curr->next;
			curr->count--;
			/*free(curr->obj);*/
			if(curr->count>=1){
				if(curr->next!=NULL){
					curr->next->count++;
				curr->deLink=1;}
				return 1;
			}
			else{
				free(curr);}
			return 1;
		}
		prev=curr;
		curr=curr->next;
	}
	return 0;
}
void SLDestroy(SortedListPtr list){
	node start=list->head;
	node next=NULL;
	for(;start!=NULL;start=next){
		next=start->next;
		free(start);
	}
	free(list);
	list->head=NULL;
}
void SLDestroyIterator(SortedListIteratorPtr iter){
	/*if we destroy the iterator in the middle,
	before we reach the end, we should decrement the count!*/
	if(iter->curr!=NULL){
		iter->curr->count--;
		/*was iterator pointing to a node that was removed?
								  *if so, we need to free it*/
		if(iter->curr->count==0){
			free(iter->curr);
		}
	}
	free(iter);
}
void *SLNextItem(SortedListIteratorPtr iter){
	if((iter==NULL)||(iter->curr==NULL)){  /*if we reached the end of list or iter was never created*/
		return NULL;
	}
	node temp;
	while(iter->curr->count>1 && iter->curr->deLink==1){
		iter->curr->count--;
		if(iter->curr->next==NULL){
			return NULL;/*we reached the end of the list*/
		}
		iter->curr=iter->curr->next;
		iter->curr->count++;
	}
	/*if iterator is the only one pointing at it*/
	while(iter->curr->count==1){
		temp=iter->curr;
		if(iter->curr->next==NULL){
			free(temp);
			return NULL;/*we reached the end of the list*/
		}
		iter->curr=iter->curr->next;
		free(temp);
	}
	void *obj=iter->curr->obj;
	iter->curr->count--;
	iter->curr=iter->curr->next;
	if(iter->curr!=NULL){
		iter->curr->count++;}
	return obj;
}
void display(SortedListPtr start){
	node i=start->head;
	KeyVal curr;
	int y;
	y=0;
	for(;i!=NULL; i=i->next){
        curr = (KeyVal)i->obj;
        printf("\n%s: %d",curr->key, curr->value);
		y++;
	}

}
