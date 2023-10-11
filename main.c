#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

typedef struct item
{
    char *word;
    int weight;
} Item;

typedef struct Node
{
    char letter;
    struct Node **childs;
    size_t childCount;
    size_t capacity;
    Item *item;
} Node;

typedef struct Vector
{
    Item **items;
    size_t size;
    size_t capacity;
} Vector;

int getlinenum(char *line, FILE *fp)
{
    int i = 0;
    char c;
    while (1)
    {
        c = fgetc(fp);
        if (c != EOF && c != '\n')
        {
            continue;
        }
        else if (c == EOF)
        {
            return -1;
        }
        else
        {
            break;
        }
    }
    return i;
}

Node *search_node(Node *node, char letter)
{
    if (node == NULL)
    {
        return NULL;
    }
    Node *child = NULL;
    for (int k = 0; k < node->childCount; k++)
    {
        if (node->childs[k] != NULL && node->childs[k]->letter == letter)
        {
            child = node->childs[k];
            break;
        }
    }
    return child;
}

void swap(Item **items, int i, int j)
{
    Item *tmp = items[i];
    items[i] = items[j];
    items[j] = tmp;
}

int partition(Item **items, int start, int stop)
{
    int pivot = items[stop]->weight;
    int i = start;
    for (int j = start; j < stop; j++)
    {
        if (items[j]->weight >= pivot)
        {
            swap(items, i, j);
            i++;
        }
    }
    swap(items, i, stop);
    return i;
}

void quick_sort(Item **items, int start, int stop)
{
    if (start < stop)
    {
        int pivot = partition(items, start, stop);
        quick_sort(items, start, pivot - 1);
        quick_sort(items, pivot + 1, stop);
    }
}

Node *create_node(Node *node, char letter)
{
    Node *new_node = malloc(sizeof(Node));
    new_node->letter = letter;
    new_node->childs = malloc(sizeof(Node *) * 1);
    new_node->childCount = 0;
    new_node->childs[0] = NULL;
    new_node->capacity = 1;
    new_node->item = NULL;
    /*when node child is full, realloc mem*/
    if (node->childCount == node->capacity)
    {
        node->childs = realloc(node->childs, sizeof(Node *) * (node->childCount * 2));
        node->capacity = node->childCount * 2;
        for (int k = node->childCount; k < node->capacity; k++)
        {
            node->childs[k] = NULL;
        }
    }
    node->childs[node->childCount++] = new_node;
    return new_node;
}

Node *get_suggestions(Node *node, Vector *vector)
{
    if (node == NULL)
    {
        return NULL;
    }
    if (node->item != NULL)
    {
        if (vector->size == vector->capacity)
        {
            vector->items = realloc(vector->items, sizeof(Item *) * (vector->size * 2));
            vector->capacity = vector->size * 2;
            vector->items[vector->size++] = node->item;
        }
        else
        {
            vector->items[vector->size++] = node->item;
        }
    }
    if (node->childCount > 0)
    {
        for (int k = 0; k < node->childCount; k++)
        {
            if (node->childs == NULL)
            {
                continue;
            }
            Node *child = node->childs[k];
            get_suggestions(child, vector);
        }
    }
}

void free_tree(Node *node) {
    if (node == NULL) {
        return;
    }
    if (node->childCount > 0) {
        for (int k = 0; k < node->childCount; k++) {
            Node *child = node->childs[k];
            free_tree(child);
        }
    }
    if (node->item != NULL) {
        free(node->item->word);
        free(node->item);
    }
    free(node->childs);
    free(node);
}

int main(int argc, char **argv)
{
    char *dictionaryFilePath = argv[1]; // this keeps the path to dictionary file
    char *queryFilePath = argv[2];      // this keeps the path to the file that keeps a list of query wrods, 1 query per line
    int wordCount = 0;                  // this variable will keep a count of words in the dictionary, telling us how much memory to allocate
    int queryCount = 0;                 // this variable will keep a count of queries in the query file, telling us how much memory to allocate for the query words

    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// read dictionary file /////////////////////////
    ////////////////////////////////////////////////////////////////////////
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; // variable to be used for line counting
    size_t lineSize;   // variable to be used for line counting

    // check if the file is accessible, just to make sure...
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file:%s\n", dictionaryFilePath);
        return -1;
    }

    // First, let's count number of lines. This will help us know how much memory to allocate
    while ((lineSize = getlinenum(&line, fp)) != -1)
    {
        wordCount++;
    }
    // Printing wordCount for debugging purposes. You can remove this part from your submission.
    printf("%d\n", wordCount);

    /////////////////PAY ATTENTION HERE/////////////////
    // This might be a good place to allocate memory for your data structure, by the size of "wordCount"
    ////////////////////////////////////////////////////

    // Read the file once more, this time to fill in the data into memory
    fseek(fp, 0, SEEK_SET); // rewind to the beginning of the file, before reading it line by line.
    char word[BUFSIZE];     // to be used for reading lines in the loop below
    int weight;
    Item **items = malloc(sizeof(Item *) * wordCount);
    for (int i = 0; i < wordCount; i++)
    {
        fscanf(fp, "%s %d\n", word, &weight);
        // Let's print them to the screen to make sure we can read input, for debugging purposes. You can remove this part from your submission.
        // printf("%s %d\n",word,weight);

        Item *item = malloc(sizeof(Item));
        char *wordCopy = malloc(sizeof(char) * BUFSIZE);
        strcpy(wordCopy, word);
        item->word = wordCopy;
        item->weight = weight;
        items[i] = item;
        /////////////////PAY ATTENTION HERE/////////////////
        // This might be a good place to store the dictionary words into your data structure
        ////////////////////////////////////////////////////
    }
    // // close the input file
    fclose(fp);

    // ////////////////////////////////////////////////////////////////////////
    // ///////////////////////// read query list file /////////////////////////
    // ////////////////////////////////////////////////////////////////////////
    fp = fopen(queryFilePath, "r");

    // check if the file is accessible, just to make sure...
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file:%s\n", queryFilePath);
        return -1;
    }

    // // First, let's count number of queries. This will help us know how much memory to allocate
    while ((lineSize = getlinenum(&line, fp)) != -1)
    {
        queryCount++;
    }
    free(line); // getlinenum internally allocates memory, so we need to free it here so as not to leak memory!!

    // Printing line count for debugging purposes. You can remove this part from your submission.

    /////////////////PAY ATTENTION HERE/////////////////
    // This might be a good place to allocate memory for storing query words, by the size of "queryCount"
    ////////////////////////////////////////////////////

    fseek(fp, 0, SEEK_SET); // rewind to the beginning of the file, before reading it line by line.
    char **queries = malloc(sizeof(char *) * queryCount);
    for (int i = 0; i < queryCount; i++)
    {
        fscanf(fp, "%s\n", word);
        // Let's print them to the screen to make sure we can read input, for debugging purposes. You can remove this part from your submission.
        char *wordCopy = malloc(sizeof(char) * BUFSIZE);
        strcpy(wordCopy, word);
        queries[i] = wordCopy;
        /////////////////PAY ATTENTION HERE/////////////////
        // This might be a good place to store the query words in a list like data structure
        ////////////////////////////////////////////////////
    }
    // close the input file
    fclose(fp);

    Node *root = malloc(sizeof(Node));
    root->childs = malloc(sizeof(Node *) * 1);
    root->capacity = 1;
    root->childCount = 0;
    root->childs[0] = NULL;
    root->item = NULL;
    for (int i = 0; i < wordCount; i++)
    {
        char *word = items[i]->word;
        size_t len = strlen(word);
        Node *node = root;
        for (int j = 0; j < len; j++)
        {
            char letter = word[j];
            Node *child = search_node(node, letter);
            if (!child)
            {
                child = create_node(node, letter);
            }
            node = child;
        }
        node->item = items[i];
    }

    for (int i = 0; i < queryCount; i++)
    {
        char *query = queries[i];
        size_t len = strlen(query);
        Node *node = root;
        for (int j = 0; j < len; j++)
        {
            char letter = query[j];
            Node *child = search_node(node, letter);
            if (!child)
            {
                node = NULL;
                printf("No suggestion!\n");
                break;
            }
            node = child;
        }
        Vector *vector = malloc(sizeof(Vector));
        vector->items = malloc(sizeof(Item *) * 1);
        vector->capacity = 1;
        vector->size = 0;
        get_suggestions(node, vector);
        quick_sort(vector->items, 0, vector->size - 1);
        int size = vector->size > 10 ? 10 : vector->size;
        for (int k = 0; k < size; k++)
        {
            Item *item = vector->items[k];
            printf("%s %d\n", item->word, item->weight);
        }
        printf("\n");
        free(vector->items);
        free(vector);
    }
    free_tree(root);
    free(queries);
    free(items);
    ////////////////////////////////////////////////////////////////////////
    ///////////////////////// reading input is done ////////////////////////
    ////////////////////////////////////////////////////////////////////////

    // Now it is your turn to do the magic!!!
    // do search/sort/print, whatever you think you need to do to satisfy the requirements of the assignment!
    // loop through the query words and list suggestions for each query word if there are any
    // don't forget to free the memory before you quit the program!

    // // OUTPUT SPECS:
    // //  use the following if no word to suggest: printf("No suggestion!\n");
    // //  use the following to print a single line of outputs (assuming that the word and weight are stored in variables named word and weight, respectively):
    // //  printf("%s %d\n",word,weight);
    // //  if there are more than 10 outputs to print, you should print the top 10 weighted outputs.

    return 0;
}
