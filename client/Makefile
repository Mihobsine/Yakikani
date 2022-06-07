SRC	=	src/main.c \
		src/client.c

OBJ	=	$(SRC:.c=.o)

NAME	=	kanjiTrainerClient

CFLAGS	=	-pedantic -W -Wextra -Wall -Iinclude

all:	$(NAME)
$(NAME):	$(OBJ)
	gcc $(OBJ) -o $(NAME)
clean:
	rm -f $(OBJ)
fclean:	clean
	rm -f $(NAME)
re:	fclean all