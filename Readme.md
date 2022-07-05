# Welcome to My Tar
***

## Task
my_tar is a useful tool to archive files within one single file, which is great for file organization and portability!
my_tar is able to create an archive file, either writing a new file (-cf mode) or displaying the archived files directly to terminal (-c mode).
It can then append files to an existing archive, either doing this indiscriminately for each file argument (-r mode) or, in the case of items of the same name, only for those files which have a newer modification date than the one inside the archive (-u mode).
Last but not least, extraction to disk from the archive is possible (-x mode).

In -c, -r, or -u mode, each specified file or directory is added to the archive in the order specified on the command line. By default, the contents of each directory are also archived. my_tar will not handle file inside subdirectory.

## Description
From main, the parsing function is called (found in "parsing.c") which fills the according struct with info for the requested mode, the archive name and, if present, the other file arguments.
In "tar_options.c", the design for individual modes can be found, each calling other functions: for all writing operations, those are found in "write_fts.c" (which creates the header calling functions in "header_strings.c"); for other archive operations, those are found in "archive_fts.c".
The important file header struct can be found in my_tar.h, along with other data structures.

## Installation
Simply use make, then run the program by typing "./my_tar" along with the options and file arguments.

## Usage
The -f option is required in all modes but -c mode.

Any requested files or directories are arguments after "./my_tar" and the flags. Flags always need to be present. If the -f flag is present, the first argument after the flags names the archive file.

Examples: "./my_tar -cf archive.tar file0 file1" (creates the archive "archive.tar" containing the two files named after it);
"./my_tar -rf archive.tar folder" (appends "folder" to the existing archive);
"./my_tar -tf archive.tar" (displays contents of the archive)
"./my_tar -xf archive.tar" (extracts contents to disk)



