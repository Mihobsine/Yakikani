#include "items.h"

void display_items(item_t **items)
{
    for (int i = 0; items[i] != NULL; i++)
    {
        printf("Item id : %d\n", items[i]->id);
        printf("Type : %s\n", items[i]->type == kanji ? "kanji" : "vocabulary");
        printf("Characters : %s\n", items[i]->characters);
        printf("Wanikani page : %s\n", items[i]->url);
        printf("Possible meanings :");
        for (int j = 0; items[i]->meanings[j] != NULL; j++)
        {
            printf(" %s", items[i]->meanings[j]);
            if (items[i]->meanings[j + 1] != NULL)
            {
                printf(",");
            }
            else
            {
                printf("\n");
            }
        }
        printf("Possible reading :");
        for (int j = 0; items[i]->readings[j] != NULL; j++)
        {
            printf(" %s", items[i]->readings[j]);
            if (items[i]->readings[j + 1] != NULL)
            {
                printf(",");
            }
            else
            {
                printf("\n");
            }
        }
        printf("\n");
    }
}

void deleteTab(char **tab)
{
    for (int i = 0; tab[i] != NULL; i++)
    {
        free(tab[i]);
    }
    free(tab);
}

void delete_items(item_t **items)
{
    for (int i = 0; items[i] != NULL; i++)
    {
        deleteTab(items[i]->meanings);
        deleteTab(items[i]->readings);
        free(items[i]->url);
        free(items[i]);
    }
    free(items);
}

char *getDataBaseString(void)
{
    char *buffer = NULL;
    long length;
    FILE *f = fopen("./data/burnedItems.json", "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }
    return (buffer);
}

char **getArrayValues(cJSON *array)
{
    int length = cJSON_GetArraySize(array);
    char **tab = malloc(sizeof(char *) * (length + 1));
    cJSON *elem = NULL;
    int i = 0;

    if (!tab)
    {
        return (NULL);
    }
    cJSON_ArrayForEach(elem, array)
    {
        tab[i] = strdup(elem->valuestring);
        if (!tab[i])
        {
            return (NULL);
        }
        i++;
    }
    tab[length] = NULL;
    return (tab);
}

item_t *init_item(const cJSON *itemJson)
{
    item_t *item = malloc(sizeof(item_t));

    if (!item)
    {
        return (NULL);
    }
    cJSON *id = cJSON_GetObjectItemCaseSensitive(itemJson, "id");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(itemJson, "type");
    cJSON *characters = cJSON_GetObjectItemCaseSensitive(itemJson, "characters");
    cJSON *url = cJSON_GetObjectItemCaseSensitive(itemJson, "url");
    cJSON *meanings = cJSON_GetObjectItemCaseSensitive(itemJson, "meanings");
    cJSON *readings = cJSON_GetObjectItemCaseSensitive(itemJson, "readings");

    item->id = id->valueint;
    item->type = strcmp(type->valuestring, "kanji") == 0 ? kanji : vocabulary;
    item->characters = strdup(characters->valuestring);
    item->url = strdup(url->valuestring);
    item->meanings = getArrayValues(meanings);
    item->readings = getArrayValues(readings);

    if (!item->characters || !item->url || !item->meanings || !item->readings)
    {
        return (NULL);
    }

    return (item);
}

item_t **init_items(void)
{
    char *fileContent = getDataBaseString();
    const cJSON *itemJson = NULL;
    const cJSON *itemsJson = NULL;
    item_t **items = NULL;
    int length = 0;
    int i = 0;

    if (!fileContent)
    {
        return (NULL);
    }
    cJSON *jsonContent = cJSON_Parse(fileContent);
    if (!jsonContent)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return (NULL);
    }

    itemsJson = cJSON_GetObjectItemCaseSensitive(jsonContent, "items");
    length = cJSON_GetArraySize(itemsJson);

    items = malloc(sizeof(item_t) * (length + 1));

    if (!items)
    {
        return (NULL);
    }

    items[length] = NULL;

    cJSON_ArrayForEach(itemJson, itemsJson)
    {
        item_t *item = init_item(itemJson);

        if (!item)
        {
            return (NULL);
        }
        items[i] = item;
        i++;
    }
    cJSON_Delete(jsonContent);
    return (items);
}

void getNbRandomIds(int nb, int length, int *tab)
{
    int count = 0;
    int tmpNb = 0;

    while (count < nb)
    {
        tmpNb = rand() % length;
        for (int i = 0; i < nb; i++)
        {
            if (tab[i] == tmpNb)
            {
                continue;
            }
            else if (tab[i] == -1)
            {

                tab[count] = tmpNb;
                count++;
                break;
            }
        }
    }
}

item_t **getRandomItems(int nb)
{
    char *fileContent = getDataBaseString();
    const cJSON *itemJson = NULL;
    const cJSON *itemsJson = NULL;
    item_t **items = malloc(sizeof(item_t) * (nb + 1));
    int length = 0;
    int i = 0;
    int j = 0;
    int *selectedIds = malloc(sizeof(int) * nb);

    if (!fileContent || !items || !selectedIds)
    {
        return (NULL);
    }
    items[nb] = NULL;
    for (int k = 0; k < nb; k++)
    {
        selectedIds[k] = -1;
    }
    cJSON *jsonContent = cJSON_Parse(fileContent);
    if (!jsonContent)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return (NULL);
    }
    itemsJson = cJSON_GetObjectItemCaseSensitive(jsonContent, "items");
    length = cJSON_GetArraySize(itemsJson);

    getNbRandomIds(nb, length, selectedIds);

    cJSON_ArrayForEach(itemJson, itemsJson)
    {
        item_t *item = NULL;

        for (int k = 0; k < nb; k++)
        {
            if (selectedIds[k] == i)
            {
                item = init_item(itemJson);
                if (!item)
                {
                    return (NULL);
                }
                items[j] = item;
                j++;
                break;
            }
        }
        if (j == nb)
        {
            break;
        }
        i++;
    }
    cJSON_Delete(jsonContent);
    return (items);
}
