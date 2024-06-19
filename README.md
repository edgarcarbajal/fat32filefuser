# fat32filefuser
A simple program C++ that takes files split by 4GiB chunks, due to FAT32 filesystem limitations, and merges them together back to 1 file.

## How to Build
Just have `make` and `clang`(ie: `g++` command) installed in your system to use the following command (assuming your current directory in the shell is the root folder of this repo):
```
make
```

`make` deals with the building. It can also deal with the deletion of binaries & execuatables by running the following command:
```
make clean
```

## How to Use
2 methods of using
- Without arguments:
```
./fat32file_fuser
```
where the directories path will be asked of you by a console prompt.

- With arguments:
```
./fat32file_fuser <source directory path> <output directory path>
```
where `<source directory path>` refers to the pathname of the directory where the file parts are located **THESE MUST BE NAMED NUMERICALLY IN ORDER** (ex: "00", "01", "02", etc...),
and `<output directory path>` refers to the pathname of the directory where the newly merged file will be placed.

---

As long as the program/service that split the files in the FAT32 filesystem did not modify the parts, you don't need to run this program, and can probably just use the `cat` command in the terminal. Of course, you would have to write the file parts in the command in order, but not hard to do.

I mostly made this small program because I just wanted to see if I could implement the same functionality, also to see some sort of progress indicator, and just pure laziness of not having to write all of the parts of the filenames by hand.

I have 2 test folders/directories that you can merge the file parts using this program and then use `cat` to make true merged files and then use `diff` command to check if they are exactly the same. I did test a larger split file of about 6.8 GiB and it seemed to work fine. I can't host such a large file size here on Github, so please find your own split files of that size to test!

### PLEASE USE AT YOUR OWN RISK!!!
Make sure you have backups of the files you plan to use on this program as I can't guarantee 100% that it wont corrupt them. I am about 99% sure it won't, but just in case.
Also to minimize file naming issues, please make sure your directories (especially the source directory) does not have any whitespaces!
