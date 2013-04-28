#ifndef __INVERTEDINDEX_H__
#define __INVERTEDINDEX_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 500
#define MMAX 500
#define MAP_SIZE 500
/* Vector de intregi
 * v   - vectorul
 * n   - numărul de elemente
 * cap - capacitatea vectorului
 * */
typedef struct Array
{
    int     *v;
    int     n, cap;
} Array_t;

/* O intrare în Map_t */
typedef struct Entry
{
    char    *word;
    Array_t   documents;
} Entry_t;

/* Nod pentru o lista simplu inlantuita */
typedef struct Node
{
    struct Node      *next;
    Entry_t          data;
} Node_t;

/* Retine o mapare Cheie - Valoare (în cazul nostru char* - Array_t)
 *
 * buckets - vector de liste simplu inlantuite.
 * size    - marimea lui buckets.
 */
typedef struct Map
{
    Node_t    **buckets;
    int     size;
} Map_t;

/* Initializeaza fiecare element din hashtable cu NULL */
Map_t* init_Map(int size);

/* Aloca memorie pentru o structura Node_t si seteaza valorile campurilor */
Node_t* aloc_node(char *key, int docID);

/* Elibereaza spatiul ocupat de hash table */
void free_Map(Map_t *map);

/* Elibereaza spatiul ocupat de o lista */
void free_list(Node_t *head);

/* Afiseaza toate intrarile dintr-o lista*/
void print_list(Node_t *head);

/* Afiseaza toate intrarile din Map */
void print_Map(Map_t* map);

/* Citeste input-ul din fisier */
int read_file(FILE *f, char* nume_f[], char* query[], int* n, int* m);

/* Scrie output-ul in fisier */
void print_file(char *query, Array_t files, FILE *f);

/* Intoarce o valoare reprezentand intregul asociat unui șir de caractere
 *
 * Se foloseste pentru a decide in ce bucket din Map_t va ajunge sirul de
 * caractere.
 * e.g. "ana" va ajunge in buckets[hash("ana") % size]
 */
unsigned long hash(unsigned char *str);

/* Adauga id-ul unui fisier la valorile cheii key din dictionar.
 * Se foloseste urmatoarea rutina:
 *
 * Se parcurge lista aflata in buckets[hash(key) % size]
 *
 * Daca se intalneste un Entry_t avand cheia key, se adauga in Array_t-ul din
 * Entry_t.
 *
 * Altfel, se adauga in lista un nou Entry_t avand cheia key si un Array_t ce
 * contine doar docID.
 */
void put_doc(Map_t *map, char *key, int docID);

/* Returneaza un Array_t cu ID-urile documentelor in care apare cheia key */
Array_t get_docs(Map_t *map, char *key);

/* Realizeaza intersectia a doua multimi de indici de fisiere. */
Array_t intersection(const Array_t files1, const Array_t files2);

/* Realizeaza reuniunea a doua multimi de indici de fisiere. */
Array_t reunion(const Array_t files1, const Array_t files2);

/* Functia ce va contine rezolvarea problemei. Tot aici va fi construit si
 * dictionarul pe baza regulilor stabilite. */
void solve();

#endif
