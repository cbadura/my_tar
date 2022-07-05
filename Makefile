NAME = my_tar

CFLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address # -O2 --pedantic

SRC = main.c \
	tar_options.c \
	write_fts.c \
	header_strings.c \
	parsing.c \
	archive_fts.c \
	helpers.c \
	string_mgmt.c \

CC = gcc
OBJECTS = $(SRC:.c=.o)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)

clean:
	rm -rf $(OBJECTS)

fclean:
	rm -f $(NAME) clean