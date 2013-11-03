#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*
int insert_into_store(struct op_code* op_instance){
     int index = hash_key_to_index(key);
// insert entry use glib api

}
int delete_from_store(struct op_code* op_instance){
      int index = hash_key_to_index(key);
// delete entry using glib api

}
int update_store_for_key(struct op_code){
      int index = hash_key_to_index(key);
// use api to fetch entry and update value using strcat

}
int lookup_store_for_key(struct op_code){
       int index = hash_key_to_index(key);
// use api to lookup entry and return value

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
struct op_code{
	     int opcode;
   	     int key;
 	     char *value;
};

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

   free(temp);
}          	
		   
 
        
