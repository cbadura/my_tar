#include <stdio.h>
#include "my_tar.h"
#include "dbg.h"

int main(int ac, char **av)
{
    if (ac < 3)
    {
        printf("%s", USAGE);
        return 1;
    }

    // create the input struct that contains info about flags, archive name & file argument
    t_input *input = parsing(av);

    // create archive & list contents to stdout
    if ((input->c == true) && (input->f == false))
        create_tar(input->tarFile, input->args, FILE_OFF);

    // create archive & store in tar file
    if ((input->c == true) && (input->f == true))
        create_tar(input->tarFile, input->args, FILE_ON);

    // list existing archive contents to stdout
    if ((input->t == true) && (input->f == true))
        list_archive(input->tarFile);

    // append new files to end of existing archive
    if ((input->r == true) && (input->f == true))
        append(input->tarFile, input->args, FILE_ON);
    else if ((input->r == true) && (input->f == false))
        printf("%s", F_OPTION_MISSING);

    // append new files only if their modification date is newer than the corresponding entry in the archive
    if ((input->u == true) && (input->f == true))
        append_newer(input->tarFile, input->args, FILE_ON);
    else if ((input->u == true) && (input->f == false))
        printf("%s", F_OPTION_MISSING);

    // extract to disk from the archive
    if ((input->x == true) && (input->f == true))
        extract(input->tarFile);
    else if ((input->x == true) && (input->f == false))
        printf("%s", F_OPTION_MISSING);

    return 0;
}



// https://github.com/calccrypto/tar

// https://github.com/Distrotech/tar/blob/273975bec1ff7d591d7ab8a63c08a02a285ffad3/src/list.c#L303-L356
