#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static uint16_t X, O;

void horizontal(void) {
    putchar('-');
    for(int i = 0; i < 2; i++) {
        putchar('+');
        putchar('-');
    }
    putchar('\n');
}

void render(void) {
    for(int i = 0; i < 9; i++) {
        putchar((X & 1<<i)?'X':((O & 1<<i)?'O':'1' + i));
        if ((i + 1) % 3 == 0) {
            putchar('\n');
            if (i < 8) {
                horizontal();
            }
        }
        else
            putchar('|');
    }
}

bool legal(int in) {
    in -= '1';
    if (in < 0)
        return false;
    if (in > 8)
        return false;
    if ((1 << in) & X)
        return false;
    if ((1 << in) & O)
        return false;
    return true;
}

bool win(int16_t in) {
    const uint16_t w[] = {0x7, 0x38, 0x160,
                          0x49, 0x92, 0x124,
                          0x111, 0x54};
    for(int i = 0; i < 8; i++) {
        if (in == w[i]) {
            return true;
        }
    }
    return false;
}

void end(const char winner) {
    const char *w = " Wins\n";
    putchar(winner);
    for (const char *i = w; *i != '\0'; i++) {
        putchar(*i);
    }
}

void player(void) {
    const int m = getchar();
    for (int w = getchar(); w != '\n'; w = getchar());
    if (!legal(m)) {
        const char *invalid = "Invalid Move";
        for (const char *i = invalid; *i != '\n'; i++)
            putchar(*i);
        return;
    }
    X |= (1 << (m - '1'));
}

#define POS(x) (1 << (x))

int about_to_win(int us, int them) {
    //check the corners first

#define CHECK(y,n)                                              \
    if ((us & POS(y)) && !((us & POS(n)) | (them & POS(n))))    \
        return n;                                               \
    if ((us & POS(n)) && !((us & POS(y)) | (them & POS(y))))    \
        return y;
        
    //check the corners first
    if (us & POS(0)) {
        CHECK(1, 2);
        CHECK(4, 8);
        CHECK(3, 6);
    }   
    if (us & POS(2)) {
        CHECK(1, 0);
        CHECK(4, 6);
        CHECK(5, 8);
    }   
    if (us & POS(6)) {
        CHECK(3, 0);
        CHECK(4, 2);
        CHECK(7, 8);
    }   
    if (us & POS(8)) {
        CHECK(5, 2);
        CHECK(4, 0);
        CHECK(7, 6);
    }
    //check the edges
    if (us & POS(1)) {
        CHECK(4, 7);
    }   
    if (us & POS(3)) {
        CHECK(4, 5);
    }   
    if (us & POS(5)) {
        CHECK(4, 3);
    }   
    if (us & POS(7)) {
        CHECK(4, 1);
    }
    return -1;
#undef CHECK
}
                          
int offense(void) {
    return about_to_win(O, X);
}

int defense(void) {
    return about_to_win(X, O);
}

int any(void) {
    //we're desperate. find a place to move
    const uint16_t moves[] = {4, 0, 2, 6, 8, 1, 3, 5, 7};

    for (int i = 0; i < 9; i++) { 
        if (!((O | X) & POS(moves[i]))) {
            return moves[i];
        }
    }
    return -1;
}

void move(void) {
    typedef int (*ptr)(void);
    const ptr o[3] = {offense, defense, any};
    for (int i = 0; i < 3; i++) {
        int p = o[i]();
        if (p >= 0) {
            O |= 1<<p;
            return;
        }
    }
    abort();
}

int main(int argc, char *argv) {
    const char *tie = "Tie\n";
    while(true) {
        render();
        if (win(X)) {
            end('X');
            return 0;
        }
        if (win(O)) {
            end('O');
            return 0;
        }
        player();
        if ((X | O) == 0x1FF) {
            for (const char *i = tie; *i != '\0'; i++) {
                putchar(*i);
            }
            return 0;
        }
        move();
    }
    render();
    return 0;
}
