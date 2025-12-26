#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

typedef struct {
	int x;
	int y;
} Vec2;

const Vec2 vec2_origin = {
    .x = 0,
    .y = 0,
};

void vec2_format(Vec2 self, FILE *f) { fprintf(f, "(%d, %d)", self.x, self.y); }

Vec2 vec2_add(Vec2 l, Vec2 r) {
	Vec2 result = {
	    .x = l.x + r.x,
	    .y = l.y + r.y,
	};
	return result;
}

Vec2 vec2_sub(Vec2 l, Vec2 r) {
	Vec2 result = {
	    .x = l.x - r.x,
	    .y = l.y - r.y,
	};
	return result;
}

Vec2 vec2(int x, int y) {
	Vec2 result = {
	    .x = x,
	    .y = y,
	};
	return result;
}

enum status { STATUS_INITIAL, STATUS_QUITTING };

struct state {
	Vec2 cursor;
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
void move_cursor_row_col(int row, int column) {
	printf("\033[%d;%dH", row, column);
}

void move_cursor(Vec2 v) { move_cursor_row_col(v.y + 1, v.x + 1); }

void clear_screen() { printf("\033[2J"); }

void clear_scrollback_buffer() { printf("\033[3J"); }

Vec2 get_dimensions() {
	struct winsize w;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
		w.ws_col = 100;
		w.ws_row = 24;
	}

	return vec2(w.ws_col, w.ws_row);
}

void present(struct state *s) {
	Vec2 d = get_dimensions();

	clear_screen();
	clear_scrollback_buffer();
	move_cursor(vec2_origin);
	puts(s->buf);

	move_cursor(vec2_sub(d, vec2(10, 0)));
	vec2_format(s->cursor, stdout);

	move_cursor(s->cursor);

	fflush(stdout);
}

int main() {
	system("stty -icanon -echo");

	struct state s = {
	    .buf = {0},
	    .fp = NULL,
	    .cursor = vec2_origin,
	    .status = STATUS_INITIAL,
	};

	init(&s);

	while (s.status != STATUS_QUITTING) {
		present(&s);

		int c = getchar();
		if (c == 'h') {
			s.cursor = vec2_add(s.cursor, vec2(-1, 0));
		}
		if (c == 'j') {
			s.cursor = vec2_add(s.cursor, vec2(0, 1));
		}
		if (c == 'k') {
			s.cursor = vec2_add(s.cursor, vec2(0, -1));
		}
		if (c == 'l') {
			s.cursor = vec2_add(s.cursor, vec2(1, 0));
		}
		if (c == 'q') {
			s.status = STATUS_QUITTING;
		}
	}

	cleanup(&s);
	system("stty icanon echo");

	return 0;
}
