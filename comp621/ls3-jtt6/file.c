#include <stdio.h>

/*
 * Requires:
 *   Nothing.
 *
 * Effects:
 *   Tries to copy the contents of the file "input.txt" to the file
 *   "output.txt".  Returns 0 if the copy completed successfully.
 *   Otherwise, returns 1.
 */
int
main(void)
{
	FILE *input_file, *output_file;
	int c, error = 0; /* no error */
	char *input_filename = "input.txt";
	char *output_filename = "output.txt";

	input_file = fopen(input_filename, "r");
	if (input_file == NULL) {
		fprintf(stderr, "Can't open %s.\n", input_filename);
		return (1); /* non-zero for error */
	}
	output_file = fopen(output_filename, "w");
	if (output_file == NULL) {
		fprintf(stderr, "Can't open %s.\n", output_filename);
		fclose(input_file);
		return (1); /* non-zero for error */
	}
	while ((c = fgetc(input_file)) != EOF)
		fputc(c, output_file);
	if (!feof(input_file)) {
		/*
		 * If feof() returns FALSE, then the above while loop
		 * didn't reach the end of file.  The EOF returned by
		 * fgetc() instead meant that an error occurred while
		 * reading from the input file.
		 */
		fprintf(stderr, "An error occurred reading %s.\n",
			input_filename);
		error = 1; /* non-zero for error */
	}
	fclose(input_file);
	fclose(output_file);
	return (error);
}
