# Make File to get the build process done easily
$(CC) = gcc
build:
	$(CC) core_main.c -g -pthread -o core_main
clean :
	rm *.exe
	rm patient_Record.txt
	