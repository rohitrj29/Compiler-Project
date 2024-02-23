#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

#define MAXELE 20
#define LINESIZE 1000
#define MAXTERM 50
#define MAXNONTERM 150


int lineNumber=0;

int noOfNonTerminals = 0; 
//struct to store grammar rules

typedef struct NTLookupEntry{
    char nonTerminal[MAXTERM];
    int ffIndex;
    int grammarIndex;
} NTLookupEntry;


typedef struct {
    int noOfElements;
    char leftElement[MAXTERM];
    char rightElements[MAXELE][MAXTERM];  
    int ruleNo;
} GrammarRule;

int ffsize = 0;
//struct to store first and follow sets
typedef struct {
    char nonTerminal[MAXTERM];
    char firstSet[MAXELE][MAXTERM];
    char followSet[MAXELE][MAXTERM];
    bool isEpsilon;
} FirstFollow;

NTLookupEntry NTLookup[MAXNONTERM];
GrammarRule grammarRule[120];
FirstFollow firstFollow[MAXNONTERM];

void intialiseFFandLookup(){
    int entry = 0;
    char prev[MAXELE];
    for(int i=0; i<102; i++)
    {
        char nonterm[MAXELE];
        strcpy(nonterm,grammarRule[i].leftElement);
        char curr[MAXELE];
        strcpy(curr,nonterm);
        
        if(strcmp(prev, curr)!=0){
            strcpy(NTLookup[entry].nonTerminal, nonterm);
            NTLookup[entry].grammarIndex = i;
            NTLookup[entry].ffIndex = entry;
            strcpy(firstFollow[entry].nonTerminal,nonterm);
            strcpy(prev,curr);
            entry++;
            ++ffsize;
        }

        if(strcmp(grammarRule[i].rightElements[0],"eps")==0)
            firstFollow[entry].isEpsilon=true;
        else 
        if(firstFollow[entry].isEpsilon != true)
            firstFollow[entry].isEpsilon=false;
        
    }
}

int main() {
    FILE *fp;

    fp = fopen("./FinalGrammar.txt", "r");

    char line[LINESIZE];


    //array to store grammar rules  
    
    int fflineNo=0;
    
    
    
    while (fgets(line, LINESIZE, fp)) {        
        char *token = strtok(line, " ");   
        int i = 0;

        while (token != NULL) {
            // Search if Non-Terminal Entry already created & If not then create
            
            if (i == 0) {
                strcpy(grammarRule[lineNumber].leftElement, token); // Copy token to leftElement
            } else {
                strcpy(grammarRule[lineNumber].rightElements[i - 1], token); // Copy token to rightElements[i-1]
            }
            i++;
            token = strtok(NULL, " ");
        }

        grammarRule[lineNumber].noOfElements = i - 1;
        grammarRule[lineNumber].ruleNo = lineNumber;
        lineNumber++;    
    }
    
    //print the grammar rules
    for(int i=0;i<lineNumber;i++){
        printf("%s ->   ",grammarRule[i].leftElement);
        for(int j=0;j<grammarRule[i].noOfElements;j++){
            printf("%s ",grammarRule[i].rightElements[j]);
        }
        printf("\n");
    }
    intialiseFFandLookup();
    for(int i=0;i<ffsize;i++){
        if(firstFollow[i].nonTerminal!=NULL)
        printf("%s\n",firstFollow[i].nonTerminal);
    }

    fclose(fp);
    return 0;
}


