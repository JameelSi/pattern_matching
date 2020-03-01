#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"pattern_matching.h"
#include<string.h>


int pm_init(pm_t * fsm){

    fsm->zerostate=(pm_state_t*)malloc(sizeof(pm_state_t));
        if(fsm->zerostate==NULL)
            return -1;
    fsm->zerostate->depth=0;
    fsm->zerostate->id=0;
    fsm->zerostate->output=NULL;
    //failure of zero state is zero state
    fsm->zerostate->fail=fsm->zerostate;
    fsm->zerostate->_transitions=(slist_t*)malloc(sizeof(slist_t));
        if(fsm->zerostate->_transitions==NULL)
            return -1;
    slist_init(fsm->zerostate->_transitions);     
    fsm->newstate=1;

    return 0;

}

int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){

    pm_labeled_edge_t *l=(pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));
        if(l==NULL)
            return -1;
    l->label=symbol;
    l->state=to_state;
    if(slist_append(from_state->_transitions,l)==-1)
        return -1;
    printf("%d -> %c -> %d\n",from_state->id,symbol,to_state->id);

    return 0;
}

pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol){

    if(state==NULL)
        return NULL;
    slist_node_t *temp=slist_head(state->_transitions);
    while(temp!=NULL){
        if(((pm_labeled_edge_t *)slist_data(temp))->label==symbol)
            return (((pm_labeled_edge_t *)slist_data(temp))->state);
        temp=slist_next(temp);
    }
    //no match found
    return NULL;
}


int  pm_addstring(pm_t * fsm ,unsigned char * str, size_t n){
    //if no characters were sent
    if(n<1)
        return -1;
    //making sure not to exceed the maximum number of states
    if((slist_size((fsm->zerostate->_transitions))+n)>PM_CHARACTERS)
        return -1;
    // counter for the string 
    int count=0;
    // temporary pointer to the current stater im at 
    pm_state_t * currState=fsm->zerostate;
     while(count<n){
        char ch=str[count];
        if(pm_goto_get(currState,ch)==NULL){
            //creating a new node and connecting it to the root
            pm_state_t * st=(pm_state_t*)malloc(sizeof(pm_state_t));
                if(st==NULL)
                    return -1;
            st->depth=currState->depth+1;
            st->id=fsm->newstate;
            st->_transitions=(slist_t*)malloc(sizeof(slist_t));
                if(st==NULL)
                    return -1;
            slist_init(st->_transitions);
            st->fail=NULL;
            //if we reached the last letter of the word then set it to be (recieving mode)
            if(count==n-1){
                st->output=(slist_t*)malloc(sizeof(slist_t));
                     if(st->output==NULL)
                        return -1;
                slist_init(st->output);
                if(slist_append(st->output,str)==-1)
                    return -1;
            }
            else
                st->output=NULL;    
            printf("Allocating state %d\n",fsm->newstate);
            if(pm_goto_set(currState,ch,st)==-1)
                return -1;
            /*a new node should be added to the fsm*/
            currState=st;

            fsm->newstate++;
        }

        // else update the current state and skip to the next char
            else{

                currState=pm_goto_get(fsm->zerostate,ch);
                //if the end of the word reached then set the current state to be (recieving mode)
                if(count==n-1){
                        if(currState->output==NULL){
                        currState->output=(slist_t*)malloc(sizeof(slist_t));
                        if(currState->output==NULL)
                            return -1;
                        slist_init(currState->output);
                        if(slist_append(currState->output,str)==-1)
                            return -1;
                        }
                    
                }
                
            }

            count++;
    }

    return 0;
}



int pm_makeFSM(pm_t *fsm){
    // implementing the queue 
    slist_t * queue=(slist_t*)malloc(sizeof(slist_t));
        if(queue==NULL)
            return -1;
    //initilaize the Q
    slist_init(queue);
    //pointer to zero states transition head 
    slist_node_t *temp=slist_head(fsm->zerostate->_transitions);
    // helping pointer
    pm_state_t* currentState;
    unsigned char currentLabel;
    for(int i=0;i<slist_size(fsm->zerostate->_transitions);i++){
        currentState=((pm_labeled_edge_t*)slist_data(temp))->state;
        currentLabel=((pm_labeled_edge_t*)slist_data(temp))->label;
        //setting all the nodes with depth 1 failure to zerostate 
        currentState->fail=fsm->zerostate;
        // adding the nodes to the queue to set thier  failure transitions
        if(slist_append(queue,currentState)==-1)
            return -1;
        printf("Settingf(%d)=%d\n",currentState->id,fsm->zerostate->id);
        temp=slist_next(temp);  
    }
    // setting the failure of the rest of the nodes 
    while(slist_size(queue)>0){
           
        pm_state_t* pops=(pm_state_t*)slist_pop_first(queue);
        // if it has no transitions (no children) skip to the next node 
        if(slist_size(pops->_transitions)>0){
            //counter for the state transition
            int count=0;
            //pointer to the first child 
            temp=slist_head(pops->_transitions);
            while(count<slist_size(pops->_transitions)){

                currentState=((pm_labeled_edge_t*)slist_data(temp))->state;
                currentLabel=((pm_labeled_edge_t*)slist_data(temp))->label;
                pm_state_t *get =pm_goto_get(pops->fail,currentLabel);

                //if theres a lable from parent node failure to a node the set it to be the current node failure
                if(get!=NULL){
                   currentState->fail=get;
                    printf("Settingf(%d)=%d\n",currentState->id,get->id);
                    // checking if the failure has an output for it to be added to the current node output
                    if(get->output!=NULL){
                        //if it a reciveng state do nothing 
                        if(currentState->output==NULL){
                            //initilaize the output 
                            currentState->output=(slist_t*)malloc(sizeof(slist_t));
                            if(currentState->output==NULL)
                                return -1;
                            slist_init(currentState->output);
              
                        }
                    }
                    //append the outputs
                    if(slist_append_list(currentState->output,get->output)==-1)
                        return -1;
                }
                else{
                    //parent failure was checked above so the idea is to keep going backwards untill either zero state is reached or a match found 
                    pm_state_t *step=pops->fail->fail;
                    while(step->id!=0){
                        get=pm_goto_get(step,currentLabel);
                        if(get!=NULL){
                            currentState->fail=step;
                            printf("Settingf(%d)=%d\n",currentState->id,get->id);
                            // checking if the failure has an output for it to be added to the current node output
                            if(get->output!=NULL){
                                //if it a reciveng state do nothing                      
                                if(currentState->output==NULL){
                                    //initilaize the output 
                                    currentState->output=(slist_t*)malloc(sizeof(slist_t));
                                    if(currentState->output==NULL)
                                        return -1;
                                    slist_init(currentState->output);
                                    }
                            }
                            //append the outputs
                            if(slist_append_list(currentState->output,get->output)==-1)
                                return -1;
                        }
                     step=step->fail;
                    }

                    // if the last while loop didnt find a match then check if zero state was reached then check if theres a match else the failure is zero state
                    if(currentState->fail==NULL){
                        get=pm_goto_get(step,currentLabel);
                        if(get!=NULL){
                           currentState->fail=get;
                            printf("Settingf(%d)=%d\n",currentState->id,get->id);
                            // checking if the failure has an output for it to be added to the current node output
                            if(get->output!=NULL){
                                //if it a reciveng state do nothing                                                      
                                if(currentState->output==NULL){
                                     //initilaize the output 
                                    currentState->output=(slist_t*)malloc(sizeof(slist_t));
                                    if(currentState->output==NULL)
                                        return -1;
                                    slist_init(currentState->output);
                                    }
                            }
                            //append the outputs
                            if(slist_append_list(currentState->output,get->output)==-1)
                                return -1;
                        }
                        else{
                            //no output should be set since its the zero state
                            currentState->fail=step;
                            printf("Settingf(%d)=%d\n",currentState->id,step->id);

                        }
                    }

                }
                //add the current node to the Q
                slist_append(queue,currentState);
                count++;
                temp=slist_next(temp);
            }
        }

           
    }

    free(queue);

    return 0;
}



slist_t* pm_fsm_search(pm_state_t * state,unsigned char * str,size_t n){
    if(n<1)
        return NULL;
    //this list is what the function will return 
    slist_t *matched_list=(slist_t*)malloc(sizeof(slist_t));
    if(matched_list==NULL)
        return NULL;
    slist_init(matched_list);
    // the head of the tree to reset the current state to be the head when no streak found in the loop
    pm_state_t*head=state;
    for(int i=0;i<n;i++){
        //keep looking until a match is found or zerostate is reached
        while(pm_goto_get(state,str[i])==NULL){
             if(state==NULL||state->id==0)
                 break;
            state=state->fail;
        }
        state=pm_goto_get(state,str[i]);
        //if a match was found 
        if(state!=NULL){
            // if its a reciving mode 
            if(state->output!=NULL){
                slist_node_t * outputPointer=slist_head(state->output);
                while(outputPointer!=NULL){
                    pm_match_t* mattched=(pm_match_t*)malloc(sizeof(pm_match_t));
                    if(mattched==NULL)
                        return NULL;
                    mattched->pattern=(char*)outputPointer->data;
                    mattched->start_pos=i-strlen(mattched->pattern)+1;
                    mattched->end_pos=i;
                    mattched->fstate=state;
                    slist_append(matched_list,mattched);
                    printf("Pattern: %s, start at: %d, ends at: %d, last state = %d\n",mattched->pattern, mattched->start_pos, mattched->end_pos, mattched->fstate->id);
				    outputPointer = slist_next(outputPointer);
                }

            }
        }
        //reset the state 
        else
              state=head;
        
        
    }

    return matched_list;
}

void pm_destroy(pm_t * fsm){
    //using Q again to destroy the Tree
    slist_t * queue=(slist_t*)malloc(sizeof(slist_t));
        if(queue==NULL)
            printf("WARNING:QUEUE IS NOT ALLOCATED IN pm_destroy");
    //initilaize the Q
    slist_init(queue);
    if(slist_append(queue,fsm->zerostate)==-1)
        printf("WARNING: couldn't enqueue in pm_destroy");

    while(slist_size(queue)>0){

        pm_state_t* pops=(pm_state_t*)slist_pop_first(queue);
        if(slist_size(pops->_transitions)>0){
            slist_node_t *temp=slist_head(pops->_transitions);
            while(temp!=NULL){
                if(slist_append(queue,((pm_labeled_edge_t*)temp->data)->state)==-1)
                    printf("WARNING:Couldn't enqueue in pm_destroy");
                temp=slist_next(temp);
                }

            }
        slist_destroy(pops->_transitions,SLIST_FREE_DATA);
        if(pops->output!=NULL)
           slist_destroy(pops->output,SLIST_LEAVE_DATA);
        free(pops);
        }
    free(queue);
}