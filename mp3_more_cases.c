#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<glib.h>


struct op_code{
             int opcode;
             int key;
             char *value;
};

GHashTable* key_value_store;

int create_hash_table(){
   key_value_store =  g_hash_table_new(g_str_hash,g_str_equal);
   if(key_value_store == NULL) return -1;
   else return 0;
}

// send an opcode instance which is dynamically allocated
//
int insert_key_value_into_store(struct op_code* op_instance){
     char *buffer;
     buffer = (char*)malloc(10);
     sprintf(buffer,"%d",op_instance->key);

     gpointer key = (gpointer)buffer;
     gpointer value = (gpointer)op_instance->value;

     g_hash_table_insert(key_value_store,key,value);
}

char* lookup_store_for_key(int key){
    
     gpointer value;
     char *buffer = (char *)malloc(10);
     sprintf(buffer,"%d",key);
     gpointer key_temp = (gpointer)buffer;
     value = g_hash_table_lookup(key_value_store,key_temp);
     free(buffer);
     return (char *)value;
}

int delete_key_value_from_store(int key){
     int status;
     char *buffer = (char *)malloc(10);
     sprintf(buffer,"%d",key);
     status = g_hash_table_remove(key_value_store,buffer);
     if(status) 
          return 0; //success
     else
          return -1; //failure
}

/*
int hash_key_to_node(int key){
      // write logic for this  [it has to iterate through all the table entries and find the nearest match] 
      
}
*/





//upto the caller to free the buffers in create_message_X cases
int create_message_INSERT(int key, char *value, char **message){
	           int len = strlen(value);
	           char *buffer = (char *)malloc(len + 20);
		   sprintf(buffer,"INSERT:%d:%s;",key,value);
		   *message = buffer;
		   return 0;
}
int create_message_DELETE(int key, char **message){
	           char *buffer = (char *)malloc(20);
	           sprintf(buffer,"DELETE:%d;",key);
		   *message = buffer;
		   return 0;
}
int create_message_UPDATE(int key, char *value, char **message){
	           int len = strlen(value);
	           char *buffer = (char *)malloc(len+20);
		   sprintf(buffer,"UPDATE:%d:%s;",key,value);
		   *message = buffer;
		   return 0;
}
int create_message_LOOKUP(int key, char **message){
	           char *buffer = (char *)malloc(20);
		   sprintf(buffer,"LOOKUP:%d;",key);
		   *message = buffer;
		   return 0;
}
int create_message_LOOKUP_RESULT(int key, char *value, char **message){
	           int len = strlen(value);
		   char *buffer = (char *)malloc(len + 100);
		   sprintf(buffer,"LOOKUP_RESULT:%d:%s;",key,value);
		   *message = buffer;
		   return 0;
}
/*
struct op_code{
	     int opcode;
   	     int key;
 	     char *value;
};
*/
// INSERT, DELETE, UPDATE, GET messages are possible
int extract_message_op(char *message, struct op_code** instance){
	           char *original = (char *)malloc(strlen(message));
                   strcpy(original,message);

		   *instance = (struct op_code *)malloc(sizeof(struct op_code));  // up-to the caller to free this

		   char *delim=":";	
		   char *token=strtok(original,delim); 	
                    
		   if (strcmp(token,"INSERT")==0){   //INSERT:KEY:VALUE;
		            (*instance)->opcode = 1; // 1 is the op-code for insert
					
			    token=strtok(NULL,delim);  //GET KEY
                    	    (*instance)->key = atoi(token);  
 
                            token=strtok(NULL,delim);    //GET VALUE
			    int len = strlen(token);     
			    char *value_instance = (char *)malloc(len);
			    strcpy(value_instance,token);
                    	    (*instance)->value = value_instance;
                       
                    	    return 1;
                   }
             					
		   if(strcmp(token,"DELETE")==0){  //DELETE:KEY;
		            (*instance)->opcode = 2; // 2 is the op-code for delete
			    token = strtok(NULL,delim);  // GET KEY
			    (*instance)->key = atoi(token);
			    (*instance)->value = NULL;
                            
			    return 1;
	           }
		   if(strcmp(token,"UPDATE")==0){ //UPDATE:KEY:VALUE;
		            (*instance)->opcode = 3; // 3 is the op-code for update
                     	    token = strtok(NULL,delim); // GET KEY
                            (*instance)->key = atoi(token);
            
                            token = strtok(NULL,delim);  // get the value to update
                            int len = strlen(token);
                            char *value_instance = (char *)malloc(len);					 
			    strcpy(value_instance, token);
			    (*instance)->value = value_instance;
                            
			    return 1;
		   }
		   if(strcmp(token,"LOOKUP")==0){  //LOOKUP:KEY;
		            (*instance)->opcode = 4; // 4 is the opcode for lookup
			    token = strtok(NULL,delim); //get key
			    (*instance)->key = atoi(token);
                            (*instance)->value = NULL;
                            
			    return 1;
		   }
		   if(strcmp(token,"LOOKUP_RESULT")==0){
			    (*instance)->opcode = 5; // 5 is the opcode for lookup result
		            token = strtok(NULL,delim); //get key
		            (*instance)->key = atoi(token);
					 
			    token = strtok(NULL,delim); // get value
			    int len = strlen(token);
			    char *value_instance = (char *)malloc(len);
			    strcpy(value_instance, token);
			    (*instance)->value = value_instance;
                            
                            return 1;
		   }
                
}
			

// replace host_id to hash_id of the host

void main(){

  // create_message_XXXX examples

   char *msg=0x0;
   char *value = "hello world";
   int key = 100;


   create_message_INSERT(100,value,&msg);
   printf("%s\n",msg);
   free(msg);
   msg=0x0; 
   
   create_message_DELETE(1234,&msg);
   printf("%s\n",msg);
   free(msg);
   msg=0x0;

   create_message_UPDATE(1234,value,&msg);
   printf("%s\n",msg);
   free(msg);
   msg=0x0;

   create_message_LOOKUP(1234,&msg);
   printf("%s\n",msg);
   free(msg);
   msg=0x0;


   create_message_LOOKUP_RESULT(1234,value,&msg);
   printf("%s\n",msg);
   free(msg);
   msg=0x0; 
  // extract_message examples
   
   printf("-----------------------\n");
   create_message_INSERT(100,value,&msg);
   struct op_code* temp;
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   printf("-------------------------\n");
   create_message_DELETE(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   printf("-------------------------\n");
   create_message_UPDATE(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   printf("--------------------------\n");
   create_message_LOOKUP(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;
   printf("--------------------------\n");

   create_message_LOOKUP(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_LOOKUP(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_LOOKUP(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   
   create_message_DELETE(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_DELETE(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_DELETE(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_DELETE(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_DELETE(1234,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   
   create_message_LOOKUP_RESULT(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
 //  free(msg);
   free(temp);
   printf("\n---------------------------\n");

   create_message_LOOKUP_RESULT(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("%s\n",msg);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
 //  free(msg);
   free(temp);
  
   
   char *msg1;
   create_message_LOOKUP_RESULT(1234,value,&msg1);
   extract_message_op(msg1,&temp);
   printf("%s\n",msg1);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
//   free(msg1);
   free(temp);
 
   char *msg2;
   create_message_LOOKUP_RESULT(1234,value,&msg2);
   extract_message_op(msg2,&temp);
   printf("%s\n",msg2);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
 //  free(msg2);

   free(temp);
   
   
   char *msg3;
   create_message_DELETE(1234,&msg3);
   printf("%s",msg3);
   extract_message_op(msg3,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
 //  free(msg3);
   free(temp);
   
   create_message_INSERT(100,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_INSERT(100,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_INSERT(100,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;
  
   create_message_INSERT(100,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;


   create_message_UPDATE(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_UPDATE(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_UPDATE(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_UPDATE(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_UPDATE(1234,value,&msg);
   extract_message_op(msg,&temp);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
   free(msg);
   free(temp);
   msg=0x0;

   create_message_LOOKUP_RESULT(1234,value,&msg2);
   char *buffer=(char *)malloc(100);
   strcpy(buffer,msg2);
   printf("printing from buffer.........%s\n",buffer);
   free(buffer);
   extract_message_op(msg2,&temp);
   printf("%s\n",msg2);
   printf("key : %d\n",temp->key);
   printf("value : %s\n",temp->value);
   printf("opcode:%d\n",temp->opcode);
 //  free(msg2);

   

   printf("===================================================\n");
   printf("====================hash table creation============\n");
   
   create_hash_table();
   insert_key_value_into_store(temp);
   char *value123 = lookup_store_for_key(1234);
   printf("%s",value123);

}          	
		   
 
        
