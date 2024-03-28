#include <cstdio>
#include <cstdlib>


int main(int argc, char *argv[])
{
	// Command-line arguments
	if (argc != 2)
	{
		std::fputs("Usage: filemeta <filename>\n", stderr);
		return EXIT_FAILURE;
	}

	const char *filename = argv[1];
	printf("Filename: %s\n", filename);

	return EXIT_SUCCESS;
}
