#include <stdio.h>

int
main(void)
{
        int cnt, i1, i2;
	int * pi1;
	int * pi2;
	pi1 = &i1;
	pi2 = &i2;

        printf("Enter two integers: ");

        cnt = scanf("%d, %d", pi1, pi2);
        if (cnt == EOF) {
                fprintf(stderr, "Error during scanf.\n");
                return (1);  /* non-zero for error */
        } else if (cnt < 2) {
                fprintf(stderr, "scanf matched %d input items instead of 2.\n",
                    cnt);
                return (2);  /* non-zero for error */
        }

        printf("\nThe product of %d and %d is %d.\n", i1, i2, i1 * i2);

        return (0);  /* no error */
}
