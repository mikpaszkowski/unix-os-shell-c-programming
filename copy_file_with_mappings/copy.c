#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define KRED  "\x1B[31m"
#define KNRM  "\x1B[0m"


void help();
void throw_error(char*);
int isNumberOfArgumentsValid(int, int);
void copy_read_write(int, int);
void copy_with_mmap(int sourceFile, int destinationFile);
void checkMappingResult(char* const sourceBuffer);
void checkCopyMemory(char* const sourceBuffer);
void unmapPagesOfMemory(char* sourceBuffer, char* destinationBuffer, off_t size);


int main(int argc, char *argv[])
{

    int m_flag = 0;

    if(argc == 1){
        help();
        exit(1);
    }

    int c;

    while((c = getopt(argc, argv, ":mh")) != -1){
        switch(c){
            case 'm':
                m_flag = 1;
                break;
            case 'h':
                help();
                exit(0);
            case '?':
                throw_error("Provided option is invalid, please enter option -h for further information");
        }
    }

    if(!isNumberOfArgumentsValid(argc, m_flag)){
        throw_error("Provided number of arguments is not valid");
    }

    /**
     * open system call opens the file specified by the pathname
     * if it does not exists this method creates one if O_CREAT flag specified
     * return  value of open is a file descriptor - a small, nonnegative integer that
     * is an index to an entry in the process's table of open file descriptors
     */
    int sourceFile = open(argv[m_flag + 1], O_RDONLY);

    struct stat stat_src;
    if(fstat(sourceFile, &stat_src) == -1)
    {
        throw_error("Error loading src file information");
        exit(1);
    }

    int destinationFile = open(argv[m_flag + 2], O_RDWR | O_CREAT, stat_src.st_mode);

    if(m_flag){
        copy_with_mmap(sourceFile, destinationFile);
    }else {
        copy_read_write(sourceFile, destinationFile);
    }

    if(close(sourceFile) == 0){
        printf("Source file closed with success\n");
    }
    if(close(destinationFile) == 0){
        printf("Destination file closed with success\n");
    }

   return 0;
}


void copy_read_write(int sourceFile, int destinationFile){

    //clearing the destination file, if -1 returned then error occured
    if(ftruncate(destinationFile, 0) == -1){
        throw_error("Clearing destination file failure");
    }

    //fstat is a function which retrieve information about the file specified
    //by the file descriptor fd
    struct stat fileStatus;
    if(fstat(sourceFile, &fileStatus) == -1){
        throw_error("Error loading src file information");
    }

    //creating buffer with a size assigned to st_size by the fstat function
    char BUFFER[fileStatus.st_size];

    int numOfReadBytes, numOfWrittenBytes;

    //if the value (number of bytes read) returned by the read is bigger than
    // 0 then success otherwise end of file
    while ((numOfReadBytes = read(sourceFile, BUFFER, fileStatus.st_size)) > 0){
        numOfWrittenBytes = write(destinationFile, BUFFER, numOfReadBytes);

        //check if the write function return negative what indicates error
        if(numOfWrittenBytes <= 0){
            throw_error("Error while writting data to the file");
        }
    }
}


void copy_with_mmap(int sourceFile, int destinationFile){

    //fstat is a function which retrieve information about the file specified
    //by the file descriptor fd
    struct stat fileStatus;
    if(fstat(sourceFile, &fileStatus) == -1){
        throw_error("Error loading src file information");
    }

    //on success mmap returns a pointer to the mapped area
    char* sourceBuffer; 
    //If addr is NULL, then the kernel chooses the (page-aligned)
    //    address at which to create the mapping; this is the most portable
    //    method of creating a new mapping.
    // If addr is not NULL, then the
    //    kernel takes it as a hint about where to place the mapping; on
    //    Linux, the kernel will pick a nearby page boundary (but always
    //    above or equal to the value specified by
    //    /proc/sys/vm/mmap_min_addr) and attempt to create the mapping there.
    sourceBuffer = mmap(NULL, fileStatus.st_size, PROT_READ,MAP_SHARED,sourceFile,0);
    checkMappingResult(sourceBuffer);
    //clearing the destination file, if -1 returned then error occured
    if(ftruncate(destinationFile, 0) == -1){
        throw_error("Clearing destination file failure");
    }

    char* destinationBuffer; 
    destinationBuffer = mmap(NULL, fileStatus.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, destinationFile,0);
    checkMappingResult(destinationBuffer);


    ftruncate(destinationFile, fileStatus.st_size);   
    
    destinationBuffer = memcpy(destinationBuffer, sourceBuffer, fileStatus.st_size);
    checkCopyMemory(destinationBuffer);

    unmapPagesOfMemory(sourceBuffer, destinationBuffer, fileStatus.st_size);
}

void unmapPagesOfMemory(char* sourceBuffer, char* destinationBuffer, off_t size){
    if(munmap(sourceBuffer, size) == -1){
        throw_error("Unmapping of source buffer failure");
    }
    if(munmap(destinationBuffer, size)){
        throw_error("Unmapping of destination buffer failure");
    }
}

void checkMappingResult(char* const sourceBuffer){
    // On error, the value MAP_FAILED (that is, (void *) -1) is returned
    if(sourceBuffer == (void *) -1){
        throw_error("Mapping file into memory failure.");
    }
}

void checkCopyMemory(char* const sourceBuffer){
    // On error, the value MAP_FAILED (that is, (void *) -1) is returned
    if(sourceBuffer == (void *) -1){
        throw_error("Copying memory failure");
    }
}


int isNumberOfArgumentsValid(int argc, int m_flag){
    return (argc == 2 && !m_flag) || ((argc == 4 && m_flag) || (argc == 3 && !m_flag)) || argc < 2;
}

void help(){
    printf("\nUsage: copy [OPTION]... PREVIOUS_FILE... NEW_FILE...\n\n");
    printf("\tcopy [-m] <file_name> <new_file_name>\n");
    printf("\tcopy [-h]\n\n");
}


void throw_error(char *message){
    printf(KRED"ERROR: ");
    printf(KRED"%s\n\n"KNRM, message);
    help();
}
