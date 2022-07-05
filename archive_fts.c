#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_tar.h"
#include "dbg.h"

static char *store_name_str(int fd);
static char *store_mtime_str(int fd);
static char *store_content_str(int fd);

void set_before_final_blocks(int fd)
{
    int offset = -(BLOCKSIZE * 2); // moves fd to the position of two blocks before the EOF
    lseek(fd, offset, SEEK_END);
}

int find_nbr_of_blocks(int fd)
{
    // find number of blocks before empty blocks in fd
    int offset = -(BLOCKSIZE * 2);
    off_t location = 0;
    location = lseek(fd, offset, SEEK_END);
    int blocks = location / BLOCKSIZE;

    // reset to beginning of file
    lseek(fd, 0, SEEK_SET);

    return blocks;
}

int find_header_print_name(int fd)
{
    char *name = malloc(sizeof(char) * (NAME_FIELD_SIZE + 2)); // one for terminating NUL & one extra to check if mode field begins after name field
    name[NAME_FIELD_SIZE + 1] = '\0';

    read(fd, name, NAME_FIELD_SIZE + 1);

    // reset to beginning of header
    lseek(fd, -(NAME_FIELD_SIZE + 1), SEEK_CUR);

    if ((name[NAME_FIELD_SIZE - 1] == '\0') && (name[NAME_FIELD_SIZE] == '0'))
    {
        printf("%s\n", name);
        free(name);
        return 1;
    }
    else
    {
        free(name);
        return 0;
    }
}

t_file_list *store_name_mtime(int fd, t_file_list *head)
{    
    // read name field into string
    char *name = store_name_str(fd);

    // move forward to mtime field
    lseek(fd, BETWEEN_NAME_MTIME_FIELD, SEEK_CUR);
    
    // read mtime field into string & store as long long
    char *timeStr = store_mtime_str(fd);
    long long mtime;
    char *ptr;
    mtime = strtoll(timeStr, &ptr, 10);

    // reset fd to beginning of header for further searching (when this ft is called again)
    int reset = NAME_FIELD_SIZE + 1 + BETWEEN_NAME_MTIME_FIELD + MTIME_FIELD_SIZE;
    lseek(fd, -reset, SEEK_CUR);

    // check is name and mtime field were found, if so add node to list; name field ends at byte 100 with NUL, mode field starts with 0
    if ((name[NAME_FIELD_SIZE - 1] == '\0') && (name[NAME_FIELD_SIZE] == '0') && timeStr[0] >= '0' && timeStr[0] <= '9' && (timeStr[MTIME_FIELD_SIZE - 1] == ' '))
    {
        head = add_node_time(head, name, mtime, FILE_ARG);
        free(name);
        free(timeStr);
        return head;
    }
    else
    {
        free(name);
        free(timeStr);
        return head;
    }
}

t_file_list *store_name_content(int fd, t_file_list *head)
{
    // read name field into string
    char *name = store_name_str(fd);
    // return fd to beginning of header block
    lseek(fd, -(NAME_FIELD_SIZE + 1), SEEK_CUR);

    if ((name == NULL) || (name[0] == '\0'))
    {
        free(name);
        return head;
    }
    // if argument is folder name, it won't have content, so store only name
    if (name[my_strlen(name) - 1] == '/')
    {
        head = add_node(head, name, FILE_ARG);
        free(name);
        return head;
    }
    
    // move forward to content if it exists here (easy because header is one block)
    lseek(fd, BLOCKSIZE, SEEK_CUR);

    // read content into string
    char *content = store_content_str(fd);

    // name field ends at byte 100 with NUL, mode field starts with 0; content must end in name field (or empty blocks, but that's taken care of in caller ft)
    if ((name[NAME_FIELD_SIZE - 1] == '\0') && (name[NAME_FIELD_SIZE] == '0') /* && (content[readBytes - 1] == '\0') && (content[readBytes - 1] == '0') */)
    {
        head = add_node_content(head, name, content, FILE_ARG);
        free(name);
        free(content);
        return head;
    }
    else
    {
        free(name);
        free(content);
        return head;
    }
}

static char *store_name_str(int fd)
{
    char *name = malloc(sizeof(char) * (NAME_FIELD_SIZE + 2)); // one for terminating NUL & one extra to check if mode field begins after name field
    name[NAME_FIELD_SIZE + 1] = '\0';
    read(fd, name, NAME_FIELD_SIZE + 1);
    return name;
}

static char *store_mtime_str(int fd)
{
    char *timeStr = malloc(sizeof(char) * MTIME_FIELD_SIZE + 1);
    timeStr[MTIME_FIELD_SIZE] = '\0';
    read(fd, timeStr, MTIME_FIELD_SIZE);
    return timeStr;
}

static char *store_content_str(int fd)
{
    char *content = malloc(sizeof(char) * BLOCKSIZE * 5);
    content[(BLOCKSIZE * 5) - 1] = '\0';
    my_memset(content, '\0', (BLOCKSIZE * 5) - 1);

    char *contentBlock = malloc(sizeof(char) * BLOCKSIZE * 2);
    contentBlock[(BLOCKSIZE * 2) - 1] = '\0';
    my_memset(contentBlock, '\0', (BLOCKSIZE * 2) - 1);

    int readBytes = 0;
    int addedBytes = 0;
    int readOn = 1;

    while (readOn == 1)
    {
        addedBytes = read(fd, contentBlock, BLOCKSIZE + NAME_FIELD_SIZE + 2);
        readBytes += addedBytes;
        my_strcat(content, contentBlock);

        // debug("addedBytes: %d, readBytes: %d", addedBytes, readBytes);
        // debug("content: %s", content);

        // check if there is another header after the block
        // debug("readBytes-2: %c - readBytes-1: %c", content[readBytes - 3], content[readBytes - 2]);
        if (((content[readBytes - 3] == '\0') && (content[readBytes - 1] == '0')) || ((content[readBytes - 3] == '\0') && (content[readBytes - 1] == '\0')))
            readOn = 0;
     
    }
    // reset to beginning of content block, advancing the file is done elsewhere
    lseek(fd, -readBytes, SEEK_CUR);
    return content;
}

t_file_list *compare_times_add_newer(t_file_list *newerFiles, char *file_arg, t_file_list *fileNameList)
{
    // compare mtimes
    long archiveTime = 0;
    long addedFileTime = 0;
    
    archiveTime = octal_to_decimal(fileNameList->mtime);
    addedFileTime = check_mtime(file_arg);
    
    if (archiveTime < addedFileTime)
        newerFiles = add_node(newerFiles, file_arg, FILE_ARG);
    
    return newerFiles;
}