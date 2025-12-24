ted: gapbuffer.o

clean:
	rm -f *.o ted

format:
	clang-format -style=file -i *.c *.h

.PHONY: clean format
