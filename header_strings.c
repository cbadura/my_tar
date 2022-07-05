#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "my_tar.h"
#include "dbg.h"

static char *build_mode_field(struct stat filestat);
static char *build_uid_field(struct stat filestat);
static char *build_gid_field(struct stat filestat);
static char *build_mode_field(struct stat filestat);
static char *build_size_field(struct stat filestat);
static char *build_mtime_field(struct stat filestat);
static char *build_typeflag_field(struct stat filestat);

char *build_field_string(char *file, int field)
{
    struct stat filestat;
    if (lstat(file, &filestat) == -1)
    {
        printf("%s", STAT_FAIL);
        return NULL;
    }

    char *fieldStr = NULL;
    if (field == MODE)
        fieldStr = build_mode_field(filestat);
    if (field == UID)
        fieldStr = build_uid_field(filestat);
    if (field == GID)
        fieldStr = build_gid_field(filestat);
    if (field == SIZE)
        fieldStr = build_size_field(filestat);
    if (field == MTIME)
        fieldStr = build_mtime_field(filestat);
    // chksum has its own ft and is called from the write ft
    if (field == TYPEFLAG)
        fieldStr = build_typeflag_field(filestat);

    return fieldStr;
}

static char *build_mode_field(struct stat filestat)
{
    // build mode string; set first four digits to 0, not used in the header
    char *fieldStr = malloc(sizeof(char) * (8 + 1));
    int i = 0;
    for (; i < 4; i++)
        fieldStr[i] = '0';

    char *oct = dec_to_octal(filestat.st_mode);
    char *tmp = malloc(sizeof(char) * (6 + 1));
    tmp[0] = '0';
    tmp[1] = '\0';
    if (my_strlen(oct) < 6)
    {
        my_strcat(tmp, oct);
        oct = my_strcpy(oct, tmp);
    }
    // copy oct to mode string, but cut the first two digits of oct (fields not used in my version of the header)
    for (i = 2; oct[i] != '\0'; i++)
        fieldStr[i + 2] = oct[i]; // start at third digit
    fieldStr[i + 2] = '\0';
    free(oct);
    return fieldStr;
}

static char *build_uid_field(struct stat filestat)
{
    // build uid string; set first three digits to 0, not used in the header
    char *fieldStr = malloc(sizeof(char) * (8 + 1));
    int i = 0;

    char *oct = dec_to_octal(filestat.st_uid);
    int len = my_strlen(oct) - 1;
    // copy oct to uid string, fill with 0s until size value starts (len)
    for (; i < 5 - len; i++)
        fieldStr[i] = '0';

    int j = 0;
    for (; oct[j] != '\0'; i++)
    {
        fieldStr[i] = oct[j];
        j++;
    }
    fieldStr[i] = '\0';

    free(oct);
    return fieldStr;
}

static char *build_gid_field(struct stat filestat)
{
    // build gid string; set first three digits to 0, not used in the header
    char *fieldStr = malloc(sizeof(char) * (8 + 1));
    int i = 0;
    for (; i < 3; i++)
        fieldStr[i] = '0';

    char *oct = dec_to_octal(filestat.st_gid);
    int len = my_strlen(oct) - 1;
    // copy oct to uid string, fill with 0s until size value starts (len)
    for (; i < 5 - len; i++)
        fieldStr[i] = '0';

    int j = 0;
    for (; oct[j] != '\0'; i++)
    {
        fieldStr[i] = oct[j];
        j++;
    }
    fieldStr[i] = '\0';

    free(oct);
    return fieldStr;
}

static char *build_mtime_field(struct stat filestat)
{
    char *fieldStr = malloc(sizeof(char) * (11 + 1));
    char *oct = dec_to_octal(filestat.st_mtimespec.tv_sec); // ((filestat.st_mtim.tv_sec*1e+9) + filestat.st_mtim.tv_nsec);
    // copy oct to time string
    int i = 0;
    for (i = 0; oct[i] != '\0'; i++)
        fieldStr[i] = oct[i];
    fieldStr[i] = '\0';

    free(oct);
    return fieldStr;
}

static char *build_size_field(struct stat filestat)
{
    char *fieldStr = malloc(sizeof(char) * (11 + 1));
    char *oct = dec_to_octal(filestat.st_size);
    int len = 0;
    if (oct == NULL)
    {
        int i = 0;
        for (; i < 11; i++)
            fieldStr[i] = '0';
        fieldStr[i] = '\0';
        return fieldStr;
    }
    else
        len = my_strlen(oct) - 1;
    // copy oct to size string, fill with 0s until size value starts (len)
    int i = 0;
    for (; i < 10 - len; i++)
        fieldStr[i] = '0';

    if (oct == NULL)
    {
        i++;
        fieldStr[i] = '\0';
        return fieldStr;
    }

    int j = 0;
    for (; oct[j] != '\0'; i++)
    {
        fieldStr[i] = oct[j];
        j++;
    }
    fieldStr[i] = '\0';

    free(oct);
    return fieldStr;
}

char *build_chksum_string(header_t *header, int filemode)
{
    char *fieldStr = malloc(sizeof(char) * (6 + 1));
    int sum;
    if (filemode == FILE_ON)
        sum = HEADER_WITHOUT_NAME; // chksum is calculated by lenghts of actual bytes in header, only file name is variable
    else
        sum = LAST_HEADER_BYTES; // if -f option is off, the nameLen automatically increases (for unknown reasons) and only last 4 bytes are missing

    int nameLen = my_strlen(header->name) + 1;
    sum += nameLen; // the final checksum
    // convert to string
    char *chksum = my_itoa(sum);
    int len = my_strlen(chksum);
    if (len == 0)
    {
        chksum[0] = '0';
        len = 1;
    }
    // copy chksum to field string, fill with 0s until size value starts (len)
    int i = 0;
    for (; i < 6 - len; i++)
        fieldStr[i] = '0';
    fieldStr[i] = '\0';
    int j = 0;
    for (; chksum[j] != '\0'; i++)
    {
        fieldStr[i] = chksum[j];
        j++;
    }
    fieldStr[i] = '\0';

    free(chksum);
    return fieldStr;
}

static char *build_typeflag_field(struct stat filestat)
{
    char *fieldStr = malloc(sizeof(char) * 3);

    if (S_ISREG(filestat.st_mode))
        fieldStr[0] = REGTYPE;

    if (S_ISLNK(filestat.st_mode))
        fieldStr[0] = LNKTYPE;

    if (S_ISDIR(filestat.st_mode))
        fieldStr[0] = DIRTYPE;

    fieldStr[1] = ' ';
    fieldStr[2] = '\0';
    return fieldStr;
}
