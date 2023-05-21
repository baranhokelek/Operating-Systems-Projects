/*
 * kufs.h
 *
 *  Created on: May 23, 2018
 *      Author: baranhokelek tumayozdemir
 */
#include <stdio.h>
#include <stdlib.h>
#ifndef KUFS_H_
#define KUFS_H_
/*
 * Pre-defined maximum possible number of files & blocks
 */
#define MAX_NUM_FILES 2000
#define MAX_NUM_BLOCKS 2000
/*
 * The original file pointer
 */
int *allocated_blocks;
int *open_file_table;
FILE *file;

/*
 * FAT Entry struct
 * filename: Name of the file(e.g. "file1.txt")
 * allocatedBlocks: Array that keeps the blocks allocated to that file
 * positionPtr: Integer that represents the position of the file pointer
 * open: Indicates whether the file is open or closed
 */
typedef struct
{
    char *filename;
    int filesize;
    int *allocatedBlocks; /*= calloc(MAX_NUM_BLOCKS, sizeof(int)) */
    int positionPtr;
    int deleteflag;
    int beforeclose;
    int *open;
} fatEntry;

/*
 * FAT struct contains a fatEntry array which has a pre-defined size
 */
typedef struct fat
{
    fatEntry *files[MAX_NUM_FILES];
} fat;

/*
 * disk_node is the fat struct that we interact with
 */
struct fat *disk_node;

/*
 * Initializes the file structure. Opens the file specified by FILE* above, fills it with '~' characters, and then closes it.
 * Closing is required since the file is opened in write mode in kufs_create_disk. We may wish to just read the file in the future, and repopening
 * is a better aproach.
 */
int kufs_create_disk(char *disk_name, int disk_size)
{
    if (disk_size <= 0)
        return -1;

    file = fopen(disk_name, "wb");
    char zero[1024 * disk_size];
    for (int i = 0; i < 1024 * disk_size; i++)
    {
        zero[i] = '~';
    }
    size_t res = fwrite(zero, sizeof(char), 1024 * disk_size, file);
    fclose(file); /* ? */
    if (res != 0)
        return -1;
    return 0;
}

/*
 * Mounts the files into a temporary buffer. First measures the file size, and then writes it completely into the buffer.
 */
int kufs_mount(char *disk_name)
{
    long size = 1024;
    long fSize;
    char *buffer;
    size_t res;
    file = fopen(disk_name, "rb+");
    if (file == NULL)
        return -1;
    buffer = (char *)malloc(sizeof(char) * size);
    if (buffer == NULL)
        return -1;
    fseek(file, 0, SEEK_END);
    fSize = ftell(file);
    rewind(file);
    res = fread(buffer, 1, size, file);
    if (res != size)
        return -1;
    allocated_blocks = calloc((int)(fSize / 1024), sizeof(int));
    open_file_table = calloc(MAX_NUM_FILES, sizeof(int));
    free(buffer); /* ? */
    return 0;
}

/*
 * Unmounts the files. First, prints the information of each file(file name & which blocks it occupies) into a fileInfo string
 * to be displayed in the File Allocation Table(FAT). Then, frees all the pointers associated with the FAT and closes the FILE*.
 */
int kufs_umount()
{
    char *fileInfo = malloc(sizeof(char) * 1024);

    for (int i = 0; i < MAX_NUM_FILES; i++)
    {

        if (disk_node->files[i] != NULL && disk_node->files[i]->deleteflag != 1)
        {
            strncat(fileInfo, disk_node->files[i]->filename, strlen(disk_node->files[i]->filename));
            strncat(fileInfo, " ", 1);
            const char *size = malloc(sizeof(int));
            sprintf(size, "%d", disk_node->files[i]->filesize);
            strncat(fileInfo, size, strlen(size));
            strncat(fileInfo, " ", 1);

            for (int j = 0; j < MAX_NUM_BLOCKS; j++)
            {
                if (disk_node->files[i]->allocatedBlocks[j] != 0)
                {

                    const char *num = malloc(sizeof(int));
                    sprintf(num, "%d", disk_node->files[i]->allocatedBlocks[j]);
                    strncat(fileInfo, num, 1);
                    strncat(fileInfo, " ", 1);
                }
            }
        }
    }
    printf("fileinfo %s\n", fileInfo);
    fseek(file, 0, SEEK_SET);
    fwrite(fileInfo, sizeof(char), strlen(fileInfo), file);
    free(fileInfo);
    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        free(disk_node->files[i]);
    }

    free(disk_node);

    free(allocated_blocks);
    free(open_file_table);
    fclose(file);
    return 0;
}

/*
 * Creates a new file to be added to the system. Finds the first NULL entry in the FAT, then initializes the fatEntry struct members & allocates them
 * appropriate memory.
 */
int kufs_create(char *filename)
{
    for (int i = 1; i < MAX_NUM_FILES; i++)
    {
        if (disk_node->files[i] == NULL)
        {
            disk_node->files[i] = malloc(sizeof(fatEntry));
            disk_node->files[i]->filename = malloc(sizeof(char) * strlen(filename));
            disk_node->files[i]->allocatedBlocks = calloc(MAX_NUM_BLOCKS, sizeof(int));
            disk_node->files[i]->open = malloc(sizeof(int));
            disk_node->files[i]->filesize = 0;
            disk_node->files[i]->positionPtr = 0;
            disk_node->files[i]->deleteflag = 0;
            strcpy(disk_node->files[i]->filename, filename);
            disk_node->files[i]->open = 0;
            disk_node->files[i]->beforeclose = 0;
            return 0;
        }
    }
    return -1;
}

/*
 * Opens the file with the name *filename. First, checks whether such a file exists; then, if it exists and it has not been opened before
 * (if its open value is 0), sets it to 1. If the opened file is the nth file that has been opened, first zero element of the open_file_table is set to n.
 */
int kufs_open(char *filename)
{
    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        if (disk_node->files[i] != NULL && disk_node->files[i]->deleteflag != 1 && strcmp(disk_node->files[i]->filename, filename) == 0)
        {
            if (disk_node->files[i]->open == 1)
            {
                disk_node->files[i]->positionPtr = disk_node->files[i]->allocatedBlocks[0] * 1024;
                disk_node->files[i]->beforeclose = disk_node->files[i]->filesize;
            }
            else
            {
                disk_node->files[i]->open = 1;
            }
            int j = 0;
            for (int k = 0; k < MAX_NUM_FILES; k++)
            {
                if (disk_node->files[k] != NULL && disk_node->files[k]->deleteflag != 1 && disk_node->files[k]->open == 1)
                {
                    open_file_table[j++] = i;
                }
            }
            return i;
        }
    }
    return 0;
}

/*
 * Closes the file. Removes it from the open_file_table.
 */
int kufs_close(int fd)
{

    if (fd != 0 && disk_node->files[fd] != NULL && disk_node->files[fd]->open == 1)
    {
        disk_node->files[fd]->open = 0;
        for (int k = 0; k < MAX_NUM_FILES; k++)
        {
            if (open_file_table[k] == fd)
            {
                open_file_table[k] = 0;
                break;
            }
        }
        return 0;
    }
    else
        return -1;
}

/*
 * Deletes the file. Closes it if it is open, and then frees the members of the fatEntry struct associated with it
 * such as filename, allocatedBlocks and open. Then, removes all the blocks allocated to this file from the allocated_blocks array.
 */
int kufs_delete(char *filename)
{
    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        for (int j = 0; j < MAX_NUM_BLOCKS; j++)
        {

            for (int k = 0; k < MAX_NUM_BLOCKS; k++)
            {
                if (disk_node->files[i] != NULL && disk_node->files[i]->deleteflag != 1 && disk_node->files[i]->allocatedBlocks[k] == allocated_blocks[j])
                {
                    allocated_blocks[j] = 0;
                }
            }
        }
    }

    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        if (disk_node->files[i] != NULL && disk_node->files[i]->deleteflag != 1 && strcmp(disk_node->files[i]->filename, filename) == 0)
        {
            kufs_close(i);
            disk_node->files[i]->deleteflag = 1;
            disk_node->files[i] = NULL;
            return 0;
        }
    }
    return -1;
}

/*
 * Writes into the file.
 */
int kufs_write(int fd, void *buf, int n)
{
    /*
     * If the file is being written on for the first time, a free block is found and allocated to the file, and its positionPtr is set to the
     * first index of that block.
     */
    int remaining = n;

    /*
     * This result is used to compare whether the number of bits written is equal to number of bits we intended to write.
     */
    size_t res = 0;
    while (remaining > 0)
    {

        if (disk_node->files[fd]->allocatedBlocks[0] == 0)
        {

            for (int i = 1; i < MAX_NUM_BLOCKS; i++)
            {
                if (allocated_blocks[i] == 0)
                {
                    allocated_blocks[i] = 1;
                    disk_node->files[fd]->allocatedBlocks[0] = i;
                    break;
                }
            }
            disk_node->files[fd]->positionPtr = disk_node->files[fd]->allocatedBlocks[0] * 1024;
        }

        /*
         * If the file has been written on before, it must have a previously-set positionPtr. We don't interfere with it here.
         * First, we find the current block the positionPtr is pointing to. Then, we initialize the lim variable, which is the beginning of the next block
         * (the limit to the number of bytes we can write into the current block)
         * Then, we update the file pointer with the positionPtr value, and start writing. However, there are two separate cases we need to consider.
         */

        /*
         * If writing n bytes after the current position exceeds the block that we are in, we need to fill the current block;
         * and then write the remaning bytes into a new block.
         */
        if ((int)((disk_node->files[fd]->positionPtr + remaining) / 1024) > (int)(disk_node->files[fd]->positionPtr / 1024))
        {
            int currentBlock = (int)(disk_node->files[fd]->positionPtr / 1024);
            int lim = (currentBlock + 1) * 1024;
            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            char *readbuf[lim - disk_node->files[fd]->positionPtr];
            int counter = 0;
            // Here before writing to file Im searching in the file for '~' characters, if the character we overwrite is ~,in this case it was empty and file size should increase
            // else it was already full and we overwrote it so file size should not increase
            for (int j = 0; j < lim - disk_node->files[fd]->positionPtr; j++)
            {
                char c = (char)fgetc(file);
                if (c == '~')
                    counter++;
            }
            disk_node->files[fd]->filesize += counter;

            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            size_t write1 = fwrite(buf, sizeof(char), lim - disk_node->files[fd]->positionPtr, file);
            int x;
            for (x = 0; x < MAX_NUM_BLOCKS; x++)
            {
                if (disk_node->files[fd]->allocatedBlocks[x] == currentBlock)
                {
                    break;
                }
            }
            /*
             * If the file is being overwritten, and there is a previously allocated block after the current one, we don't allocate a new block to the file.
             */
            if (disk_node->files[fd]->allocatedBlocks[x + 1] != 0)
            {
                disk_node->files[fd]->positionPtr = disk_node->files[fd]->allocatedBlocks[x + 1] * 1024;
                char *buf2 = malloc(sizeof(char) * (remaining - (int)write1));
                strncpy(buf2, buf + (int)write1, remaining - (int)write1 + 1);
                fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
                char *readbuf[remaining - (int)write1];
                int counter = 0;

                for (int j = 0; j < remaining - (int)write1; j++)
                {
                    char c = (char)fgetc(file);
                    if (c == '~')
                        counter++;
                }
                disk_node->files[fd]->filesize += counter;
                fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
                size_t write2 = fwrite(buf2, sizeof(char), strlen(buf2), file);
                disk_node->files[fd]->positionPtr += strlen(buf2);
                fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
                res = write1 + write2;
            }
            /*
             * Else, we need to allocate a new free block to the file, and continue writing there.
             */
            else
            {
                int k;
                for (k = 0; k < MAX_NUM_BLOCKS; k++)
                {
                    if (disk_node->files[fd]->allocatedBlocks[k] == 0)
                        break;
                }
                for (int i = 1; i < MAX_NUM_BLOCKS; i++)
                {
                    if (allocated_blocks[i] == 0)
                    {
                        allocated_blocks[i] = 1;
                        disk_node->files[fd]->allocatedBlocks[k] = i;
                        break;
                    }
                }
                disk_node->files[fd]->positionPtr = disk_node->files[fd]->allocatedBlocks[k] * 1024;
                char *buf2 = malloc(sizeof(char) * (remaining - (int)write1));
                strncpy(buf2, buf + (int)write1, remaining - (int)write1);
                fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
                char *readbuf[remaining - (int)write1];
                int counter = 0;

                for (int j = 0; j < remaining - (int)write1; j++)
                {
                    char c = (char)fgetc(file);
                    if (c == '~')
                        counter++;
                }

                disk_node->files[fd]->filesize += counter;

                fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
                size_t write2 = fwrite(buf2, sizeof(char), strlen(buf2), file);
                disk_node->files[fd]->positionPtr += strlen(buf2);
                fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
                res = write1 + write2;
            }
        }
        else
        {
            if (allocated_blocks[(int)((disk_node->files[fd]->positionPtr) / 1024)] == 0)
            {
                allocated_blocks[(int)(disk_node->files[fd]->positionPtr / 1024)] = 1;
                int k;
                for (k = 0; k < MAX_NUM_BLOCKS; k++)
                {
                    if (disk_node->files[fd]->allocatedBlocks[k] == 0)
                    {
                        disk_node->files[fd]->allocatedBlocks[k] = (int)(disk_node->files[fd]->positionPtr / 1024);
                        break;
                    }
                }
            }
            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            char *readbuf[remaining];
            int counter = 0;

            for (int j = 0; j < remaining; j++)
            {
                char c = (char)fgetc(file);
                if (c == '~')
                    counter++;
            }

            disk_node->files[fd]->filesize += counter;

            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            size_t write3 = fwrite(buf, sizeof(char), remaining, file);
            disk_node->files[fd]->positionPtr += remaining;
            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            res = write3;
        }
        remaining -= res;
    }
    return res;
}

/*
 * Reads the file. Current implementation tracks the end of file by keeping the index of first encounter with the "garbage" character '~'.
 */
int kufs_read(int fd, void *buf, int n)
{
    void *buf2 = malloc(sizeof(char));
    int k, i = 0, j;
    size_t res = malloc(sizeof(size_t));
    for (k = 0; k < MAX_NUM_BLOCKS; k++)
    {
        if (disk_node->files[fd]->allocatedBlocks[k] == 0)
            break;
    }
    char *temp = malloc(sizeof(char));
    fseek(file, disk_node->files[fd]->positionPtr + n, SEEK_SET);
    fread(temp, 1, 1, file);
    char *test = "~";
    int flag = 1;
    while (flag)
    {
        if ((int)((disk_node->files[fd]->positionPtr + n) / 1024) == (int)(disk_node->files[fd]->positionPtr / 1024) && strcmp(temp, test) != 0)
        {
            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            res = fread(buf2, 1, n, file);
            strncpy(buf + i, buf2, n);
            i += disk_node->files[fd]->positionPtr;
            flag = 0;
        }
        else if ((int)((disk_node->files[fd]->positionPtr + n) / 1024) == (int)(disk_node->files[fd]->positionPtr / 1024) && strcmp(temp, test) == 0)
        {
            while (strcmp(temp, test) == 0)
            {
                n--;
                fseek(file, disk_node->files[fd]->positionPtr + n, SEEK_SET);
                fread(temp, 1, 1, file);
            }
            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            res = fread(buf, 1, n, file);
            strncpy(buf + i, buf2, n);
            i += disk_node->files[fd]->positionPtr;

            flag = 0;
        }
        else if ((int)((disk_node->files[fd]->positionPtr + n) / 1024) != (int)(disk_node->files[fd]->positionPtr / 1024))
        {

            int blockNo = (int)(disk_node->files[fd]->positionPtr / 1024);
            int k;
            for (k = 0; k < MAX_NUM_BLOCKS; k++)
            {
                if (disk_node->files[fd]->allocatedBlocks[k] == blockNo)
                    break;
            }
            int lim = (blockNo + 1) * 1024;
            buf2 = malloc(sizeof(char) * lim - disk_node->files[fd]->positionPtr);
            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            res = fread(buf2, 1, lim - disk_node->files[fd]->positionPtr, file);
            blockNo = disk_node->files[fd]->allocatedBlocks[k + 1];
            strncpy(buf + i, buf2, lim - disk_node->files[fd]->positionPtr);
            i += lim - disk_node->files[fd]->positionPtr;
            disk_node->files[fd]->positionPtr = disk_node->files[fd]->allocatedBlocks[k + 1] * 1024;
            fseek(file, disk_node->files[fd]->positionPtr, SEEK_SET);
            n = n - res;
        }
    }
    return res;
}

/*
 * Updates the positionPtr by finding which block nth byte corresponds to, and changing positionPtr to that value.
 */
int kufs_seek(int fd, int n)
{
    int nthBlock = (int)(n / 1024);
    disk_node->files[fd]->positionPtr = disk_node->files[fd]->allocatedBlocks[nthBlock] * 1024 + n % 1024;
    return 0;
}

/*
 * Prints the name of existing files & blocks that are allocated to them
 */
void kufs_dump_fat()
{
    for (int i = 0; i < MAX_NUM_FILES; i++)
    {
        if (disk_node->files[i] != NULL && disk_node->files[i]->deleteflag != 1)
        {
            printf("%s: ", disk_node->files[i]->filename);
            for (int j = 0; j < MAX_NUM_BLOCKS; j++)
            {
                if (disk_node->files[i]->allocatedBlocks[j] != 0)
                    printf("%d ", disk_node->files[i]->allocatedBlocks[j]);
                else
                    break;
            }
            printf("\n");
        }
    }
}
#endif /* KUFS_H_ */
