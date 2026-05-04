#include <stdio.h>
#include <string.h>

int
main(void)
{
        int cnt;
        char string[10];
        
        printf("Enter a string: ");

        cnt = scanf("%10s", string);
        if (cnt == EOF) {
                fprintf(stderr, "Error during scanf.\n");
                return (1);  /* non-zero for error */
        } else if (cnt < 1) {
                fprintf(stderr, "scanf matched %d input items instead of 1.\n",
                    cnt);
                return (2);  /* non-zero for error */
        }
 
        printf("\nThe length of the string is %zu.\n",
            strlen(string));
 
        return (0);  /* no error */
}
