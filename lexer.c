//#include "structures.h"
//#include "lexer.h"
#define bufferSize 4
#define maxVarSize 60
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
                return returnToken(tokenInfo, "error",NULL,lineNumber);
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
                    char *tk_id = "TK_FUN_ID";
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
                    char *tk_id = "TK_FUN_ID";
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


int main() {
    // Initialize File Pointer
    FILE* filePointer;
    filePointer = fopen("C:\\Users\\91620\\Desktop\\Compiler\\t2.txt", "r");

    if (filePointer == NULL) {
        printf("Failed to open file!\n");
        return 1;
    }

    // Initialize Twin Buffer
    TwinBuffer *twinBuffer = (TwinBuffer *) malloc (sizeof(TwinBuffer));
    twinBuffer  ->  forward = bufferSize;
    twinBuffer  ->  lexBegin = 0;
    twinBuffer  ->  inUseBuffer = 2;

    // Call function to get buffer
    filePointer=getStream(filePointer, twinBuffer);
    TokenInfo *tkinfo;
    while(filePointer!=NULL){
        tkinfo=getNextToken(twinBuffer,filePointer);
        if(tkinfo->tkId==NULL)
        printf("%s\n","error");
        else
        {
            printf("%s ",tkinfo->tkId);
            printf("%s \n",tkinfo->value);
        }
        if(tkinfo->value=="$")
        break;
    }
    fclose(filePointer);
    return 0;
}