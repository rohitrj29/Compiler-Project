#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXELE 20
#define LINESIZE 1000
#define MAXTERM 50
#define MAXNONTERM 150
#define MAXRULES 120
#define TOTALRULES 102
#define PARSECOL 57

int lineNumber = 0;

// Struct to store grammar rules
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

// Arrays to store data
GrammarRule grammarRule[MAXRULES];
NTLookupEntry NTLookup[MAXNONTERM];
FirstFollow firstFollow[MAXNONTERM];
char terminals[PARSECOL][MAXTERM];
// char *terminals[54] = {"$", "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM", "TK_FUNID", "TK_RECORDID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON", "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL", "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE"};
// int ffsize = 0;
int noOfNonTerminals = 0; // Added for storing the total number of non-terminals

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

    for (int i = 0; i < lineNumber; i++)
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

    for(int i=grammarInd; i<102;i++) {
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

void populateFirst(){
    for(int i=0;i<noOfNonTerminals;i++){
        if(firstFollow[i].visited==true){
            continue;
        }
        firstFollow[i].noOfFirst=0;
        findFirst(i, NTLookup[i].grammarIndex);
    }
}

void findFollow(int ffIndex){
    for(int i=0;i<lineNumber;i++){
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

                                if(firstFollow[check.ffIndex].followCalc==true){
                                    //popoulate follow set with first set of right element
                                    populateFollowFromFirst(ffIndex, check.ffIndex);
                                    //populate follow set with follow set of right element
                                    populateFollowFromFollow(ffIndex, check.ffIndex);
                                    found=0;
                                }
                                else
                                {
                                
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
                                }
                                
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

void populateFollow(){
    firstFollow[0].noOfFollow=1;
    strcpy(firstFollow[0].followSet[0],"$");
    for(int i=1;i<noOfNonTerminals;i++){
        firstFollow[i].noOfFollow=0;
        findFollow(i);
    }
}

/*
parser table population
using first and follow set of non terminals
*/


//fill terminals from finalGrammar.txt

//TODO: automate terminals population

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
    if((element[0] >= 'A' && element[0] <= 'Z')|| element[0] == '$')
    {
        return true;
    }
    return false;
}

// int ffsize = sizeof(firstFollow) / sizeof(FirstFollow);
int parseTable[55][PARSECOL];

// bool hasepsilon(int ntindex)
// {
//     for(int i = 0; i < firstFollow[ntindex].noOfFirst; i++)
//     {
//         if(strcmp(firstFollow[ntindex].firstSet[i], "eps") == 0)
//         {
//             return true;
//         }
//     }
//     return false;
// }

void createParseTable()
{   
    memset(parseTable, -1, sizeof(parseTable));
    //TODO: Implement sync

    for(int gindex = 0; gindex<102; gindex++)
    {
        NTLookupEntry entry = getNTLookup(grammarRule[gindex].leftElement);
        int ntindex = entry.ffIndex;
        FirstFollow ff = firstFollow[ntindex];
        GrammarRule rule = grammarRule[gindex];

        //only right[0] needs to be checked
        // for(int rhsIndex = 0; rhsIndex < rule.noOfElements; rhsIndex++)
        // {
            char element[MAXTERM];
            strcpy(element, rule.rightElements[0]);
            if(strcmp(element, "eps") == 0)
            {
                for(int findex = 0; findex < ff.noOfFollow; findex++)
                {
                    char followElement[MAXTERM];
                    strcpy(followElement, ff.followSet[findex]);
                    int followIndex = getTerminalIndex(followElement);
                    parseTable[ntindex][followIndex] = gindex;
                }
                
            }
            else if(isTerminal(element))
            {
                int terminalIndex = getTerminalIndex(element);
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
                    int firstIndex = getTerminalIndex(firstElement);
                    parseTable[ntindex][firstIndex] = gindex;
                }


            }

    }
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
            
        
    }
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
    for (int i = 0; i < lineNumber; i++)
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

int main()
{
    FILE *fp;
    fp = fopen("./FinalGrammar.txt", "r");

    if (fp == NULL)
    {
        printf("File not found\n");
        return 1; // Return with error status
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
                strcpy(grammarRule[lineNumber].leftElement, token);
            }
            else
            {
                strcpy(grammarRule[lineNumber].rightElements[i - 1], token);
            }
            i++;
            token = strtok(NULL, " ");
            
        }

        grammarRule[lineNumber].noOfElements = i - 1;
        grammarRule[lineNumber].ruleNo = lineNumber;
        lineNumber++;
    }

    fclose(fp);

    

    intialiseFFandLookup();
    
    populateTerminals();
    populateFirst();
    populateFollow();
    createParseTable();
    fillSyncInParseTable();

    //printParseTable();
    printFirstFollow();
    

    return 0;
}
