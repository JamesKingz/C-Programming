#include <stdio.h>

int main()
{
    int i;
    int min;
    int max;

    printf("Enter the Beginning: ");
    scanf("%d", &min);
    printf("Enter the End: ");
    scanf("%d", &max);

    if (min <= 1 && max <= 30) {
        for (i = min; i <= max; i++) {
            printf("%d\n", i);
        }
    } else {
        printf("Range must start at 1 or lower and end at 30 or higher.\n");
    }

    return 0;
}
