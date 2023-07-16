#include <stdio.h>
#include <math.h>
#include <stdlib.h>
int main()
{
    int   i = 0;
    char  a[20];
    FILE* file = fopen("a.txt", "w+");
    for (i = 0; i < 3600; i++) {
        fwrite(a, sizeof(char), sprintf(a, "%.6f,\n", sinf(i / 572.9578)), file);
    }
    fclose(file);
}