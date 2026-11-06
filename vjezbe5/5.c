/*
Napisati program koji iz datoteke čita postfiks izraz i zatim korištenjem stoga računa
rezultat. Stog je potrebno realizirati preko vezane liste.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS
#define ALLOC_FAILED_MSG "Alociranje memorije nije uspjelo"
#define ALLOC_FAILED_CODE -1
#define FOPEN_FAILED_MSG "Otvaranje datoteke nije uspjelo"
#define FOPEN_FAILED_CODE -2
#define PARSE_FAILED_MSG "Prilozena datoteka sadrzi neispravan sadrzaj, provjerite sintaksu"
#define PARSE_FAILED_CODE -3
#define ELEMENT_ERROR_MSG "Nedostatak brojeva za operaciju, provjerite operacije"
#define ELEMENT_ERROR_CODE -4
#define FILENAME "postfix.txt"
#define MAX_DIGITS 10
#define STRING_MAX 512
#define DEBUG 1

typedef struct _Stack* Position;

typedef struct _Stack {
    int num;
    Position next;
} Stack;

int readVerifyFile(char*, char*);
int processBuffer(char*, Position);
int procOp(char, Position);
int push(int, Position);
int pop(Position, int*);
int fetchBegin(Position, int*);
int insertBegin(Position, Position);
int removeBegin(Position);
int removeAll(Position);

int main()
{
    int res;
    // inicijalizacija jer koristim strcat
    char buffer[STRING_MAX] = { 0 };
    Stack head = { 0 };

    res = readVerifyFile(FILENAME, buffer);
    if (res) return res;
    printf("Procitano: %s\n", buffer);

    // LOOP (napravi f-ju):
    // a) pushaj do operacije
    // b) obavi operaciju (oslobodi sve)
    //  - izracunaj novi el
    //  - oslobodi p i q (pop)
    //  - push rezultat...
    // c) krivi element! error
    //  - oslobodi stack
    //  - javi grešku
    //  - izađi iz programa
    // opet isto...

    res = processBuffer(buffer, &head);
    if (res)
    {
        removeAll(&head);
        return res;
    }

    printf("Rezultat: %d\n", head.next->num);

    return 0;
}

int readVerifyFile(char* fileName, char* buffer)
{
    // prolazimo kroz datoteku i kopiramo ju u buffer
    // znak po znak provjeravajući postoji li ikakav invalid znak
    char temp[MAX_DIGITS];

    // ovo je za strtol da provjerimo točnost
    // ne sprema se ništa novo nego doslovno pokazuje na temp
    char* endptr;

    FILE* fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror(FOPEN_FAILED_MSG);
        fclose(fp);
        return FOPEN_FAILED_CODE;
    }

    while (fscanf(fp, "%s", temp) == 1)
    {
        // koristimo strtol umjesto atoi jer je rigorozniji
        // npr. atoi(temp) će priznati "4," dok ovo neće
        strtol(temp, &endptr, 10);

        // trebamo provjeriti samo prvi znak
        // zbog toga nam ne treba strcmp
        if (!(*endptr == '\0'
            || *temp == '+' || *temp == '-'
            || *temp == '*' || *temp == '/'))
        {
            fputs(PARSE_FAILED_MSG, stderr);
            printf("\nProblematican string: \"%s\"\n", temp);
            return PARSE_FAILED_CODE;
        }
        strcat(buffer, temp);
        strcat(buffer, " ");
    }

    fclose(fp);

    return 0;
}

int processBuffer(char* buffer, Position head)
{
    int ret = 0;
    char* token = strtok(buffer, " ");

    while (token != NULL)
    {
        #if DEBUG
            printf("%s\n", token);
        #endif

        if (*token == '+' || *token == '-'
            || *token == '*' || *token == '/')
            ret = procOp(*token, head);
        else // broj
            ret = push(atoi(token), head);
        if (ret) return ret;
        token = strtok(NULL, " ");
    }

    return 0;
}

int procOp(char op, Position head)
{
    // eto, može dijeljenje, zašto ne
    int i, elem[2], res;

    // trebamo ići ovakvim redoslijedom zbog dijeljenja
    for (i = 1; i >= 0; i--)
    {
        if (pop(head, &elem[i]) == 0)
        {
            fputs(ELEMENT_ERROR_MSG, stderr);
            return ELEMENT_ERROR_CODE;
        }
    }
    
    #if DEBUG
        printf("Radim... %d %c %d = ", elem[0], op, elem[1]);
    #endif

    switch (op)
    {
        case '+':
            res = elem[0] + elem[1];
            break;
        case '-':
            res = elem[0] - elem[1];
            break;
        case '*':
            res = elem[0] * elem[1];
            break;
        case '/':
            res = elem[0] / elem[1];
            break;
    }

    #if DEBUG
        printf("%d\n", res);
    #endif

    push(res, head);

    return 0;
}

int push(int newNum, Position head)
{
    Position newItem = (Position)malloc(sizeof(Stack));
    newItem->num = newNum;
    insertBegin(newItem, head); // ovo postavlja newItem->next

    return 0;
}

int pop(Position head, int* result)
{
    // naš pop uklanja vrijednost dok ju vraća (kroz pointer)
    int ret = fetchBegin(head, result);
    removeBegin(head);
    return ret;
}

int fetchBegin(Position head, int* result)
{
    // vraća status, rezultat ide kroz pointer
    // 1 = dobiven jedan element
    // 0 = dobiveno 0 elemenata
    if (head->next != NULL)
    {
        *result = head->next->num;
        return 1;
    }
    return 0;
}

int insertBegin(Position newP, Position head)
{
    newP->next = head->next;
    head->next = newP;

    return 0;
}

int removeBegin(Position head)
{
    // isti return kao i kod fetch
    Position temp;
    if (head->next != NULL)
    {
        temp = head->next;
        head->next = head->next->next;
        free(temp);
        return 1;
    }
    return 0;
}

int removeAll(Position p)
{
    int i;
    Position temp;

    for (i = 0; p->next != NULL; i++)
    {
        temp = p->next;
        if (i == 0)
            p->next = NULL;
        else
            free(p);
        p = temp;
    }

    return 0;
}
