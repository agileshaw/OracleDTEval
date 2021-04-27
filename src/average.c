#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    char line[1024];
    double tmp, total = 0;
    int count = 0;

    if (argc < 2) {
        printf("No file input!\n");
        printf("Usage: ./average [filename]\n");
        exit(1);
    }

    FILE *file = fopen(argv[1], "r+");
    if (file == NULL) {
        printf("Cannot open file: %s\n", argv[1]);
        exit(1);
    }

    while (fgets(line, 1024, file)) {
        if (line[0] != '\n') {
            tmp = strtof(line, NULL);
            total += tmp;
            count++;
        }
    }

    printf("%f\n", total/(double)count);

    return 0;
}
