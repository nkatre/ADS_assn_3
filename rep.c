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



inputBufferStruct *ptr = NULL;
sub_fo *small_fo=NULL;


int totalSortedFiles=0;
int inputBuffer[1000]={0};
int outputBuffer[1000]={0};
int max_inputBufferSize=1000;  //change
int max_outputBufferSize=1000;  //change


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



int main(int argc, char *argv[]){


	if(strcmp("--replacement",argv[1])==0)
	mergeType =3;


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

	if(mergeType == 3){   // --replacement
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
			minElement = findMinElementSequentialSearch(ptr,start,totalSortedFiles);
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
