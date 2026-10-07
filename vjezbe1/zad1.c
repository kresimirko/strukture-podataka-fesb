/*
1. Napisati program koji prvo pročita koliko redaka ima datoteka, tj. koliko ima studenata
zapisanih u datoteci. Nakon toga potrebno je dinamički alocirati prostor za niz struktura
studenata (ime, prezime, bodovi) i učitati iz datoteke sve zapise. Na ekran ispisati ime,
prezime, apsolutni i relativni broj bodova.
Napomena: Svaki redak datoteke sadrži ime i prezime studenta, te broj bodova na kolokviju.
relatvan_br_bodova = br_bodova/max_br_bodova*100
*/

#define _CRT_SECURE_NO_WARNINGS
#define FILE_OPEN_FAILED 1
#define ALLOC_FAILED 2
#define STRING_MAX 50
#define MAX_POINTS 60

#include <stdio.h>
#include <stdlib.h>

struct _Student;
typedef struct _Student
{
    /* mogao bih koristiti manji broj od 50 za imena i prezimena,
    ali neka svaki string bude iste duljine za konzistentnost */
    char firstName[STRING_MAX];
    char lastName[STRING_MAX];
    int points;
} Student;

int main()
{
    int i, studentCount = 0; // i ćemo kasnije inicijalizirati
    char buffer[STRING_MAX];
    Student* students = NULL;

    FILE* studentsFile = fopen("studenti.txt", "r");
    if (studentsFile == NULL)
    {
        printf("Greska - datoteka se nije mogla otvoriti\n");
        return FILE_OPEN_FAILED;
    }

    while (!feof(studentsFile))
    {
        fgets(buffer, sizeof(buffer), studentsFile);
        studentCount++;
    }
    rewind(studentsFile); // jer je fgets došao do kraja

    students = (Student*)malloc(sizeof(Student) * studentCount);
    if (students == NULL)
    {
        printf("Greska - memorija za studente se nije mogla alocirati\n");
        return ALLOC_FAILED;
    }

    for (i = 0; i < studentCount; i++)
        fscanf(studentsFile, "%s %s %d",
            students[i].firstName,
            students[i].lastName,
            &students[i].points);

    fclose(studentsFile); // ne treba nam više, sve je u memoriji

    for (i = 0; i < studentCount; i++)
        printf("Ime i prezime: %s %s\nAps. bodovi: %d\nRel. bodovi: %f\n\n", 
            students[i].firstName,
            students[i].lastName,
            students[i].points,
            (float)students[i].points / MAX_POINTS * 100);

    free(students); // cleanup

    return 0;
}
