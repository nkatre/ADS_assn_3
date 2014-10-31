/*
 * final.c
 *
 *  Created on: Oct 27, 2014
 *      Author: nkatre
 */

/*  1 --basic
	2 input.bin
	3 sort.bin
*/

// TODO : 40 to 100 in all the structs

#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <limits.h>
#include <sys/time.h>

#pragma pack(1)
typedef struct{
	int *integers;
	int int_size;    // size of integers in this struct
	int int_start;
	char filename[40];
	int available;
}inputBufferStruct;

typedef struct{
	char filename[40];
	int offset;
	int left;
	int total;
	int available;
}sub_fo;

typedef struct{
	char filename[40];
	int offset;
	int left;
	int total;
	int available;
}super_fo;


inputBufferStruct *ptr = NULL;
inputBufferStruct *superPtr=NULL;
sub_fo *small_fo=NULL;
super_fo *big_fo = NULL;

int totalSortedFiles=0;
int inputBuffer[1000]={0};
int outputBuffer[1000]={0};
int max_inputBufferSize=1000;  //change
int max_outputBufferSize=1000;  //change

int superFiles = 15;  // change    added for --multistep
int superRun=0;

int currentHeapSize = 750;                  // added for --replacement
int currentInputBufSize = 250;
int currentSecHeapSize=0;




int act_outputBufferSize=0;
int act_inputBufferSize=0;
int mergeType=0;  //mergeType


int totalBytes=0;
int totalInts = 0;

// --basic

void garbageCollector(sub_fo *small_fo, inputBufferStruct *ptr,int size, int mergeType);
inputBufferStruct* resizeArray(inputBufferStruct *ptr, int counter,int size, char file[]);
inputBufferStruct* readValues(inputBufferStruct *ptr, int counter, int size);
void fileInput(int start, int end, int maxPossible, sub_fo *small_fo);
void printPtr(inputBufferStruct *ptr, int total);
void writeSortedRuns(int start, int end, inputBufferStruct *ptr, char file[]);
int findMinElementSequentialSearch(inputBufferStruct *ptr,int start,int size);
int checkOverflow(inputBufferStruct *ptr, int counter);
inputBufferStruct* readNewInputsFromFile(inputBufferStruct *ptr, int counter);
sub_fo* modify_small_fo_Values(sub_fo *small_fo,int counter, int diff);
void readOutputFile(char file[]);

// -- multistep

void createSuperSortedRuns(int start, int end, inputBufferStruct *ptr, char file[]);
void superFileInput(int start, int end, int maxPossible, super_fo *big_fo);
void superWriteSortedRuns(int start, int end, inputBufferStruct *superPtr, char file[]);
int superFindMinElementSequentialSearch(inputBufferStruct *superPtr,int start,int size);
int superCheckOverflow(inputBufferStruct *superPtr, int counter);
inputBufferStruct* superReadNewInputsFromFile(inputBufferStruct *superPtr, int counter);
super_fo* modify_big_fo_Values(super_fo *big_fo,int counter, int diff);
inputBufferStruct* superReadValues(inputBufferStruct *superPtr, int counter, int size);


// --replacment

char *getRunFileName(char *input, int totalSortedFiles, char *output);
sub_fo* registerSortedFilesForReplacement(char input[],int totalSortedFiles);


sub_fo* register_sub_fo(sub_fo *small_fo, int counter, char file[], int offset, int total, int left){
	if(small_fo==NULL){
		small_fo = (sub_fo *)malloc(sizeof(sub_fo));
		small_fo[counter].available = 1;
		strncpy(small_fo[counter].filename,file,strlen(file));
		//strncpy(small_fo[counter].filename,file,_countof(small_fo[counter].filename) - 1);
		small_fo[counter].filename[strlen(file)] = '\0';
		small_fo[counter].offset = offset;
		small_fo[counter].total = total;
		small_fo[counter].left = left;
	}
	else if(small_fo!=NULL){
		        small_fo = (sub_fo *)realloc(small_fo,(counter+1)*sizeof(sub_fo));
				small_fo[counter].available = 1;
				strncpy(small_fo[counter].filename,file,strlen(file));
			    small_fo[counter].filename[strlen(file)] = '\0';
				small_fo[counter].offset = offset;
				small_fo[counter].total = total;
				small_fo[counter].left = left;
	}
	return small_fo;
}



super_fo* register_super_fo(super_fo *big_fo, int counter, char file[], int offset, int total, int left){
	if(big_fo==NULL){
		big_fo = (super_fo *)malloc(sizeof(super_fo));
		big_fo[counter].available = 1;
		strncpy(big_fo[counter].filename,file,strlen(file));
		big_fo[counter].filename[strlen(file)] = '\0';
		big_fo[counter].offset = offset;
		big_fo[counter].total = total;
		big_fo[counter].left = left;
	}
	else if(big_fo!=NULL){
		        big_fo = (super_fo *)realloc(big_fo,(counter+1)*sizeof(super_fo));
		        big_fo[counter].available = 1;
		        strncpy(big_fo[counter].filename,file,strlen(file));
		        big_fo[counter].filename[strlen(file)] = '\0';
		        big_fo[counter].offset = offset;
		        big_fo[counter].total = total;
		        big_fo[counter].left = left;
	}
	return big_fo;
}



int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
void print(sub_fo *small_fo){
	int i=0;
	for(i=0;i<totalSortedFiles;i++){
	printf("FileName : %s\n",small_fo[i].filename);
	printf("Offset : %i\n",small_fo[i].offset);
	printf("Left : %i\n",small_fo[i].left);
	printf("Total : %i\n",small_fo[i].total);
	printf("Available : %i\n",small_fo[i].available);
	}
}

void print_super(super_fo *big_fo,int start, int end){
	int i=0;
	for(i=start;i<end;i++){
	printf("FileName : %s\n",big_fo[i].filename);
	printf("Offset : %i\n",big_fo[i].offset);
	printf("Left : %i\n",big_fo[i].left);
	printf("Total : %i\n",big_fo[i].total);
	printf("Available : %i\n",big_fo[i].available);
	}
}

void createSortedRuns(int start, int end, char file[]){

	FILE *givenFile = NULL;
	givenFile=fopen(file,"r+b");
	rewind(givenFile);
	int i=0;
    /*printf("File:%s\n",file);
    printf("File Length:%zu\n",strlen(file));*/
	//char *temp = (char *)malloc(sizeof(char)*(strlen(file)+5));
    char temp[50]={0};

	for(i=start; i<end; i++){
		fseek(givenFile,0,SEEK_CUR);
		fread(&inputBuffer[act_inputBufferSize],sizeof(int),1,givenFile);
		act_inputBufferSize++;

		if((act_inputBufferSize==max_inputBufferSize) ||(i==(end-1))){

			//sort
			qsort(inputBuffer,act_inputBufferSize,sizeof(int),compare);

			// write to file

			strncpy(temp,file,strlen(file));

			//int k=0;
			char counter[4]={0};
			/*for(k=0;k<4;k++){
				counter[k]=0;
			}*/


			int written = snprintf(counter,5, ".%03d", totalSortedFiles);
			//printf("Written: %i\n",written);
			counter[sizeof(counter)] = '\0';
			/*printf("Temp: %s\n",temp);
			printf("Counter: %s\n",counter);
			printf("Written: %i\n",written);*/
			strncat(temp,counter,written);


			FILE *int_File = NULL;
			int_File=fopen(temp,"w+b");
			rewind(int_File);
			fwrite(&inputBuffer,sizeof(int),act_inputBufferSize,int_File);

			// register the sub_fo information
			small_fo=register_sub_fo(small_fo,totalSortedFiles,temp, 0 ,act_inputBufferSize,act_inputBufferSize);


			// increase the totalSortedFiles
			totalSortedFiles+=1;


			// Reinitialize
						int j=0;
						for(j=0;j<max_inputBufferSize;j++){
							inputBuffer[j]=0;
						}
						act_inputBufferSize=0;

						int len = strlen(temp);
					    temp[len-4] = '\0';
		   // Garbage
						fclose(int_File);
						int_File = NULL;



		}
	} //for_end
	fclose(givenFile);
	givenFile=NULL;

/*	free(temp);
	temp=NULL;*/


} // func_end



int main(int argc, char *argv[]){

    if(strcmp("--basic",argv[1])==0)
	mergeType = 1;
    if(strcmp("--multistep",argv[1])==0)
	mergeType =2;
	if(strcmp("--replacement",argv[1])==0)
	mergeType =3;


	if(mergeType==1){

		// get the start time
		 struct timeval start;
		 gettimeofday( &start, NULL );

    FILE *inputReader = NULL;
    inputReader=fopen(argv[2],"r+b");
    rewind(inputReader);

    // get total integers
    fseek(inputReader,0,SEEK_END);
    totalBytes = ftell(inputReader);
    totalInts = totalBytes/sizeof(int);

    fclose(inputReader);
    inputReader=NULL;

    createSortedRuns(0,totalInts,argv[2]);     // small_fo info  and totalSorteFiles is achieved

   /* printf("************Before File Input Call - Small FO******************\n");
    print(small_fo);*/


    // get max possible input from each file
    int maxPossible = max_inputBufferSize/totalSortedFiles;
    fileInput(0,totalSortedFiles,maxPossible,small_fo);   // ptr initialized
   /* printf("************After File Input Call - Small FO******************\n");
      print(small_fo);
    printf("************After File Input Call - Pointer******************\n");
    printPtr(ptr,totalSortedFiles);*/

    writeSortedRuns(0,totalInts,ptr,argv[3]);
   /* printf("************Small FO******************");
         print(small_fo);
       printf("************Pointer******************");
       printPtr(ptr,totalSortedFiles);

    readOutputFile(argv[3]);*/

    garbageCollector(small_fo,ptr,(totalSortedFiles), mergeType);
    		// get the end time
     	 	 struct timeval end;
     	 	 gettimeofday( &end, NULL );


     	 	//print the total time to run the program
     	 	 struct timeval exec_tm;
     	 	 exec_tm.tv_sec=end.tv_sec-start.tv_sec;
     	 	 exec_tm.tv_usec=abs(end.tv_usec-start.tv_usec);
     	 	 printf( "Time: %ld.%06ld", exec_tm.tv_sec, exec_tm.tv_usec );
     	 	 printf("\n");

	}
	if(mergeType==2){
		// get the start time
			 struct timeval start;
			 gettimeofday( &start, NULL );

	    FILE *inputReader = NULL;
	    inputReader=fopen(argv[2],"r+b");
	    rewind(inputReader);

	    // get total integers
	    fseek(inputReader,0,SEEK_END);
	    totalBytes = ftell(inputReader);
	    totalInts = totalBytes/sizeof(int);

	    fclose(inputReader);
	    inputReader=NULL;

	    createSortedRuns(0,totalInts,argv[2]);  // small_fo info  and totalSorteFiles is achieved

	    /*printf("************Before File Input Call - Small FO******************");
	    print(small_fo);*/

/*

	    if((totalSortedFiles%mergeF)==0)
	    	superFiles=totalSortedFiles/mergeF;
	    else
	    	superFiles=(totalSortedFiles/mergeF) + 1;

	    //int maxPossible = (max_inputBufferSize/superFiles) + (max_inputBufferSize%superFiles);

	    int *maxPossible = (int *)malloc(sizeof(int)*superFiles);
	    int i=0;

	    for(i=0;i<superFiles;i++){
	    	maxPossible[i]=totalSortedFiles/superFiles;
	    }
	    for(i=0;i<(totalSortedFiles%superFiles);i++){
	    	maxPossible[i]+=1;
	    }
*/

	    int startIndex = 0;
	    int endIndex = 0;
	    int remainingFiles=totalSortedFiles;
	    int i=0;

	  /*  for(i=0;i<superFiles;i++){
	    	startIndex=endIndex;
	    	endIndex += maxPossible[i];
	    	fileInput(startIndex,endIndex,maxPossible[i],small_fo);  // initializes ptr
	    	createSuperSortedRuns(startIndex,endIndex,ptr,argv[2]);
	    }*/
        int max=0;
	    for(i=startIndex;i<totalSortedFiles;i=endIndex){
	    	startIndex = i;

	    	if(remainingFiles>=superFiles){
	    		endIndex += superFiles;
	    		max = (max_inputBufferSize/superFiles);
	    		fileInput(startIndex,endIndex,max,small_fo);  // ptr is achieved and small_fo is updated
	    	}
	    	else if(remainingFiles<superFiles){
	    		endIndex = totalSortedFiles;
	    		max = (max_inputBufferSize/remainingFiles);
	    		fileInput(startIndex,endIndex,max,small_fo);  // ptr is achieved and small_fo is updated
	    	}
	    	createSuperSortedRuns(startIndex,endIndex,ptr,argv[2]);   // big_fo and superRun is achieved
	    	remainingFiles -= superFiles;
	    }
	    remainingFiles=0;
	   /* printPtr(ptr,totalSortedFiles);
        print(small_fo);
        print_super(big_fo,0,superRun);*/

        // get max possible input from each file
        int maxPossible = max_inputBufferSize/superRun;

        superFileInput(0,superRun,maxPossible,big_fo);   // superPtr is achieved and big_fo is updated
        /*printPtr(superPtr,superRun);
*/

        int j=0;
        for(j=0;j<max_outputBufferSize;j++){
        outputBuffer[j]=0;
        }
        act_outputBufferSize=0;


        superWriteSortedRuns(0,totalInts,superPtr,argv[3]);

        /*readOutputFile(argv[3]);*/


/*
        // print super files
        printf("%s\n","input.bin.20.super.000");
        readOutputFile("input.bin.20.super.000");
        printf("%s\n","input.bin.20.super.001");
          readOutputFile("input.bin.20.super.001");
          printf("%s\n","input.bin.20.000");
                 readOutputFile("input.bin.20.000");
                 printf("%s\n","input.bin.20.001");
                        readOutputFile("input.bin.20.001");
                        printf("%s\n","input.bin.20002");
                               readOutputFile("input.bin.20.002");
                               printf("%s\n","input.bin.20.003");
                                      readOutputFile("input.bin.20.003");*/


        garbageCollector(small_fo,ptr,totalSortedFiles,2);

        // get the end time
             	 	 struct timeval end;
             	 	 gettimeofday( &end, NULL );


             	 	//print the total time to run the program
             	 	 struct timeval exec_tm;
             	 	 exec_tm.tv_sec=end.tv_sec-start.tv_sec;
             	 	 exec_tm.tv_usec=abs(end.tv_usec-start.tv_usec);
             	 	 printf( "Time: %ld.%06ld", exec_tm.tv_sec, exec_tm.tv_usec );
             	 	 printf("\n");

	}

	if(mergeType==3){

		// get the start time
		 struct timeval start;
		 gettimeofday( &start, NULL );

    FILE *inputReader = NULL;
    inputReader=fopen(argv[2],"r+b");
    rewind(inputReader);

    // get total integers
    fseek(inputReader,0,SEEK_END);
    totalBytes = ftell(inputReader);
    totalInts = totalBytes/sizeof(int);

    fclose(inputReader);
    inputReader=NULL;

    FILE *outputWriter = NULL;

    char temp[100]={0};

    int lastFileSize = 0;
    int totalSortedNumbers = 0;


    inputReader=fopen(argv[2],"r+b");
    rewind(inputReader);

    lastFileSize = fread(inputBuffer,sizeof(int), 1000, inputReader);
    if(lastFileSize < 750){
    	currentInputBufSize = 0;
    	currentHeapSize = lastFileSize;
    }
    else{
    	currentInputBufSize = lastFileSize - currentHeapSize;
    }

    totalSortedFiles = 0;
    getRunFileName(argv[2],totalSortedFiles,temp);

    int *currentInputBufPtr = NULL;
    int *currentHeapPtr = NULL;


    currentInputBufPtr = inputBuffer + currentHeapSize;
    currentHeapPtr = inputBuffer;

    outputWriter = fopen(temp,"w+b");

    while(1){

    	if(currentHeapSize==0){
    		break;
    	}

    	qsort(inputBuffer,currentHeapSize,sizeof(int),compare);
    	outputBuffer[act_outputBufferSize] = *currentHeapPtr;

    	act_outputBufferSize++;
    	totalSortedNumbers++;
    	if((*currentHeapPtr<=*currentInputBufPtr) && (currentInputBufSize > 0)){
    		*currentHeapPtr = *currentInputBufPtr;
    		currentInputBufPtr++;
    		currentInputBufSize--;

    		if(currentInputBufSize==0){
    			currentInputBufSize = fread(inputBuffer+750,sizeof(int), 250,inputReader);
    			currentInputBufPtr = inputBuffer + 750;
    		}

    	}
    	else{
    		*currentHeapPtr = *(currentHeapPtr+ currentHeapSize-1);
    		if(currentInputBufSize>0){
    			*(currentHeapPtr+currentHeapSize-1)= *currentInputBufPtr;
    			currentSecHeapSize++;
    			currentInputBufSize--;
    			currentInputBufPtr++;

    			if(currentInputBufSize==0){
    				currentInputBufSize = fread(inputBuffer+750, sizeof(int),250,inputReader);
    				currentInputBufPtr = inputBuffer+750;
    			}
    		}

    		currentHeapSize--;
    		if(currentHeapSize==0){
    			if(act_outputBufferSize > 0){
    				fwrite(outputBuffer,sizeof(int),act_outputBufferSize,outputWriter);
    				// register the sub_fo information
    				//small_fo=register_sub_fo(small_fo,totalSortedFiles,temp, 0 ,act_outputBufferSize,act_outputBufferSize);
    				act_outputBufferSize=0;
    				int len = strlen(temp);
    				temp[len-4]='\0';

    			}
    			fclose(outputWriter);
    			if(currentSecHeapSize>0){
    				totalSortedFiles++;
    				   getRunFileName(argv[2],totalSortedFiles,temp);
    				outputWriter = fopen(temp,"w+b");
    				currentHeapSize = currentSecHeapSize;
    				if(currentHeapSize<750)
    					memmove(inputBuffer,inputBuffer+750-currentSecHeapSize,currentSecHeapSize*sizeof(int));
    				currentSecHeapSize=0;
    			}
    		}
    	}
    	if(act_outputBufferSize==1000){
    		fwrite(outputBuffer,sizeof(int),act_outputBufferSize,outputWriter);
    		// register the sub_fo information
    		//small_fo=register_sub_fo(small_fo,totalSortedFiles,temp, 0 ,act_outputBufferSize,act_outputBufferSize);
    		act_outputBufferSize=0;
    		int len = strlen(temp);
    	    temp[len-4]='\0';
    	}

    }


    if(act_outputBufferSize>0){
    	fwrite(outputBuffer,sizeof(int),act_outputBufferSize,outputWriter);
    	// register the sub_fo information
    	//small_fo=register_sub_fo(small_fo,totalSortedFiles,temp, 0 ,act_outputBufferSize,act_outputBufferSize);
    	fclose(outputWriter);
    	int len = strlen(temp);
        temp[len-4]='\0';
    }


   fclose(inputReader);
   inputReader=NULL;

   int i=0;
   for(i=0;i<max_outputBufferSize;i++){
	   outputBuffer[i]=0;
   }
   act_outputBufferSize=0;

  /* printf("%i\n",totalSortedFiles);
   printf("%i\n",totalSortedNumbers);*/
   totalSortedFiles +=1;
   // register small_fo information
   small_fo = registerSortedFilesForReplacement(argv[2],totalSortedFiles);

    int maxPossible = max_outputBufferSize/totalSortedFiles;
    fileInput(0,totalSortedFiles,maxPossible,small_fo);   // ptr initialized
  /* printf("************After File Input Call - Small FO******************\n");
      print(small_fo);
    printf("************After File Input Call - Pointer******************\n");
    printPtr(ptr,totalSortedFiles);
*/
    writeSortedRuns(0,totalInts,ptr,argv[3]);
   /* printf("************Small FO******************");
         print(small_fo);
       printf("************Pointer******************");
       printPtr(ptr,totalSortedFiles);

    readOutputFile(argv[3]);*/

    garbageCollector(small_fo,ptr,totalSortedFiles, mergeType);
    		// get the end time
     	 	 struct timeval end;
     	 	 gettimeofday( &end, NULL );


     	 	//print the total time to run the program
     	 	 struct timeval exec_tm;
     	 	 exec_tm.tv_sec=end.tv_sec-start.tv_sec;
     	 	 exec_tm.tv_usec=abs(end.tv_usec-start.tv_usec);
     	 	 printf( "Time: %ld.%06ld", exec_tm.tv_sec, exec_tm.tv_usec );
     	 	 printf("\n");

	}
	return 0;
}

void garbageCollector(sub_fo *small_fo, inputBufferStruct *ptr,int size, int mergeType){

	if((mergeType == 1)||(mergeType == 3)){   // --basic
    free(small_fo);
    small_fo=NULL;

    int i=0;
    for(i=0;i<size;i++){
    	free(ptr[i].integers);
    	ptr[i].integers=NULL;
    }
    free(ptr);
    ptr=NULL;
	}


	if(mergeType == 2){   // --multistep
	    free(small_fo);
	    small_fo=NULL;

	    free(big_fo);
	    big_fo=NULL;

	    int i=0;
	    for(i=0;i<size;i++){
	    	free(ptr[i].integers);
	    	ptr[i].integers=NULL;
	    }
	    free(ptr);
	    ptr=NULL;

	    for(i=0;i<superRun;i++){
	    	    	free(superPtr[i].integers);
	    	    	superPtr[i].integers=NULL;
	    	    }
	    	    free(superPtr);
	    	    superPtr=NULL;

		}

}

inputBufferStruct* resizeArray(inputBufferStruct *ptr, int counter,int size, char file[]){
	if(ptr==NULL){
		ptr = (inputBufferStruct *)malloc(sizeof(inputBufferStruct));
		ptr[counter].int_size = size;
		ptr[counter].int_start = 0;
		ptr[counter].integers = (int*)(malloc(sizeof(int)*size));
		strncpy(ptr[counter].filename,file,strlen(file));
		ptr[counter].filename[strlen(file)] = '\0';
		ptr[counter].available = 1;
	}
	else if(ptr!=NULL){
		ptr = (inputBufferStruct *)realloc(ptr,(counter+1)*sizeof(inputBufferStruct));
		ptr[counter].int_size = size;
		ptr[counter].int_start = 0;
		ptr[counter].integers = (int*)(malloc(sizeof(int)*size));
		strncpy(ptr[counter].filename,file,strlen(file));
		ptr[counter].filename[strlen(file)] = '\0';
		ptr[counter].available = 1;
	}
	return ptr;
}
inputBufferStruct* readValues(inputBufferStruct *ptr, int counter, int size){
	if(ptr == NULL){
		printf("Cannot read the file to an empty structure \n");
	}
	else{
		FILE *tempInputFile = NULL;
		tempInputFile=fopen(ptr[counter].filename,"r+b");
		rewind(tempInputFile);
		int offset = (small_fo[counter].offset) * sizeof(int);
		fseek(tempInputFile,offset,SEEK_SET);
		int i=0;
		for(i=0;i<size;i++){
			fseek(tempInputFile,0,SEEK_CUR);
			fread(&ptr[counter].integers[i],sizeof(int),1,tempInputFile);
			//offset += sizeof(int);
		}
		ptr[counter].int_size=size;
		ptr[counter].available=1;
		ptr[counter].int_start=0;
		//garbage collection
		fclose(tempInputFile);
		tempInputFile=NULL;
	}

	return ptr;
}
void fileInput(int start, int end, int maxPossible, sub_fo *small_fo){   // we know that the total size of small_fo = totalSortedFiles = ptr size
	int i=0;
	for(i=start;i<end;i++){
		if(maxPossible<=small_fo[i].left){
			ptr=resizeArray(ptr,i,maxPossible,small_fo[i].filename);
			ptr = readValues(ptr,i,maxPossible);
			small_fo[i].offset += maxPossible;
			small_fo[i].left -= maxPossible;
		}
		else if(maxPossible>small_fo[i].left){
			ptr = resizeArray(ptr,i,small_fo[i].left,small_fo[i].filename);
			ptr = readValues(ptr,i,small_fo[i].left);
			small_fo[i].offset += small_fo[i].left;
			small_fo[i].left = 0;
		}
		if(small_fo[i].left==0){             // TODO : Should I remove this condition
			small_fo[i].available=0;
		}
	}

}

void printPtr(inputBufferStruct *ptr, int total){
	int i=0,j=0;
	for(i=0;i<total;i++){
		printf("Pointer number: %d\n",i);
		printf("Size of pointer is : %i\n",ptr[i].int_size);
		printf("Starting Index: %i\n",(ptr[i].int_start));
		printf("Associated file name is: %s\n",ptr[i].filename);
		for(j=0;j<ptr[i].int_size;j++)
		printf("Elements of the integer are: %i\n",ptr[i].integers[j]);
		printf("Available: %i\n",ptr[i].available);
	}
}
void writeSortedRuns(int start, int end, inputBufferStruct *ptr, char file[]){    // here the end is totalInts for mergeType==1

	FILE *outFile = NULL;
	//printf("%s\n",file);
    outFile=fopen(file,"w+b");
	rewind(outFile);
	int i=0;
	int offset_OB = 0;
	int minElement =0;
	for(i=start;i<end;i++){
		if((mergeType==1) || (mergeType==3))
			minElement = findMinElementSequentialSearch(ptr,start,totalSortedFiles);
		if(mergeType==2)                                                             // changes for mergeType == 2
			minElement = findMinElementSequentialSearch(ptr,start,superRun);   // this end would be replaced*/
		outputBuffer[act_outputBufferSize] = minElement;
		act_outputBufferSize++;

		if((act_outputBufferSize==max_outputBufferSize) ||(i==(end-1))){

            fseek(outFile,offset_OB,SEEK_SET);
            fwrite(&outputBuffer,sizeof(int),act_outputBufferSize,outFile);
            offset_OB +=act_outputBufferSize * sizeof(int);
            // garbage Collection
            int j=0;
            for(j=0;j<max_outputBufferSize;j++){
            	outputBuffer[j]=0;
            }
            act_outputBufferSize=0;


            /*printf("************Small FO******************Counter: %i\n");
            print(small_fo);
            printf("************Pointer******************Counter: %i\n");
            printPtr(ptr,totalSortedFiles);*/

		}
	}
	fclose(outFile);
	outFile=NULL;
}

int findMinElementSequentialSearch(inputBufferStruct *ptr,int start,int size){    // changes for mergeType == 2 , added start

	int minElement = INT_MAX;
	int minElementStructIndex = 0;
    int i=0;
    int startIndex = 0;
    for(i=start;i<size;i++){
    	if(checkOverflow(ptr,i)==1)
    		ptr=readNewInputsFromFile(ptr,i);
    	startIndex = ptr[i].int_start;
    	if(	(ptr[i].available==1) &&
    			(ptr[i].integers[startIndex]<=minElement) &&
    			(checkOverflow(ptr,i)==0)){
    		minElement=ptr[i].integers[startIndex];
    		minElementStructIndex = i;
    	}
    }
    ptr[minElementStructIndex].int_start++; // mark the minElement in the Struct as VISITED
    int result = checkOverflow(ptr,minElementStructIndex); // Check for Overflow
    if(result==1){   // read the next set of integers and return min
    	ptr = readNewInputsFromFile(ptr,minElementStructIndex);
    }
    return minElement;
}
int checkOverflow(inputBufferStruct *ptr, int counter){
	if(ptr[counter].int_start >= ptr[counter].int_size)
		return 1;
	else
		return 0;
}
inputBufferStruct* readNewInputsFromFile(inputBufferStruct *ptr, int counter){
	int noOfIntegersLeftToBeRead=0;

	    	noOfIntegersLeftToBeRead = small_fo[counter].left;

    	if(noOfIntegersLeftToBeRead >= ptr[counter].int_size){
    		ptr = readValues(ptr,counter,ptr[counter].int_size);
    		// modify small_fo Values
            small_fo = modify_small_fo_Values(small_fo,counter,ptr[counter].int_size);
    	}

    	else
    		{
    		if(noOfIntegersLeftToBeRead == 0)
    		ptr[counter].available = 0;
    	    else{
    	    	if(noOfIntegersLeftToBeRead < ptr[counter].int_size)
    	    	{
    		   ptr = readValues(ptr,counter,noOfIntegersLeftToBeRead);
    		    // modify small_fo Values
    		   small_fo = modify_small_fo_Values(small_fo,counter,noOfIntegersLeftToBeRead);
    	    	}
    	     }
    	}
    return ptr;
}
sub_fo* modify_small_fo_Values(sub_fo *small_fo,int counter, int diff){
	small_fo[counter].left -= diff;
	small_fo[counter].offset +=diff;
	if(small_fo[counter].left==0){      // TODO: This can be <= instead of ==
		small_fo[counter].available=0;
	}

	return small_fo;
}
void readOutputFile(char file[]){
     FILE *OFReader=NULL;
     OFReader = fopen(file,"r+b");
     rewind(OFReader);
     fseek(OFReader,0,SEEK_END);
     int totalB = ftell(OFReader);
     int totalI = totalB/sizeof(int);
     rewind(OFReader);
     int *outputInt = (int *)malloc(sizeof(int)*totalI);
     int i=0;
     int offset = 0;
     for(i=0;i<totalI;i++){
    	 fseek(OFReader,offset,SEEK_SET);
    	 fread(&outputInt[i],sizeof(int),1,OFReader);
    	 printf("%i\n",outputInt[i]);
    	 offset += sizeof(int);
     }
     fclose(OFReader);
     OFReader=NULL;

     free(outputInt);
     outputInt=NULL;
}

void createSuperSortedRuns(int start, int end, inputBufferStruct *ptr, char file[]){    // here the end is totalInts for mergeType==1

	int i=0;

	int totalInts_bet_start_end = 0;

    for(i=start;i<end;i++){
    	totalInts_bet_start_end += small_fo[i].total;
    }

    int minElement =0;
    char temp[100]={0};
    int offset_super=0;
	for(i=0;i<totalInts_bet_start_end;i++){

		if(mergeType==2)
			minElement = findMinElementSequentialSearch(ptr,start,end);
		outputBuffer[act_outputBufferSize] = minElement;
		act_outputBufferSize++;

		if((act_outputBufferSize==max_outputBufferSize) ||(i==(totalInts_bet_start_end-1))){


			qsort(outputBuffer,act_outputBufferSize,sizeof(int),compare);

						// write to file

						strncpy(temp,file,strlen(file));
						char counter[4]={0};
						int written = snprintf(counter,5, ".%03d", superRun);
						counter[sizeof(counter)] = '\0';

						strncat(temp,".super",6);
						strncat(temp,counter,written);
						temp[strlen(file)+10]='\0';

						FILE *int_File = NULL;

						if((int_File=fopen(temp, "r+b"))==NULL)
							{
							int_File = fopen( temp, "w+b" );
							}
						rewind(int_File);
						fseek(int_File,offset_super,SEEK_SET);
						fwrite(&outputBuffer,sizeof(int),act_outputBufferSize,int_File);
                        offset_super += (act_outputBufferSize*sizeof(int));


						if(totalInts_bet_start_end == (offset_super/sizeof(int))){
							    // register the sup_fo information
								big_fo=register_super_fo(big_fo,superRun,temp,0,totalInts_bet_start_end,totalInts_bet_start_end);
								// increase the total super run files
								superRun+=1;
						}

						// Reinitialize
									int j=0;
									for(j=0;j<max_outputBufferSize;j++){
										outputBuffer[j]=0;
									}
									act_outputBufferSize=0;

									int len = strlen(temp);
								    temp[len-10] = '\0';
					   // Garbage
									fclose(int_File);
									int_File = NULL;


            /*printf("************Small FO******************Counter: %i\n");
            print(small_fo);
            printf("************Pointer******************Counter: %i\n");
            printPtr(ptr,totalSortedFiles);*/

		}
	}
}
void superFileInput(int start, int end, int maxPossible, super_fo *big_fo){   // we know that the total size of big_fo = superRun = superPtr
	int i=0;
	for(i=start;i<end;i++){
		if(maxPossible<=big_fo[i].left){
			superPtr=resizeArray(superPtr,i,maxPossible,big_fo[i].filename);
			superPtr= superReadValues(superPtr,i,maxPossible);
			big_fo[i].offset += maxPossible;
			big_fo[i].left -= maxPossible;
		}
		else if(maxPossible>big_fo[i].left){
			superPtr = resizeArray(superPtr,i,big_fo[i].left,big_fo[i].filename);
			superPtr = superReadValues(superPtr,i,big_fo[i].left);
			big_fo[i].offset += big_fo[i].left;
			big_fo[i].left = 0;
		}
		if(big_fo[i].left==0){             // TODO : Should I remove this condition
			big_fo[i].available=0;
		}
	}
}




void superWriteSortedRuns(int start, int end, inputBufferStruct *superPtr, char file[]){    // here the end is totalInts for mergeType==1

	FILE *outFile = NULL;
	//printf("%s\n",file);
    outFile=fopen(file,"w+b");
	rewind(outFile);
	int i=0;
	int offset_OB = 0;
	int minElement =0;
	for(i=start;i<end;i++){
	                                                         // changes for mergeType == 2
			minElement = superFindMinElementSequentialSearch(superPtr,start,superRun);   // this end would be replaced*/
		outputBuffer[act_outputBufferSize] = minElement;
		act_outputBufferSize++;

		if((act_outputBufferSize==max_outputBufferSize) ||(i==(end-1))){

            fseek(outFile,offset_OB,SEEK_SET);
            fwrite(&outputBuffer,sizeof(int),act_outputBufferSize,outFile);
            offset_OB +=act_outputBufferSize * sizeof(int);
            // garbage Collection
            int j=0;
            for(j=0;j<max_outputBufferSize;j++){
            	outputBuffer[j]=0;
            }
            act_outputBufferSize=0;


            /*printf("************Small FO******************Counter: %i\n");
            print(big_fo);
            printf("************Pointer******************Counter: %i\n");
            printPtr(superPtr,superRun);*/

		}
	}
	fclose(outFile);
	outFile=NULL;
}

int superFindMinElementSequentialSearch(inputBufferStruct *superPtr,int start,int size){    // changes for mergeType == 2 , added start

	int minElement = INT_MAX;
	int minElementStructIndex = 0;
    int i=0;
    int startIndex=0;
    for(i=start;i<size;i++){
    	if(checkOverflow(superPtr,i)==1)
        		superPtr=superReadNewInputsFromFile(superPtr,i);
    	startIndex = superPtr[i].int_start;
    	if(	(superPtr[i].available==1) &&
    			(superPtr[i].integers[startIndex]<=minElement) &&
    			(superCheckOverflow(superPtr,i)==0)){
    		minElement=superPtr[i].integers[startIndex];
    		minElementStructIndex = i;
    	}
    }
    superPtr[minElementStructIndex].int_start++; // mark the minElement in the Struct as VISITED
    int result = superCheckOverflow(superPtr,minElementStructIndex); // Check for Overflow
    if(result==1){   // read the next set of integers and return min
    	superPtr = superReadNewInputsFromFile(superPtr,minElementStructIndex);                    // Changes Required here
    }
    return minElement;
}
int superCheckOverflow(inputBufferStruct *superPtr, int counter){
	if(superPtr[counter].int_start >= superPtr[counter].int_size)
		return 1;
	else
		return 0;
}
inputBufferStruct* superReadNewInputsFromFile(inputBufferStruct *superPtr, int counter){
	int noOfIntegersLeftToBeRead=0;
	    	noOfIntegersLeftToBeRead = big_fo[counter].left;

    	if(noOfIntegersLeftToBeRead >= superPtr[counter].int_size){
    		superPtr = superReadValues(superPtr,counter,superPtr[counter].int_size);
    		// modify big_fo Values
            big_fo = modify_big_fo_Values(big_fo,counter,superPtr[counter].int_size);
    	}

    	else
    		{
    		if(noOfIntegersLeftToBeRead == 0)
    		superPtr[counter].available = 0;
    	    else{
    	    	if(noOfIntegersLeftToBeRead < superPtr[counter].int_size)
    	    	{
    		   superPtr = superReadValues(superPtr,counter,noOfIntegersLeftToBeRead);
    		    // modify big_fo Values
    		   big_fo = modify_big_fo_Values(big_fo,counter,noOfIntegersLeftToBeRead);
    	    	}
    	     }
    	}
    return superPtr;
}
super_fo* modify_big_fo_Values(super_fo *big_fo,int counter, int diff){
	big_fo[counter].left -= diff;
	big_fo[counter].offset +=diff;
	if(big_fo[counter].left==0){      // TODO: This can be <= instead of ==
		big_fo[counter].available=0;
	}

	return big_fo;
	}
inputBufferStruct* superReadValues(inputBufferStruct *superPtr, int counter, int size){
	if(superPtr == NULL){
		printf("Cannot read the file to an empty structure \n");
	}
	else{
		FILE *tempInputFile = NULL;
		tempInputFile=fopen(superPtr[counter].filename,"r+b");
		rewind(tempInputFile);
		int offset = (big_fo[counter].offset) * sizeof(int);
		fseek(tempInputFile,offset,SEEK_SET);
		int i=0;
		for(i=0;i<size;i++){
			fseek(tempInputFile,0,SEEK_CUR);
			fread(&superPtr[counter].integers[i],sizeof(int),1,tempInputFile);
			//offset += sizeof(int);
		}
		superPtr[counter].int_size=size;
		superPtr[counter].available=1;
		superPtr[counter].int_start=0;
		//garbage collection
		fclose(tempInputFile);
		tempInputFile=NULL;
	}

	return superPtr;
}

char *getRunFileName(char *input, int totalSortedFiles, char *output){

	strncpy(output,input,strlen(input));
	output[strlen(input)]='\0';
	char counter[4]={0};
				/*for(k=0;k<4;k++){
					counter[k]=0;
				}*/


				int written = snprintf(counter,5, ".%03d", totalSortedFiles);
				//printf("Written: %i\n",written);
				counter[sizeof(counter)] = '\0';
				/*printf("Temp: %s\n",temp);
				printf("Counter: %s\n",counter);
				printf("Written: %i\n",written);*/
				strncat(output,counter,written);

				return output;
}
sub_fo* registerSortedFilesForReplacement(char input[],int totalSortedFiles){
	char temp[100]={0};
	int i=0;
	FILE *tempPtr=NULL;
	int bytes=0;
	int Ints =0;
	int len=0;
	for(i=0;i<totalSortedFiles;i++){
		getRunFileName(input,i,temp);
		tempPtr = fopen(temp,"rb");
		rewind(tempPtr);
		fseek(tempPtr,0,SEEK_END);
		bytes = ftell(tempPtr);
		Ints = bytes/sizeof(int);
		small_fo = register_sub_fo(small_fo,i,temp,0,Ints,Ints);
		len = strlen(temp);
		temp[len-4]='\0';
		fclose(tempPtr);
		tempPtr=NULL;
	}
	return small_fo;
}
