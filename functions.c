#include "invertedindex.h"

Map_t* init_Map(int size)
{
    int i;
    Map_t *map = NULL;

    map = (Map_t*)malloc(sizeof(Map_t));
    if(!map){
        printf("\nAlocare memorie Map esuata\n"); return NULL;
    }
    map->buckets = malloc(size * sizeof(Node_t));
    if(!map->buckets){
        printf("\nAlocare memorie buckets esuata\n"); return NULL;
    }

    for(i=0; i<size; i++)
    {
        map->buckets[i] = NULL;
    }
    map->size = size;
    return map;
}

void put_doc(Map_t *map, char *key, int docID)
{
    /* ATENTIE : map->buckets[...] trebuie sa fie NULL in cazul in care nu exista key */
    unsigned int index = 0;
    index = hash(key) % map->size;
    Node_t *head = map->buckets[index];
    Node_t *tmp = head, *last;
    int i;

    if (head == NULL)    /* Lista este vida */
    {
        head = aloc_node(key, docID);
        if (!head){
            printf("Nu s-a putut initializa lista"); return;
        }
        map->buckets[index] = head;
    }
    else                /* Daca lista nu e vida */
    {
        while(tmp != NULL)
        {
            if (strcmp(tmp->data.word, key) == 0)   /* Cuvantul este deja in dictionar */
            {
                for(i=0; i<tmp->data.documents.n &&
                      tmp->data.documents.n<tmp->data.documents.cap; i++)
                {
                    /* Cauta daca mai exista deja valoarea docID in vector */
                    if (tmp->data.documents.v[i] == docID)
                        break;
                }
                /* Daca valoarea docID nu mai exista atunci este adaugata in vector */
                if(i == tmp->data.documents.n)
                {
                    tmp->data.documents.v[i] = docID;
                    tmp->data.documents.n++;
                }
                break;      /* Se iese din ciclul while deoarece a fost gasita cheia key */
            } // end of if (strcmp .... )
            last = tmp;
            tmp = tmp->next;
        } // end of while(...)

        /* Sirul key nu a fost gasit in lista , deci trebuie adaugat */
        if (tmp == NULL)
        {
            tmp = aloc_node(key, docID);
            if (!tmp){
                printf("Nu s-a putut adauga nod in lista"); return;
            }
            last->next = tmp;
            map->buckets[index] = head;
        }

    } // end of else { ... }
}

Node_t* aloc_node(char *key, int docID)
{
    Node_t *tmp;

    tmp = (Node_t*)malloc(sizeof(Node_t));
    if(!tmp)  return NULL;
    tmp->next = NULL;

    tmp->data.word = strdup(key);
    if(!tmp->data.word){
        free(tmp); return NULL;
    }

    tmp->data.documents.v = (int*)malloc(NMAX * sizeof(int));
    if(!tmp->data.documents.v){
        free(tmp->data.word);
        free(tmp); return NULL;
    }

    tmp->data.documents.v[0] = docID;
    tmp->data.documents.n = 1;
    tmp->data.documents.cap = NMAX;
    return tmp;
}

void print_list(Node_t *head)
{
    Node_t *iter;
    int i;

    for(iter=head; iter != NULL; iter=iter->next)
    {
        printf("%s: ", iter->data.word);
        for(i=0; i<iter->data.documents.n; i++)
        {
            printf("%i ", iter->data.documents.v[i]);
        }
        printf("\n");
    }
}

void print_Map(Map_t* map)
{
    int i;

    for(i=0; i<map->size; i++)
    {
        /* Afiseaza doar intrarile in hashtable care sunt diferite de NULL */
        if (map->buckets[i] != NULL)
        {
            print_list(map->buckets[i]);
            printf("\n");
        }
    }
}

void free_Map(Map_t *map)
{
    int i;

    for(i=0; i<map->size; i++)
    {
        if (map->buckets[i] != NULL)
            free_list(map->buckets[i]);
    }
    free(map->buckets);
    free(map);
}

void free_list(Node_t *head)
{
    Node_t *tmp;

    while(head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp->data.documents.v);
        free(tmp->data.word);
        free(tmp);
    }
}

unsigned long hash (unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    while(( c = *str++) != 0)
    {
        hash = ((hash << 3) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

/* Returneaza un Array_t cu ID-urile documentelor în care apare cheia key */
Array_t get_docs(Map_t *map, char *key)
{
    int index = hash(key) % map->size;
    Node_t *head = map->buckets[index];
    Node_t *tmp = head;

    while(tmp != NULL)
    {
        if (strcmp(tmp->data.word, key) == 0)
        {
            return tmp->data.documents;
        }
        tmp = tmp->next;
    }
}

/* Realizeaza intersectia a doua multimi de indici de fisiere. */
Array_t intersection(const Array_t files1, const Array_t files2)
{
    Array_t files_tmp;
    int v[500] = { 0 };
    int i, j, k;

    for(i=0, k=0; i<files1.n; i++)
    {
        for(j=0; j<files2.n; j++)
        {
            if (files1.v[i] == files2.v[j])
            {
                v[k++] = files1.v[i];
                break;
            }
        }
    }

    if (k != 0){
        files_tmp.v = v;
        files_tmp.n = k;
        files_tmp.cap = NMAX;
    }
    else{
        files_tmp.v = v;
        files_tmp.n = 0;
        files_tmp.cap = NMAX;
    }

    return files_tmp;
}

/* Realizeaza reuniunea a doua multimi de indici de fisiere. */
Array_t reunion(const Array_t files1, const Array_t files2)
{
    Array_t files_tmp, files_intersec;
    int v[500] = { 0 };
    int i, j, k, flag;

    files_intersec = intersection(files1, files2);
    if (files_intersec.n == 0)
    {
        /* Daca intersectia multimilor este 0, adauga toate elementele*/
        for(i=0; i<files1.n; i++)
            v[i] = files1.v[i];
        for(j=0, k=i; j<files2.n; j++, k++)
            v[k] = files2.v[j];

        files_tmp.v = v;
        files_tmp.n = k;
        files_tmp.cap = NMAX;
    }
    else
    {   /* Reuniunea = elementele multimii A + elementele multimii (B - Intersec(A si B) ) */
        for(i=0; i<files1.n; i++)
            v[i] = files1.v[i];

        for(j=0; j<files2.n; j++)
        {
            flag = 0;    /* Indica daca elementul din B se afla in intersectie */
            for(k=0; k<files_intersec.n && !flag; k++)
            {
                if (files2.v[j] == files_intersec.v[k]){
                    flag = 1;
                }
            }
            if (!flag)
            {
                v[i++] = files2.v[j];
            }
        }
        files_tmp.v = v;
        files_tmp.n = i;
        files_tmp.cap = NMAX;
    }
    return files_tmp;
}
