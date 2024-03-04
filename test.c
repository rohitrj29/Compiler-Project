#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
//lexer

#include<math.h>

#define maxVarSize 100


#define MAX_SIZE 150
#define MAXELE 20
#define LINESIZE 100000
#define MAXTERM 50
#define MAXNONTERM 150
#define MAXRULES 120
#define TOTALRULES 95
#define PARSECOL 57
#define bufferSize 4
#define SIZE 10000



#define MAXSIZE 100




#define SIZE 10000





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


typedef struct
{
    int noOfElements;
    char leftElement[MAXTERM];
    char rightElements[MAXELE][MAXTERM];
    int ruleNo;
} GrammarRule;

// Struct to store NT lookup entry
typedef struct
{
    char nonTerminal[MAXTERM];
    int ffIndex;
    int grammarIndex;
} NTLookupEntry;

// Struct to store first and follow sets
typedef struct
{
    char nonTerminal[MAXTERM];
    char firstSet[MAXELE][MAXTERM];
    char followSet[MAXELE][MAXTERM];
    int noOfFirst;
    int noOfFollow;
    bool isEpsilon;
    bool visited;
    bool followCalc;
    bool lock;
} FirstFollow;

typedef struct ParseTreeNode {
    char lexeme[MAXTERM];
    struct ParseTreeNode* children[MAX_SIZE];
    int numChildren;
    int outIndex;
} ParseTreeNode;

typedef struct {
    char lexeme[MAXTERM];
    ParseTreeNode *nodePointer;
} StackElement;

typedef struct {
    StackElement *items[MAX_SIZE];
    int top;
} Stack;


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

unsigned int hash(char *key);
void insertIntoHash(HashMap *map, char *key, char *value);
HashMap *initializeHashMap();
char *getValue(HashMap *map, char *key);
void destroyHashMap(HashMap *map);


int lexerLineNumber = 0;
int lenflag = 0;
int idlen = 0;


 int lexerLineNumber;

FILE *getStream(FILE *filePointer, TwinBuffer* twinBuffer);
int fromZeroToWhere(char currentCharacter);
TokenInfo* returnToken (TokenInfo *tokenInfo, char *value, char *tokenID, int lineNumber);
TokenInfo* getNextToken(TwinBuffer* twinBuffer, FILE *filePointer);
void runLexerOnly();

Stack* initializeStack(Stack *stack);
int isEmpty(Stack *stack);
int isFull(Stack *stack);
void push(Stack *stack, StackElement *str);
StackElement* pop(Stack *stack);
StackElement* peek(Stack *stack);
void freeStack(Stack *stack);

StackElement *createNewStackElement (char lexeme[MAXTERM]);
ParseTreeNode* createNewParseTreeNode(char *lex);

void freeStack(Stack *stack);
void printInOrder(ParseTreeNode *root);
bool present(char element[MAXTERM], char array[MAXELE][MAXTERM], int noOfEleInArray);
void populateTerminals();
void populateFirstFromAnother(int ffIndex1, int ffIndex2);
void populateFirstFromSingleElement(int ffIndex, char _element[MAXTERM]);
void populateFollowFromFirst(int ffIndex1, int ffIndex2);
void populateFollowFromFollow(int ffIndex1, int ffIndex2);
void populateFollowFromElement(int ffIndex, char _element[MAXTERM]);
void intialiseFFandLookup();

NTLookupEntry getNTLookup(char *nonTerminal);
void findFirst(int ffind, int grammarInd);
void populateFirst();
void findFollow(int ffIndex);
void populateFollow();
int getTerminalIndex(char *terminal);
bool isTerminal(char *element);
void createParseTable();
void fillSyncInParseTable();
void createParseTree(char **input) ;
void printTerminals();
void printFirstFollow();
void printGrammarRules();
void printParseTable();
char **populateInputStream();



// hash table start

#include<stdlib.h>
#include<string.h>
#include<math.h>

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





// end of hash table

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
        lexerLineNumber++;
        return 0;
    } 
    
    if(currentCharacter>='b'&& currentCharacter<='d')
    {
        idlen++;
        return 1;
    }
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





TokenInfo* returnToken (TokenInfo *tokenInfo, char *value, char *tokenID, int lexerLineNumber) {
    tokenInfo -> value = value;
    tokenInfo -> tkId = tokenID;
    tokenInfo -> linenumber = lexerLineNumber;

    return tokenInfo;
}

void generateErrorMessage(char **errorMessage, int lexerLineNumber, char *temp) {
    *errorMessage = (char *) malloc(sizeof(char) * MAXSIZE);

    strcpy(*errorMessage, "Line ");
    char str[5];
    sprintf(str, "%d", lexerLineNumber + 1);
    strcat(*errorMessage, str);

    char str1[100] = " Error: Unknown Pattern: <";
    strcat(*errorMessage, str1);
    strcat(*errorMessage, temp);
    strcat(*errorMessage, ">");

    printf("%s\n", *errorMessage);
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
                    idlen++;                   
                } else if (currentCharacter >= 'a' && currentCharacter <= 'z') {
                    state = 6;
                    idlen++;
                } else {                    
                    // Retract 1
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_FIELDID";
                    twinBuffer -> lexBegin = twinBuffer -> forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                }

                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;

            case 2: 
                if (currentCharacter >= 'b' && currentCharacter <= 'd') {
                    // Do Nothing
                    idlen++;
                    if(lenflag == 0 && idlen > 20)
                    {
                        lenflag = 1;
                       // printf("LENGTH ERROR!");   
                    }
                } else if (currentCharacter - '0' >= 2 && currentCharacter - '0' <= 7) {
                    state = 3;
                    idlen++;
                    if(lenflag == 0 && idlen > 20)
                    {
                        lenflag = 1;
                     //   printf("LENGTH ERROR!");   
                    }

                } else {
                    // Retract 1                    
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_ID";
                    twinBuffer->lexBegin = twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber); 
                }
                
                temp[tempIndex] = currentCharacter;
                tempIndex++;

                twinBuffer -> forward ++;
                break;

            case 3: 
                if (currentCharacter - '0' >= 2 && currentCharacter - '0' <= 7)  {
                    // Do Nothing
                    idlen++;
                    if(lenflag == 0 && idlen > 20)
                    {
                        lenflag = 1;
                      //  printf("LENGTH ERROR!");   
                    }
                } else {
                    // Retract 1                    
                    // twinBuffer -> forward --;
                    char *tk_id = "TK_ID";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber); 
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
                    //lexerLineNumber ++;
                    state = 0;
                }

                break;

            case 6: 
                if (currentCharacter >= 'a' && currentCharacter <= 'z') {
                    // Do Nothing
                    idlen++;
                    if(lenflag == 0 && idlen > 20)
                    {
                        lenflag = 1;
                       // printf("LENGTH ERROR!");   
                    }
                } else {                    
                    // Retract 1                    
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_FIELDID";
                    twinBuffer -> lexBegin = twinBuffer -> forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
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
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
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
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
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
                    twinBuffer -> lexBegin = twinBuffer -> forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    
                    return returnToken(tokenInfo, errorMessage, NULL, lexerLineNumber);
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
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
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
                temp[tempIndex]='\0';
                char *errorMessage;
                generateErrorMessage(&errorMessage, lexerLineNumber, temp);

                return returnToken(tokenInfo, errorMessage, NULL,lexerLineNumber);
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
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage, NULL,lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                    break;

                }
                else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage, NULL,lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
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
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage, NULL,lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                }

                break;

            case 30:
            {
                char *tk_id = "TK_SQL";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
            }
                break;
            
            case 31: 
                {char *tk_id = "TK_SQR";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
                
            case 32:
                {char *tk_id = "TK_COMMA";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
            
            case 33:
                {char *tk_id = "TK_SEM";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
                
            case 34:
                {char *tk_id = "TK_COLON";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;

            case 35:{
                char *tk_id = "TK_DOT";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;

            case 36:{
                char *tk_id = "TK_OP";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;

            case 37: {
                char *tk_id = "TK_CL";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
            
            case 38: {
                char *tk_id = "TK_PLUS";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
            
            case 39: {
                char *tk_id = "TK_MINUS";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
            
            case 40:{
                char *tk_id = "TK_MUL";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
                
            case 41:{
                char *tk_id = "TK_DIV";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;

            case 42:{
                char *tk_id = "TK_NOT";
                twinBuffer->lexBegin=twinBuffer->forward;
                temp[tempIndex]='\0';
                return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);}
                break;
            
            case 43:
                if(currentCharacter == '&'){
                    state = 44;
                } else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage, NULL,lexerLineNumber);
                    break;
                }
                
                    temp[tempIndex] = currentCharacter;
                    tempIndex++;

                    twinBuffer -> forward ++;
                    break;
                
            case 44:
                if(currentCharacter == '&'){
                    char *tk_id = "TK_AND";
                    char *tk_v="&&&";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, tk_v, tk_id, lexerLineNumber);
                }
                else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage, NULL,lexerLineNumber);
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
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage, NULL,lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                }
                else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo,errorMessage,NULL,lexerLineNumber);
                    break;
                }
                break;
            
            case 49: 
                if(currentCharacter == '='){
                    char *tk_id = "TK_EQ";
                    char *tk_v = "==";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, tk_v, tk_id, lexerLineNumber);
                }else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage,NULL,lexerLineNumber);
                    break;
                }
                break;
                
            case 51: 
                if(currentCharacter == '='){
                    char *tk_id = "TK_NE";
                    char *tk_v = "!=";
                    twinBuffer->forward++;
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, tk_v, tk_id, lexerLineNumber);
                }else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage,NULL,lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                }else {
                    // Retract 1
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_LT";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
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
                    
                    return returnToken(tokenInfo, tk_val, tk_id, lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                } else {
                    //error
                    twinBuffer->lexBegin=twinBuffer->forward;
                    temp[tempIndex]='\0';
                    char *errorMessage;
                    generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                    return returnToken(tokenInfo, errorMessage,NULL,lexerLineNumber);
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
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                }   else {
                    // Retract 1
                    //twinBuffer -> forward --;
                    char *tk_id = "TK_GT";
                    twinBuffer->lexBegin=twinBuffer->forward;
                    return returnToken(tokenInfo, temp, tk_id, lexerLineNumber);
                }
                break;
            
            case 62:{
                char *tk_id ="EOF";
                return returnToken(tokenInfo, "$",tk_id,lexerLineNumber);
            }
                break;

            case 63:
            //todo: default case when starts with unknown symbol
                //twinBuffer->forward++;
                twinBuffer->lexBegin=twinBuffer->forward;
                char *errorMessage;
                generateErrorMessage(&errorMessage, lexerLineNumber, temp);
                return returnToken(tokenInfo, errorMessage ,NULL,lexerLineNumber);
                break;

        }

        
    }
}

void runLexerOnly() {
    // Initialize File Pointer
    FILE* filePointer;
    filePointer = fopen("C:\\Users\\91620\\Desktop\\CoCo\\Compiler-Project\\t2.txt", "r");

    if (filePointer == NULL) {
        printf("Failed to open file!\n");
        exit(1);
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
        tkinfo=getNextToken(twinBuffer, filePointer);
        if(tkinfo->tkId==NULL)
        {
            // printf("Line no. %d  Lexical Error\n",lexerLineNumber+1);
            continue;
        }
        else if(strcmp(tkinfo->value,"$")==0){
            printf("%s",tkinfo->value);
            break;
        }
        else
        {   
            if((strlen(tkinfo->value) <= 20) | (strcmp(tkinfo->tkId,"TK_FUNID")==0 & strlen(tkinfo->value) <= 30))
            {
                if(strcmp(getValue(myMap,tkinfo->value),"KEY NOT FOUND" )==0){
                    insertIntoHash(myMap,tkinfo->value,tkinfo->tkId);
                
                }    
            
                printf("Line %d Lexeme: %s Token %s \n", lexerLineNumber+1,tkinfo->value, getValue(myMap,tkinfo->value));
                // printf("%s ",tkinfo->tkId);
                // printf("%s \n",tkinfo->value);
            }
            else {
                printf("Line %d Error:  Identifier is too long\n", lexerLineNumber+1);
            }
        }
        
    }

    printf("Only Lexical analyzer module developed!\n");

    destroyHashMap(myMap);
    fclose(filePointer);
    return;
}


//parser.c




int parserLineNumber = 0;

// Arrays to store data
GrammarRule grammarRule[MAXRULES];
NTLookupEntry NTLookup[MAXNONTERM];
FirstFollow firstFollow[MAXNONTERM];
char terminals[PARSECOL][MAXTERM];
int parseTable[55][PARSECOL];

char **token;
char **value;
int *lineNo; 

int noOfNonTerminals = 0; // Added for storing the total number of non-terminals

Stack* initializeStack(Stack *stack);
int isEmpty(Stack *stack);
int isFull(Stack *stack);
void push(Stack *stack, StackElement *str);
StackElement* pop(Stack *stack);
StackElement* peek(Stack *stack);
void freeStack(Stack *stack);


Stack* initializeStack(Stack *stack) {
    stack = (Stack *) malloc(sizeof(Stack));
    stack->top = -1;
    return stack;
}

int isEmpty(Stack *stack) {
    return (stack->top == -1);
}

int isFull(Stack *stack) {
    return (stack->top == MAX_SIZE - 1);
}

void push(Stack *stack, StackElement *stackEle) {
    if (isFull(stack)) {
        printf("Stack overflow!\n");
        return;
    }

    stack->top++;
    stack->items[stack->top] = stackEle;
}

StackElement* pop(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack underflow!\n");
        return NULL;
    }

    return stack->items[stack->top--];
}


StackElement* peek(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack is empty!\n");
        return NULL;
    }

    return stack->items[stack->top];
}

StackElement *createNewStackElement (char lexeme[MAXTERM]) {
    StackElement *stackElement = (StackElement *) malloc (sizeof(StackElement));
    stackElement -> nodePointer = NULL;
    strcpy(stackElement -> lexeme, lexeme);

    return stackElement;
}

ParseTreeNode* createNewParseTreeNode(char *lex) {
    ParseTreeNode *treeElement = (ParseTreeNode *) malloc (sizeof(ParseTreeNode));
    treeElement -> children[0] = NULL;
    strcpy(treeElement -> lexeme , lex);
    treeElement -> numChildren = 0;
    treeElement -> outIndex = -1;
    return treeElement;
}

void freeStack(Stack *stack) {
    while (!isEmpty(stack)) {
        free(pop(stack));
    }
}

void printParseTree(ParseTreeNode *root) {
    if (root->lexeme == NULL) {
        return;
    }

    // Traverse each child node
    for (int i = root -> numChildren - 1; i > 0 ; i--) {
        printParseTree(root -> children[i]);
        
    }
    if(root->outIndex>=0){
        printf("Terminal : %s  Lexeme : %s Line No : %d \n", root->lexeme, value[root->outIndex],lineNo[root->outIndex]);
    }
    else{
    printf("Non-Terminal : %s  \n", root->lexeme);
    }

    // value , line no, token
    printParseTree(root->children[0]);

}

bool present(char element[MAXTERM], char array[MAXELE][MAXTERM], int noOfEleInArray) {
    for (int i = 0; i < noOfEleInArray; i++) {
        if(strcmp(element, array[i]) == 0) return true;
    }
    
    return false;
}

void populateTerminals(){
    int index=1;
    strcpy(terminals[0],"$");
    for(int i=0;i<TOTALRULES;i++){
        for(int j=0;j<grammarRule[i].noOfElements;j++){
            if((grammarRule[i].rightElements[j][0]=='T' ) && present(grammarRule[i].rightElements[j],terminals,58)==false){
                strcpy(terminals[index],grammarRule[i].rightElements[j]);
                index++;
            }
        }
    }
}

void populateFirstFromAnother(int ffIndex1, int ffIndex2) {
    int initialElementsInFirst = firstFollow[ffIndex1].noOfFirst, intialElementsInFF2 = firstFollow[ffIndex2].noOfFirst;
    
    for (int i = 0; i < intialElementsInFF2; i++) {
        char element[MAXTERM];
        strcpy(element, firstFollow[ffIndex2].firstSet[i]);
        
        if (present(element, firstFollow[ffIndex1].firstSet, initialElementsInFirst) || strcmp(element, "eps") == 0) continue;
        else {
            strcpy(firstFollow[ffIndex1].firstSet[initialElementsInFirst ++], element);
            firstFollow[ffIndex1].noOfFirst ++;
        }
    }
}

void populateFirstFromSingleElement(int ffIndex, char _element[MAXTERM]) {
    int initialElementsInFirst = firstFollow[ffIndex].noOfFirst;
    char element[MAXTERM];
    strcpy(element, _element);
    
    if (present(element, firstFollow[ffIndex].firstSet, initialElementsInFirst)) return;
    else {
        strcpy(firstFollow[ffIndex].firstSet[initialElementsInFirst], element);
        firstFollow[ffIndex].noOfFirst ++;
    }
}

void populateFollowFromFirst(int ffIndex1, int ffIndex2) {
    int initialElementsInFollow = firstFollow[ffIndex1].noOfFollow, intialElementsInFirst = firstFollow[ffIndex2].noOfFirst;
    
    for (int i = 0; i < intialElementsInFirst; i++) {
        char element[MAXTERM];
        strcpy(element, firstFollow[ffIndex2].firstSet[i]);
        
        if (present(element, firstFollow[ffIndex1].followSet, initialElementsInFollow) || strcmp(element, "eps") == 0) continue;
        else {
            strcpy(firstFollow[ffIndex1].followSet[initialElementsInFollow ++], element);
            firstFollow[ffIndex1].noOfFollow ++;
        }
    }
}

void populateFollowFromFollow(int ffIndex1, int ffIndex2) {
    int initialElementsInFollow1 = firstFollow[ffIndex1].noOfFollow, intialElementsInFollow2 = firstFollow[ffIndex2].noOfFollow;
    
    for (int i = 0; i < intialElementsInFollow2; i++) {
        char element[MAXTERM];
        strcpy(element, firstFollow[ffIndex2].followSet[i]);
        
        if (present(element, firstFollow[ffIndex1].followSet, initialElementsInFollow1) || strcmp(element, "eps") == 0) continue;
        else {
            strcpy(firstFollow[ffIndex1].followSet[initialElementsInFollow1 ++], element);
            firstFollow[ffIndex1].noOfFollow ++;
        }
    }
}

void populateFollowFromElement(int ffIndex, char _element[MAXTERM]) {
    int initialElementsInFollow = firstFollow[ffIndex].noOfFollow;
    char element[MAXTERM];
    strcpy(element, _element);
    
    if (present(element, firstFollow[ffIndex].followSet, initialElementsInFollow) || strcmp(element, "eps") == 0) return;
    else {
        strcpy(firstFollow[ffIndex].followSet[initialElementsInFollow], element);
        firstFollow[ffIndex].noOfFollow ++;
    }
}

// Function to initialize first and follow sets and NT lookup
void intialiseFFandLookup() {
    int entry = 0;
    char prev[MAXTERM] = "";

    for (int i = 0; i < parserLineNumber; i++)
    {
        char nonterm[MAXTERM];
        strcpy(nonterm, grammarRule[i].leftElement);

        // Check if the current non-terminal is different from the previous one
        if (strcmp(prev, nonterm) != 0)
        {
            // Copy non-terminal to NTLookup entry
            strcpy(NTLookup[entry].nonTerminal, nonterm);
            NTLookup[entry].grammarIndex = i;
            NTLookup[entry].ffIndex = entry;
            strcpy(firstFollow[entry].nonTerminal, nonterm);

            // Update previous non-terminal
            strcpy(prev, nonterm);

            // Increment entry count
            entry++;
        }

        firstFollow[entry - 1].visited = false;
        firstFollow[entry - 1].followCalc = false;
        // Check if the first element of the production rule is epsilon
        if (strcmp(grammarRule[i].rightElements[0], "eps") == 0)
            firstFollow[entry - 1].isEpsilon = true;
        else
            firstFollow[entry - 1].isEpsilon = false;
    }

    // Set the total number of non-terminals
    noOfNonTerminals = entry;
}


NTLookupEntry getNTLookup(char *nonTerminal)
{
    for (int i = 0; i < noOfNonTerminals; i++)
    {
        if (strcmp(NTLookup[i].nonTerminal, nonTerminal) == 0)
        {
            return NTLookup[i]; // Return the specific entry found
        }
    }

    // Handle the case where no entry is found
    // This could be an error condition, so you might want to handle it accordingly
    // For now, let's return a default initialized entry
    NTLookupEntry defaultEntry = {"", 0, 0}; // Or whatever default values you want to set
    return defaultEntry;
}




void findFirst(int ffind, int grammarInd) {

    for(int i=grammarInd; i<TOTALRULES;i++) {
        if(strcmp(grammarRule[i].leftElement, NTLookup[ffind].nonTerminal) != 0) {
            break;
        }

        int j=grammarRule[i].noOfElements;
        for(int k=0;k<j;k++)
        {

            //TODO: if the first is already calculated
            //copy all first elements to the current non terminal

            //if the element is a terminal
            if((grammarRule[i].rightElements[k][0]=='T') || strcmp(grammarRule[i].rightElements[k], "eps") == 0){
                populateFirstFromSingleElement(ffind, grammarRule[i].rightElements[k]);
                break;
            }
            else{
                    NTLookupEntry check= getNTLookup(grammarRule[i].rightElements[k]);
                    if(firstFollow[check.ffIndex].isEpsilon) {
                        
                        if(firstFollow[check.ffIndex].visited==true){
                            //populate
                            populateFirstFromAnother(ffind, check.ffIndex);
                        }
                        else{
                        findFirst(check.ffIndex,check.grammarIndex);
                        //populate first set
                        populateFirstFromAnother(ffind, check.ffIndex);
                        }
                        if(k==j-1){
                            //populate epsilon in first set
                            populateFirstFromSingleElement(ffind, "eps");
                            break;
                        }
                        
                        
                    }
                    else{
                        if(firstFollow[check.ffIndex].visited==true){
                            //populate
                            populateFirstFromAnother(ffind, check.ffIndex);
                        }
                        else{
                            findFirst(check.ffIndex,check.grammarIndex);
                            //populate first set
                            populateFirstFromAnother(ffind, check.ffIndex);
                        }
                        break;
                    }
                    
                }
                
            }
        
        }

    firstFollow[ffind].visited=true;
    return;
}


void findFollow(int ffIndex){
    for(int i=0;i<parserLineNumber;i++){
        int found=0;
        
        for(int j=0;j<grammarRule[i].noOfElements;j++){
            if(strcmp(grammarRule[i].rightElements[j],firstFollow[ffIndex].nonTerminal)==0){
                found=1;
            }
            if(found==1){
                
                //last element 
                if(strcmp(grammarRule[i].rightElements[j],firstFollow[ffIndex].nonTerminal)==0 && j==grammarRule[i].noOfElements-1){
                    
                    NTLookupEntry check= getNTLookup(grammarRule[i].leftElement);
                    if(firstFollow[check.ffIndex].lock==true){
                        found=0;
                        continue;
                    }
                    if(strcmp(grammarRule[i].leftElement,grammarRule[i].rightElements[grammarRule[i].noOfElements-1])==0){
                        break;
                    }
                    firstFollow[ffIndex].lock=true;
                    findFollow(check.ffIndex);
                    firstFollow[ffIndex].lock=false;
                    //populate follow set with follow set of left element
                    populateFollowFromFollow(ffIndex, check.ffIndex);
                    found=0;
                    

                }
                // not the last element
                else{
                    if(grammarRule[i].rightElements[j+1][0]=='T'){
                        //populate follow set with rightElement[j+1] as it is a terminal
                        populateFollowFromElement(ffIndex, grammarRule[i].rightElements[j+1]);
                        found=0;
                        break;
                    }
                    else{
                        NTLookupEntry check= getNTLookup(grammarRule[i].rightElements[j+1]);
                        if(firstFollow[check.ffIndex].lock==true){
                            found=0;
                            continue;
                        }
                            if(firstFollow[check.ffIndex].isEpsilon){

                                // if(firstFollow[check.ffIndex].followCalc==true){
                                //     //popoulate follow set with first set of right element
                                //     populateFollowFromFirst(ffIndex, check.ffIndex);
                                //     //populate follow set with follow set of right element
                                //     populateFollowFromFollow(ffIndex, check.ffIndex);
                                //     found=0;
                                // }
                                // else
                                // {
                                
                                    if(j+1==grammarRule[i].noOfElements-1){
                                    
                                        //populate follow set with the first of last element i.e. j+1 current
                                        populateFollowFromFirst(ffIndex, check.ffIndex);
                                        NTLookupEntry check= getNTLookup(grammarRule[i].leftElement);
                                        firstFollow[ffIndex].lock=true;
                                        findFollow(check.ffIndex);
                                        firstFollow[ffIndex].lock=false;
                                        //populate follow set with follow set of left element
                                        populateFollowFromFollow(ffIndex, check.ffIndex);
                                        found=0;
                                    
                                    }
                                    else{
                                    
                                         //populate the follow set with the first of rightElement[j+1] -eps we are currently at
                                        populateFollowFromFirst(ffIndex, check.ffIndex);
                                        firstFollow[ffIndex].lock=true;
                                        findFollow(check.ffIndex);
                                        firstFollow[ffIndex].lock=false;
                                     
                                    
                                    }
                                // }
                                
                            }
                            else{
                                //populate follow set with first set of check.nonTerminal
                                populateFollowFromFirst(ffIndex, check.ffIndex);
                                found=0;
                            }
                        
                    }
                        
                }
            }
        }
    }
    firstFollow[ffIndex].followCalc=true;
    return;
}


/*
parser table population
using first and follow set of non terminals
*/

//pass a terminal and get its index
int getTerminalIndex(char *terminal)
{
    for(int i = 0; i < PARSECOL; i++)
    {
        if(strcmp(terminals[i], terminal) == 0)
        {
            return i;
        }
    }
    return -1;
}

bool isTerminal(char *element)
{
    if((element[0] == 'T')|| element[0] == '$')
    {
        return true;
    }
    return false;
}

void fillSyncInParseTable()
{
    for(int i = 0; i < noOfNonTerminals; i++)
    {
        
            
        for(int k = 0; k < firstFollow[i].noOfFollow; k++)
        {
            int followIndex = getTerminalIndex(firstFollow[i].followSet[k]);
            if(parseTable[i][followIndex] == -1)
                parseTable[i][followIndex] = -2;
        }
        // for(int k = 0; k < firstFollow[i].noOfFirst; k++)
        // {
        //     int firstIndex = getTerminalIndex(firstFollow[i].firstSet[k]);
        //     if(strcmp(firstFollow[i].firstSet[k],"eps")==0)
        //     continue;
        //     if(parseTable[i][firstIndex] == -1)
        //         parseTable[i][firstIndex] = -2;
        // }
            
        
    }
}

void createParseTable()
{   
    memset(parseTable, -1, sizeof(parseTable));

    for(int gindex = 0; gindex<TOTALRULES; gindex++)
    {
        NTLookupEntry entry = getNTLookup(grammarRule[gindex].leftElement);
        int ntindex = entry.ffIndex;
        FirstFollow ff = firstFollow[ntindex];
        GrammarRule rule = grammarRule[gindex];
        // ntindex=10
        //only right[0] needs to be checked
        int flag=1;
        for(int rhsIndex = 0; flag==1 && rhsIndex < rule.noOfElements; rhsIndex++)
        {   
            flag=0;
            char element[MAXTERM];
            strcpy(element, rule.rightElements[rhsIndex]);
            if(strcmp(element, "eps") == 0)
            {
                for(int findex = 0; findex < ff.noOfFollow; findex++)
                {
                    char followElement[MAXTERM];
                    strcpy(followElement, ff.followSet[findex]);
                    int followIndex = getTerminalIndex(followElement);
                    if(followIndex==56){
                        bool test=true;
                    }
                    parseTable[ntindex][followIndex] = gindex;
                }
                
            }
            else if(isTerminal(element))
            {
                int terminalIndex = getTerminalIndex(element);
                if(terminalIndex==56){
                    bool test=true;
                }
                parseTable[ntindex][terminalIndex] = gindex;
            }
            else
            {
                // variable is non-terminal
                int nonTerminalIndex = getNTLookup(element).ffIndex;
                for(int findex = 0; findex < firstFollow[nonTerminalIndex].noOfFirst; findex++)
                {
                    
                    char firstElement[MAXTERM];
                    strcpy(firstElement, firstFollow[nonTerminalIndex].firstSet[findex]);
                    if(strcmp(firstElement, "eps") == 0){
                        //first of next element until not eps is matched
                        flag=1;
                        if(rhsIndex==rule.noOfElements-1){
                            for(int findex = 0; findex < ff.noOfFollow; findex++)
                            {
                                char followElement[MAXTERM];
                                strcpy(followElement, ff.followSet[findex]);
                                int followIndex = getTerminalIndex(followElement);
                                if(followIndex==56){
                                    bool test=true;
                                }
                                parseTable[ntindex][followIndex] = gindex;
                            }
                            flag=0;
                        }
                    }
                    int firstIndex = getTerminalIndex(firstElement);
                    if(firstIndex==56){
                        bool test=true;
                    }
                    if(firstIndex!=-1)
                    parseTable[ntindex][firstIndex] = gindex;
                }


            }
        }

    }
    fillSyncInParseTable();
}




void parseInputSourceCode() {
    Stack *myStack;
    myStack = initializeStack(myStack);
    int ind = 0;
    char currtoken[MAXTERM];
    strcpy(currtoken, token[ind ++]);

    ParseTreeNode *root = createNewParseTreeNode(grammarRule[0].leftElement);    
    root->outIndex=-1;
    StackElement *dollar = createNewStackElement("$");
    push(myStack, dollar);

    StackElement *startSymbolEle = createNewStackElement(grammarRule[0].leftElement);
    startSymbolEle -> nodePointer = root;
    push(myStack, startSymbolEle);

    int success = 1;
    while (strcmp(currtoken, "EOF") != 0) {  

        //printing lexical error

        if(strcmp(currtoken,"ERROR")==0){
            success=0;
            printf("Line No. %d Error:  %s  \n",lineNo[ind-1],value[ind-1]);
            strcpy(currtoken, token[ind ++]);
            continue;
        }

        StackElement *topElement = peek(myStack);
        //index of token in the parse table and index of topElement in the parse table
        int tokenIndex = getTerminalIndex(currtoken);
        int topElementIndex = -1;
        if(!isTerminal(topElement -> lexeme)){
            topElementIndex = getNTLookup(topElement -> lexeme).ffIndex;
        }
        
        if(strcmp(topElement->lexeme,"TK_LIST")==0){
            bool test=true;
        }

        ParseTreeNode *currTreePointer = topElement -> nodePointer;
        
        int tableValue = -3;
        if(topElementIndex>=0)
        tableValue=parseTable[topElementIndex][tokenIndex];
        
        if (isTerminal(currtoken) && isTerminal(topElement -> lexeme) && strcmp(currtoken, topElement -> lexeme) == 0) {
            topElement -> nodePointer -> outIndex=ind-1;
            strcpy(currtoken, token[ind ++]);
            pop (myStack);
            free (topElement);
        }else if (currtoken[0]=='$' && topElement -> lexeme[0] == '$') {
            break;
        } else if (topElementIndex == -1) {
            success=0;
            printf("Error: The token is %s for lexeme %s doesn't match the expected token %s \n",currtoken,value[ind-1],topElement->lexeme);
            strcpy(currtoken, token[ind ++]);
            pop (myStack);
            free (topElement);

        }  else if (tableValue == -2) {
            success=0;
            printf("Sync Error: stack pop %s \n",topElement->lexeme);
            pop (myStack);
            free (topElement);            
        } else if (tableValue == -1) {
            success=0;
            printf("Line No. %d Error: token skipped %s with value %s  \n",lineNo[ind-1],currtoken,value[ind-1]);

            strcpy(currtoken, token[ind ++]);
        } else if (tableValue >= 0) {
            GrammarRule rule = grammarRule[tableValue];
            pop(myStack);
            free(topElement);
            for (int i = rule.noOfElements - 1; i >= 0; i--) {
                if(strcmp(rule.rightElements[i], "eps") == 0) 
                {   
                    //printf("%s\n",myStack->items[myStack->top]->lexeme);
                    // pop(myStack);
                    // free(topElement);
                    continue;
                }
                
                StackElement *newElement = createNewStackElement(rule.rightElements[i]);                
                ParseTreeNode *treeNode = createNewParseTreeNode(rule.rightElements[i]);

                newElement -> nodePointer = treeNode;

                int originalChildren = currTreePointer -> numChildren;
                currTreePointer -> children[originalChildren] = treeNode;
                currTreePointer -> numChildren ++;

                push(myStack, newElement);
            }
        } 
        else {
            /*error cases
            1. if both the top of the stack and token is a terminal is not equal to it
            */
           success = 0;
           break;
        }
    }

    if (!success ) {
        
    } else {
        printf("Input Source Code is syntactically correct ........\n");
        printf("The inorder traversal of the parse tree is as follows: \n");
        // printf("%s\n", root -> lexeme);
        printParseTree(root);
    }

    // if stack is not empty pop it out 
    free(myStack);
}

/*
end of parser table population
*/

void printTerminals(){
    //print terminals
    for(int i=0;i<PARSECOL;i++){
        printf(" %d %s \n",i,terminals[i]);
    }
}

void printFirstFollow(){
    for(int i=0;i<noOfNonTerminals;i++){
        printf("First set of %s \n ",firstFollow[i].nonTerminal);
        for(int j=0;j<firstFollow[i].noOfFirst;j++){
            printf("%s ",firstFollow[i].firstSet[j]);
        }
        printf("\n");
        printf("Follow set of %s \n ",firstFollow[i].nonTerminal);
        for(int j=0;j<firstFollow[i].noOfFollow;j++){
            printf("%s ",firstFollow[i].followSet[j]);
        }
        printf("\n");

    }
}

void printGrammarRules(){
    // print the grammar rules
    for (int i = 0; i < parserLineNumber; i++)
    {
        printf("%s ->   ", grammarRule[i].leftElement);
        for (int j = 0; j < grammarRule[i].noOfElements; j++)
        {
            printf("%s ", grammarRule[i].rightElements[j]);
        }
        printf("\n");
    }
}
void printParseTable(){
    printf("Parse Table\n");
    printf("Non Terminals ");
    for(int i=0;i<PARSECOL;i++){
        printf("%s ",terminals[i]);
    }
    printf("\n");
    for(int i=0;i<noOfNonTerminals;i++){
        printf("%s ",firstFollow[i].nonTerminal);
        for(int j=0;j<PARSECOL;j++){
            if(parseTable[i][j]==-1){
                printf("error ");
            }
            else if(parseTable[i][j]==-2){
                printf("sync ");
            }
            else{
                printf("%d ",parseTable[i][j]);
            }
        }
        printf("\n");
    }
}

// char **populateInputStream() {
//     FILE *fp;
//     fp = fopen("./parseTreeCustomInput.txt", "r");

//     char line[LINESIZE], **input;
//     input = (char **) malloc (sizeof(char *) * LINESIZE);
//     int i = 0;

//     while (fgets(line, LINESIZE, fp) != NULL) {
//         char *token = strtok(line, " ");

//         while (token != NULL) {
//             input[i] = (char *) malloc (sizeof(char) * MAXTERM);
//             strcpy(input[i ++], token);

//             token = strtok(NULL, " ");
//         }
//     }

//     fclose(fp);
//     return input;
// }

void  computeFirstAndFollowSets(){
    //populate first of all non terminals
    for(int i=0;i<noOfNonTerminals;i++){
        if(firstFollow[i].visited==true){
            continue;
        }
        firstFollow[i].noOfFirst=0;
        findFirst(i, NTLookup[i].grammarIndex);
    }
    //populate follow of all non terminals
    firstFollow[0].noOfFollow=1;
    strcpy(firstFollow[0].followSet[0],"$");
    for(int i=1;i<noOfNonTerminals;i++){
        firstFollow[i].noOfFollow=0;
        findFollow(i);
    }
}

void startParsing()
{
    FILE *fp;
    fp = fopen("./FinalGrammar.txt", "r");

    if (fp == NULL)
    {
        printf("File not found\n");
        exit(1); // Return with error status
    }

    char line[LINESIZE];

    while (fgets(line, LINESIZE, fp) != NULL)
    {
        char *token = strtok(line, " ");
        
        int i = 0;

        while (token != NULL)
        {   
            
            int len=strlen(token);
            if(token[len-1]=='\n'){
                token[len-1]='\0';
            }
            
            if (i == 0)
            {
                strcpy(grammarRule[parserLineNumber].leftElement, token);
            }
            else
            {
                strcpy(grammarRule[parserLineNumber].rightElements[i - 1], token);
            }
            i++;
            token = strtok(NULL, " ");
            
        }

        grammarRule[parserLineNumber].noOfElements = i - 1;
        grammarRule[parserLineNumber].ruleNo = parserLineNumber;
        parserLineNumber++;
    }

    fclose(fp);    

    intialiseFFandLookup();
    populateTerminals();


    computeFirstAndFollowSets();
    createParseTable();
    
    //  printParseTable();
    // printFirstFollow();
    
    return;
}


void runLexerAndParser() {
    // Initialize File Pointer
    FILE* filePointer;
    filePointer = fopen("C:\\Users\\91620\\Desktop\\CoCo\\Compiler-Project\\t6.txt", "r");

    if (filePointer == NULL) {
        printf("Failed to open file!\n");
        exit(1);
    }

    startParsing();
    // Initialize Twin Buffer
    TwinBuffer *twinBuffer = (TwinBuffer *) malloc (sizeof(TwinBuffer));
    twinBuffer  ->  forward = bufferSize;
    twinBuffer  ->  lexBegin = 0;
    twinBuffer  ->  inUseBuffer = 2;

    //create new hashtable and initialise with keywords
    HashMap *myMap = initializeHashMap();
    // Call function to get buffer
    filePointer = getStream(filePointer, twinBuffer);
    TokenInfo *tkinfo;

    int capacity = MAX_SIZE;

    token = (char **) malloc (sizeof(char *) * MAX_SIZE);
    value = (char **) malloc (sizeof(char *) * MAX_SIZE);
    lineNo = (int *) malloc (sizeof(int) * MAX_SIZE);

    int ind=0;
    while(filePointer!=NULL) {
        
        tkinfo=getNextToken(twinBuffer, filePointer);
        if(tkinfo->tkId==NULL)
        {
            // printf("Line no. %d  Lexical Error\n",lexerLineNumber+1);
            lineNo[ind] = lexerLineNumber+1;
            token[ind] = (char *) malloc (sizeof(char) * MAXTERM);
            strcpy(token[ind], "ERROR");
            value[ind] = (char *) malloc (sizeof(char) * MAXTERM);
            strcpy(value[ind],tkinfo->value);
            continue;
        }
        else if(strcmp(tkinfo->value,"$")==0){
            // printf("%s \n",tkinfo->value);
            lineNo[ind] = lexerLineNumber +1;
            token[ind] = (char *) malloc (sizeof(char) * MAXTERM);
            strcpy(token[ind],tkinfo->tkId);
            value[ind] = (char *) malloc (sizeof(char) * MAXTERM);
            strcpy(value[ind],tkinfo->value);
            break;
        }
        else
        {   
            if( (strlen(tkinfo->value) <= 20) | (strcmp(tkinfo->tkId,"TK_FUNID")==0 & strlen(tkinfo->value) <= 30))
            {
                if(strcmp(getValue(myMap,tkinfo->value),"KEY NOT FOUND" )==0){
                    insertIntoHash(myMap,tkinfo->value,tkinfo->tkId);
                
                }    
            
                // printf("Line No. %d Lexeme %s  Token %s \n", lexerLineNumber+1,tkinfo->value, getValue(myMap,tkinfo->value));
                lineNo[ind]=lexerLineNumber+1;
                token[ind] = (char *) malloc (sizeof(char) * MAXTERM);
                strcpy(token[ind], getValue(myMap,tkinfo->value));
                value[ind] = (char *) malloc (sizeof(char) * MAXTERM);
                strcpy(value[ind],tkinfo->value);
                
            }
            else {
                lineNo[ind]=lexerLineNumber+1;
                token[ind] = (char *) malloc (sizeof(char) * MAXTERM);
                strcpy(token[ind], "ERROR");
                value[ind] = (char *) malloc (sizeof(char) * MAXTERM);
                strcpy(value[ind],"Identifier is too long");
                // printf("Line No. %d Error:  Identifier is too long\n", lexerLineNumber+1);
            }
        }
        ind++;
        if(ind==capacity){
            capacity = capacity * 2;
            token = (char **) realloc(token, sizeof(char *) * capacity);
            value = (char **) realloc(value, sizeof(char *) * capacity);
            lineNo = (int *) realloc(lineNo, sizeof(int) * capacity);
        }
    }

    parseInputSourceCode();
    //printtoken();
    //printf("Both lexical and syntax analysis modules implemented\n");

    destroyHashMap(myMap);
    fclose(filePointer);
    return;
}



int main() {
    runLexerAndParser();
    
}