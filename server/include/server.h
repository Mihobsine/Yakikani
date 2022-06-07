#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "items.h"

typedef enum
{
    meaning,
    reading,
} review_type;

int run_server(int argc, char *argv[]);

#endif /* !SERVER_H_ */
