#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "my_tar.h"
#include "dbg.h"

int create_tar(char *tar, t_file_list *args, int filemode)
{
    int fd;
    if (filemode == FILE_ON)
    {
        fd = open(tar, O_WRONLY | O_CREAT, 0644);
        if (fd == -1)
        {
            printf("%s", FILE_CREATE_FAIL);
            return -1;
        }
    }
    else (fd = -1);

    header_t *header = malloc(sizeof(header_t));
    
    if (filemode == FILE_OFF)
        print_file(fd, header, args, filemode);
    
    if (filemode == FILE_ON)
        write_file(fd, header, args, filemode);

    close(fd);
    free(header);
    return 0;
}

int list_archive(char *tar)
{
    int fd;
    fd = open(tar, O_RDONLY);
    if (fd == -1)
    {
        printf("%s", TAR_OPEN_FAIL);
        return -1;
    }

    // find number of blocks before empty blocks in this file
    int blocks = find_nbr_of_blocks(fd);

    // move through all blocks & print names if header is found
    for (int i = 0; i < blocks; i++)
    {
        find_header_print_name(fd);
        lseek(fd, BLOCKSIZE, SEEK_CUR);
    }
    
    close(fd);
    return 0;
}

int append(char *tar, t_file_list *args, int filemode)
{
    int fd;
    fd = open(tar, O_RDWR);
    if (fd == -1)
    {
        printf("%s", TAR_OPEN_FAIL);
        return -1;
    }
    // move fd to the start of the final empty blocks (1024 NUL bytes) using lseek()
    set_before_final_blocks(fd);
    
    // create header & use write ft for appending
    header_t *header = malloc(sizeof(header_t));
    write_file(fd, header, args, filemode);

    close(fd);
    free(header);
    return 0;
}

int append_newer(char *tar, t_file_list *args, int filemode)
{
    int fd;
    fd = open(tar, O_RDWR);
    if (fd == -1)
    {
        printf("%s", TAR_OPEN_FAIL);
        return -1;
    }

    t_file_list *archiveFileList = NULL;
    t_file_list *newFiles = NULL;

    // find number of blocks before empty blocks in this file
    int blocks = find_nbr_of_blocks(fd);

    // create list for file names in archive with their mtimes
    for (int i = 0; i < blocks; i++)
    {
        archiveFileList = store_name_mtime(fd, archiveFileList);
        lseek(fd, BLOCKSIZE, SEEK_CUR);
    }

    t_file_list *first = archiveFileList; // for returning to beginning of list after each loop
    t_file_list *checkArchiveList = archiveFileList; // for the search_list_for_name ft, which has to search the WHOLE list
    // find match in file names, store in new list
    for (; args; args = args->next)
    {
        archiveFileList = first;
        for (; archiveFileList; archiveFileList = archiveFileList->next)
        {
            if (my_strcmp(args->file_arg, archiveFileList->file_arg) == 0)
                newFiles = compare_times_add_newer(newFiles, args->file_arg, archiveFileList);
            else
            {
                if ((search_list_for_name(newFiles, args->file_arg) == 0) && (search_list_for_name(checkArchiveList, args->file_arg) == 0))
                    newFiles = add_node(newFiles, args->file_arg, FILE_ARG);
            }
        }
    }
    close(fd);
    
    // call append ft with new list
    append(tar, newFiles, filemode);
    return 0;
}

int extract(char *tar)
{
    if (tar == NULL) return -1;

    int fdIn = open(tar, O_RDWR);
    if (fdIn == -1)
    {
        printf("%s", TAR_OPEN_FAIL);
        return -1;
    }

    // find number of blocks before empty blocks in this file
    int blocks = find_nbr_of_blocks(fdIn);

    // create list for file names in archive with their contents
    t_file_list *archiveFileList = NULL;
    for (int i = 0; i < blocks - 1; i++)
    {
        archiveFileList = store_name_content(fdIn, archiveFileList);
        lseek(fdIn, 0, SEEK_SET);
        lseek(fdIn, BLOCKSIZE * (i + 1), SEEK_CUR);
    }
    // iterate through all files in archive and create new files with their content
    int fdOut;
    while (archiveFileList)
    {
        // if archive item is a folder, create folder
        int len = my_strlen(archiveFileList->file_arg);
        if (archiveFileList->file_arg[len - 1] == '/')
        {
            mkdir(archiveFileList->file_arg, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            archiveFileList = archiveFileList->next;
            continue;
        }
        
        fdOut = open(archiveFileList->file_arg, O_WRONLY | O_CREAT, 777);
        if (fdOut == -1)
        {
            printf("%s", FILE_CREATE_FAIL);
            return -1;
        }
        if (archiveFileList->content)
            write(fdOut, archiveFileList->content, my_strlen(archiveFileList->content));
        
        archiveFileList = archiveFileList->next;
    }

    close(fdOut);
    return 0;
}
