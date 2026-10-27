/*
2. Definirati strukturu osoba (ime, prezime, godina rođenja) i napisati program koji:
A. dinamički dodaje novi element na početak liste,
B. ispisuje listu,
C. dinamički dodaje novi element na kraj liste,
D. pronalazi element u listi (po prezimenu),
E. briše određeni element iz liste,
U zadatku se ne smiju koristiti globalne varijable.

3. Prethodnom zadatku dodati funkcije:
A. dinamički dodaje novi element iza određenog elementa,
B. dinamički dodaje novi element ispred određenog elementa,
C. sortira listu po prezimenima osoba,
D. upisuje listu u datoteku,
E. čita listu iz datoteke.
*/

#define _CRT_SECURE_NO_WARNINGS
#define ALLOC_FAILED_MSG "Alociranje memorije nije uspjelo"
#define FOPEN_FAILED_MSG "Otvaranje datoteke nije uspjelo"
#define STRING_MAX 30

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Person* Position;

typedef struct _Person {
    char firstName[STRING_MAX];
    char lastName[STRING_MAX];
    int year;
    Position next;
} Person;

// zadatak 2
int insertBegin(char*, char*, int, Position);
int printList(Position);
int insertEnd(char*, char*, int, Position);
Position findLastName(char*, Position);
int deletePerson(char*, Position);

// zadatak 3
int insertAfter(char*, char*, char*, int, Position);
int insertBefore(char*, char*, char*, int, Position);
int sortByLastName(Position);
int writeToFile(char*, Position);
int readFromFile(char*, Position);

// ostalo, utility
Position allocNewPerson(char*, char*, int);
int insertAfterPos(Position, Position);
int freeAll(Position);

// zapravo sam kod
int main()
{
    Person head = {
        .firstName = {0},
        .lastName = {0},
        .year = 0,
        .next = NULL
    };

    insertBegin("Ivo", "Ivic", 2006, &head);
    insertBegin("Mate", "Matic", 2007, &head);
    insertEnd("Masa", "Mihic", 2006, &head);
    printList(&head);

    printf("=============================\n\n");
    insertAfter("Ivic", "Jana", "Janic", 2005, &head);
    insertBefore("Ivic", "Marko", "Markic", 2005, &head);
    deletePerson("Ivic", &head);
    printList(&head);

    printf("=============================\n\n");
    sortByLastName(&head);
    printList(&head);

    printf("=============================\n\n");
    writeToFile("people.txt", &head);
    readFromFile("people.txt", &head);
    printList(&head);

    freeAll(&head);
    // sam head ne možemo brisati jer je na stacku, a ne heapu

    return 0;
}

Position allocNewPerson(char* firstName, char* lastName, int year)
{
    Position newPerson = (Position)malloc(sizeof(Person));

    if (newPerson == NULL)
    {
        perror(ALLOC_FAILED_MSG);
        free(newPerson);
        return NULL;
    }

    strcpy(newPerson->firstName, firstName);
    strcpy(newPerson->lastName, lastName);
    newPerson->year = year;
    newPerson->next = NULL;

    return newPerson;
}

int insertBegin(char* firstName, char* lastName, int year, Position head)
{
    Position newPerson = allocNewPerson(firstName, lastName, year);
    if (newPerson == NULL) return -1;

    insertAfterPos(head, newPerson);

    return 0;
}

int insertEnd(char* firstName, char* lastName, int year, Position p)
{
    Position newPerson = allocNewPerson(firstName, lastName, year);
    if (newPerson == NULL) return -1;

    while (p->next != NULL)
        p = p->next;

    insertAfterPos(p, newPerson);

    return 0;
}

int insertAfter(char* exLastName, char* firstName, char* lastName, int year, Position p)
{
    // ex za existing
    Position newPerson = allocNewPerson(firstName, lastName, year);
    if (newPerson == NULL) return -1;

    Position exPerson = findLastName(exLastName, p);
    if (exPerson == NULL) return 0; // ...promijeniti...?

    insertAfterPos(exPerson, newPerson);
    return 0;
}

int insertBefore(char* exLastName, char* firstName, char* lastName, int year, Position p)
{
    // ex za existing
    Position newPerson = allocNewPerson(firstName, lastName, year);
    if (newPerson == NULL) return -1;

    Position exPersonPrev = NULL;
    for (p = p->next; p != NULL && p->next != NULL; p = p->next)
        if (strcmp(p->next->lastName, exLastName) == 0)
            exPersonPrev = p;
    if (exPersonPrev == NULL) return 0; // ...promijeniti...?

    insertAfterPos(exPersonPrev, newPerson);
    return 0;
}

int insertAfterPos(Position p, Position newP)
{
    newP->next = p->next;
    p->next = newP;

    return 0;
}

int printList(Position head)
{
    Position p = head->next;

    while (p != NULL)
    {
        printf("firstName: %s\nlastName: %s\nyear: %d\n\n",
            p->firstName, p->lastName, p->year);
            p = p->next;
    }
    
    return 0;
}

Position findLastName(char* lastName, Position p)
{
    for (p = p->next; p != NULL; p = p->next)
        if (strcmp(p->lastName, lastName) == 0)
            return p;

    return NULL;
}

int sortByLastName(Position p)
{
    // prema prezentaciji s predavanja - bubble sort
    Position j, prev_j, temp, end;

    end = NULL;
    while (p->next != end)
    {
        prev_j = p;
        j = p->next;

        while (j->next != end)
        {
            if (strcmp(j->lastName, j->next->lastName) > 0)
            {
                temp = j->next;
                prev_j->next = temp;
                j->next = temp->next;
                temp->next = j;
                j = temp;
            }
            prev_j = j;
            j = j->next;
        }
        end = j;
    }

    return 0;
}

int deletePerson(char* lastName, Position p)
{
    Position temp;

    // Ovako nam je prof. Linda pokazala
    while (p->next != NULL && strcmp(p->next->lastName, lastName) != 0)
        p = p->next;
    
    if (p->next == NULL)
        return 0; // valjda nije greška ako nije nađeno ništa

    temp = p->next;
    p->next = temp->next;
    free(temp);

    return 0;
}

int freeAll(Position p)
{
    int i;
    Position temp;

    // automatski preskače ako je lista već prazna
    for (i = 0; p->next != NULL; i++)
    {
        temp = p->next;

        if (i == 0) // head, on mora znati da je lista prazna
            p->next = NULL;
        else // ako nije head, onda je din. aloc. memorija (tj. element)
            free(p);

        p = temp;
    }

    return 0;
}

int writeToFile(char* fileName, Position p)
{
    if (p->next == NULL) return 0; // prazna lista
    p = p->next;

    FILE* fp = fopen(fileName, "w");
    if (fp == NULL)
    {
        perror(FOPEN_FAILED_MSG);
        fclose(fp);
        return -1;
    }

    while (p != NULL)
    {
        fprintf(fp, "%s %s %d\n",
            p->firstName, p->lastName, p->year);
            p = p->next;
    }
    
    fclose(fp);
    return 0;
}

int readFromFile(char* fileName, Position p)
{
    Position tempNewPerson;
    char tempFirstName[STRING_MAX];
    char tempLastName[STRING_MAX];
    int tempYear;

    freeAll(p); // overwriteamo postojeću listu

    FILE* fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        perror(FOPEN_FAILED_MSG);
        fclose(fp);
        return -1;
    }

    while (fscanf(fp, "%s %s %d", tempFirstName, tempLastName, &tempYear) == 3)
        insertEnd(tempFirstName, tempLastName, tempYear, p);
    
    fclose(fp);
    return 0;
}
