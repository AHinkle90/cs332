#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int sumOfDigits(int n)
{
    int sum = 0;

    if (n <= 0)
        return -1;

    while (n > 0)
    {
        sum = sum + (n % 10);
        n = n / 10;
    }

    return sum;
}

int UABMaxMinDiff(int arr[], int size)
{
    int max = arr[0];
    int min = arr[0];

    for (int i = 1; i < size; i++)
    {
        if (arr[i] > max)
            max = arr[i];

        if (arr[i] < min)
            min = arr[i];
    }

    return max - min;
}

void replaceEvenWithZero(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] % 2 == 0)
            arr[i] = 0;
    }
}

bool perfectSquare(int n)
{
    if (n < 0)
        return false;

    for (int i = 0; i * i <= n; i++)
    {
        if (i * i == n)
            return true;
    }

    return false;
}

int countVowels(char s[])
{
    int count = 0;

    for (int i = 0; i < strlen(s); i++)
    {
        if (s[i] == 'a' || s[i] == 'A' ||
            s[i] == 'e' || s[i] == 'E' ||
            s[i] == 'i' || s[i] == 'I' ||
            s[i] == 'o' || s[i] == 'O' ||
            s[i] == 'u' || s[i] == 'U')
        {
            count++;
        }
    }

    return count;
}

void printArray(int arr[], int size)
{
    printf("[");

    for (int i = 0; i < size; i++)
    {
        printf("%d", arr[i]);

        if (i < size - 1)
            printf(", ");
    }

    printf("]\n");
}

int main()
{
    /* sumOfDigits */
    printf("sumOfDigits(123) = %d\n", sumOfDigits(123));
    printf("sumOfDigits(405) = %d\n", sumOfDigits(405));
    printf("sumOfDigits(0) = %d\n", sumOfDigits(0));
    printf("sumOfDigits(7) = %d\n", sumOfDigits(7));
    printf("sumOfDigits(-308) = %d\n\n", sumOfDigits(-308));

    /* UABMaxMinDiff */
    int arr1[] = {3, 7, 2, 9};
    printf("UABMaxMinDiff(arr1) = %d\n", UABMaxMinDiff(arr1, 4));
    
    int arr2[] = {5, 5, 5, 5, 5, 5};
    printf("UABMaxMinDiff(arr2) = %d\n", UABMaxMinDiff(arr2, 6));
    
    int arr3[] = {-2, 4, -1, 6, 5};
    printf("UABMaxMinDiff(arr3) = %d\n\n", UABMaxMinDiff(arr3, 5));

    /* replaceEvenWithZero */
    int arr4[] = {1, 2, 3, 4};
    replaceEvenWithZero(arr4, 4);
    printf("arr4 after replaceEvenWithZero: ");
    printArray(arr4, 4);

    int arr5[] = {2, 4, 6};
    replaceEvenWithZero(arr5, 3);
    printf("arr5 after replaceEvenWithZero: ");
    printArray(arr5, 3);

    int arr6[] = {1, 3, 5};
    replaceEvenWithZero(arr6, 3);
    printf("arr6 after replaceEvenWithZero: ");
    printArray(arr6, 3);

    /* perfectSquare */
    printf("\nperfectSquare(16) = %s\n", perfectSquare(16) ? "True" : "False");
    printf("perfectSquare(15) = %s\n", perfectSquare(15) ? "True" : "False");
    printf("perfectSquare(25) = %s\n", perfectSquare(25) ? "True" : "False");
    printf("perfectSquare(36) = %s\n\n", perfectSquare(36) ? "True" : "False");

    /* countVowels */
    printf("countVowels(\"Hello World\") = %d\n", countVowels("Hello World"));
    printf("countVowels(\"UAB CS\") = %d\n", countVowels("UAB CS"));
    printf("countVowels(\"Python\") = %d\n", countVowels("Python"));
    printf("countVowels(\"aeiou\") = %d\n", countVowels("aeiou"));

    return 0;
}