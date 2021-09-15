#include<string.h>
#include<stdio.h>
#include<stdlib.h>

int main() {
    char *s=NULL;
    size_t len=0;
    getline(&s,&len,stdin); 
    char *token;
    char **tokens = NULL;
    token = strtok(s, " ");
    int numberOfTokens = 0;
    while (token != NULL) {
        tokens = (char**)realloc(tokens, sizeof(char*) * (numberOfTokens + 1));
        tokens[numberOfTokens] = token;
        numberOfTokens++;
        token = strtok(NULL, " ");
    }

    for (int i = 0; i < numberOfTokens; i++) {
        printf("%s\n", tokens[i]);
    }

    size_t len2 = (&tokens)[1] - tokens;
    printf("%zu\n", len2);

}
