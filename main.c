#include "mapred.h"
void checkArgs(int argc, char **argv, Map_Func map, Reduce_Func reduce);
void printHelp(void);
int isNumber(char *string);

int main(int argc, char **argv)
{
    checkArgs(argc, argv);
    int numMaps = atoi(argv[2]);
    int numReds = atoi(argv[3]);
    FILE *inputs[numMaps];
    SortedListPtr lists[numMaps];
    CompareFuncT cf = compareStrings;
    Map_Func map;
    Reduce_Func reduce;

    splitInput(argv);
    assignFilePtrs(inputs, numMaps,argv[4]);
    pthread_t mapid[numMaps];
    pthread_t redid[numReds];
    int i, err;
    for(i = 0; i < numMaps; i++)
    {
        lists[i] = SLCreate(cf);
        err = pthread_create(&mapid[i], NULL, map, (void*)createArgPtr(inputs[i],lists[i]));
        if(err != 0)
        {
            fprintf(stderr, "\nERROR: Failed to create map worker thread: %s", strerror(err));
            exit(EXIT_FAILURE);
        }
    }
    void *threadResult;
    for(i = 0; i < numMaps; i++)
    {
        pthread_join(mapid[i], &threadResult);
    }
    free(threadResult);

    for(i = 0; i < numMaps; i++)
    {
        printf("\n\nlist:%d",i);
        display(lists[i]);
    }
    cleanup(argv[4], numMaps, inputs, lists);




    return 0;
}

void printHelp()
{
    printf("\nThis program accepts 5 arguments: \narg1: 'wordcount' or 'sort'");
    printf("\narg2: integer number of map workers\narg3: integer number of reduce workers");
    printf("\narg4: input file\narg5: outputfile");
    printf("\nEXAMPLE: wordcount 10 4 input.txt output.txt\n");
}
void checkArgs(int argc, char **argv, Map_Func map, Reduce_Func reduce)
{
    /* Description: checks arguments for validity
    ** Modifies: nothing
    ** Returns: nothing
    */
    FILE *input;

    if(argc == 2)
    {
        if(strcmp("-help", argv[1]) == 0)
        {
            printHelp();
            exit(EXIT_SUCCESS);
        }
    }
    if(argc != 6)
    {
        fprintf(stderr,"\nERROR: Expected 5 arguments but received: %d\n", argc-1);
        fprintf(stderr, "\nRerun with '-help' to display help screen\n");
        exit(EXIT_FAILURE);
    }
    if(!isNumber(argv[2]))
    {
        fprintf(stderr,"\nERROR: arg 2 expected to be integer number of map workers, but received: %s\n", argv[2]);
        fprintf(stderr, "\nRerun with '-help' to display help screen\n");
        exit(EXIT_FAILURE);
    }
    if(!isNumber(argv[3]))
    {
        fprintf(stderr,"\nERROR: expected arg 3 to be integer number of reduce workers, but received: %s\n", argv[3]);
        fprintf(stderr, "\nRerun with '-help' to display help screen\n");
        exit(EXIT_FAILURE);
    }
    if((strcmp(argv[1],"wordcount") == 0) && (strcmp(argv[1], "sort")== 0))
    {
        map = map_wordcount;
    }
    else if((strcmp(argv[1], "sort")== 0) )
    {

    }
    else
    {
        fprintf(stderr,"\nERROR expected arg 1 to be 'wordcount' or 'sort' but received: %s", argv[1]);
        fprintf(stderr, "\nRerun with '-help' to display help screen\n");
        exit(EXIT_FAILURE);
    }
    if((input = fopen(argv[4],"r")) == NULL)
       {
           fprintf(stderr, "ERROR: failed to open input file '%s' please check file directory", argv[4]);
           fprintf(stderr, "\nRerun with '-help' to display help screen\n");
           exit(EXIT_FAILURE);
       }
    fclose(input);

}
int isNumber(char *string)
{
    /* Description: checks if a string is comprised of only digits
    ** Modifies: nothing
    ** Returns: 0 if string is not all digits; 1 if string is all digits
    */
    int len = strlen(string);
    int i;
    for(i = 0; i < len; i++)
    {
        if(!isdigit(string[i])){return 0;}
    }
    return 1;
}
