/*
2. Definirati strukturu osoba (ime, prezime, godina rođenja) i napisati program koji:
A. dinamički dodaje novi element na početak liste,
B. ispisuje listu,
C. dinamički dodaje novi element na kraj liste,
D. pronalazi element u listi (po prezimenu),
E. briše određeni element iz liste,
U zadatku se ne smiju koristiti globalne varijable.
*/

#define _CRT_SECURE_NO_WARNINGS
#define ALLOC_FAILED_MSG "Alociranje memorije nije uspjelo"
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

int insertBegin(char*, char*, int, Position);
int insertAfter(Position, Position);
int insertEnd(char*, char*, int, Position);
int printList(Position);
Position findLastName(char*, Position);
int deletePerson(char*, Position);

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
    insertEnd("Masa", "Matic", 2006, &head);
    printList(&head);
    findLastName("Ivic", &head);
    deletePerson("Ivic", &head);
    printf("=============================\n");
    printList(&head);

    return 0;
}

int insertBegin(char* firstName, char* lastName, int year, Position head)
{
    Position newPerson = (Position)malloc(sizeof(Person));

    if (newPerson == NULL)
    {
        perror(ALLOC_FAILED_MSG);
        free(newPerson);
        exit(errno);
    }

    strcpy(newPerson->firstName, firstName);
    strcpy(newPerson->lastName, lastName);
    newPerson->year = year;

    insertAfter(head, newPerson);

    return 0;
}

int insertEnd(char* firstName, char* lastName, int year, Position head)
{
    Position p = head->next;

    while (p->next != NULL)
        p = p->next;

    Position newPerson = (Position)calloc(1, sizeof(*newPerson));

    if (newPerson == NULL)
    {
        perror(ALLOC_FAILED_MSG);
        free(newPerson);
        exit(errno);
    }

    strcpy(newPerson->firstName, firstName);
    strcpy(newPerson->lastName, lastName);
    newPerson->year = year;

    insertAfter(p, newPerson);

    return 0;
}

int insertAfter(Position p, Position newP)
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
