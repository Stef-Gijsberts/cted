#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

enum status { STATUS_INITIAL, STATUS_QUITTING };

struct state {
	size_t cursor;
	FILE *fp;
	enum status status;
	char buf[1024];
};

void init(struct state *s) {
	const char *path = "./text.txt";
	s->fp = fopen(path, "r");

	size_t n = fread(s->buf, 1, sizeof(s->buf) - 1, s->fp);
	s->buf[n] = '\0';
}

void cleanup(struct state *s) { fclose(s->fp); }

/**
 * Move the cursor. The top left is (1, 1).
 */
void move_cursor(int row, int column) { printf("\033[%d;%dH", row, column); }

void clear_screen() { printf("\033[2J"); }

void clear_scrollback_buffer() { printf("\033[3J"); }

struct dimensions {
	int cols;
	int rows;
};

struct dimensions get_dimensions() {
	struct winsize w;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
		w.ws_col = 100;
		w.ws_row = 24;
	}

	struct dimensions result = {.cols = w.ws_col, .rows = w.ws_row};

	return result;
}

void present(struct state *s) {
	struct dimensions d = get_dimensions();

	clear_screen();
	clear_scrollback_buffer();
	move_cursor(1, 1);
	puts(s->buf);

	move_cursor(d.rows, d.cols - 10);
	printf("== %d ==", s->cursor);

	move_cursor(1, s->cursor);

	fflush(stdout);
}

int main() {
	system("stty -icanon -echo");

	struct state s = {
	    .buf = {0},
	    .fp = NULL,
	    .cursor = 0,
	    .status = STATUS_INITIAL,
	};

	init(&s);

	while (s.status != STATUS_QUITTING) {
		int c = getchar();

		if (c == 'h') {
			s.cursor--;
		}
		if (c == 'l') {
			s.cursor++;
		}
		if (c == 'q') {
			s.status = STATUS_QUITTING;
		}

		present(&s);
	}

	cleanup(&s);
	system("stty icanon echo");

	return 0;
}
