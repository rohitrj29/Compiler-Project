#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>


//fill terminals from finalGrammar.txt
char *terminals[53] = {"eps", "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM", "TK_FUNID", "TK_RECORDID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON", "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL", "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE"};
//TODO: automate terminals population


int getTerminalIndex(char *terminal)
{
    for(int i = 0; i < 53; i++)
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
    if(element[0] >= 'A' && element[0] <= 'Z')
    {
        return true;
    }
    return false;
}

int ffsize = sizeof(firstFollow) / sizeof(FirstFollow);
int parseTable[ffsize][53];

bool hasepsilon(int ntindex)
{
    for(int i = 0; i < firstFollow[ntindex].noOfFirst; i++)
    {
        if(strcmp(firstFollow[ntindex].firstSet[i], "eps") == 0)
        {
            return true;
        }
    }
    return false;
}

void createParseTable()
{
    memset(parseTable, -1, sizeof(parseTable));
    //TODO: Implement sync

    for(int gindex = 0; gindex<102; gindex++)
    {
        NTLookupEntry entry = NTLookup[gindex];
        int ntindex = entry.ffIndex;
        FirstFollow ff = firstFollow[ntindex];
        GrammarRule rule = grammarRule[gindex];

        for(int rhsIndex = 0; rhsIndex < rule.noOfElements; rhsIndex++)
        {
            char element[MAXTERM];
            strcpy(element, rule.rightElements[rhsIndex]);
            if(strcmp(element, "eps") == 0)
            {
                for(int findex = 0; findex < ff.noOfFollow; findex++)
                {
                    char followElement[MAXTERM];
                    strcpy(followElement, ff.followSet[findex]);
                    int followIndex = getTerminalIndex(followElement);
                    parseTable[ntindex][followIndex] = gindex;
                }
                // TODO: $ case
                break;
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

                if(hasepsilon(nonTerminalIndex))
                {
                    for(int findex = 0; findex < ff.noOfFollow; findex++)
                    {
                        char followElement[MAXTERM];
                        strcpy(followElement, ff.followSet[findex]);
                        int followIndex = getTerminalIndex(followElement);
                        parseTable[ntindex][followIndex] = gindex;
                    }

                    // todo : handle dollar in follow case
                }

            }
        }

    }
}
