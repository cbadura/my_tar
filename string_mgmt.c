#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "dbg.h"

int my_strlen(char *str)
{
    int counter = 0;
    while(str[counter] != '\0')
        counter++;    
    return counter;
}

char *my_strcpy(char *dst, char *src)
{
    // create a pointer pointing to the beginning of the destination string's address
    char *ptr = dst;

    while(*src != '\0')
    {
        *dst = *src;
        src++;
        dst++; //
    }
    *dst = *src; 
    
    // or simply
    // while ((*dst++ = * src++)); // extra parentheses around assignment to silence warning
    return ptr;
}

char *my_strdup(char *str)
{
    int size;
    char *dupStr;
    size = my_strlen(str) + 1;

    dupStr = (char*)malloc(sizeof(char) * size);
    if(dupStr == NULL)
        return NULL;
    
    for(int i = 0; i < size; i++)
        dupStr[i] = str[i];
    dupStr[size-1] = '\0';
    
    return dupStr;   
}

int my_strcmp(char *s1, char *s2) 
{
    while (*s1)
    {
        // if characters differ, or end of the second string is reached
        if (*s1 != *s2)
            break;

        // move to the next pair of characters
        s1++;
        s2++;
        // return the ASCII difference after converting `char*` to `unsigned char*`
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}                  

void my_strcat(char *dest, char *src)
{
    while(*dest) dest++;
    while((*dest++ = *src++));
    --dest;
}

int my_putchar(char c)
{
    return write(1, &c, 1);
}

void my_putstr(char *string)
{
    while(*string)
    { 
        my_putchar(*string);
        string++;
    }
}

void my_memset(char *str, char c, int n)
{
    int i = 0;
    for (; i < n; i++)
        str[i] = c; 
    // str[i] = '\0';
}
