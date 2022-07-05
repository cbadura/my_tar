#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "my_tar.h"
#include "dbg.h"

static int my_iterative_pow(int nbr, int pwr);
static char *reverse_string(char *word);

/*
*** utility functions ***
*/

int not_just_dots(char *str)
{
    int check = 0; 
    check = my_strcmp(str, ".") != 0 && my_strcmp(str, "..") != 0;
    return check;  // 0 if just dots, 1 if not just dots
}

long long check_mtime(char *file)
{
    struct stat filestat;
    if (lstat(file, &filestat) == -1)
    {
        printf("%s", STAT_FAIL);
        return -1;
    }
    // printf("time: %f\n", (filestat.st_mtimespec.tv_sec*1e+9) + (filestat.st_mtimespec.tv_nsec));
    // printf("time: %f\n", (filestat.st_mtim.tv_sec*1e+9) + filestat.st_mtim.tv_nsec));

    return (filestat.st_mtimespec.tv_sec);
    // return((filestat.st_mtim.tv_sec*1e+9) + filestat.st_mtim.tv_nsec);
}

long octal_to_decimal(long nbr)
{
    int power = 0, decimal = 0, r;
    
    while (nbr > 0)
    {
        // retrieving the right-most digit of n
        r = nbr % 10;
        
        // dividing n by 10 to remove the right-most digits, already scanned in previous step
        nbr = nbr / 10;
        
        decimal = decimal + r * my_iterative_pow(8, power);    
        ++power; 
    }
    return decimal;
}

static int my_iterative_pow(int nbr, int pwr)
{
    if(pwr < 0) return 0;
    if(pwr == 0) return 1;

    int base = nbr;
    while(pwr > 1) {
        nbr *= base;
        pwr--;
    }
    return nbr;
}

/*
*** added string functions ***
*/

char *dec_to_octal(int dec)
{
    if (dec == 0) return NULL;

    int len = 0;
    int baseLen = 8;
    char digits[] = "01234567";

    long numCopy = dec; // for handling it in the next steps (and only there)

    // find length of num in the num system (according to baseLen) for malloc!!
    do {
        len++;
        numCopy /= baseLen;
    } while (numCopy != 0);

    char* buffer = malloc(sizeof(int) * len);

    // add null terminator, then reverse string
    buffer[len] = '\0';
    len--;

    while (dec != 0) {
        buffer[len] = digits[dec % baseLen];
        dec /= baseLen;
        len--;
    }
    return buffer;
}

char *my_itoa(int num)
{
    int len = 0;
    long numCopy = num;
    do {
    len++;
    numCopy /= 10;  // for decimal nbrs
    } while (numCopy != 0);

    char* str = malloc(sizeof(int) * len);
    int i = 0;
 
    // handle 0 explicitly, otherwise empty string is printed for 0
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // process individual digits
    while (num != 0)
    {
        int rem = num % 10;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/10;
    }
    str[i] = '\0';
 
    reverse_string(str);
    return str;
}

static char *reverse_string(char *word)
{
    int length, c;
    char *begin, *end;
    
    length = my_strlen(word);
    begin  = word;  // gives begin the same address as word
    end    = word;  // gives end the same address as word
    
    for (c = 0; c < length - 1; c++)
        end++;

    for (c = 0; c < length/2; c++)
    {      
        char temp;
        temp   = *end;
        *end   = *begin;
        *begin = temp;
    
        begin++;
        end--;
    }
    return word;
}

/* 
*** list functions ***
 */

t_file_list *add_node(t_file_list *head, char *name, file_type type)
{
    t_file_list *new = malloc(sizeof(t_file_list));
    new->file_arg = malloc(sizeof(char) * my_strlen(name) + 1);
    my_strcpy(new->file_arg, name);
    new->type = type;
    new->next = NULL;

    if (head == NULL)
    {
        head = new;
        return head;
    }
    // avoid dereferencing head
    t_file_list *tmp = head;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new;
    return head;
}

t_file_list *add_node_time(t_file_list *head, char *name, long long mtime, file_type type)
{
    t_file_list *new = malloc(sizeof(t_file_list));
    new->file_arg = malloc(sizeof(char) * my_strlen(name) + 1);
    my_strcpy(new->file_arg, name);
    new->mtime = mtime;
    new->type = type;
    new->next = NULL;

    if (head == NULL)
    {
        head = new;
        return head;
    }
    // avoid dereferencing head
    t_file_list *tmp = head;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new;
    return head;
}

t_file_list *add_node_content(t_file_list *head, char *name, char *archiveContent, file_type type)
{
    t_file_list *new = malloc(sizeof(t_file_list));
    new->file_arg = malloc(sizeof(char) * my_strlen(name) + 1);
    my_strcpy(new->file_arg, name);
    new->content = malloc(sizeof(char) * my_strlen(archiveContent) + 1);
    my_strcpy(new->content, archiveContent);
    new->type = type;
    new->next = NULL;

    if (head == NULL)
    {
        head = new;
        return head;
    }
    // avoid dereferencing head
    t_file_list *tmp = head;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new;
    return head;
}

int search_list_for_name(t_file_list *head, char *name)
{
    int match = 0;
    t_file_list *current = head;

    while (current != NULL)
    {
        if (my_strcmp(current->file_arg, name) == 0)
            match = 1;
        
        current = current->next;
    }
    return match;
}

void print_list(t_file_list *head)
{
    t_file_list *current = head;
    while (current != NULL)
    {
        printf("%s - ", current->file_arg);
        current = current->next;
    }
    printf("\n");
}