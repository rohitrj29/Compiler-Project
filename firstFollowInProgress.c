#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXELE 20
#define LINESIZE 1000
#define MAXTERM 50
#define MAXNONTERM 150
#define MAXRULES 120

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
} FirstFollow;

// Arrays to store data
GrammarRule grammarRule[MAXRULES];
NTLookupEntry NTLookup[MAXNONTERM];
FirstFollow firstFollow[MAXNONTERM];

int ffsize = 0;
int noOfNonTerminals = 0; // Added for storing the total number of non-terminals

bool present(char element[MAXTERM], char array[MAXELE][MAXTERM], int noOfEleInArray) {
    for (int i = 0; i < noOfEleInArray; i++) {
        if(strcmp(element, array[i]) == 0) return true;
    }
    
    return false;
}

void populateFirstFromAnother(int ffIndex1, int ffIndex2) {
    int initialElementsInFirst = firstFollow[ffIndex1].noOfFirst, intialElementsInFF2 = firstFollow[ffIndex2].noOfFirst;
    
    for (int i = 0; i < intialElementsInFF2; i++) {
        char element[MAXTERM];
        strcpy(element, firstFollow[ffIndex2].firstSet[i]);
        
        if (present(element, firstFollow[ffIndex1].firstSet, initialElementsInFirst)) continue;
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


// find if element's isepsilon is true in NTLookup
// NTLookupEntry* searchInLookup(char element[]){
//     int i=0;
//     while(NTLookup[i].nonTerminal!=NULL){
//         if(strcmp(NTLookup[i].nonTerminal,element)==0){
//             return NTLookup[i];
//         }
//         i++;
//     }
//     return NULL;
// }


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
            if(grammarRule[i].rightElements[k][0]>='A' && grammarRule[i].rightElements[k][0]<='Z'  || strcmp(grammarRule[i].rightElements[k], "eps") == 0){
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
        firstFollow[i].noOfFirst=0;
        findFirst(i, NTLookup[i].grammarIndex);
    }
}

void findFollow(int ffIndex){
    for(int i=0;i<lineNumber;i++){
        int found=0;
        int noteps=0; //unused variable
        for(int j=0;j<grammarRule[i].noOfElements;j++){
            if(strcmp(grammarRule[i].rightElements[j],firstFollow[ffIndex].nonTerminal)==0){
                found=1;
            }
            if(found==1){
                
                //last element 
                if(strcmp(grammarRule[i].rightElements[j],firstFollow[ffIndex].nonTerminal)==0 && j==grammarRule[i].noOfElements-1){
                    
                    NTLookupEntry check= getNTLookup(grammarRule[i].leftElement);
                    if(strcmp(grammarRule[i].leftElement,grammarRule[i].rightElements[grammarRule[i].noOfElements-1])==0){
                        break;
                    }
                    findFollow(check.ffIndex);
                    //populate follow set with follow set of left element
                    populateFollowFromFollow(ffIndex, check.ffIndex);
                    found=0;
                    

                }
                // not the last element
                else{
                    if(grammarRule[i].rightElements[j+1][0]>='A' && grammarRule[i].rightElements[j+1][0]<='Z'){
                        //populate follow set with rightElement[j+1] as it is a terminal
                        populateFollowFromElement(ffIndex, grammarRule[i].rightElements[j+1]);
                        found=0;
                        break;
                    }
                    else{
                        NTLookupEntry check= getNTLookup(grammarRule[i].rightElements[j+1]);
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
                                        findFollow(check.ffIndex);
                                        //populate follow set with follow set of left element
                                        populateFollowFromFollow(ffIndex, check.ffIndex);
                                        found=0;
                                    
                                    }
                                    else{
                                    
                                         //populate the follow set with the first of rightElement[j+1] -eps we are currently at
                                        populateFollowFromFirst(ffIndex, check.ffIndex);
                                        findFollow(check.ffIndex);
                                     
                                    
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
}

void populateFollow(){
    firstFollow[0].noOfFollow=1;
    strcpy(firstFollow[0].followSet[0],"$");
    for(int i=1;i<noOfNonTerminals;i++){
        firstFollow[i].noOfFollow=0;
        findFollow(i);
    }
}


int main()
{
    FILE *fp;
    fp = fopen("./newGrammar.txt", "r");

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

    intialiseFFandLookup();
    //populateFirst();
    // populateFollow();

    for (int i = 0; i < noOfNonTerminals; i++)
    {
        printf("%s %d %d %d\n", NTLookup[i].nonTerminal, NTLookup[i].ffIndex, NTLookup[i].grammarIndex,firstFollow[i].isEpsilon);
    }

    for(int i=0;i<noOfNonTerminals;i++){
        printf("First set of %s \n ",firstFollow[i].nonTerminal);
        for(int j=0;j<firstFollow[i].noOfFirst;j++){
            printf("%s ",firstFollow[i].firstSet[j]);
        }
        printf("\n");
        printf("Follow set of %s \n ",firstFollow[i].nonTerminal);
        for(int j=0;j<firstFollow[i].noOfFirst;j++){
            printf("%s ",firstFollow[i].followSet[j]);
        }
        printf("\n");

    }

    return 0;
}
