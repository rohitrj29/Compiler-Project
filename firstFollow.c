#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

#define MAXELE 20
#define LINESIZE 1000
#define MAXTERM 50
#define MAXNONTERM 150

//struct to store grammar rules
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
} FirstFollow;

//function to search for a non-terminal in the grammar rules
int searchNonTerminal(char *str, FirstFollow *firstFollow, int noOfNT) {
    for (int i = 0; i < noOfNT; i++) {
        if (strcmp(firstFollow[i].nonTerminal, str) == 0) {
            return i;
        }
    }
    return -1;
}


// if terminal (capital first) found and nonterminal is noteps then store and return
// if first rhs is not eps then call first of first eps, else call for both that and first of the next rhs

int searchGrammarIndex(GrammarRule grammarRule, char* nonTerm){
    int i=0;
    while(grammarRule[i].leftELement != NULL){
        if(strcmp(grammarRule[i].leftElement,nonTerm)==0){
            return i;
        }
        i++;
    }
    return -1;
}

void findFirst(FirstFollow *firstFollow, GrammarRule *grammarRule,int noOfNonTerminals)
{
    for(int i=0;i<noOfNonTerminals;i++){
        char nonTerm = firstFollow[i].nonTerminal;
        int ind= searchGrammarIndex(grammarRule,nonTerm);

    }
    
    if(rule[0][0]>='A' && rule[0][0]<='Z') firstFollow[index].
}

int main() {
    FILE *fp;

    fp = fopen("grammar.txt", "r");

    char line[LINESIZE];
    int lineNumber=0;

    //array to store grammar rules  
    GrammarRule grammarRule[100];
    FirstFollow firstFollow[MAXNONTERM];

    int noOfNonTerminals = 0; 
    
    while (fgets(line, LINESIZE, fp)) {        
        char *token = strtok(line, " ");   
        int i = 0;

        while (token != NULL) {
            // Search if Non-Terminal Entry already created & If not then create
            int avail = searchNonTerminal(token, firstFollow, noOfNonTerminals);

            if (avail == -1 && token[0] >='a' && token[0] <= 'z') {
                strcpy(firstFollow[noOfNonTerminals].nonTerminal, token);
                
                firstFollow[noOfNonTerminals].isEpsilon = false; // Explicitly set isEpsilon here
                noOfNonTerminals++;
            }
            
            if (i == 0) {
                strcpy(grammarRule[lineNumber].leftElement, token); // Copy token to leftElement
            } else {
                strcpy(grammarRule[lineNumber].rightElements[i - 1], token); // Copy token to rightElements[i-1]
            }

            if (strcpy("eps", token) == 0) {
                int ind = searchNonTerminal(grammarRule[lineNumber].leftElement, firstFollow, noOfNonTerminals);
                firstFollow[ind].isEpsilon = true;
            }
            i++;
            token = strtok(NULL, " ");
        }

        grammarRule[lineNumber].noOfElements = i - 1;
        grammarRule[lineNumber].ruleNo = lineNumber;
        lineNumber++;    
    }
    
    for(int i=0;i<noOfNonTerminals;i++){
        printf("%s\n",firstFollow[i].nonTerminal);
    }

    fclose(fp);
    return 0;
}

