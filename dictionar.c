#include "invertedindex.h"

int read_file(FILE *f, char *nume_f[], char *query[], int *n, int *m)
{
    int i;
    char tmp[2001];

    while(!feof(f))
    {
        /* citeste n nume fisiere precizat in enunt < 500 */
        if (fscanf(f, "%i\n", n) != 1)
            break;
        for(i=0; i<*n && !feof(f); i++)
        {
            if (!fscanf(f, "%[^\r\n]\n", tmp)){
                printf("\nFisierul %i nu s-a putut citi", i); return 1;
            }

            if ((nume_f[i] = strdup(tmp)) == NULL){
                printf("\nNu s-a putut creea copie nume fisier\n"); return 2;
            }
        }

        /* citeste m query  < 500 */
        if (fscanf(f, "%i\n", m) != 1)
            break;
        for(i=0; i<*m && !feof(f); i++)
        {
            if (!fscanf(f, "%[^\r\n]\n", tmp)){
                printf("\nQuery-ul %i nu s-a putut citi", i); return 3;
            }
            if ((query[i] = strdup(tmp)) == NULL){
                printf("\nNu s-a putut creea copie sir query\n"); return 4;
            }
        }

    }/* end of while */
    fclose(f);
    return 0;
}

void print_file(char *query, Array_t files, FILE* f)
{
    int i;

    fprintf(f, "%s: ", query);
    for(i=0; i<files.n; i++)
    {
        fprintf(f, "%i ", files.v[i]);
    }
    fputc('\n', f);
}

/* Functia ce va contine rezolvarea problemei. Tot aici va fi construit si
 * dictionarul pe baza regulilor stabilite. */
void solve()
{
    int n=0, m=0;
    int i, l, v[500] = {0};
    FILE *f1, *f2, *ftmp;
    Map_t *map;
    Array_t files1, files2;
    char *nume_f[500], *query[500], tmp[2001], *tok, ch;

    if ((f1 = fopen("input.in", "rt")) == NULL){
        printf("Eroare deschidere fisier date intrare\n"); return;
    }
    if (read_file(f1, nume_f, query, &n, &m))
        return;

    /* ***********************
     * Building the Dictionary
      ************************ */

    map = init_Map(MAP_SIZE);
    if(!map){
        printf("\nEroare alocare memorie dictionar\n");
        return;
    }

    for(i=0; i<n; i++)
    {
        if((ftmp = fopen(nume_f[i], "rt")) == NULL){
            printf("\nFisierul cu docID = %i nu s-a putut deschide\n", i); return;
        }
        while(!feof(ftmp))
        {
            if(!fscanf(ftmp, "%[^\r\n]\n", tmp)){
                printf("Eroare citire linie in fisierul %i - %s", i, nume_f[i]); return;
            }
            tok = strtok(tmp, " ,.;:\"\'!?()[]{}<>`~@#$%^&*+=\r\n");
            put_doc(map, tok, i);
            while(tok != NULL)
            {
                tok = strtok(NULL, " ,.;:\"\'!?()[]{}<>`~@#$%^&*+=\r\n");
                if(tok != NULL)
                    put_doc(map, tok, i);
            }

        }// end of while(!feof...)
        fclose(ftmp);
    }//end of for(...)


    /* ******************************
     *          Parsing Query
     ******************************* */

    if ((f2 = fopen("output.out", "wt")) == NULL){
        printf("Eroare deschidere fisier iesire\n"); return;
    }
    for(i=0; i<m; i++)
    {
        files1.v = v;
        files1.n = 0;
        l = 0;      /* Memoreaza numarul de "cuvinte" din query */

        strcpy(tmp, query[i]);
        tok = strtok(tmp, " ");
        files1 = get_docs(map, tok);
        while(tok !=NULL)
        {
            l++;
            tok = strtok(NULL, " ");
            if ((tok != NULL) && (l%2 != 0))
            {
                ch = *tok;
            }
            else if ( l%2 == 0)
                 {
                     files2 = get_docs(map, tok);
                     switch(ch)
                     {
                     case '|' :  files1 = reunion(files1, files2); break;
                     case '&' :  files1 = intersection(files1, files2); break;
                     default  :  printf("\nFisier cu format gresit\n");
                     }
                 }
        }// end of while(...)
        /* Scriem rezultatul query-ului in fisier */
        print_file(query[i], files1, f2);
    }
    fclose(f2);
    free_Map(map);
}
