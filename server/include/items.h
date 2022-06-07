#ifndef ITEMS_H_
#define ITEMS_H_

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "cJSON.h"

typedef enum
{
    kanji,
    vocabulary,
} item_type;

typedef struct item_s
{
    int id;
    item_type type;
    char *characters;
    char *url;
    char **meanings;
    char **readings;
} item_t;

void display_items(item_t **items);
item_t **init_items(void);
item_t **getRandomItems(int nb);
void delete_items(item_t **items);

#endif /* !ITEMS_H_ */
