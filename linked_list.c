#include "linked_list.h"


void list_insert_node(struct list_node **head, void*data){
   if(head == NULL){
      //fprintf(stderr,"Error: list_insert_node git a Null pointer for head\n");
      head = (struct list_node**) malloc(sizeof(struct list_node));
      *head = NULL;
   }
   struct list_node *temp = (struct list_node*) malloc(sizeof(struct list_node));
   if(*head == NULL){
      temp->next = NULL;
   }else{
      temp->next = *head;
   }
      
   
   temp->data = data;
   *head = temp;
}

void list_remove_node(struct list_node **head){
   if(head == NULL)
      return;
   struct list_node * tmp = ((struct list_node *)*head)->next;
   free(*head);
   *head = tmp;
         
}
struct list_node *list_get_last_node(struct list_node *head){
   while(NULL != head->next) {
      head = head->next;
   }
   return head;
}
