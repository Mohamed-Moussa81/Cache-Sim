#include<stdlib.h>
#include<stdio.h>
#include<math.h>

int read,write,cacheHit,cacheMiss =0;
struct List{
	long int tag;
	int valid;
	struct List* next;
	int frequency;
};
struct List* add_to_list(struct List* ptr){
	struct List* temp = (struct List*)malloc(sizeof(struct List));
	temp->tag = -1;
	temp->valid = 0;
	temp->frequency = 0;
	temp->next = ptr->next;
	ptr->next = temp;
	return temp;
}
int compare(char* str1, char* str2 ){
	int i =0;
	while(str1[i] == str2[i]){
			if(str1[i] =='\0' || str2[i] == '\0'){
				break;
			}
			i++;
	}
	if(str1[i] =='\0' && str2[i] =='\0'){
		return 1;
	}
	else {
		return 0;
	}
}
void init_Cache(struct List** cache, int lines, int sets){
	int i;
	int j;
	//struct List* ptr;
	for(i=0;i<sets;i++){
		//ptr = cache[i];
		cache[i] = (struct List*)malloc(sizeof(struct List));
		cache[i]->tag =-1;
		cache[i] ->valid =0;
		cache[i]->frequency =0;
		cache[i] ->next = NULL;
		for(j=1;j<lines;j++){
			add_to_list(cache[i]);
		}
	}
}
void replace_fifo(struct List* cache, long int* evictindex, long int index, long int tagvalue,int lines){
	struct List* ptr = cache;
	int count = 0;
	int evictnum = evictindex[index]; 
	//printf("evictnum: %d\n",evictnum);
	while(count!=evictnum){
		ptr = ptr->next;
		count++;

	}
	ptr->tag = tagvalue;
	ptr->valid = 1;
	evictnum++;
	if(evictnum == lines){
		evictnum = 0;
	}
	evictindex[index] = evictnum;
	read++;


}
int set_timeAccess(struct List* cache,long int tagValue){
	struct List* ptr = cache;
	int maxFreq = 0;
	//find max and set frequency to this value.
	while(ptr!=NULL){
		if(ptr->frequency > maxFreq){
			maxFreq = ptr->frequency;
		}
		ptr = ptr->next;
		
	}
	return maxFreq +1;
}
void replace_lru(struct List* cache,long int tagValue){
	struct List* ptr = cache;
	int count = 0;
	//traverse set and check for smallest frequency
	int evictLocation;
	int lowestFrequency = ptr->frequency;
	while(ptr!=NULL){
		if(ptr->frequency <= lowestFrequency){
			lowestFrequency = ptr->frequency;
			evictLocation = count;
		}
		ptr = ptr->next;
		count++;
	}
	ptr = cache;
	count = 0;
	while(count!=evictLocation){
		ptr = ptr->next;
		count++;
	}
	ptr->tag = tagValue;
	ptr->valid = 1;
	ptr->frequency = set_timeAccess(cache,tagValue);
	//set it to the max it has most recent
	read++;

}

void read_cache(struct List** cache,long int index, long int tagvalue,long int* evictIndex,int lines,char* evictPolicy){
	//read the tagvalue at a specific set, iteratite over the entire lines
	struct List* ptr =cache[index];
	while(ptr!=NULL){
		if(ptr->tag ==tagvalue && ptr->valid ==1){
			//hit found
			cacheHit++;
			if(compare(evictPolicy,"lru")){
				ptr->frequency = set_timeAccess(cache[index],tagvalue);
			}
			return;
		}
		ptr = ptr->next;
	}
	ptr = cache[index];
	//else we need to add miss, and populate following cache line, see if evicition is needed
	cacheMiss++;
	while(ptr!=NULL){
		if(ptr->valid ==0){
			read++;
			ptr->tag = tagvalue;
			ptr->valid = 1;
			if(compare(evictPolicy,"lru")){
				ptr->frequency = set_timeAccess(cache[index],tagvalue);
			}
			return;
		}
		ptr = ptr->next;
	}
	//if we reach here then we need to evict, need to replace value at a specific line denoted by evictindex
	if(compare(evictPolicy,"fifo")){
	replace_fifo(cache[index],evictIndex,index,tagvalue,lines);
}
else if(compare(evictPolicy,"lru")){
	replace_lru(cache[index],tagvalue);
}

}
void write_cache(struct List** cache,long int index, long int tagvalue, long int* evictIndex, int lines,char* evictPolicy){
	struct List* ptr = cache[index];
	while(ptr!=NULL){
		if(ptr->tag == tagvalue && ptr->valid ==1){
			//hit found, simulate writing to memory
			cacheHit++;
			write++;
			if(compare(evictPolicy,"lru")){
				ptr->frequency = set_timeAccess(cache[index],tagvalue);
			}
			return;
		}
		ptr = ptr->next;
	}
	//else its a miss need to populate following cache line evict if needed.
	ptr = cache[index];
	cacheMiss++;
	while(ptr!=NULL){
		if(ptr->valid ==0){
			read++;
			write++;
			ptr->tag = tagvalue;
			ptr->valid = 1;
			if(compare(evictPolicy,"lru")){
				ptr->frequency = set_timeAccess(cache[index],tagvalue);
			}
			return;
		}
		ptr = ptr->next;
	}
	//evict
	if(compare(evictPolicy,"fifo")){
	replace_fifo(cache[index],evictIndex,index,tagvalue,lines);
}
else if(compare(evictPolicy,"lru")){
	replace_lru(cache[index],tagvalue);
}
	write++;

}
void check_cache(struct List** cache,int set){
	int i;
	 int lines =0;
	 struct List* ptr;
	for(i=0;i<set;i++){
		ptr = cache[i];
		while(ptr!=NULL){
			printf("Cache set: %d and line: %d, tag value is:%ld, valid value is : %d\n",i,lines,ptr->tag,ptr->valid);
			lines++;
			ptr = ptr->next;
			}
		lines =0;
	}
}
int string_len(char* str){
	int i;
	for(i=0;str[i]!='\0';i++){

	}
	return i;
}
int my_atoi(char* str){
	int res =0;
	//sign denotes positive sign
	int sign=1;
	int i=0;
	if(str[0] =='-'){
		sign = -1;
		i++;
	}
	for(;str[i]!='\0';i++){
		res = res * 10 + str[i] -'0';

	}
	return res * sign;
}

int powerOfTwo(int x){
	while((x%2) ==0 && x>1){
		x /= 2;
	}
	return x==1;
}
long int setIndex_calculator(unsigned long address,int blockshift,int setBitsize){
	int long mask =0;
	int i;
		for(i=0;i<setBitsize;i++){
			mask += (int)pow(2,i);
		}
		 int long setindex = address>> blockshift;
		setindex = setindex & mask;
		return setindex;

}
long int tagValue_calculator(unsigned long address, int setBit_blockbit_shift, int tagBitsize){
	int long mask =0;
	int i;
	for(i=0;i<tagBitsize;i++){
		mask += (int)pow(2,i);
	}
	int long tagvalue = address>> setBit_blockbit_shift;
	tagvalue = tagvalue & mask;
	return tagvalue;
}


int main(int argc, char *argv[]){
int cacheSize,blockSize,lines,setNums,setTypeLength,numBlock_Bits,numSet_index_bit,numtag_index_bit;
char* evictPolicy;
char* setType;
char* assoCheck = (char*)malloc(sizeof(char) *6 + 1);
char* assocNlength;
int i;
char* command = (char*)malloc(sizeof(char)+1);
unsigned long address;
long int* evictIndex;
//holds the index of the next value to get evicted for each corresponding set
FILE* inputFile;
	if(argc !=6){
		free(command);
		free(assoCheck);
		printf("Error not enough arguments");
		return 1;
	}
	//need to implement atoi function to convert command line input from string to integer
	cacheSize = my_atoi(argv[1]);
	if(!powerOfTwo(cacheSize) || cacheSize<=0){
		free(command);
		free(assoCheck);
		printf("Cache size is not a power of two");
		return 1;
	}
	blockSize=my_atoi(argv[2]);
	if(!powerOfTwo(blockSize) || blockSize <=0){
		free(command);
		free(assoCheck);
		printf("Block size is not a power of two");
		return 1;
	}
	evictPolicy = argv[3];
	if(!(compare(evictPolicy,"fifo") || compare(evictPolicy,"lru"))){
		free(command);
		free(assoCheck);
		printf("Invalid  cache policy");
		return 1;
	}
	setType = argv[4];
	setTypeLength = string_len(setType);
	for(i=0;i<6;i++){
		assoCheck[i] = setType[i];
	}
	assoCheck[6] = '\0';

	if(!(compare(setType,"direct") || compare(setType,"assoc"))){
		if(compare(assoCheck,"assoc:")){
			int length = setTypeLength - 6;
			assocNlength = (char*)malloc(sizeof(char)*length +1);
			for(i=0;i<length+1;i++){
				assocNlength[i] = setType[i+6];
			}
			lines = my_atoi(assocNlength);
			setNums = cacheSize/(blockSize * lines);
			if(!powerOfTwo(lines)){
				printf("not a valid power of two");
				free(command);
				free(assoCheck);
				free(assocNlength);
				return 1;

			}
			//free(assocNlength);
			//printf("lines:%d",lines);
		}
		else {
				free(command);
				free(assoCheck);
		printf("invalid settype\n");
		return 1;
	}
		//check if n way associative trim the number part and compare to assoc: if its that form
		//read the number after and set the line to it;
		//else error
	}
	if(compare(setType,"direct")){
		lines = 1;
		setNums = cacheSize/blockSize;
	}
	else if(compare(setType,"assoc")){
		lines = cacheSize/blockSize;
		setNums = 1;
	}
//calculate bits for tag,set,offset
numBlock_Bits = (int)((log(blockSize))/log(2));
numSet_index_bit=(int)((log(setNums)/ log(2)));
numtag_index_bit = 48-(numBlock_Bits+ numSet_index_bit);
	//take a substring of settype compare; set lines
	/*printf("cache size is: %d\n",cacheSize);
	printf("Block size is: %d\n",blockSize);
	printf("evictPolicy  is: %s\n",evictPolicy);
	printf("set nums is %d\n",setNums);
	printf("set Type is: %s\n",setType);
	printf("lines: %d\n",lines);
	printf("block bits: %d\n",numBlock_Bits);
	printf("set index bits: %d\n",numSet_index_bit);
	printf("tag index bits: %d\n",numtag_index_bit);*/
//Cache should be set to a pointer pointer
struct List** Cache =(struct List**)malloc(sizeof(struct List*) * setNums);
evictIndex=(long int*)malloc(sizeof(long int)* setNums);
init_Cache(Cache,lines,setNums);
//check_cache(Cache,setNums);
for(i=0;i<setNums;i++){
	evictIndex[i] =0;
}
inputFile = fopen(argv[5],"r");
if(inputFile == NULL){
	free(command);
	free(evictIndex);
	struct List* ptrNext = NULL;
	for(i=0;i<setNums;i++){
	struct List* ptr = Cache[i];
	while(ptr!=NULL){
		ptrNext = ptr->next;
		free(ptr);
		ptr = ptrNext;
	}
}
if(compare(assoCheck,"assoc:")){
	free(assocNlength);
}
free(assoCheck);
free(Cache);
	printf("File is empty");
	return 1;
}
//cache set time to read the trace file.
while(fscanf(inputFile,"%c",command) !=EOF){
	if(*command == '#'){
		break;
	}
		fscanf(inputFile,"%lx\n",&address);
		//printf("Command is: %c address is : %lx\n",*command,address);
		//figure out how to find the setindex, and tag value, thats all you need
		//find the index set bits by shifting by num tag snf then mask with a value based on how many set bits it is
		
		//set idnex calculator
		//finds set index
		//set index is correct, should it be a long int or int?
		long  int setindex = setIndex_calculator(address,numBlock_Bits,numSet_index_bit);
		long int tagvalue = tagValue_calculator(address,numSet_index_bit + numBlock_Bits,numtag_index_bit);
		//printf("set index is: %ld the tag value is: %ld \n",setindex,tagvalue);
		if(*command == 'W'){
			//printf("write");
			write_cache(Cache,setindex,tagvalue,evictIndex,lines,evictPolicy);


		}
		else if(*command =='R'){
			//printf("read");
			read_cache(Cache,setindex,tagvalue,evictIndex,lines,evictPolicy);

		}

}
printf("Memory reads: %d\n",read);
printf("Memory writes: %d\n",write);
printf("Cache hits: %d\n",cacheHit);
printf("Cache misses: %d\n",cacheMiss);
//deallocate cache, evictindex, command , assoccheck, assocnlength
free(evictIndex);
free(command);
struct List* ptrNext = NULL;
for(i=0;i<setNums;i++){
	struct List* ptr = Cache[i];
	while(ptr!=NULL){
		ptrNext = ptr->next;
		free(ptr);
		ptr = ptrNext;
	}
}
if(compare(assoCheck,"assoc:")){
	free(assocNlength);
}
free(assoCheck);
free(Cache);

}
