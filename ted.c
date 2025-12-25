#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/ioctl.h>

struct state {
    char buf[1024];
    FILE *fp;
    size_t cursor;
};

void init(struct state *s) {
    const char *path = "./text.txt";
    s->fp = fopen(path, "r");

    size_t n = fread(s->buf, 1, sizeof(s->buf) - 1, s->fp);
    s->buf[n] = '\0';
}

void cleanup(struct state *s) {
    fclose(s->fp);
}

/**
 * Move the cursor. The top left is (1, 1).
 */
void move_cursor(int line, int column) {
    printf("\033[%d;%dH", line, column);
}

void clear_screen() {
    printf("\033[2J");
}

void clear_scrollback_buffer() {
    printf("\033[3J");
}

void present(struct state *s) {
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        w.ws_col = 100;
        w.ws_row = 24;
    }

    clear_screen();
    clear_scrollback_buffer();
    move_cursor(1, 1);
    puts(s->buf);

    move_cursor(w.ws_row, w.ws_col - 10);
    printf("== %d ==", s->cursor);

    fflush(stdout);
}

int main() {
    system("stty -icanon -echo");

    struct state s = {
        .buf = { 0 },
        .fp = NULL,
        .cursor = 0,
    };

    init(&s);

    for (;;) {
        int c = getchar();
        
        if (c == 'h') {
            s.cursor--;
        }
        if (c == 'l') {
            s.cursor++;
        }

        present(&s);
    }

    cleanup(&s);
    return 0;
}
