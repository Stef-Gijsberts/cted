ted: ted.o gapbuffer.c

clean:
	rm -f *.o ted run-tests

format:
	clang-format -style=file -i *.c *.h

test: run-tests
	./run-tests

.PHONY: clean format test
