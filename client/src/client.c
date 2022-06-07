#include "client.h"

void displayReviewMessage(char *buf)
{
    printf("%s of %s '%s'\n", (buf[0] == 'R' ? "Reading" : "Meaning"), (buf[1] == 'K' ? "kanji" : "vocabulary"), buf + 2);
}

void displayInvalidResponseMessage(char *buf)
{
    printf("%s\n", buf + 1);
}

void displayEndReviewMessage(char *buf)
{
    printf("End of the review session.\nYou did %s mistakes.\n", buf + 3);
}

int run_client(int argc, char *argv[])
{
    int sockfd, len;
    char buf[BUFSIZ];
    struct sockaddr_in serv;
    int port;
    if (argc != 3)
    {
        printf("Usage: ./%s host port\n", argv[0]);
        exit(1);
    }
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    serv.sin_family = PF_INET;
    port = strtol(argv[2], NULL, 10);
    serv.sin_port = htons(port);
    inet_aton(argv[1], &serv.sin_addr);

    if (connect(sockfd,
                (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        perror("connect");
        exit(1);
    }
    len = recv(sockfd, buf, BUFSIZ, 0);
    buf[len] = '\0';

    printf("%s\n", buf);
    while (strncasecmp(buf, "!exit\n", 6) != 0)
    {
        printf("==> ");
        if ((fgets(buf, BUFSIZ, stdin)) == NULL)
        {
            perror("fgets");
        }
        len = send(sockfd, buf, strlen(buf), 0);
        if (strncasecmp(buf, "!exit\n", 6) == 0)
        {
            break;
        }
        len = recv(sockfd, buf, BUFSIZ, 0);
        if (len == -1)
        {
            close(sockfd);
            return (0);
        }
        buf[len] = '\0';
        if (strcmp(buf, "OK") == 0)
        {
            printf("Review is starting :\n");
            len = recv(sockfd, buf, BUFSIZ, 0);
            buf[len] = '\0';
            if (strcmp(buf, "ERROR") == 0)
            {
                printf("Internal server error, review is canceled\n");
            }
            displayReviewMessage(buf);
        }
        else if (strncmp(buf, "I", 1) == 0)
        {
            displayInvalidResponseMessage(buf);
            len = recv(sockfd, buf, BUFSIZ, 0);
            buf[len] = '\0';
            displayReviewMessage(buf);
        }
        else if (strcmp(buf, "V") == 0)
        {
            printf("Good answer !!!\n\n");
            len = recv(sockfd, buf, BUFSIZ, 0);
            buf[len] = '\0';
            if (strncmp(buf, "END", 3) == 0)
            {
                displayEndReviewMessage(buf);
            }
            else
            {
                displayReviewMessage(buf);
            }
        }
        else
        {
            printf("%s\n", buf);
        }
    }
    close(sockfd);
    return (0);
}
