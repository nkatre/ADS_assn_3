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
sub_fo *small_fo=NULL;
super_fo *big_fo = NULL;

int totalSortedFiles=0;
int inputBuffer[1000]={0};
int outputBuffer[1000]={0};
int max_inputBufferSize=1000;  //change
int max_outputBufferSize=1000;  //change


int act_outputBufferSize=0;
int act_inputBufferSize=0;
int mergeType=0;  //mergeType


int totalBytes=0;
int totalInts = 0;

void garbageCollector(sub_fo *small_fo, inputBufferStruct *ptr,int size, int mergeType);
inputBufferStruct* resizeArray(inputBufferStruct *ptr, int counter,int size, char file[]);
inputBufferStruct* readValues(inputBufferStruct *ptr, int counter, int size);
void fileInput(int start, int end, int maxPossible, sub_fo *small_fo);
void printPtr(inputBufferStruct *ptr, int total);
void writeSortedRuns(int start, int end, inputBufferStruct *ptr, char file[]);
int findMinElementSequentialSearch(inputBufferStruct *ptr, int size);
int checkOverflow(inputBufferStruct *ptr, int counter);
inputBufferStruct* readNewInputsFromFile(inputBufferStruct *ptr, int counter);
sub_fo* modify_small_fo_Values(sub_fo *small_fo,int counter, int diff);
void readOutputFile(char file[]);

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

/*

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
*/

//void fileInput()

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

    createSortedRuns(0,totalInts,argv[2]);
    printf("************Before File Input Call - Small FO******************");
    print(small_fo);

    // get max possible input from each file
    int maxPossible = max_inputBufferSize/totalSortedFiles;
    fileInput(0,totalSortedFiles,maxPossible,small_fo);
    printf("************After File Input Call - Small FO******************");
      print(small_fo);
    printf("************After File Input Call - Pointer******************");
    printPtr(ptr,totalSortedFiles);

    writeSortedRuns(0,totalInts,ptr,argv[3]);
    printf("************Small FO******************");
         print(small_fo);
       printf("************Pointer******************");
       printPtr(ptr,totalSortedFiles);
    readOutputFile(argv[3]);
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

	}

	return 0;
}

void garbageCollector(sub_fo *small_fo, inputBufferStruct *ptr,int size, int mergeType){

	if(mergeType == 1){   // --basic
    free(small_fo);
    small_fo=NULL;

    int i=0;
    for(i=0;i<totalSortedFiles;i++){
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
void writeSortedRuns(int start, int end, inputBufferStruct *ptr, char file[]){

	FILE *outFile = NULL;
	//printf("%s\n",file);
    outFile=fopen(file,"w+b");
	rewind(outFile);
	int i=0;
	int offset_OB = 0;

	for(i=start;i<end;i++){
		int minElement = findMinElementSequentialSearch(ptr,totalSortedFiles);
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

int findMinElementSequentialSearch(inputBufferStruct *ptr, int size){

	int minElement = INT_MAX;
	int minElementStructIndex = 0;
    int i=0;
    for(i=0;i<size;i++){
    	int startIndex = ptr[i].int_start;
    	if((ptr[i].integers[startIndex]<=minElement) &&
    			(ptr[i].available==1) &&
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
	if(ptr[counter].int_start == ptr[counter].int_size)
		return 1;
	else
		return 0;
}
inputBufferStruct* readNewInputsFromFile(inputBufferStruct *ptr, int counter){

    	int noOfIntegersLeftToBeRead = small_fo[counter].left;

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
