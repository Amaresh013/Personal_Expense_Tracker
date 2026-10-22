# Makefile: type "make" to build, "make clean" to remove built files.
CC = gcc
CFLAGS = -Wall -Wextra
OBJS = main.o input.o storage.o expense.o report.o category.o auth.o

expense_tracker: $(OBJS)
	$(CC) $(CFLAGS) -o expense_tracker $(OBJS)

%.o: %.c expense.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o expense_tracker expenses.dat users.dat categories.dat
