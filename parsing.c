#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include "my_tar.h"
#include "dbg.h"

static t_input *fill_struct(t_input *input, char **argv);
static t_input *add_argument(char *arg, t_input *input);

/* 
*** parsing functions ***
 */

t_input *parsing(char **argv)
{
    t_input *input = malloc(sizeof(t_input));
    t_file_list *head = NULL;
    input->args = head;
    input->c = false;
    input->r = false;
    input->t = false;
    input->u = false;
    input->x = false;
    input->f = false;
    
    // find nbr of file names in argv
    int argSize = 0;
    for (int i = 1; argv[i]; i++)
        if (argv[i][0] != '-') argSize++;

    input->size = argSize;
    input->tarFile = NULL;
    input = fill_struct(input, argv);
    if (input->args)
    {
        debug("ARG LIST");
        print_list(input->args);
    }
    return input;
}

static t_input *fill_struct(t_input *input, char **argv)
{
    for (int i = 1; argv[i]; i++)
    {
        if (my_strcmp(argv[i], "-c") == 0)
            input->c = true;
        else if (my_strcmp(argv[i], "-r") == 0)
            input->r = true;
        else if (my_strcmp(argv[i], "-t") == 0)
            input->t = true;
        else if (my_strcmp(argv[i], "-u") == 0)
            input->u = true;
        else if (my_strcmp(argv[i], "-x") == 0)
            input->t = true;
        else if (my_strcmp(argv[i], "-f") == 0)
            input->f = true;
        else if (my_strcmp(argv[i], "-cf") == 0)
        {
            input->c = true;
            input->f = true;
        }
        else if (my_strcmp(argv[i], "-rf") == 0)
        {
            input->r = true;
            input->f = true;
        }
        else if (my_strcmp(argv[i], "-tf") == 0)
        {
            input->t = true;
            input->f = true;
        }
        else if (my_strcmp(argv[i], "-uf") == 0)
        {
            input->u = true;
            input->f = true;
        }
        else if (my_strcmp(argv[i], "-xf") == 0)
        {
            input->x = true;
            input->f = true;
        }
        else
        {
            if (input->f == true && input->tarFile == NULL)
            {
                input->tarFile = malloc(sizeof(char) * my_strlen(argv[i]));
                input->tarFile = argv[i];
                i++;
            }

            if (input->c == true || input->r == true || input->u == true)
                input = add_argument(argv[i], input);
        }
    }
    return input;
}

static t_input *add_argument(char *arg, t_input *input)
{
    char *path = NULL;
    int type = 0;
    int check = 0; // to check for just-dot-files


    DIR *dirp = opendir(arg);
    struct dirent *de;
    // add file to list
    if (dirp == NULL)
    {
        input->args = add_node(input->args, arg, FILE_ARG);
        return input;
    }
    else
    {
        path = malloc(sizeof(char) * (my_strlen(arg) + 1 + 1));
        path = my_strcpy(path, arg);
        my_strcat(path, "/");
        input->args = add_node(input->args, path, FOLDER_ARG);
    }
    
    while ((de = readdir(dirp)))
    {
        // determine type of each item in dir
        type = (de->d_type == DT_DIR ? FOLDER_ARG : FILE_ARG);

        // build path string & add item to list
        path = malloc(sizeof(char) * (my_strlen(arg) + 1 + my_strlen(de->d_name) + 1));
        path = my_strcpy(path, arg);
        my_strcat(path, "/");
        my_strcat(path, de->d_name);
        if ((check = not_just_dots(de->d_name)) == 1)
            input->args = add_node(input->args, path, type);
    }
    free(path);
    closedir(dirp);
    return input;
}