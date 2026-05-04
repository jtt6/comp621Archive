#include <stdio.h>

int
main(int argc, char *argv[])
{
        FILE *input_file, *output_file;
        int error, number, sum = 0;
        char *input_filename, *output_filename = "SUM.bin";

	/* Filename must be the only argument. */
        if (argc == 2)
                input_filename = argv[argc - 1];
        else {
                fprintf(stderr, "Wrong number of arguments.\n");
                return (1);
        }

        input_file = fopen(input_filename, "r");
        if (input_file == NULL) {
                fprintf(stderr, "Can't open %s.\n", input_filename);
                return (1);  /* non-zero for error */
        }
 
        output_file = fopen(output_filename, "w");
        if (output_file == NULL) {
                fclose(input_file);
                fprintf(stderr, "Can't open %s.\n", output_filename);
                return (1);  /* non-zero for error */
        }
 
        while ((number = getw(input_file)) != EOF)
                sum += number;

        printf("The sum is %d.\n", sum);
 
        if (putw(sum, output_file) == EOF) {
                fprintf(stderr, "Unable to write sum.\n");
		error = 1;
        } else
		error = 0;
 
        fclose(input_file);
        fclose(output_file);
 
        return (error);
}
