#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

enum status { STATUS_NORMAL, STATUS_INSERT, STATUS_QUITTING, STATUS_MAX };

const char *status_name[STATUS_MAX] = {
    [STATUS_NORMAL] = "normal",
    [STATUS_INSERT] = "insert",
    [STATUS_QUITTING] = "quitting",
};

void status_format(enum status self, FILE *f) {
	fprintf(f, "%s", status_name[self]);
}

struct state {
	size_t point;
	enum status status;
	char buf[1024];
};

const char *path = "./text.txt";

void state_init(struct state *s) {
	FILE *fp = fopen(path, "r");
	size_t n = fread(s->buf, 1, sizeof(s->buf) - 1, fp);
	s->buf[n] = '\0';
	fclose(fp);
}

void state_close(struct state *s) {
	FILE *fp = fopen(path, "w");
	size_t n = fputs(s->buf, fp);
	fclose(fp);
}

/**
 * Move the cursor. The top left is (1, 1).
 */
void move_cursor_row_col(int row, int column) {
	printf("\033[%d;%dH", row, column);
}

void move_cursor(Vec2 v) { move_cursor_row_col(v.y + 1, v.x + 1); }

void save_cursor() { printf("\0337"); }

void restore_cursor() { printf("\0338"); }

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

	for (size_t i = 0; s->buf[i] != '\0'; i++) {
		if (i == s->point) {
			save_cursor();
		}
		if (s->buf[i] == '\n') {
			putchar('\r');
		}
		putchar(s->buf[i]);
	}

	// Write status
	move_cursor(vec2_sub(d, vec2(24, 0)));
	status_format(s->status, stdout);

	// Write point
	move_cursor(vec2_sub(d, vec2(8, 0)));
	printf("%d", s->point);

	restore_cursor();

	fflush(stdout);
}

int main() {
	system("stty raw");

	struct state s = {
	    .buf = {0},
	    .point = 0,
	    .status = STATUS_NORMAL,
	};

	state_init(&s);

	while (s.status != STATUS_QUITTING) {
		present(&s);

		int c = getchar();

		if (s.status == STATUS_NORMAL) {
			if (c == 'h') {
				s.point--;
				continue;
			}
			if (c == 'l') {
				s.point++;
				continue;
			}
			if (c == 'i') {
				s.status = STATUS_INSERT;
				continue;
			}
			if (c == 'q') {
				s.status = STATUS_QUITTING;
				continue;
			}
		}
		if (s.status == STATUS_INSERT) {
			// Escape
			if (c == '\x1B') {
				s.status = STATUS_NORMAL;
				continue;
			}

			// Backspace
			if ((c == 0x7F) || (c == 0x08)) {
				if (s.point <= 0) {
					continue;
				}
				// Move all to the left
				memmove(&s.buf[s.point - 1], &s.buf[s.point],
					sizeof(s.buf) - s.point - 1);
				// Move the point
				s.point--;
				continue;
			}

			// Move all to the right
			memmove(&s.buf[s.point + 1], &s.buf[s.point],
				sizeof(s.buf) - s.point - 1);

			// Insert at the newly created empty spot
			s.buf[s.point] = c;

			// Move the point
			s.point++;

			continue;
		}
	}

	state_close(&s);
	system("stty cooked");

	return 0;
}
