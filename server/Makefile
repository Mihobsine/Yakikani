SRC	=	src/main.c \
		src/cJSON.c \
		src/items.c \
		src/server.c

OBJ	=	$(SRC:.c=.o)

NAME	=	kanjiTrainerServ

CFLAGS	=	-pedantic -W -Wextra -Wall -Iinclude

all:	$(NAME)
$(NAME):	$(OBJ)
	gcc $(OBJ) -o $(NAME)
clean:
	rm -f $(OBJ)
fclean:	clean
	rm -f $(NAME)
re:	fclean all
