#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

#define MAXELE 20
#define LINESIZE 1000
#define MAXTERM 50
#define MAXNONTERM 150
#define MAXRULES 120



int lineNumber=0;


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


//struct to store first and follow sets
typedef struct {
    char nonTerminal[MAXTERM];
    char firstSet[MAXELE][MAXTERM];
    char followSet[MAXELE][MAXTERM];
    bool isEpsilon;
    
    bool visited;
} FirstFollow;


int ffsize = 0;
int noOfNonTerminals = 0;
NTLookupEntry NTLookup[MAXNONTERM];
GrammarRule grammarRule[120]  ;
FirstFollow firstFollow[MAXNONTERM];

void intialiseFFandLookup(){
    int entry = 0;
    char prev[MAXTERM]="";
    for(int i=0; i<lineNumber; i++)
    {
        char nonterm[MAXTERM];
        strcpy(nonterm,grammarRule[i].leftElement);
        
        if(strcmp(prev, curr)!=0){
            strcpy(NTLookup[entry].nonTerminal, nonterm);
            NTLookup[entry].grammarIndex = i;
            NTLookup[entry].ffIndex = entry;
            strcpy(firstFollow[entry].nonTerminal,nonterm);
            strcpy(prev,curr);
            entry++;
            ffsize++;
        }
        firstFollow[entry-1].visited=false;
        if(strcmp(grammarRule[i].rightElements[0],"eps")==0)
            firstFollow[entry-1].isEpsilon=true;
        else 
            firstFollow[entry-1].isEpsilon=false;
        
    }
}

// find if element's isepsilon is true in NTLookup
NTLookupEntry* searchInLookup(char element[]){
    int i=0;
    while(NTLookup[i].nonTerminal!=NULL){
        if(strcmp(NTLookup[i].nonTerminal,element)==0){
            return NTLookup[i];
        }
        i++;
    }
    return NULL;
}


void findFirst(int ffind, int grammarInd){

    for(int i=grammarInd; i<102;i++) {
        if(strcmp(grammarRule[i].leftElement, NTLookup[ffind].nonTerminal)!=0){
            break;
        }
        
        int j=grammarRule[i].noOfElements;
        for(int k=0;k<j;k++)
        {
            //if the element is a terminal or epsilon
            if(grammarRule[i].rightElements[k][0]>='A' && grammarRule[i].rightElements[k][0]<='Z' || strcmp(grammarRule[i].rightElements[k], "eps") == 0){
                strcpy(firstFollow[ffind].firstSet[],grammarRule[i].rightElements[k])
                break;
            }
            else{
                NTLookup *check= searchInLU(grammarRule[i].rightElements[k]);
                if(firstFollow[check->ffIndex].isEpsilon) {
                    
                    if(firstFollow[check->ffIndex].visited==true){
                        //populate
                    }
                    else{
                    findFirst(check->ffIndex,check->grammarIndex);
                    //populate first set
                    }
                    if(k==j-1){
                        //populate epsilon in first set
                        break;
                    }
                    
                    
                }
                else{
                    if(firstFollow[check->ffIndex].visited==true){
                        //populate
                    }
                    else{
                    findFirst(check->ffIndex,check->grammarIndex);
                    //populate first set
                    }
                    break;
                }
                
            }
            
        }
    
    }

    firstFollow[ffind].visited=true;
    return;

}

//Follow
/*  1.  For the start symbol S, place $ in Follow(S).
    2.  For any production rule A → αB,
        Follow(B) = Follow(A)
    3.  For any production rule A → αBβ,
        If ∈ ∉ First(β), then Follow(B) = First(β)
        If ∈ ∈ First(β), then Follow(B) = { First(β) – ∈ } ∪ Follow(A)
*/

//handle A -> aA (follow(A) = follow(A))
//



void populateFirst(){
    for(int i=0;i<ffsize;i++){
        findFirst(i,NTLookup[i].grammarIndex);
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
        printf("%s %d %d \n", NTLookup[i].nonTerminal,NTLookup[i].ffIndex,NTLookup[i].grammarIndex);
    }
    
    fclose(fp);
    return 0;
}