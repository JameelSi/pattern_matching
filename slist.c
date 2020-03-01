#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "slist.h"

void slist_init(slist_t * list){

    slist_head(list)=NULL;
    slist_tail(list)=NULL;
    slist_size(list)=0;
    
}

void slist_destroy(slist_t * list,slist_destroy_t flag){
    //making sure that the list is not empty
    if(list!=NULL){
    if(slist_size(list)>0){
        slist_node_t *temp=NULL; 
        while(slist_head(list)!=NULL){
            temp=slist_head(list);
            slist_head(list)=slist_next(slist_head(list));
            //checking wether the data allocated on heap or not 
            if(flag==SLIST_FREE_DATA)
                free(slist_data(temp));
            free(temp);
        }
    }
            free(list); 
    }
}

void *slist_pop_first(slist_t *list){

    if(slist_size(list)==0)
        return NULL;

    slist_node_t *temp;
    temp=slist_head(list); 
    slist_head(list)=slist_next(slist_head(list));
    slist_next(temp)=NULL;
    slist_size(list)--;
    void*data=slist_data(temp);
    free(temp);

    return data;

}


int slist_append(slist_t * list,void * data){

    slist_node_t *temp=(slist_node_t*)malloc(sizeof(slist_node_t));
    if(temp==NULL)
        return -1;
    slist_data(temp)=data;
    slist_next(temp)=NULL;

    //if the list is empty 
    if(slist_size(list)==0){
    slist_head(list)=temp;
    slist_tail(list)=temp;  
  
    }

    else{
    slist_next(slist_tail(list))=temp;
    slist_tail(list)=temp;
    }

     slist_size(list)++;

    return 0;
}

int slist_prepend(slist_t * list,void * data){

    slist_node_t *temp=(slist_node_t*)malloc(sizeof(slist_node_t));
    if(temp==NULL)
        return -1;
    slist_data(temp)=data;
    slist_next(temp)=NULL;
    if(slist_size(list)==0){

        slist_head(list)=temp;
        slist_tail(list)=temp;  
  
    }
    else{
        slist_next(temp)=slist_head(list);
        slist_head(list)=temp;
    }
        slist_size(list)++;

        return 0;

}

int slist_append_list(slist_t* to, slist_t* from){
        //if the second list is empty then theres nothing to be copied
        if(from==NULL)
            return 0;
        //if the first list is empty then slist_append cant be used since its pointer to NULL so return an error
        if(to==NULL)
            return -1;
        slist_node_t *temp=slist_head(from);
        while(temp!=NULL){
            //making sure malloc successeded
            if(slist_append(to,slist_data(temp))==-1)
                return -1;
            temp=slist_next(temp);
        }
          return 0;
}