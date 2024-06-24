#include <iostream>

#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

class Editor {
    public:
        static struct termios orig_termios;

};


void die(const char *str)
{
    perror(str);
    exit(1);
}

void disableRawMode(void)
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &(Editor::orig_termios)) == -1)
        die("tcsetattr");
}

void enableRawMode(void)
{
    tcgetattr(STDIN_FILENO, &(Editor::orig_termios));
    atexit(disableRawMode);
    struct termios raw = Editor::orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}


void editorDrawRows(void)
{
    int y;

    for (y = 0; y < 55; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}


void refresh_screen(void)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void handle_keyboard(void)
{
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1) die("read");
    if (iscntrl(c)) {
        printf("%d\r\n", c);
    } else {
        printf("%d ('%c')\r\n", c, c);
    }
    if (c == CTRL_KEY('q'))
        exit(0);
}

int main()
{




    enableRawMode();
    while (1) {
        refresh_screen();
        handle_keyboard();
    }
    return 0;
}