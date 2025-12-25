#include<stdio.h>
#include<stdlib.h>

struct state {
    char buf[1024];
    FILE *fp;
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

void move_cursor_to_top_left() {
    printf("\033[H");
}

void clear_screen() {
    printf("\033[2J");
}

void clear_scrollback_buffer() {
    printf("\033[3J");
}

void present(struct state *s) {
    clear_screen();
    clear_scrollback_buffer();
    move_cursor_to_top_left();
    fflush(stdout);
    puts(s->buf);
}

int main() {
    system("stty -icanon -echo");

    struct state s = {
        .buf = { 0 },
        .fp = NULL,
    };

    init(&s);

    for (;;) {
        int c = getchar();
        
        // TODO: handle commands

        present(&s);
    }

    cleanup(&s);
    return 0;
}
