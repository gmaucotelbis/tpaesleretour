FILES=second_preim_48_fillme.c xoshiro256starstar.c
FLAGS= -std=c99
.SILENT: ex1 ex1q1 ex1q2 ex1q3 ex1q4

all: ex1 ex2

ex1: ex1q1 ex1q2 ex1q3 ex1q4

ex2: ex2q1

ex1q1:ex1q1.c
	gcc ex1q1.c -o ex1q1 $(FILES) $(FLAGS)
	./ex1q1

ex1q2:ex1q2.c
	gcc ex1q2.c -o ex1q2 $(FILES) $(FLAGS)
	./ex1q2

ex1q3:ex1q3.c
	gcc ex1q3.c -o ex1q3 $(FILES) $(FLAGS)
	./ex1q3

ex1q4:ex1q4.c
	gcc ex1q4.c -o ex1q4 $(FILES) $(FLAGS)
	./ex1q4

ex2q1:ex2q1.c
	gcc ex2q1.c -o ex2q1 $(FILES) $(FLAGS)
	./ex2q1

clean:
	rm -f ex1q1
	rm -f ex1q2
	rm -f ex1q3
	rm -f ex1q4
	rm -f ex2q1
	rm -f *.o
