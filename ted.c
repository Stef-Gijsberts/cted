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

void present(struct state *s) {
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
        
        if (c == 'p') {
            present(&s);
        }
    }

    cleanup(&s);
    return 0;
}
