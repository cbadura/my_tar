#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "my_tar.h"
#include "dbg.h"

static char *write_name(int fd, header_t *header, char *file, int filemode);
static char *write_mode(int fd, header_t *header, char *file, int filemode);
static char *write_uid(int fd, header_t *header, char *file, int filemode);
static char *write_gid(int fd, header_t *header, char *file, int filemode);
static char *write_size(int fd, header_t *header, char *file, int filemode);
static char *write_mtime(int fd, header_t *header, char *file, int filemode);
static char *write_chksum(int fd, header_t *header, int filemode);
static char *write_typeflag(int fd, header_t *header, char *file, int filemode);
static void fill_header(int fd);
static char *write_content(int fd, char *file, int filemode);
static void write_empty_blocks(int fd);

void write_file(int fd, header_t *header, t_file_list *head, int filemode)
{
    // write header info & file content to archive
    for (; head; head = head->next)
    {
        write_name(fd, header, head->file_arg, filemode);
        write_mode(fd, header, head->file_arg, filemode);
        write_uid(fd, header, head->file_arg, filemode);
        write_gid(fd, header, head->file_arg, filemode);
        write_size(fd, header, head->file_arg, filemode);
        write_mtime(fd, header, head->file_arg, filemode);
        write_chksum(fd, header, filemode);
        write_typeflag(fd, header, head->file_arg, filemode);
        fill_header(fd);
        write_content(fd, head->file_arg, filemode);
    }
    write_empty_blocks(fd);
}

void print_file(int fd, header_t *header, t_file_list *head, int filemode)
{
    // determine len of list for malloc
    int listLen = 0;
    t_file_list *tmp = head;
    for (; tmp; tmp = tmp->next)  
        listLen++;

    char *headerStr = malloc(sizeof(char) * (HEADER_LEN + 1)); // at the end: added '\n's at end of file entries and end of tar printout
    char *contentStr = malloc(sizeof(char) * (HEADER_LEN + (BLOCKSIZE * 5) + 1)); // at the end: added '\n's at end of file entries and end of tar printout

    // print header info & file content to terminal
    for (; head; head = head->next)  
    {
        headerStr = write_name(fd, header, head->file_arg, filemode);
        
        if (write_mode(fd, header, head->file_arg, filemode) == NULL) return;

        my_strcat(headerStr, write_mode(fd, header, head->file_arg, filemode));
        my_strcat(headerStr, write_uid(fd, header, head->file_arg, filemode));
        my_strcat(headerStr, write_gid(fd, header, head->file_arg, filemode));
        my_strcat(headerStr, write_size(fd, header, head->file_arg, filemode));
        my_strcat(headerStr, write_mtime(fd, header, head->file_arg, filemode));
        my_strcat(headerStr, write_chksum(fd, header, filemode));
        my_strcat(headerStr, write_typeflag(fd, header, head->file_arg, filemode));
        printf("%s", headerStr);
        contentStr = write_content(fd, head->file_arg, filemode);
        my_strcat(contentStr, "\n");
        printf("%s", contentStr);
    }
    // free(headerStr);
    free(contentStr);
}

static char *write_name(int fd, header_t *header, char *file, int filemode)
{
    // set name field to NULL BYTES
    my_memset(header->name, '\0', NAME_FIELD_SIZE);
    // copy file name to header fied
    int i = 0;
    for (; file[i] != '\0'; i++)
        header->name[i] = file[i];

    if (filemode == FILE_OFF)
        return header->name;

    if (filemode == FILE_ON)
    {
        write(fd, header->name, NAME_FIELD_SIZE);
        return NULL;
    }
    return NULL;
}

static char *write_mode(int fd, header_t *header, char *file, int filemode)
{
    char *mode = build_field_string(file, MODE);
    if (mode == NULL)
        return NULL;
    // set mode field
    int i = 0;
    for (; mode[i + 2] != '\0'; i++) // too many 0s in the string, cut 2 off
        header->mode[i] = mode[i + 2];
    header->mode[i] = ' ';
    i++;
    header->mode[i] = '\0';
    free(mode);

    if (filemode == FILE_OFF)
        return header->mode;

    if (filemode == FILE_ON)
    {
        write(fd, header->mode, my_strlen(header->mode) + 1);
        return NULL;
    }
    return NULL;
}

static char *write_uid(int fd, header_t *header, char *file, int filemode)
{
    char *uid = build_field_string(file, UID);
    if (uid == NULL)
        return NULL;
    // set uid field
    int i = 0;
    for (; uid[i] != '\0'; i++)
        header->uid[i] = uid[i];
    header->uid[i] = ' ';
    i++;
    header->uid[i] = '\0';
    free(uid);

    if (filemode == FILE_OFF)
        return header->uid;

    if (filemode == FILE_ON)
    {
        write(fd, header->uid, my_strlen(header->uid) + 1);
        return NULL;
    }
    return NULL;
}

static char *write_gid(int fd, header_t *header, char *file, int filemode)
{
    char *gid = build_field_string(file, GID);
    if (gid == NULL)
        return NULL;
    // set gid field
    int i = 0;
    for (; gid[i] != '\0'; i++)
        header->gid[i] = gid[i];
    header->gid[i] = ' ';
    i++;
    header->gid[i] = '\0';
    free(gid);

    if (filemode == FILE_OFF)
        return header->gid;

    if (filemode == FILE_ON)
    {
        write(fd, header->gid, my_strlen(header->gid) + 1);
        return NULL;
    }
    return NULL;
}

static char *write_size(int fd, header_t *header, char *file, int filemode)
{
    char *size = build_field_string(file, SIZE);
    if (size == NULL)
        return NULL;
    // set size field
    int i = 0;
    for (; size[i] != '\0'; i++)
        header->size[i] = size[i];
    header->size[i] = ' ';
    i++;
    header->size[i] = '\0';
    free(size);

    if (filemode == FILE_OFF)
        return header->size;

    if (filemode == FILE_ON)
    {
        write(fd, header->size, my_strlen(header->size) + 1);
        return NULL;
    }
    return NULL;
}

static char *write_mtime(int fd, header_t *header, char *file, int filemode)
{
    char *time = build_field_string(file, MTIME);
    if (time == NULL)
        return NULL;
    // set mtime field
    int i = 0;
    for (; time[i] != '\0'; i++)
        header->mtime[i] = time[i];
    header->mtime[i] = ' ';
    i++;
    header->mtime[i] = '\0';
    free(time);

    if (filemode == FILE_OFF)
        return header->mtime;

    if (filemode == FILE_ON)
    {
        write(fd, header->mtime, my_strlen(header->mtime) + 1);
        return NULL;
    }
    return NULL;
}

static char *write_chksum(int fd, header_t *header, int filemode)
{
    char *chksum = build_chksum_string(header, filemode);
    // set chksum field
    int i = 0;
    for (; chksum[i] != '\0'; i++)
        header->chksum[i] = chksum[i];
    header->chksum[i] = ' ';
    i++;
    header->chksum[i] = '\0';
    free(chksum);

    if (filemode == FILE_OFF)
        return header->chksum;

    if (filemode == FILE_ON)
    {
        write(fd, header->chksum, my_strlen(header->chksum) + 1);
        return NULL;
    }
    return NULL;
}

static char *write_typeflag(int fd, header_t *header, char *file, int filemode)
{
    char *typeflag = build_field_string(file, TYPEFLAG);
    if (typeflag == NULL)
        return NULL;
    char c = ' ';
    // set typeflag field
    header->typeflag = typeflag[0];

    if (filemode == FILE_OFF)
        return typeflag;

    if (filemode == FILE_ON)
    {
        write(fd, &header->typeflag, 1);
        write(fd, &c, 1);
        free(typeflag);
        return NULL;
    }
    return NULL;
}

static void fill_header(int fd)
{
    char *buff = malloc(HEADER_PADDING);
    buff[HEADER_PADDING - 1] = '\0';
    my_memset(buff, '\0', HEADER_PADDING - 1);

    write(fd, buff, HEADER_PADDING);
    free(buff);
}

static char *write_content(int fd, char *file, int filemode)
{
    // write contents of src to tar
    int fdIn;
    fdIn = open(file, O_RDONLY);
    if (fdIn == -1)
    {
        printf("Couldn't read file\n");
        return NULL;
    }

    char *buff = malloc(BLOCKSIZE * 5);
    buff[(BLOCKSIZE * 5) - 1] = '\0';
    my_memset(buff, '\0', (BLOCKSIZE * 5) - 1);

    if (filemode == FILE_OFF)
    {
        read(fdIn, buff, BLOCKSIZE * 5);
        close(fdIn);
        buff[(BLOCKSIZE * 5) - 1] = '\0';
        return buff;
    }

    if (filemode == FILE_ON)
    {
        int readBytes = 0;
        int readOn = 1;
        while (readOn == 1)
        {
            buff[BLOCKSIZE - 1] = '\0';
            my_memset(buff, '\0', BLOCKSIZE - 1);
            readBytes = read(fdIn, buff, BLOCKSIZE);
            
            if (readBytes > 0)
                write(fd, buff, BLOCKSIZE);
            else
                readOn = 0;
        }

        close(fdIn);
        free(buff);
        return NULL;
    }
    return NULL;
}

static void write_empty_blocks(int fd)
{
    char *buff = malloc(BLOCKSIZE);
    buff[BLOCKSIZE - 1] = '\0';
    my_memset(buff, '\0', BLOCKSIZE - 1);

    for (int j = 0; j < 2; j++)
        write(fd, buff, BLOCKSIZE);

    free(buff);
}