//#include "structures.h"
//#include "lexer.h"
#define bufferSize 4
#define maxVarSize 60
#define SIZE 10000
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>






//info of the token returned by the lexer
typedef struct {
    char *value;
    char *tkId;
    int linenumber;
} TokenInfo;

//structure for the symbol table
typedef struct {
    char *tokId;
    char *entry;
} HashEntry;

//structure for twin buffer 
typedef struct {
    char buffer1[bufferSize];
    char buffer2[bufferSize];
    int forward;
    int lexBegin;
    int inUseBuffer; // 1 for First and 2 for Second
} TwinBuffer;








int lineNumber = 0;

FILE *getStream(FILE *filePointer, TwinBuffer* twinBuffer) {

    //if filepointer is end of file, return NULL
    size_t bytesRead;
    
    if (twinBuffer -> inUseBuffer == 1 && twinBuffer -> forward == bufferSize) {

        // Load the second buffer 
        bytesRead = fread(twinBuffer -> buffer2, sizeof(char), bufferSize, filePointer);
        //printf("%s\n","if1");
        //for (int i = 0; i < bufferSize; i ++) printf("%c", twinBuffer  ->  buffer2[i]);
        if(bytesRead<bufferSize){
            twinBuffer -> buffer2[bytesRead]='\0';
        }
        twinBuffer -> inUseBuffer = 2; 
        twinBuffer -> forward = 0;
        

    } else if(twinBuffer  ->  inUseBuffer==2 && twinBuffer  ->  forward == bufferSize) {

        // Load the first buffer
        bytesRead = fread(twinBuffer -> buffer1, sizeof(char), bufferSize, filePointer);
        //printf("%s\n","if2");
        //for (int i = 0; i < bufferSize; i ++) printf("%c", twinBuffer  ->  buffer1[i]);
        if(bytesRead<bufferSize){
            twinBuffer -> buffer1[bytesRead]='\0';
        }
        twinBuffer -> inUseBuffer = 1; 
        twinBuffer -> forward = 0;

    } else {
        twinBuffer -> forward ++;
    }

    if (bytesRead == 0) {
        // End of file
        return NULL;
    }
     
    return filePointer;
}




int fromZeroToWhere(char currentCharacter) {    

    if(currentCharacter == '\n') {
        lineNumber++;
        return 0;
    } 
    
    if(currentCharacter>='b'&& currentCharacter<='d') return 1;
    if((currentCharacter>='e' && currentCharacter<='z') || currentCharacter=='a') return 6;
    if(currentCharacter>='0' && currentCharacter<='9') return 8;
    if (currentCharacter == '_') return 19;
    if (currentCharacter == '#') return 24;
    if (currentCharacter == '[') return 30;
    if (currentCharacter == ']') return 31;
    if (currentCharacter == ',') return 32;
    if (currentCharacter == ';') return 33;
    if (currentCharacter == ':') return 34;
    if (currentCharacter == '.') return 35;
    if (currentCharacter == '<') return 53;
    if (currentCharacter == '>') return 60;
    if (currentCharacter == '(') return 36;
    if (currentCharacter == ')') return 37;
    if (currentCharacter == '+') return 38;
    if (currentCharacter == '-') return 39;
    if (currentCharacter == '*') return 40;
    if (currentCharacter == '/') return 41;
    if (currentCharacter == '~') return 42; 
    if (currentCharacter == '&') return 43;
    if (currentCharacter == '@') return 46;
    if (currentCharacter == '=') return 49;
    if (currentCharacter == '!') return 51;
    if (currentCharacter == '%') return 5;
    if(currentCharacter=='\0') return 62;
    if(currentCharacter == ' ') return 0;
    if(currentCharacter == '\t') return 0;    
    if(currentCharacter == '\0') return 0;  
    return 63;
}





TokenInfo* returnToken (TokenInfo *tokenInfo, char *value, char *tokenID, int lineNumber) {
    tokenInfo -> value = value;
    tokenInfo -> tkId = tokenID;
    tokenInfo -> linenumber = lineNumber;

    return tokenInfo;
}



TokenInfo* getNextToken(TwinBuffer* twinBuffer, FILE *filePointer) {
    
    char *currentBuffer;
    int state = 0;
    char *temp=malloc(sizeof(maxVarSize));
    int tempIndex = 0;
    // TODO: Check max possible size & check if buffer se badha ho .......
    // TODO: Line Number
    //todo: malloc temp here
    TokenInfo *tokenInfo = (TokenInfo *) malloc(sizeof(TokenInfo));

    while(1) {

        if (twinBuffer -> forward >= bufferSize) {
            filePointer = getStream(filePointer, twinBuffer);
        }
        currentBuffer = twinBuffer -> buffer2;

        if (twinBuffer -> inUseBuffer == 1) {
        currentBuffer = twinBuffer -> buffer1;
        }
        char currentCharacter = currentBuffer[twinBuffer -> forward];
        
        switch(state) {
            case 0:
                state = fromZeroToWhere(currentCharacter);
                // store in lexeme buffer
                if(state != 0 && currentCharacter!='%') {
                    temp[tempIndex] = currentCharacter;
                    tempIndex++;
                }
                
                twinBuffer -> forward ++;
                break;

            case 1:                
                if (currentCharacter - '0' >= 2 && currentCharacter - '0' <= 7) {
                    state = 2;                    
                } else if (currentCharacter >= 'a' && currentCharacter <= 'z') {
                    state = 6;
                } else {                    
                    // Retract 1
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_FIELDID";
                    twinBuffer -> lexBegin = twinBuffer -> forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }

                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;

            case 2: 
                if (currentCharacter >= 'b' && currentCharacter <= 'd') {
                    // Do Nothing
                } else if (currentCharacter - '0' >= 2 && currentCharacter - '0' <= 7) {
                    state = 3;
                } else {
                    // Retract 1                    
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_ID";
                    twinBuffer->lexBegin = twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber); 
                }
                
                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;

            case 3: 
                if (currentCharacter - '0' >= 2 && currentCharacter - '0' <= 7)  {
                    // Do Nothing
                } else {
                    // Retract 1                    
                    // twinBuffer -> forward --;
                    char *tk_id = "TK_ID";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber); 
                }
                
                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;      
                
            case 5:
                if (currentCharacter != '\n' && currentCharacter!='\0') {
                    state = 5;
                    twinBuffer -> forward ++;
                } else {
                    lineNumber ++;
                    state = 0;
                }

                break;

            case 6: 
                if (currentCharacter >= 'a' && currentCharacter <= 'z') {
                    // Do Nothing
                } else {                    
                    // Retract 1                    
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_FIELDID";
                    twinBuffer -> lexBegin = twinBuffer -> forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }
                
                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;                        
            
            //number and real number dfa
            case 8:
            if(currentCharacter >= '0' && currentCharacter<='9'){
                state=8;
            }
            else if(currentCharacter=='.'){
                state=10;
            }
            else{
                // Retract 1
                char *tk_id = "TK_NUM";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);
            }
            temp[tempIndex] = currentCharacter;
            tempIndex++;

            twinBuffer -> forward ++;
            break;

            case 10:
            if(currentCharacter >= '0' && currentCharacter<='9'){
                state=11;
            }
            else{
                // Retract 2
                twinBuffer -> forward --;
                char *tk_id = "TK_NUM";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);
            }
            temp[tempIndex] = currentCharacter;
            tempIndex++;

            twinBuffer -> forward ++;
            break;

            case 11:
            if(currentCharacter >= '0' && currentCharacter<='9'){
                state=12;
            }
            else 
            {
                //error
                twinBuffer->lexBegin=twinBuffer->forward;
                
                return returnToken(tokenInfo,"error" , NULL,lineNumber);
                break;
            }
            temp[tempIndex] = currentCharacter;
            tempIndex++;

            twinBuffer -> forward ++;
            break;

            case 12:
            if(currentCharacter=='E'){
                state=15;
            }
            else{
                // Retract 1
                char *tk_id = "TK_RNUM";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);
            }
            temp[tempIndex] = currentCharacter;
            tempIndex++;

            twinBuffer -> forward ++;
            break;

            case 15:
            
            if(currentCharacter >= '0' && currentCharacter<='9'){
                state=17;
            }
            else if(currentCharacter=='+' || currentCharacter=='-'){
                state=16;
            }
            else {
                //error
                twinBuffer->lexBegin=twinBuffer->forward;
                return returnToken(tokenInfo, "error",NULL,lineNumber);
                break;
            }
            temp[tempIndex] = currentCharacter;
            tempIndex++;

            twinBuffer -> forward ++;
            break;

            case 16:
            if(currentCharacter >= '0' && currentCharacter<='9'){
                state=17;
            }
            else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
            }
            temp[tempIndex] = currentCharacter;
            tempIndex++;

            twinBuffer -> forward ++;
            break;

            case 17:

                if(currentCharacter >= '0' && currentCharacter<='9'){
                    temp[tempIndex] = currentCharacter;
                    tempIndex++;

                    twinBuffer -> forward ++;
                    char *tk_id = "TK_RNUM";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                    break;

                }
                else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                
                break;
            

            case 19:
                if((currentCharacter >= 'a' && currentCharacter <= 'z') || (currentCharacter >= 'A' && currentCharacter <= 'Z')) {
                   state = 20;
                }
                twinBuffer -> forward ++;
                temp[tempIndex] = currentCharacter;
                tempIndex++;
                break;
                

            case 20:
                if((currentCharacter >= 'a' && currentCharacter <= 'z') || (currentCharacter >= 'A' && currentCharacter <= 'Z')) {
                   state = 20;
                   twinBuffer -> forward ++;
                }
                else if(currentCharacter >= '0' && currentCharacter<='9')
                {
                    state = 21;
                    twinBuffer -> forward ++;
                }
                else
                {
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_FUNID";
                    temp[tempIndex]='\0';
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }
                temp[tempIndex] = currentCharacter;
                tempIndex++;
                break;
            
            

            case 21: 
                if(currentCharacter >= '0' && currentCharacter<='9')
                {
                    state = 21;
                    twinBuffer -> forward ++;
                }
                else
                {
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_FUNID";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }
                temp[tempIndex] = currentCharacter;
                tempIndex++;
                break;            
                            
            case 24:
                if(currentCharacter >= 'a' && currentCharacter <= 'z') {
                    state = 28;
                    temp[tempIndex] = currentCharacter;
                    twinBuffer -> forward ++;
                    tempIndex++;
                }else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                break;

            case 28:
                if(currentCharacter >= 'a' && currentCharacter <= 'z') {
                    state = 28;
                    temp[tempIndex] = currentCharacter;
                    twinBuffer -> forward ++;
                    tempIndex++;
                }
                else {
                    // Retract 1
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_RUID";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }

                break;

            case 30:
            {
                char *tk_id = "TK_SQL";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);
            }
                break;
            
            case 31: 
                {char *tk_id = "TK_SQR";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
                
            case 32:
                {char *tk_id = "TK_COMMA";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
            
            case 33:
                {char *tk_id = "TK_SEM";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
                
            case 34:
                {char *tk_id = "TK_COLON";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;

            case 35:{
                char *tk_id = "TK_DOT";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;

            case 36:{
                char *tk_id = "TK_OP";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;

            case 37: {
                char *tk_id = "TK_CL";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
            
            case 38: {
                char *tk_id = "TK_PLUS";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
            
            case 39: {
                char *tk_id = "TK_MINUS";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
            
            case 40:{
                char *tk_id = "TK_MUL";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
                
            case 41:{
                char *tk_id = "TK_DIV";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;

            case 42:{
                char *tk_id = "TK_NOT";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lineNumber);}
                break;
            
            case 43:
                if(currentCharacter == '&'){
                    state = 44;
                    twinBuffer->forward++;
                }else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                break;
                
            case 44:
                if(currentCharacter == '&'){
                    char *tk_id = "TK_AND";
                    char *tk_v="&&&";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, tk_v, tk_id, lineNumber);
                }
                else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                break;
            
            case 46:
                if(currentCharacter == '@'){
                    state = 47;
                    twinBuffer->forward++;
                }else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                break;
            case 47:
                if(currentCharacter == '@'){
                    char *tk_id = "TK_OR";
                    temp="@@@";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }
                else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                break;
            
            case 49: 
                if(currentCharacter == '='){
                    char *tk_id = "TK_EQ";
                    temp = "==";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                break;
                
            case 51: 
                if(currentCharacter == '='){
                    char *tk_id = "TK_NE";
                    char *tk_v = "!=";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, tk_v, tk_id, lineNumber);
                }else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }
                break;

            case 53: 
                if(currentCharacter == '-'){
                    state = 54;
                }else if( currentCharacter=='='){
                    char *tk_id = "TK_LE";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='=';
                    temp[tempIndex+1]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }else {
                    // Retract 1
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_LT";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }
                
                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;
                
            case 54:
                if(currentCharacter == '-') {
                    state = 55;
                } else {
                    // Retract 2
                    twinBuffer -> forward --;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    char *tk_id = "TK_LT";
                    char *tk_val="<";
                    
                    return returnToken(tokenInfo, tk_val, tk_id, lineNumber);
                }
                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;
                
            case 55: 
                if(currentCharacter == '-') {
                    char *tk_id = "TK_ASSIGNOP";
                    temp[tempIndex] = currentCharacter;
                    tempIndex++;
                    twinBuffer -> forward ++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                } else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, "error",NULL,lineNumber);
                    break;
                }

                break;

            case 60: 
                if(currentCharacter == '=') {
                    char *tk_id = "TK_GE";  
                    twinBuffer -> forward ++;
                    temp[tempIndex] = currentCharacter;
                    tempIndex++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }   else {
                    // Retract 1
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_GT";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, temp, tk_id, lineNumber);
                }
                break;
            
            case 62:{
                char *tk_id ="EOF";
                return returnToken(tokenInfo, "$",tk_id,lineNumber);
            }
                break;

            case 63:
            //todo: default case when starts with unknown symbol
                //twinBuffer->forward++;
                twinBuffer->lexBegin=twinBuffer->forward;
                return returnToken(tokenInfo, "error",NULL,lineNumber);
                break;

        }

        
    }
}




//symbol table


// Define a structure for a key-value pair
typedef struct KeyValuePair {
    char *key;
    char *value;
    struct KeyValuePair *next;
} Element;

// Define the hashmap structure
typedef struct HashMap {
    struct KeyValuePair *buckets[SIZE];
} HashMap;

// Hash function to determine the bucket index for a given key
unsigned int hash(char *key) {
    unsigned int hash_value = 0;
    while (*key != '\0') {
        hash_value = (hash_value << 5) + *key++;
    }

    return hash_value % SIZE;
}

// Function to insert a key-value pair into the hashmap
void insertIntoHash(HashMap *map, char *key, char *value) {
    unsigned int index = hash(key);

    // Check if the key already exists in the bucket
    Element *current = map -> buckets[index];
    while (current != NULL) {
        if (strcmp(current -> key, key) == 0) {
            // Key already exists, update the value
            current -> value = value;
            return;
        }

        current = current -> next;
    }

    // Key doesn't exist, create a new key-value pair
    Element *newPair = (Element*) malloc(sizeof(Element));
    newPair -> key = strdup(key);
    newPair -> value = value;
    newPair -> next = map -> buckets[index];
    map -> buckets[index] = newPair;
}

// Function to initialize a new hashmap
HashMap *initializeHashMap() {
    HashMap *myMap = (HashMap*) malloc (sizeof(HashMap));
    for (int i = 0; i < SIZE; i++) {
        myMap -> buckets[i] = NULL;
    }

    // Inserting key-value pairs
    insertIntoHash(myMap, "with", "TK_WITH");
    insertIntoHash(myMap, "parameters", "TK_PARAMETERS");
    insertIntoHash(myMap, "end", "TK_END");
    insertIntoHash(myMap, "while", "TK_WHILE");
    insertIntoHash(myMap, "union", "TK_UNION");
    insertIntoHash(myMap, "endunion", "TK_ENDUNION");
    insertIntoHash(myMap, "definetype", "TK_DEFINETYPE");
    insertIntoHash(myMap, "as", "TK_AS");
    insertIntoHash(myMap, "type", "TK_TYPE");
    insertIntoHash(myMap, "_main", "TK_MAIN");
    insertIntoHash(myMap, "global", "TK_GLOBAL");
    insertIntoHash(myMap, "parameter", "TK_PARAMETER");
    insertIntoHash(myMap, "list", "TK_LIST");
    insertIntoHash(myMap, "input", "TK_INPUT");
    insertIntoHash(myMap, "output", "TK_OUTPUT");
    insertIntoHash(myMap, "int", "TK_INT");
    insertIntoHash(myMap, "real", "TK_REAL");
    insertIntoHash(myMap, "endwhile", "TK_ENDWHILE");
    insertIntoHash(myMap, "if", "TK_IF");
    insertIntoHash(myMap, "then", "TK_THEN");
    insertIntoHash(myMap, "endif", "TK_ENDIF");
    insertIntoHash(myMap, "read", "TK_READ");
    insertIntoHash(myMap, "write", "TK_WRITE");
    insertIntoHash(myMap, "return", "TK_RETURN");
    insertIntoHash(myMap, "call", "TK_CALL");
    insertIntoHash(myMap, "record", "TK_RECORD");
    insertIntoHash(myMap, "endrecord", "TK_ENDRECORD");
    insertIntoHash(myMap, "else", "TK_ELSE");

    return myMap;
}

// Function to search for a key in the hashmap
char *getValue(HashMap *map, char *key) {
    unsigned int index = hash(key);
    Element *current = map -> buckets[index];

    // Iterate through the linked list at the bucket
    while (current != NULL) {
        if (strcmp(current -> key, key) == 0) {
            // Key found, return the corresponding value
            return current -> value;
        }

        current = current->next;
    }

    // Key not found
    return "KEY NOT FOUND";
}

// Function to free the memory used by the hashmap
void destroyHashMap(HashMap *map) {
    for (int i = 0; i < SIZE; i++) {
        Element *current = map -> buckets[i];

        while (current != NULL) {
            Element *next = current->next;

            free(current->key);
            free(current);

            current = next;
        }
    }
    free(map);
}




int main() {
    // Initialize File Pointer
    FILE* filePointer;
    filePointer = fopen("C:\\Users\\91934\\Desktop\\Compiler-Project\\t2.txt", "r");

    if (filePointer == NULL) {
        printf("Failed to open file!\n");
        return 1;
    }

    // Initialize Twin Buffer
    TwinBuffer *twinBuffer = (TwinBuffer *) malloc (sizeof(TwinBuffer));
    twinBuffer  ->  forward = bufferSize;
    twinBuffer  ->  lexBegin = 0;
    twinBuffer  ->  inUseBuffer = 2;

    //create new hashtable and initialise with keywords
    HashMap *myMap = initializeHashMap();
    // Call function to get buffer
    filePointer=getStream(filePointer, twinBuffer);
    TokenInfo *tkinfo;
    while(filePointer!=NULL){
        tkinfo=getNextToken(twinBuffer,filePointer);
        if(tkinfo->tkId==NULL)
        {
            printf("%s\n","error");
            continue;
        }
        else if(tkinfo->value=="$"){
            break;
        }
        else
        {   
            if(strcmp(getValue(myMap,tkinfo->value),"KEY NOT FOUND" )==0){
                insertIntoHash(myMap,tkinfo->value,tkinfo->tkId);
                
            }
            printf("%s ", getValue(myMap,tkinfo->value));
            //printf("%s ",tkinfo->tkId);
            //printf("%s \n",tkinfo->value);
        }
        
    }
    destroyHashMap(myMap);
    fclose(filePointer);
    return 0;
}