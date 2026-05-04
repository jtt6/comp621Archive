#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 1000

void quicksort(int data[MAXSIZE], int first, int last)
{
    int i, j, pivot, temp;

    if (first<last) {
        pivot=first;
        i=first;
        j=last;

        while (i<j)
        {
            while (data[i] <= data[pivot] && i<last) // BUG FIXED
                i++;
            while (data[j] > data[pivot])
                j--;
            if (i<j)
            {
                temp=data[i];
                data[i]=data[j];
                data[j]=temp;
            }
        }

        temp=data[pivot];
        data[pivot]=data[j];
        data[j]=temp;
        
        quicksort(data, first, j-1);
        quicksort(data, j+1, last);

   }
}

int binarysearch(int data[MAXSIZE], int search, int first, int last)
{
    int middle = (first+last)/2;
    
    while (first <= last) {
        if (data[middle] < search)
            first = middle + 1; // BUG FIXED
        else if (data[middle] == search)
            return 1;
        else
            last = middle - 1; // BUG FIXED
        middle = (first + last)/2;
    }
    return 0;
}

int main()
{
    int count, data[MAXSIZE], search;
    
    printf("Enter data (non-negative integers), terminated by a negative:\n");
    count=0;
    while (count<MAXSIZE) {
        scanf("%d", &(data[count]));
        if (data[count]<0) {
            break;
        }
        count++;
    }

    quicksort(data, 0, count-1);
        
    printf("Search for what data, terminated by a negative:\n");
    while (1) {
        scanf("%d", &search);
        if (search<0)
            break;
        else if (binarysearch(data, search, 0, count-1))
            printf("found\n");
        else
            printf("not found\n");
    }

   return 0;
}
