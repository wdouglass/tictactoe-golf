#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>

static void small_putchar(const char x) {
    syscall(SYS_write, (unsigned int)STDOUT_FILENO, &x, (size_t)1);
}

static char small_getchar(void) {
    char x;
    syscall(SYS_read, (unsigned int)STDIN_FILENO, &x, (size_t)1);
    return x;
}

static char small_exit(void) {
    syscall(SYS_exit, 0);
}

static uint16_t X, O;

static void horizontal(void) {
    small_putchar('-');
    for(int i = 0; i < 2; i++) {
        small_putchar('+');
        small_putchar('-');
    }
    small_putchar('\n');
}

static void render(void) {
    for(int i = 0; i < 9; i++) {
        small_putchar((X & 1<<i)?'X':((O & 1<<i)?'O':'1' + i));
        if ((i + 1) % 3 == 0) {
            small_putchar('\n');
            if (i < 8) {
                horizontal();
            }
        }
        else
            small_putchar('|');
    }
}

static bool legal(int in) {
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

#define POS(x) (1 << (x))

static bool win(int16_t in) {
    const uint16_t w[] = {
        POS(0)|POS(1)|POS(2),
        POS(3)|POS(4)|POS(5),
        POS(6)|POS(7)|POS(8),
        POS(0)|POS(3)|POS(6),
        POS(1)|POS(4)|POS(7),
        POS(2)|POS(5)|POS(8),
        POS(0)|POS(4)|POS(8),
        POS(2)|POS(4)|POS(6)};
    for(int i = 0; i < 8; i++) {
        if ((in & w[i]) == w[i]) {
            return true;
        }
    }
    return false;
}

static void end(const char winner) {
    const char *w = " Wins\n";
    small_putchar(winner);
    for (const char *i = w; *i != '\0'; i++) {
        small_putchar(*i);
    }
}

static void player(void) {
    const int m = small_getchar();
    for (int w = small_getchar(); w != '\n'; w = small_getchar());
    if (!legal(m)) {
        const char *invalid = "Invalid Move";
        for (const char *i = invalid; *i != '\n'; i++)
            small_putchar(*i);
        return;
    }
    X |= (1 << (m - '1'));
}

static int about_to_win(int us, int them) {
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
                          
static int offense(void) {
    return about_to_win(O, X);
}

static int defense(void) {
    return about_to_win(X, O);
}

static int any(void) {
    //we're desperate. find a place to move
    const uint16_t moves[] = {4, 0, 2, 6, 8, 1, 3, 5, 7};

    for (int i = 0; i < 9; i++) { 
        if (!((O | X) & POS(moves[i]))) {
            return moves[i];
        }
    }
    return -1;
}

static void move(void) {
    typedef int (*ptr)(void);
    const ptr o[3] = {offense, defense, any};
    for (int i = 0; i < 3; i++) {
        int p = o[i]();
        if (p >= 0) {
            O |= 1<<p;
            return;
        }
    }
    small_exit();
}

int main(void) {
    const char *tie = "Tie\n";
    while(true) {
        render();
        if (win(X)) {
            end('X');
            small_exit();
        }
        if (win(O)) {
            end('O');
            small_exit();
        }
        player();
        if ((X | O) == 0x1FF) {
            for (const char *i = tie; *i != '\0'; i++) {
                small_putchar(*i);
            }
            small_exit();
        }
        move();
    }
    render();
    small_exit();
}
