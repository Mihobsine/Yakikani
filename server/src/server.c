#include "server.h"

int getRandomId(int *tab, int size)
{
    int remainings = 0;
    int randomId = 0;
    int j = 0;

    for (int i = 0; i < size; i++)
    {
        if (!tab[i])
        {
            remainings++;
        }
    }
    if (!remainings)
    {
        return (-1);
    }
    randomId = rand() % remainings;
    for (int i = 0; i < size; i++)
    {
        if (!tab[i])
        {
            if (j == randomId)
            {
                return (i);
            }
            else
            {
                j++;
            }
        }
    }
    return (-1);
}

int checkResponse(char *buf, item_t **items, int type, int id)
{
    if (type == meaning)
    {
        for (int i = 0; items[id]->meanings[i] != NULL; i++)
        {
            if (strcmp(buf, items[id]->meanings[i]) == 0)
            {
                return (1);
            }
        }
    }
    else
    {
        for (int i = 0; items[id]->readings[i] != NULL; i++)
        {
            if (strcmp(buf, items[id]->readings[i]) == 0)
            {
                return (1);
            }
        }
    }
    return (0);
}

char *getWordList(item_t **items, int type, int id)
{
    char buf[BUFSIZ];
    char tmp[BUFSIZ];

    memset(buf, 0, BUFSIZ);
    if (type == meaning)
    {
        for (int i = 0; items[id]->meanings[i] != NULL; i++)
        {
            sprintf(tmp, " %s", items[id]->meanings[i]);
            strcat(buf, tmp);
            if (items[id]->meanings[i + 1] != NULL)
            {
                strcat(buf, ",");
            }
            else
            {
                strcat(buf, "\n");
            }
        }
    }
    else
    {
        for (int i = 0; items[id]->readings[i] != NULL; i++)
        {
            sprintf(tmp, " %s", items[id]->readings[i]);
            strcat(buf, tmp);
            if (items[id]->readings[i + 1] != NULL)
            {
                strcat(buf, ",");
            }
            else
            {
                strcat(buf, "\n");
            }
        }
    }

    return (strdup(buf));
}

int execOneReview(int size, int clientfd, char *addr,
                  int port, int *readings, int *meanings,
                  item_t **items)
{
    int type = rand() % 2;
    int id = (type == meaning) ? getRandomId(meanings, size) : getRandomId(readings, size);
    char buf[BUFSIZ];
    int len;
    int valid = 0;

    if (id == (-1))
    {
        type = !type;
        id = (type == meaning) ? getRandomId(meanings, size) : getRandomId(readings, size);
    }
    sprintf(buf, "%s%s%s", ((type == meaning) ? "M" : "R"),
            ((items[id]->type == kanji) ? "K" : "V"),
            items[id]->characters);
    printf("for %s: %d\n", addr, port);
    printf("Sending %s\n", buf);
    send(clientfd, buf, strlen(buf), 0);
    len = recv(clientfd, buf, BUFSIZ, 0);
    buf[len - 1] = '\0';
    printf("from %s: %d\n", addr, port);
    printf("Answer received is %s\n", buf);
    if (strncasecmp(buf, "!exit", 5) == 0)
    {
        delete_items(items);
        free(meanings);
        free(readings);
        close(clientfd);
        exit(0);
    }
    valid = checkResponse(buf, items, type, id);
    if (!valid)
    {
        printf("for %s: %d\n", addr, port);
        printf("Invalid %s\n", (type == meaning) ? "meaning" : "reading");
        char *wordList = getWordList(items, type, id);
        sprintf(buf, "IWrong answer...\nPossible(s) %s :%s\nSee the WaniKani page for more informations :\n%s\n\n", (type == meaning) ? "meaning(s)" : "reading(s)", wordList, items[id]->url);
        send(clientfd, buf, strlen(buf), 0);
        free(wordList);
        sleep(1);
    }
    else
    {
        printf("for %s: %d\n", addr, port);
        printf("Valid %s\n", (type == meaning) ? "meaning" : "reading");
        if (type == meaning)
        {
            meanings[id] = 1;
        }
        else
        {
            readings[id] = 1;
        }
        send(clientfd, "V", 1, 0);
        sleep(1);
    }
    return (valid);
}

int getTestSize(char *buf)
{
    int size = -1;
    char *endptr = NULL;

    if (strncasecmp(buf, "!start ", 7) != 0)
    {
        return (-1);
    }
    if (buf[7] == '\0')
    {
        return (-1);
    }
    size = strtol(buf + 7, &endptr, 10);
    if (endptr != buf + 7 + strlen(buf + 7) - 1)
    {
        return (-1);
    }
    return (size);
}

void launchTest(int size, int clientfd, char *addr, int port)
{
    int *readings = malloc(sizeof(int) * size);
    int *meanings = malloc(sizeof(int) * size);
    item_t **items = getRandomItems(size);
    int remaining = size * 2;
    int mistakes = 0;
    char buf[BUFSIZ];

    if (!items)
    {
        printf("for %s: %d\n", addr, port);
        printf("Recupration of items error\n");
        send(clientfd, "ERROR", 5, 0);
        return;
    }
    if (!readings || !meanings)
    {
        printf("for %s: %d\n", addr, port);
        printf("Readings or meanings allocation error\n");
        send(clientfd, "ERROR", 5, 0);
        delete_items(items);
        return;
    }
    for (int i = 0; i < size; i++)
    {
        readings[i] = 0;
        meanings[i] = 0;
    }
    while (remaining)
    {
        int result = execOneReview(size, clientfd, addr, port, readings, meanings, items);
        if (result)
        {
            remaining = remaining - 1;
        }
        else
        {
            mistakes++;
        }
    }
    printf("for %s: %d\n", addr, port);
    printf("End of review\n");
    sprintf(buf, "END%d", mistakes);
    send(clientfd, buf, strlen(buf), 0);
    delete_items(items);
    free(readings);
    free(meanings);
}

void handle_client(int clientfd, char *addr, int port)
{
    int len;
    char buf[BUFSIZ];
    int size = 0;

    memset(buf, 0, BUFSIZ);

    len = send(clientfd, "Welcome to WaniKani burned material trainer !!!\n\nUse \"!start nb\" command to start a review.\n\n\"nb\" must be included between 1 and 50 to work.\nUse \"!exit\" to quit.\n", 162, 0);

    while (strncasecmp(buf, "!exit\n", 6) != 0)
    {
        len = recv(clientfd, buf, BUFSIZ, 0);
        buf[len] = '\0';
        printf("from %s: %d\n", addr, port);
        printf("<== %s\n", buf);
        if (strncasecmp(buf, "!exit\n", 6) == 0)
        {
            break;
        }
        size = getTestSize(buf);
        if (size > 0 && size <= 50)
        {
            send(clientfd, "OK", 2, 0);
            launchTest(size, clientfd, addr, port);
        }
        else
        {
            printf("Invalid command\n");
            len = send(clientfd, "Not a valid command please use \"!start nb\" (with nb replaced by number between 1 and 50) or \"!exit\"\n", 100, 0);
        }
    }
    send(clientfd, "Session closed\n", 15, 0);
    close(clientfd);
}

int run_server(int argc, char *argv[])
{
    int sockfd;
    int new_sockfd;
    struct sockaddr_in serv, cint;
    socklen_t sin_siz;
    int port;

    if (argc != 3)
    {
        printf("Usage ./prog host port\n");
        exit(1);
    }
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    printf("socket() called\n");
    serv.sin_family = PF_INET;
    port = strtol(argv[2], NULL, 10);
    serv.sin_port = htons(port);
    inet_aton(argv[1], &serv.sin_addr);
    sin_siz = sizeof(struct sockaddr_in);
    if (bind(sockfd, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        perror("bind");
        exit(1);
    }
    printf("bind() caled\n");
    if (listen(sockfd, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(1);
    }
    printf("listen() called\n");

    while (1)
    {
        if ((new_sockfd = accept(sockfd, (struct sockaddr *)&cint, &sin_siz)) < 0)
        {
            perror("accept");
        }
        printf("connect from %s: %d\n", inet_ntoa(cint.sin_addr), ntohs(cint.sin_port));
        int child = fork();

        if (child == 0)
        {
            close(sockfd);
            srand(time(NULL));
            handle_client(new_sockfd, inet_ntoa(cint.sin_addr), ntohs(cint.sin_port));
            exit(0);
        }
        close(new_sockfd);
    }
    close(sockfd);
    return (0);
}
