*** Project ***
-c Create a new archive containing the specified items.
-r Like -c, but new entries are appended to the archive. The -f option is required.
-t List archive contents to stdout.
-u Like -r, but new entries are added only if they have a modification date newer than the corresponding entry in the archive. The -f option is required.
-x Extract to disk from the archive. If a file with the same name appears more than once in the archive, each copy will be extracted, with later copies overwriting (replacing) earlier copies.

In -c, -r, or -u mode, each specified file or directory is added to the archive in the order specified on the command line.
By default, the contents of each directory are also archived.

Unless specifically stated otherwise, options are applicable in all operating modes:
-f file Read the archive from or write the archive to the specified file. The filename can be standard input or standard output.
my_tar will not handle files inside subdirectories.

*** Archives: file entries and file headers *** 
Physically, an archive consists of a series of file entries terminated by an end-of-archive entry, which consists of two 512 blocks of zero bytes.
A file entry usually describes one of the files in the archive (an archive member), and consists of a file header and the contents of the file.
File headers contain file names and statistics, checksum information which tar uses to detect file corruption, and information about file types.

The header contains a file size field that determines the length of the file. The binary file data immediately follows the header. In order to preserve the 512-byte-block structure, the last block is padded with NUL characters unless the file size is divisible by 512.

In some cases – directories, for example – the header size field is set to zero and the next block is another header.

*** Blocks ***
A tar archive file contains a series of blocks. Each block contains BLOCKSIZE bytes. Although this format may be thought of as being on magnetic tape, other media are often used.

Each file archived is represented by a header block which describes the file, followed by zero or more blocks which give the contents of the file. At the end of the archive file there are two 512-byte blocks filled with binary zeros as an end-of-file marker. A reasonable system should write such end-of-file marker at the end of an archive, but must not assume that such a block exists when reading an archive. In particular GNU tar always issues a warning if it does not encounter it.

The blocks may be blocked for physical I/O operations. Each record of n blocks (where n is set by the ‘--blocking-factor=512-size’ (‘-b 512-size’) option to tar) is written with a single ‘write ()’ operation.

(Blocking factor:) The data in an archive is grouped into blocks, which are 512 bytes. Blocks are read and written in whole number multiples called records. The number of blocks in a record (i.e., the size of a record in units of 512 bytes) is called the blocking factor. The ‘--blocking-factor=512-size’ (‘-b 512-size’) option specifies the blocking factor of an archive. The default blocking factor is typically 20 (i.e., 10240 bytes), but can be specified at installation.

*** TAR octal numbers ***

All numbers, especially filesizes, in the header, are encoded as octal numbers with trailing zeroes in TARs. Digits are represented as ASCII characters.

Additionally, the octal numbers may (but do not have to) contain trailing NULs.

*** Other ***
tar always treats its first argument as an option (or cluster of options), even if it does not start with dash
___

chmod:
#include <sys/types.h>
int chmod(const char *path, mode_t mode);
The function chmod() sets the file permission bits of the file specified by the pathname path to mode.
chmod() verifies that the process owner (user) either owns the file specified by path (or fildes), or is the super-user.
pon successful completion, a value of 0 is returned.  Otherwise, a value of -1 is returned

open(2)
close|(2)

fstat(2)
#include <sys/stat.h>
int fstat(int fildes, struct stat *buf);
The fstat() obtains the same information about an open file known by the file descriptor fildes.
stat(2)
lstat(2)
int lstat(const char *restrict path, struct stat *restrict buf);
lstat() returns information about the link, while stat() returns information about the file the link references.

 struct stat { /* when _DARWIN_FEATURE_64_BIT_INODE is NOT defined */
         dev_t    st_dev;    /* device inode resides on */
         ino_t    st_ino;    /* inode's number */
         mode_t   st_mode;   /* inode protection mode */
         nlink_t  st_nlink;  /* number of hard links to the file */
         uid_t    st_uid;    /* user-id of owner */
         gid_t    st_gid;    /* group-id of owner */
         dev_t    st_rdev;   /* device type, for special file inode */
         struct timespec st_atimespec;  /* time of last access */
         struct timespec st_mtimespec;  /* time of last data modification */
         struct timespec st_ctimespec;  /* time of last file status change */
         off_t    st_size;   /* file size, in bytes */
         quad_t   st_blocks; /* blocks allocated for file */
         u_long   st_blksize;/* optimal file sys I/O ops blocksize */
         u_long   st_flags;  /* user defined flags for file */
         u_long   st_gen;    /* file generation number */
     };
____

getpwuid
struct passwd *getpwuid(uid_t uid);

These functions obtain information from opendirectoryd(8), including records in /etc/master.passwd which is described in master.passwd(5).  Each entry in the database is
     defined by the structure passwd found in the include file <pwd.h>:

           struct passwd {
                   char    *pw_name;       /* user name */
                   char    *pw_passwd;     /* encrypted password */
                   uid_t   pw_uid;         /* user uid */
                   gid_t   pw_gid;         /* user gid */
                   time_t  pw_change;      /* password change time */
                   char    *pw_class;      /* user access class */
                   char    *pw_gecos;      /* Honeywell login info */
                   char    *pw_dir;        /* home directory */
                   char    *pw_shell;      /* default shell */
                   time_t  pw_expire;      /* account expiration */
                   int     pw_fields;      /* internal: fields filled in */
           };

     The functions getpwnam(), getpwuid(), and getpwuuid() search the password database for the given login name, user uid, or user uuid respectively, always returning the first
     one encountered.

     Note that the password file /etc/master.passwd does not contain user UUIDs.  The UUID for a user may be found using mbr_uid_to_uuid().

     The secure password database file
     /etc/master.passwd  The current password file

#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>

getgrgid
#include <grp.h>
#include <uuid/uuid.h>
int getgrgid_r(gid_t gid, struct group *grp, char *buffer, size_t bufsize, struct group **result);
These functions obtain information from opendirectoryd(8), including records in /etc/group which is described in group(5).  Each line of the database is defined by the
     structure group found in the include file <grp.h>:

           struct group {
                   char    *gr_name;       /* group name */
                   char    *gr_passwd;     /* group password */
                   gid_t   gr_gid;         /* group id */
                   char    **gr_mem;       /* group members */
           };

     The functions getgrnam(), getgrgid(), and getgruuid() search the group database for the given group name pointed to by name, the group id given by gid, or the UUID given by
     uuid respectively, returning the first one encountered.  Identical group names, group gids, or uuids may result in undefined behavior.

The functions getgrnam_r(), getgrgid_r(), and getgruuid_r() are alternative versions of getgrnam(), getgrgid(), and getgruuid() respectively.  The caller must provide
     storage for the results of the search in the grp, buffer, bufsize, and result arguments.  When these functions are successful, the grp argument will be filled-in, and a
     pointer to that argument will be stored in result.  If an entry is not found or an error occurs, result will be set to NULL.
They return a pointer to a group structure on success or NULL if the entry is not found or if an error occurs. 

getxattr - get an extended attribute value
listxattr
#include <sys/xattr.h>
ssize_t getxattr(const char *path, const char *name, void *value, size_t size, u_int32_t position, int options);

Extended attributes extend the basic attributes of files and directories in the file system.  They are stored as name:data pairs associated with file system objects (files,
     directories, symlinks, etc).

     The getxattr() function retrieves up to size bytes of data from the extended attribute identified by name associated with path into the pre-allocated buffer pointed to by
     value.  The function returns the number of bytes of data retrieved.

     An extended attribute's name is a simple NULL-terminated UTF-8 string.  position specifies an offset within the extended attribute.  In the current implementation, this
     argument is only used with the resource fork attribute.  For all other extended attributes, this parameter is reserved and should be zero.

     On success, value contains the data associated with name.  When value is set to NULL, getxattr() returns current size of the named attribute.  This facility can be used to
     determine the size of a buffer sufficiently large to hold the data currently associated with the attribute.

     options specify options for retrieving extended attributes:

     XATTR_NOFOLLOW  do not follow symbolic links.  getxattr() normally returns information from the target of path if it is a symbolic link.  With this option, getxattr() will
                     return extended attribute data from the symbolic link instead.

ssize_t listxattr(const char *path, char *namebuf, size_t size, int options);
listxattr() retrieves a list of names of extended attributes associated with the given path in the file system.

lseek(2)
#include <unistd.h>

off_t lseek(int fildes, off_t offset, int whence);
The lseek() function repositions the offset of the file descriptor fildes to the argument offset, according to the directive whence.  The argument fildes must be an open
     file descriptor.  lseek() repositions the file pointer fildes as follows:

           If whence is SEEK_SET, the offset is set to offset bytes.

           If whence is SEEK_CUR, the offset is set to its current location plus offset bytes.

           If whence is SEEK_END, the offset is set to the size of the file plus offset bytes.

           If whence is SEEK_HOLE, the offset is set to the start of the next hole greater than or equal to the supplied offset.  The definition of a hole is provided below.

           If whence is SEEK_DATA, the offset is set to the start of the next non-hole file region greater than or equal to the supplied offset.

The lseek() function allows the file offset to be set beyond the end of the existing end-of-file of the file. If data is later written at this point, subsequent reads of the data in the gap return bytes of zeros (until data is actually written into the gap).

major
minor
- manage a device number
#define _BSD_SOURCE             /* See feature_test_macros(7) */#include
<sys/types.h>
dev_t makedev(int maj, int min);
unsigned int major(dev_t dev);unsigned
int minor(dev_t dev);

A device ID consists of two parts: a major ID, identifying the class of the device, and a minor ID, identifying a specific instance of a device in that class. A device ID is represented using the type dev_t.
Given major and minor device IDs, makedev() combines these to produce a device ID, returned as the function result. This device ID can be given to mknod(2), for example.

The major() and minor() functions perform the converse task: given a device ID, they return, respectively, the major and minor components. These macros can be useful to, for example, decompose the device IDs in the structure returned by stat(2).

mkdir(2)
opendir
readdir

readlink
symlink
unlink(2)

time
ctime
utime
