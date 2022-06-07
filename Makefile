NAME	=	kanjiTrainer

all:	$(NAME)
$(NAME):
	make -C ./server
	make -C ./client
clean:
	make clean -C ./server
	make clean -C ./client
fclean:	clean
	make fclean -C ./server
	make fclean -C ./client
re:	fclean all
