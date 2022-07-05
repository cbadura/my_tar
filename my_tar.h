#ifndef MY_LS_H
#define MY_LS_H

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef FILE_TYPE
#define FILE_TYPE
typedef enum enum_file_type {
  FILE_ARG,
  FOLDER_ARG
} file_type;
#endif

#ifndef FILE_LIST
#define FILE_LIST
typedef struct s_file_list {
    char *file_arg;
    file_type type;
    long long mtime;
    char *content;
    struct s_file_list *next;
} t_file_list;
#endif

#ifndef STRUCT_INPUT
#define STRUCT_INPUT
typedef struct s_input {
    bool c;
    bool r;
    bool t;
    bool u;
    bool x;
    bool f;
    int size;
    t_file_list *args;
    char *tarFile;
} t_input;
#endif

#ifndef FIELDS
#define FIELDS
typedef enum enum_fields {
  MODE,
  UID,
  GID,
  SIZE,
  MTIME,
  CHKSUM,
  TYPEFLAG
} fields;
#endif

#ifndef FILEMODE
#define FILEMODE
typedef enum file_mode_t {
  FILE_ON,
  FILE_OFF
} filemode;
#endif

#define USAGE "Usage: my_tar [flags] [archive file] [files to be archived]\n"
#define FILE_CREATE_FAIL "Couldn't create file\n"
#define TAR_OPEN_FAIL "Couldn't open archive\n"
#define STAT_FAIL "Could not use stat. File may not exist\n"
#define F_OPTION_MISSING "-f flag needed with this option"
#define BLOCKSIZE 512
#define HEADER_LEN 160 // with spaces, the len is actually different from the header struct's numbers
#define HEADER_PADDING 352 // BLOCKSIZE - HEADER_LEN, for filling the header with NUL bytes in write_file()
#define NAME_FIELD_SIZE 100
#define BETWEEN_NAME_MTIME_FIELD 36 // for storing mtime in list
#define MTIME_FIELD_SIZE 12
#define HEADER_WITHOUT_NAME 49 // for difficult chksum computation
#define LAST_HEADER_BYTES 4 // for difficult chksum computation

/* tar Header Block, from POSIX 1003.1-1990.  */
/* POSIX header.  */

#ifndef HEADER
#define HEADER
typedef struct header_s
{                              /* byte offset */
  char name[NAME_FIELD_SIZE];   /*   0 */
  char mode[8];                 /* 100 */
  char uid[8];                  /* 108 */   // owners's numeric user ID
  char gid[8];                  /* 116 */   // group's numeric user ID
  char size[12];                /* 124 */   // in bytes (octal base)
  char mtime[12];               /* 136 */   // in Unix time format (octal)
  char chksum[8];               /* 148 */   // checksum for header record (checksum of all the bytes in the header, assuming that the chksum field itself is all blanks.)
  char typeflag;                /* 156 */   // link indicator (file type). The link field is 1 for a linked file, 2 for a symbolic link, and 0 otherwise. A directory is indicated by a trailing slash (/) in its name.              
} header_t;
#endif

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define DIRTYPE  '5'            /* directory */

/* Bits used in the mode field, values in octal.  */
                                /* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

int create_tar(char *tar, t_file_list *args, int filemode);
int append(char *tar, t_file_list *args, int filemode);
int append_newer(char *tar, t_file_list *args, int filemode);
int list_archive(char *tar);
int extract(char *tar);

t_input *parsing(char **argv);
void write_file(int fd, header_t *header, t_file_list *head, int filemode);
void print_file(int fd, header_t *header, t_file_list *head, int filemode);

char *build_field_string(char *file, int field);
char *build_chksum_string(header_t *header, int filemode);

void set_before_final_blocks(int fd);
int find_nbr_of_blocks(int fd);
int find_header_print_name(int fd);
t_file_list *store_name_mtime(int fd, t_file_list *head);
t_file_list *compare_times_add_newer(t_file_list *newerFiles, char *file_arg, t_file_list *fileNameList);
t_file_list *store_name_content(int fd, t_file_list *head);

long long check_mtime(char *file);
int not_just_dots(char *str);
char *dec_to_octal(int dec);
long octal_to_decimal(long nbr);
char *my_itoa(int num);
t_file_list *add_node(t_file_list *head, char *name, file_type type);
t_file_list *add_node_time(t_file_list *head, char *name, long long mtime, file_type type);
t_file_list *add_node_content(t_file_list *head, char *name, char *archiveContent, file_type type);
int search_list_for_name(t_file_list *head, char *name);
void print_list(t_file_list *head);

int my_strcmp(char *s1, char *s2);
int my_strlen(char *str);
void my_strcat(char *dest, char *src);
char *my_strcpy(char *dst, char *src);
void my_memset(char *str, char c, int n);

#endif
