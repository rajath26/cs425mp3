#include<stdio.h>
#include<string.h>

#define SMALL_BUF_SZ              100
#define MED_BUF_SZ                1024
#define LONG_BUF_SZ               4096
#define SUCCESS 0
#define ERROR -1

int main(int argc, char *argv[])
{

   int rc = ERROR;
   char opCode[SMALL_BUF_SZ];
char key[SMALL_BUF_SZ];
char value[LONG_BUF_SZ]; 

    char *token;

    int opCodeSet = 0,
        keySet = 0,
        valueSet = 0,
                insert = 0,
        lookup = 0,
        update = 0,
        delete = 0,
        invalidOpCode = 0;


    // OP CODE
    token = strtok(argv[1], ":::");
    if ( token != NULL )
    {
        strcpy(opCode, token);
        opCodeSet = 1;
        if ( (strcasecmp(opCode, "INSERT") == 0) )
            insert = 1;
        else if ( SUCCESS == strcasecmp(opCode, "LOOKUP") )
            lookup = 1;
        else if ( SUCCESS == strcasecmp(opCode, "UPDATE") )
            update = 1;
        else if ( SUCCESS == strcasecmp(opCode, "DELETE") )
            delete = 1;
        else
        {
            invalidOpCode = 1;
            rc = ERROR;
            goto rtn;
        }
        printf("\nPASSED OP CODE is %s\n", opCode);
    }

    // KEY
    token = strtok(NULL, ":::");
    if ( token != NULL )
    {
         strcpy(key, token);
         keySet = 1;
         printf("\nKEY: %s\n", key);
}

    // VALUE 
    if (insert || update)
    {
        token = strtok(NULL, ":::");
        if ( token != NULL )
        {
            strcpy(value, token);
            valueSet = 1;
            printf("\nVALUE: %s\n", value);
        }
    }

    if ( (insert && keySet && valueSet) || (lookup && keySet && !valueSet) || (update && keySet && valueSet) || (delete && keySet && !valueSet) )
    {
        rc = SUCCESS;
        goto rtn;
    }
    else
    {
        rc = ERROR;
        printf("\nINVALID SYNTAX\n");
    }

  rtn:
    if (invalidOpCode)
        printf("\nINVALID OP CODE. TRY AGAIN!!!");
    return rc;


}
