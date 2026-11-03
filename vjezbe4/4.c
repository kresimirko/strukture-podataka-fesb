/*
4. Napisati program za zbrajanje i množenje polinoma. Koeficijenti i eksponenti se
čitaju iz datoteke.
Napomena: Eksponenti u datoteci nisu nužno sortirani.
*/

// U datoteci: 5 0 4 3 9 1 --> 5x^0 + 4x^3 + 9x^1

#define _CRT_SECURE_NO_WARNINGS
#define ALLOC_FAILED_MSG "Alociranje memorije nije uspjelo"
#define ALLOC_FAILED_CODE -1
#define FOPEN_FAILED_MSG "Otvaranje datoteke nije uspjelo"
#define FOPEN_FAILED_CODE -2
#define PARSE_FAILED_MSG "Prilozena datoteka nije ispravno formatirana ili joj fali polinoma"
#define PARSE_FAILED_CODE -3
#define POLY_FILE "poly.txt"
#define POLY_COUNT 2

#include <stdio.h>
#include <stdlib.h>

typedef struct _Poly* Position;

typedef struct _Poly {
    int coeff;
    int pow;
    Position next;
} Poly;

int readPolyFile(char*, Poly[]);
int insertNewPoly(int, int, Position);
int insertAfterPos(Position, Position);
int removeAfterPos(Position);
int printPoly(Position);
int clearPoly(Position);
int addTwoPoly(Position, Position, Position);
int multiTwoPoly(Position, Position, Position);

int main()
{
    int i, res;

    // skroz ok jer nam trebaju nule
    Poly heads[POLY_COUNT] = { 0 };
    Poly result = { 0 };

    res = readPolyFile(POLY_FILE, heads);
    if (res) return res; // ako se dogodila kakva greška

    // verifikacija
    for (i = 0; i < POLY_COUNT; i++)
        printPoly(heads[i].next);
    
    addTwoPoly(&heads[0], &heads[1], &result);
    printf("=== ZBROJEN POLINOM ===\n");
    printPoly(result.next);

    clearPoly(&result);

    multiTwoPoly(&heads[0], &heads[1], &result);
    printf("=== POMNOZEN POLINOM ===\n");
    printPoly(result.next);

    // cleanup
    for (i = 0; i < POLY_COUNT; i++)
        clearPoly(&heads[i]);
    clearPoly(&result);

    return 0;
}

int readPolyFile(char* polyFile, Poly heads[])
{
    int i, temp1, temp2;

    FILE* fp = fopen(polyFile, "r");
    if (fp == NULL)
    {
        perror(FOPEN_FAILED_MSG);
        fclose(fp);
        return FOPEN_FAILED_CODE;
    }

    // čitamo do kraja datoteke
    i = 0;
    while (i < POLY_COUNT)
    {
        if (fscanf(fp, "%d %d", &temp1, &temp2) != 2)
        {
            fputs(PARSE_FAILED_MSG, stderr);
            return PARSE_FAILED_CODE;
        }

        insertNewPoly(temp1, temp2, &heads[i]);

        // idemo li u drugi polinom? je li kraj datoteke?
        // (datoteka može, a i ne mora imati newline(ove) na kraju)
        temp1 = fgetc(fp);
        if (temp1 == EOF && i < POLY_COUNT - 1)
        {
            // nedovoljno polinoma u datoteci
            fputs(PARSE_FAILED_MSG, stderr);
            return PARSE_FAILED_CODE;
        }
        if (temp1 == '\n')
        {
            i++;
            // printf("========\n");
        }
        if (temp1 == EOF) // ako datoteka nema newline na kraju
            break;
    }
    // printf("========\n");

    return 0;
}

int insertNewPoly(int coeff, int pow, Position q)
{
    // koef. 0 odbacujemo, ali nećemo izaći iz programa zbog toga
    if (coeff == 0) return -1;

    while (q != NULL)
    {
        if (q->next == NULL ||
            q->next != NULL && pow > q->next->pow)
        {
            // printf("DODAJEM NOVI POLY: %d %d\n", coeff, pow);
            Position newPoly = (Position)malloc(sizeof(Poly));
            if (newPoly == NULL)
            {
                perror(ALLOC_FAILED_MSG);
                return ALLOC_FAILED_CODE;
            }
            newPoly->coeff = coeff;
            newPoly->pow = pow;
            newPoly->next = NULL;
            insertAfterPos(q, newPoly);
            break;
        }
        else if (q->next != NULL && pow == q->next->pow)
        {
            if (q->next->coeff + coeff == 0)
            {
                // printf("BRISEM POSTOJECI POLY jer je koeficijent 0\n");
                removeAfterPos(q);
                break;
            }
            // printf("MIJENJAM POSTOJECI POLY: %d %d\n",
            //     q->next->coeff + coeff, pow);
            q->next->coeff += coeff;
            break;
        }
        else if (q->next != NULL && pow < q->next->pow)
            q = q->next;
    }

    return 0;
}

int insertAfterPos(Position p, Position newP)
{
    newP->next = p->next;
    p->next = newP;

    return 0;
}

int removeAfterPos(Position p)
{
    if (p->next != NULL)
    {
        p->next = p->next->next;
        free(p->next);
    }
    return 0;
}

int printPoly(Position p)
{
    int i = 0;

    printf("> ");

    while (p != NULL)
    {
        if (i > 0) printf(" + ");
        printf("%dx^%d", p->coeff, p->pow);
        p = p->next;
        i++;
    }
    
    putchar('\n');

    return 0;
}

int clearPoly(Position p)
{
    // objašnjenje koda u prošlom zadatku
    int i;
    Position temp;

    for (i = 0; p->next != NULL; i++)
    {
        temp = p->next;

        if (i == 0) // head
            p->next = NULL;
        else
            free(p);

        p = temp;
    }

    return 0;
}

int addTwoPoly(Position first, Position second, Position result)
{
    for (; first != NULL; first = first->next)
        insertNewPoly(first->coeff, first->pow, result);
    for (; second != NULL; second = second->next)
        insertNewPoly(second->coeff, second->pow, result);

    return 0;
}

int multiTwoPoly(Position first, Position second, Position result)
{
    // jer ćemo proći kroz second nekoliko puta, a kroz first jedanput
    // pa trebamo znati gdje se vratiti
    Position secondStart = second;

    while (first != NULL)
    {
        while (second != NULL)
        {
            insertNewPoly(
                first->coeff * second->coeff,
                first->pow + second->pow,
                result);

            second = second->next;
        }
        first = first->next;
        second = secondStart;
    }

    // potvrdio u Desmosu da radi :)

    return 0;
}
