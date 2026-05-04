#include <stdio.h>

int
main(void)
{
        char input[10];

        while (fgets(input, 10, stdin) != NULL) {
                puts(input);
        }

        return (0);  /* no error */
}
