#include <stdint.h>
#include <algorithm>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <WiFiClientSecure.h>
// #include "SHA1_HASH.h"
#include "EEPROM.h"
#include <functional>
#include <vector>
#include <memory>


#include "esp_task_wdt.h"



#include "SoftwareSerial.h"

#include "BasicStepperDriver.h"




#define lengthInBytes(object) (unsigned char *)(&object+1)-(unsigned char *)(&object)



template <typename T,typename FUNC>
void withinTemplate(T loopIterations,FUNC loopCAllBack) {
	unsigned long loopCounter = loopIterations;	//just in case it was passed a const
	while (loopCounter--) {
		//we can do any other thing whithin this loop to do it sort of in parallel 
		loopCAllBack();
	}
}


template<typename T,typename FUNC>
void duringTemplate(T loopIterations,FUNC loopCAllBack) {
	unsigned long loopCounter = loopIterations;	//just in case it was passed a const
	while (loopCounter--) {
		//we can do any other thing whithin this loop to do it sort of in parallel 
		loopCAllBack((loopIterations - (loopCounter + 1)));
	}
}

typedef unsigned long argLoop;

#define within(_LOOP_ITERATIONS_,_LOOP_BODY_) withinTemplate <const unsigned long,const std::function<void(void)>> (_LOOP_ITERATIONS_,[&](void){_LOOP_BODY_})			//every thing is passed by ref 
#define during(_LOOP_ITERATIONS_,_LOOP_BODY_) duringTemplate <const unsigned long,const std::function<void(unsigned long)>> (_LOOP_ITERATIONS_,[&]_LOOP_BODY_)			//every thing is passed by ref






volatile uint32_t *_outputRegisterLow=((volatile uint32_t*)0X3FF44004UL);
volatile uint32_t *_outputRegisterHigh=((volatile uint32_t*)0X3FF44010UL);

volatile uint32_t *_outputRegisterLowSet=((volatile uint32_t*)0X3FF44008UL);
volatile uint32_t *_outputRegisterHighSet=((volatile uint32_t*)0X3FF44014UL);

volatile uint32_t *_outputRegisterLowClear=((volatile uint32_t*)0X3FF4400CUL);
volatile uint32_t *_outputRegisterHighClear=((volatile uint32_t*)0X3FF44018UL);




#define outputRegisterLow (*_outputRegisterLow)
#define outputRegisterHigh (*_outputRegisterHigh)

#define outputRegisterLowSet (*_outputRegisterLowSet)
#define outputRegisterHighSet (*_outputRegisterHighSet)

#define outputRegisterLowClear (*_outputRegisterLowClear)
#define outputRegisterHighClear (*_outputRegisterHighClear)


#define inputRegisterLow (*((volatile uint32_t*)0x3FF4403CUL))
#define inputRegisterHigh (*((volatile uint32_t*)0x3FF44040UL))



volatile uint32_t *_outputEnableRegisterLow=((volatile uint32_t*)0X3FF44020UL);
volatile uint32_t *_outputEnableRegisterHigh=((volatile uint32_t*)0X3FF44024UL);

volatile uint32_t *_outputEnableRegisterLowSet=((volatile uint32_t*)0X3FF44028UL);
volatile uint32_t *_outputEnableRegisterHighSet=((volatile uint32_t*)0X3FF4402CUL);

volatile uint32_t *_outputEnableRegisterLowClear=((volatile uint32_t*)0X3FF44030UL);
volatile uint32_t *_outputEnableRegisterHighClear=((volatile uint32_t*)0X3FF44034UL);




#define outputEnableRegisterLow (*_outputEnableRegisterLow)
#define outputEnableRegisterHigh (*_outputEnableRegisterHigh)

#define outputEnableRegisterLowSet (*_outputEnableRegisterLowSet)
#define outputEnableRegisterHighSet (*_outputEnableRegisterHighSet)

#define outputEnableRegisterLowClear (*_outputEnableRegisterLowClear)
#define outputEnableRegisterHighClear (*_outputEnableRegisterHighClear)









#define _DW digitalWrite
#define _DR digitalRead
#define _PM pinMode







unsigned long _CURRENT_TIME_;
#define microSec(_DELAY_IN_US_) _CURRENT_TIME_=micros()+_DELAY_IN_US_;  while(micros()<_CURRENT_TIME_);


unsigned long _CALIBRATED_DELAY_=0;
float _DELAY_CALIBRATING_FACTOR_=1.0;
#define _US(_US_DELAY_) _CALIBRATED_DELAY_=_US_DELAY_*_DELAY_CALIBRATING_FACTOR_; while(_CALIBRATED_DELAY_--)NOP();

void delayAutoCalibrate(void){
    unsigned long lastKnownTime=micros();
    _US(1000000);
    _DELAY_CALIBRATING_FACTOR_=1000000.0f/(float)(micros()-lastKnownTime);
}



#define _delay_us _US
// #define RTosDelay_us(_TIME_IN_US_) vTaskDelay(_TIME_IN_US_ / ( ( TickType_t ) 1 / configTICK_RATE_HZ )) //this doess not really work
#define _delay_ms(_TIME_IN_MS_) vTaskDelay(_TIME_IN_MS_ / portTICK_PERIOD_MS)





void microSecDelay(unsigned long timeInMicroSec){
    _delay_us(timeInMicroSec);
}



#define endTask() vTaskDelete(NULL)









//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////    LEGACY-CODE    ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////



unsigned short CLR_LENGTH=0;									//this value will be reseted to zero after clearing the string/uint_8 pointer
unsigned char * CLR(unsigned char *deletedString,unsigned short _CLR_LENGTH=0){
	CLR_LENGTH=(CLR_LENGTH)?CLR_LENGTH:_CLR_LENGTH;
	unsigned char *returnedString=deletedString;
	while(*deletedString||(CLR_LENGTH-=(CLR_LENGTH!=0))){
		*deletedString=0;
		deletedString++;	
	}
	return returnedString;
}


unsigned char UNDEFINED[10]="undefined";

unsigned char UNDEFINED_VALUE=0;
unsigned char *NO_DATA=&UNDEFINED_VALUE;


unsigned short stringCounter(unsigned char *counted){
	unsigned short counter=0;
	while(*counted){
		counter++;
		counted++;
	}
	return counter;
}

unsigned char equalStrings(unsigned char *stringPointer1,unsigned char *stringPointer2){
	unsigned short diffCounter;
	if((diffCounter=stringCounter(stringPointer1))!=stringCounter(stringPointer2))
		return 0;
	while((stringPointer1[--diffCounter]==stringPointer2[diffCounter])&&diffCounter);
	return (stringPointer1[diffCounter]==stringPointer2[diffCounter]);
}

unsigned char *_CS(unsigned char *bigString,unsigned char *smallString){
	unsigned char *smallStringLocation=bigString+stringCounter(bigString);		// lucky for us c/c++ support pointer arthematic
	while(*smallString){
		*smallStringLocation=*smallString;
		smallString++;
		smallStringLocation++;
	}
	return bigString;
}



unsigned long strToUint32(unsigned char *str) {
	unsigned char coun = 0;
	unsigned long num;
	while (*str) {
		if (((*str) > 0x2F) && ((*str) < 0x3A)) {
			if (!coun)
				num =(unsigned) str;
			coun++;
		}
		else if (coun)
			break;
		str++;
	}
	if (!coun)
		return 0;
	str = (unsigned char*)num;
	unsigned long ret = 0;
	unsigned x = 1;
	while (coun) { //while (coun > 0)
		ret += (str[coun - 1] - 0x30)*x;
		x *= 10;
		coun--;
	}
	return ret;
}

#define HEX_CHARS(FOUR_BIT) (((FOUR_BIT+0x30)*(FOUR_BIT<10))|((FOUR_BIT+0x37)*(FOUR_BIT>9)))
void intToHex(unsigned long decimalFrom,unsigned char *hexaDecimalForm){
	CLR(hexaDecimalForm);
	unsigned char numLength=8-((((decimalFrom&0xFF000000UL)!=0)+((decimalFrom&0x00FF0000UL)!=0)+((decimalFrom&0x0000FF00UL)!=0)+1)*2);
	for(unsigned char i=numLength;i<8;i++){
		unsigned char fourBits=(unsigned char)((decimalFrom&((unsigned long)(0xf0000000>>(4*i))))>>(4*(7-i)));
		*hexaDecimalForm=HEX_CHARS(fourBits);
		hexaDecimalForm++;
	}
	return;
}

unsigned char inttostr(unsigned long num, unsigned char *str) {
	static unsigned char ucoun;
	for (unsigned char clrCoun = 0; clrCoun < ucoun; clrCoun++) {
		str[clrCoun] = 0;
	}
	ucoun = 0;
	if (num) {
		unsigned long sum = 0;
		unsigned long r = 10000000;
		for (unsigned char i = 0; i < 8; i++) {
			unsigned char flg = 0;
			for (unsigned char j = 9; j > 0; j--) {
				if ((num - sum) >= r * j) {
					sum += r * j;
					*str = (j)+0x30;
					ucoun++;
					str++;
					flg = 1;
					break;
				}
			}
			if (flg == 0 && sum != 0) {
				*str = 0x30;
				ucoun++;
				str++;
			}
			r /= 10;
		}
	}
	else {
		*str = 0x30;
		ucoun = 1;
	}
	return ucoun;
}


unsigned char globalStringNameThatYouWillNeverUse[11]="";


unsigned char GLOBAL_64_BIT_INT_TO_STRING[21]="";
unsigned char* inttostring(uint64_t num){
	CLR(GLOBAL_64_BIT_INT_TO_STRING,21);
	unsigned char finalPointerIndex=20;
	uint64_t modOperator=1;
	uint64_t conversionAccumulator=0;

	while((conversionAccumulator=(num%(modOperator*=10)))!=num)
		GLOBAL_64_BIT_INT_TO_STRING[--finalPointerIndex]=(conversionAccumulator/(modOperator/10))+0x30;
	GLOBAL_64_BIT_INT_TO_STRING[--finalPointerIndex]=(conversionAccumulator/(modOperator/10))+0x30;		// for the very last digit

	return GLOBAL_64_BIT_INT_TO_STRING+finalPointerIndex;		// lucky of us the c++ support pointer arthematic
}

unsigned char *intToHexaDecimal(unsigned long num){
	unsigned char *makeStr=globalStringNameThatYouWillNeverUse;
	*makeStr=0x30;
	makeStr++;
	*makeStr=0x78;
	makeStr++;
	intToHex(num,makeStr);
	return globalStringNameThatYouWillNeverUse;
}


#define C_HEX intToHexaDecimal
#define C_INT inttostring


unsigned char* longToString(long num){
	unsigned char *signedStr=globalStringNameThatYouWillNeverUse;
	if(num<0){
		*signedStr=0x2D;
		signedStr++;
		num*=(-1);
	}
	inttostr((unsigned long)num, signedStr);
	return globalStringNameThatYouWillNeverUse;
}


unsigned strint(unsigned char *str){
	unsigned ret=0;
	unsigned char coun=0;
	while((str[coun])&&(str[coun]>0x2F)&&(str[coun]<0x3A)){
		coun++;
	}
	unsigned x=1;
	while(coun>0){
		ret+=(str[coun-1]-0x30)*x;
		x*=10;
		coun--;
	}
	return ret;
}

uint64_t getInt(unsigned char *numStr){
	unsigned char numLength=stringCounter(numStr);
	uint64_t result=0;
	uint64_t factor=1;
	while(numLength--){
		result+=(numStr[numLength]-0x30)*factor;
		factor*=10;
	}
	return result;
}

unsigned long getInt32_t(unsigned char *numStr){
	unsigned char numLength=stringCounter(numStr);
	unsigned long result=0;
	unsigned long factor=1;
	while(numLength--){
		result+=(numStr[numLength]-0x30)*factor;
		factor*=10;
	}
	return result;
}





















unsigned char *_$StrLastUsedStr=(unsigned char*)malloc(1);
unsigned char* _$Str(const char* basicStr){
	free(_$StrLastUsedStr);
	unsigned short basicStrLength=stringCounter((unsigned char*)basicStr)+1;		//better not to calculate it twice !!
	_$StrLastUsedStr=(unsigned char*)calloc(basicStrLength,sizeof(unsigned char));
	_CS(_$StrLastUsedStr,(unsigned char*)basicStr);
	return _$StrLastUsedStr;
}

unsigned char* _$Str(unsigned char* basicStr){
	return basicStr;
}

unsigned char* _$Str(char* basicStr){
	return (unsigned char*)basicStr;
}



unsigned char* _$Str(unsigned long num){
	return inttostring(num);
}

unsigned char* _$Str(unsigned short num){
	return inttostring(num);
}

unsigned char* _$Str(unsigned char num){
	return inttostring(num);
}

unsigned char* _$Str(long num){
	return longToString(num);
}

unsigned char* _$Str(short num){
	return longToString(num);
}

unsigned char* _$Str(char num){
	return longToString(num);
}


unsigned char* _$Str(int num){
	return longToString(num);
}

unsigned char *_$Str(double num){
	#define extraDigits 5
	const float decimalPlace=1e5f;
	unsigned char *biggerNumber=longToString(num*decimalPlace);
	unsigned char biggerNumberCharCount=0;
	while(biggerNumber[biggerNumberCharCount++]);
	// biggerNumberCharCount--;                                                             //not sure why this is cancelled
	unsigned char decimalPointIndex=(--biggerNumberCharCount)-extraDigits;
	unsigned char endsWithZero=1;
	while((biggerNumberCharCount--)-decimalPointIndex){
		biggerNumber[biggerNumberCharCount+1]=biggerNumber[biggerNumberCharCount];
		if(endsWithZero){
			endsWithZero=!(biggerNumber[biggerNumberCharCount]-0x30);
			biggerNumber[biggerNumberCharCount+1]*=(biggerNumber[biggerNumberCharCount+1]!=0x30);
		}
	}    
	biggerNumber[decimalPointIndex]=0x2E;                                                   //finally adding the decimal point
	if(!biggerNumber[decimalPointIndex+1])
		biggerNumber[decimalPointIndex+1]=0x30;
	return biggerNumber;
}







unsigned long globalVariadicStringCounter=0;
unsigned long variadicStringCounter(void){
	return globalVariadicStringCounter;
}
template<typename T,typename... Types>
unsigned long variadicStringCounter(T strArg,Types... str2){

	unsigned char* str1=_$Str(strArg);
	globalVariadicStringCounter+=stringCounter(str1);

	return variadicStringCounter(str2...);
}




unsigned char *_$CS=(unsigned char*)malloc(1);
unsigned char _CSS_FirstTimeRunning=1;


unsigned char* $(void){
	_CSS_FirstTimeRunning=1;
	return _$CS;
}



template<typename T,typename... Types>
unsigned char* $(T strArg,Types... str2){
	if(_CSS_FirstTimeRunning){
		globalVariadicStringCounter=variadicStringCounter(str2...)+stringCounter(_$Str(strArg))+1;
		free(_$CS);
		_$CS=(unsigned char*)calloc(globalVariadicStringCounter,sizeof(unsigned char));
		_CSS_FirstTimeRunning=0;
	}
	unsigned char* str1=_$Str(strArg);
	_CS(_$CS,str1);
	return $(str2...);
}




struct FREE_DELETE{
	void operator()(void* x) { free(x); }
};

std::shared_ptr<unsigned char>smartCache(unsigned char*cachedData){
    std::shared_ptr<unsigned char>sharedSmartPointer((unsigned char*)(calloc(stringCounter(cachedData)+1,sizeof(unsigned char))),FREE_DELETE());
    _CS(sharedSmartPointer.get(),cachedData);
    return sharedSmartPointer;
}

#define SMART_CACHE(cachedData) std::shared_ptr<unsigned char>SMART_CACHE##__LINE__##VAR;cachedData=(SMART_CACHE##__LINE__##VAR=smartCache(cachedData)).get();

#define CACHE_BYTES(CACHED_DATA) (CACHED_DATA=_CS(((unsigned char*)calloc(stringCounter(CACHED_DATA)+1,sizeof(unsigned char))),CACHED_DATA))


unsigned char included(unsigned char singleChar,unsigned char *targetStr){
	unsigned short targetStrCounter=0;
	while((singleChar!=targetStr[targetStrCounter++])&&targetStr[targetStrCounter-1]);	//that way its more consistant
	return (singleChar==targetStr[targetStrCounter-1]);
}



























//^ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


unsigned char *skipStringJSON(unsigned char *jsonString){
	if(*jsonString==0x22){//stop searching inside a string
		unsigned char *backSlash=jsonString-1;
		if(*backSlash==0x3A){
			while((*jsonString!=0x22)&&(*backSlash!=0x5C)){
			backSlash=jsonString-1;
			jsonString++;
			}
		}
	}
	return jsonString;
}

unsigned char*skipArrayJSON(unsigned char *jsonString){
	if(*jsonString==0x5B){//skip an array
		jsonString++;
		unsigned short arrayStart=1;
		unsigned short arrayEnd=0;
		while(arrayStart!=arrayEnd){
			arrayStart+=(*jsonString==0x5B);
			arrayEnd+=(*jsonString==0x5D);
			jsonString=skipStringJSON(jsonString);
			jsonString++;
		}
	}
	return jsonString;
}

unsigned char *skipObjectJSON(unsigned char *jsonString){
	if(*jsonString==0x7B){//skip an object
		jsonString++;
		unsigned short objectStart=1;
		unsigned short objectEnd=0;
		while(objectStart!=objectEnd){
			objectStart+=(*jsonString==0x7B);
			objectEnd+=(*jsonString==0x7D);
			jsonString=skipStringJSON(jsonString);
			jsonString++;
		}
	}
	return jsonString;
}

unsigned char *JSON_OBJECT_FOUND_LOCATION=UNDEFINED;
unsigned char JSON(unsigned char *requestedJSON,unsigned char *jsonString,unsigned char *objectString){
	#define OBJECT_STRING_MAX_LENGTH 35
	unsigned char objectBuffer[OBJECT_STRING_MAX_LENGTH]="";
	unsigned short jsonArrayIndex=-1;
	unsigned char subObject=1;
	unsigned char *deadEndOfString=jsonString+stringCounter(jsonString);
	unsigned char *objectScanner=requestedJSON;
	while(*objectScanner){
		subObject+=(*objectScanner==0x2E);
		objectScanner++;
	}
	for(unsigned char subObjectCounter=0;subObjectCounter<subObject;subObjectCounter++){	//start
		jsonString++;
		jsonArrayIndex=-1;
		unsigned char *jsonObject=objectBuffer;
		*jsonObject=0x22;
		jsonObject++;
		while(*requestedJSON&&(*requestedJSON!=0x2E)&&(*requestedJSON!=0x5B)){
			*jsonObject=*requestedJSON;
			requestedJSON++;
			jsonObject++;
		}
		*jsonObject=0x22;
		jsonObject++;
		while(jsonObject<(objectBuffer+OBJECT_STRING_MAX_LENGTH)){
			*jsonObject=0;
			jsonObject++;
		}
		jsonObject = objectBuffer;
		if(*requestedJSON==0x2E){
			requestedJSON++;
		}
		else if(*requestedJSON==0x5B){
			requestedJSON++;
			jsonArrayIndex=strint(requestedJSON);
			requestedJSON+=stringCounter(inttostring((unsigned long)jsonArrayIndex))+1;//very inefficiant but i dont care!! its a cleaner code
		}

		while(*jsonObject){ //start searching
			if(*jsonObject==*jsonString){//object to be found
				unsigned char *notString=jsonString-1;
				if(*notString!=0x3A){
					jsonObject++;
				}
			}
			else{
				jsonObject=objectBuffer;
			}
			jsonString=skipStringJSON(jsonString);
			jsonString=skipObjectJSON(jsonString);
			jsonString=skipArrayJSON(jsonString);
			if(jsonString==deadEndOfString){//object not found
				return 0;
			}
			jsonString++;
		}//object found

		jsonString++;//skip the ":"
		unsigned char objectTypeArray=0;
		searchInsideArray:
		unsigned short objectStartBracket=0;
		unsigned short objectEndBracket=0;
		unsigned short arrayStartBracket=0;
		unsigned short arrayEndBracket=0;
		unsigned short doubleQuoates=0;
		unsigned char *objectLocation=jsonString;

		while(1){
			objectStartBracket+=(*objectLocation==0x7B);
			objectEndBracket+=(*objectLocation==0x7D);
			arrayStartBracket+=(*objectLocation==0x5B);
			arrayEndBracket+=(*objectLocation==0x5D);
			unsigned char *validStringCheck=objectLocation-1;
			doubleQuoates+=((*validStringCheck!=0x5C)&&(*objectLocation==0x22));
			if(objectLocation==deadEndOfString){//object not found
				return 0;
			}
			while(doubleQuoates&0x01){
				objectLocation++;
				validStringCheck=objectLocation-1;
				doubleQuoates+=((*validStringCheck!=0x5C)&&(*objectLocation==0x22));
			}
			if((*objectLocation==0x2C) && (objectStartBracket == objectEndBracket) && (arrayStartBracket == arrayEndBracket))
				break;
			if(objectEndBracket>objectStartBracket)
				break;
			if(arrayEndBracket>arrayStartBracket)
				break;
			objectLocation++;
		}
		
		if(objectTypeArray){
			goto backToArrayCounter;
		}
		deadEndOfString=objectLocation;
		if((jsonArrayIndex!=0xFFFF)&&(*jsonString==0x5B)){
			jsonString++;
			objectTypeArray=1;
			while(jsonArrayIndex){
				goto searchInsideArray;
				backToArrayCounter:
				jsonString=objectLocation+1;
				if((*objectLocation!=0x2C)&&jsonArrayIndex!=1){
					return 0;
				}
				jsonArrayIndex--;
			}
			objectTypeArray=0;
			jsonArrayIndex=-1;
			goto searchInsideArray;
		}
	}

	// JSON_OBJECT_FOUND_LOCATION=jsonString;
	while(jsonString<deadEndOfString){
		*objectString=*jsonString;
		objectString++;
		jsonString++;
	}
	while(*objectString){
		*objectString=0;
		objectString++;
	}
	return 1;
}



#define JSON_OBJECT_SIZE 101
// unsigned char UNDEFINED[10]="undefined";
unsigned char JSON_OBJECT_FOUND[JSON_OBJECT_SIZE]="";

unsigned char *jsonParse(unsigned char *userObject,unsigned char *jsonObject){
	if(JSON(userObject,jsonObject,JSON_OBJECT_FOUND)){
		if(*JSON_OBJECT_FOUND==0x20){
			unsigned short charSpaceCounter=1;
			while(JSON_OBJECT_FOUND[charSpaceCounter++]==0x22);
			charSpaceCounter--;
			unsigned char *charSpacePointer=JSON_OBJECT_FOUND;
			unsigned char *realDataPointer=charSpacePointer+charSpaceCounter;
			while(*realDataPointer){
				*charSpacePointer=*realDataPointer;
				charSpacePointer++;
				realDataPointer++;
			}
			CLR(charSpacePointer);
		}
		if((*JSON_OBJECT_FOUND)==0x22){
			unsigned char* makeStrJSON=JSON_OBJECT_FOUND;
			unsigned short jsonObjectLength=stringCounter(makeStrJSON)-2;
			unsigned char *clearString=makeStrJSON+1;
			while(jsonObjectLength){
				(*makeStrJSON)=(*clearString);
				*clearString=0;
				clearString++;
				makeStrJSON++;
				jsonObjectLength--;
			}
			*clearString=0;
		}
		return JSON_OBJECT_FOUND;
	}
	return UNDEFINED;
}

#define json(argJSON1,argJSON2) jsonParse((unsigned char *)argJSON1,(unsigned char *)argJSON2)


unsigned char *JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=NULL;
unsigned char *JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND=NULL;

unsigned char JSON_LOW_MEMORY_USAGE(unsigned char *requestedJSON,unsigned char *jsonString){		//this has to be only used with one object to avoid memory corruption  
	#define OBJECT_STRING_MAX_LENGTH 35
	unsigned char objectBuffer[OBJECT_STRING_MAX_LENGTH]="";
	unsigned short jsonArrayIndex=-1;
	unsigned char subObject=1;
	unsigned char *deadEndOfString=jsonString+stringCounter(jsonString);
	unsigned char *objectScanner=requestedJSON;
	while(*objectScanner){
		subObject+=(*objectScanner==0x2E);
		objectScanner++;
	}
	for(unsigned char subObjectCounter=0;subObjectCounter<subObject;subObjectCounter++){	//start
		jsonString++;
		jsonArrayIndex=-1;
		unsigned char *jsonObject=objectBuffer;
		*jsonObject=0x22;
		jsonObject++;
		while(*requestedJSON&&(*requestedJSON!=0x2E)&&(*requestedJSON!=0x5B)){
			*jsonObject=*requestedJSON;
			requestedJSON++;
			jsonObject++;
		}
		*jsonObject=0x22;
		jsonObject++;
		while(jsonObject<(objectBuffer+OBJECT_STRING_MAX_LENGTH)){
			*jsonObject=0;
			jsonObject++;
		}
		jsonObject = objectBuffer;
		if(*requestedJSON==0x2E){
			requestedJSON++;
		}
		else if(*requestedJSON==0x5B){
			requestedJSON++;
			jsonArrayIndex=strint(requestedJSON);
			requestedJSON+=stringCounter(inttostring((unsigned long)jsonArrayIndex))+1;//very inefficiant but i dont care!! its a cleaner code
		}

		while(*jsonObject){ //start searching
			if(*jsonObject==*jsonString){//object to be found
				unsigned char *notString=jsonString-1;
				if(*notString!=0x3A){
					jsonObject++;
				}
			}
			else{
				jsonObject=objectBuffer;
			}
			jsonString=skipStringJSON(jsonString);
			jsonString=skipObjectJSON(jsonString);
			jsonString=skipArrayJSON(jsonString);
			if(jsonString==deadEndOfString){//object not found
				return 0;
			}
			jsonString++;
		}//object found

		jsonString++;//skip the ":"
		unsigned char objectTypeArray=0;
		searchInsideArray:
		unsigned short objectStartBracket=0;
		unsigned short objectEndBracket=0;
		unsigned short arrayStartBracket=0;
		unsigned short arrayEndBracket=0;
		unsigned short doubleQuoates=0;
		unsigned char *objectLocation=jsonString;

		while(1){
			objectStartBracket+=(*objectLocation==0x7B);
			objectEndBracket+=(*objectLocation==0x7D);
			arrayStartBracket+=(*objectLocation==0x5B);
			arrayEndBracket+=(*objectLocation==0x5D);
			unsigned char *validStringCheck=objectLocation-1;
			doubleQuoates+=((*validStringCheck!=0x5C)&&(*objectLocation==0x22));
			if(objectLocation==deadEndOfString){//object not found
				return 0;
			}
			while(doubleQuoates&0x01){
				objectLocation++;
				validStringCheck=objectLocation-1;
				doubleQuoates+=((*validStringCheck!=0x5C)&&(*objectLocation==0x22));
			}
			if((*objectLocation==0x2C) && (objectStartBracket == objectEndBracket) && (arrayStartBracket == arrayEndBracket))
				break;
			if(objectEndBracket>objectStartBracket)
				break;
			if(arrayEndBracket>arrayStartBracket)
				break;
			objectLocation++;
		}
		
		if(objectTypeArray){
			goto backToArrayCounter;
		}
		deadEndOfString=objectLocation;
		if((jsonArrayIndex!=0xFFFF)&&(*jsonString==0x5B)){
			jsonString++;
			objectTypeArray=1;
			while(jsonArrayIndex){
				goto searchInsideArray;
				backToArrayCounter:
				jsonString=objectLocation+1;
				if((*objectLocation!=0x2C)&&jsonArrayIndex!=1){
					return 0;
				}
				jsonArrayIndex--;
			}
			objectTypeArray=0;
			jsonArrayIndex=-1;
			goto searchInsideArray;
		}
	}

	// JSON_OBJECT_FOUND_LOCATION=jsonString;

	JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=deadEndOfString;
	JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND=jsonString;


	// while(jsonString<deadEndOfString){
	// 	*objectString=*jsonString;
	// 	objectString++;
	// 	jsonString++;
	// }
	// while(*objectString){
	// 	*objectString=0;
	// 	objectString++;
	// }
	return 1;
}

unsigned char *LAST_CONST_JSON_OBJECT;
unsigned char lastByteRemoved=0;
unsigned char *_constJson(unsigned char *requestedJSON,unsigned char *jsonString){
	// static unsigned char lastByteRemoved;
	LAST_CONST_JSON_OBJECT=jsonString;
	if(lastByteRemoved&&(JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR!=NULL)&&(!(*JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR)))
		*JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=lastByteRemoved;		//lucky for us cpp support pointer arthematic
	unsigned char objectDefined=JSON_LOW_MEMORY_USAGE(requestedJSON,jsonString);
	while(*JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND==0x20)JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND++;		//when you make a simple algorithm it really pays off
	JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR-=(*JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND==0x22);			//when you make a simple algorithm it really pays off
	JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND+=(*JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND==0x22);			//when you make a simple algorithm it really pays off
	lastByteRemoved=*JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR;
	*JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=0;
	if(!objectDefined)
		return UNDEFINED;
	return JSON_LOW_MEMORY_USAGE_JSON_OBJECT_FOUND;
}

#define clearConstJsonBuffer() *JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=1; CLR(LAST_CONST_JSON_OBJECT);

#define constJsonReset() *JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=lastByteRemoved
#define constJsonResetUndo() *JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=0

#define constJson(REQUESTED_JSON,JSON_STRING) _constJson((unsigned char*)REQUESTED_JSON,JSON_STRING)

unsigned char *constJsonValidate(unsigned char *jsonString){
	constJsonReset();
	unsigned char *validJsonString=jsonString;
	unsigned char *deadEndOfString=jsonString+stringCounter(jsonString);
	unsigned short jsonArrayIndex=-1;
	unsigned char objectTypeArray=0;
	searchInsideArray:
	unsigned short objectStartBracket=0;
	unsigned short objectEndBracket=0;
	unsigned short arrayStartBracket=0;
	unsigned short arrayEndBracket=0;
	unsigned short doubleQuoates=0;
	unsigned char *objectLocation=jsonString;

	while(1){
		objectStartBracket+=(*objectLocation==0x7B);
		objectEndBracket+=(*objectLocation==0x7D);
		arrayStartBracket+=(*objectLocation==0x5B);
		arrayEndBracket+=(*objectLocation==0x5D);
		unsigned char *validStringCheck=objectLocation-1;
		doubleQuoates+=((*validStringCheck!=0x5C)&&(*objectLocation==0x22));
		if(objectLocation==deadEndOfString){//object not found
			// constJsonResetUndo();
			return validJsonString;
		}
		while(doubleQuoates&0x01){
			objectLocation++;
			validStringCheck=objectLocation-1;
			doubleQuoates+=((*validStringCheck!=0x5C)&&(*objectLocation==0x22));
		}
		if((*objectLocation==0x2C) && (objectStartBracket == objectEndBracket) && (arrayStartBracket == arrayEndBracket))
			break;
		if(objectEndBracket>objectStartBracket)
			break;
		if(arrayEndBracket>arrayStartBracket)
			break;
		objectLocation++;
	}
	
	if(objectTypeArray){
		goto backToArrayCounter;
	}
	deadEndOfString=objectLocation;
	if((jsonArrayIndex!=0xFFFF)&&(*jsonString==0x5B)){
		jsonString++;
		objectTypeArray=1;
		while(jsonArrayIndex){
			goto searchInsideArray;
			backToArrayCounter:
			jsonString=objectLocation+1;
			if((*objectLocation!=0x2C)&&jsonArrayIndex!=1){
				// constJsonResetUndo();
				return validJsonString;
			}
			jsonArrayIndex--;
		}
		objectTypeArray=0;
		jsonArrayIndex=-1;
		goto searchInsideArray;
	}

	JSON_LOW_MEMORY_USAGE_DEAD_END_OF_STR=deadEndOfString;

	lastByteRemoved=*deadEndOfString;
	*deadEndOfString=0;

	return validJsonString;
}

unsigned char *recursiveJsonAlgorithm(unsigned char *recursiveAlgorithmData){
	constJsonReset();
	return recursiveAlgorithmData;
}


unsigned char *makeJsonObject(unsigned char **objectKeys,unsigned char **objectValues){
	unsigned long finalObjectSize=3;													// taking acount for {} and \0
	static unsigned char *finalObject=NULL;
	if(finalObject!=NULL)
		free(finalObject);
	unsigned long objectKeysCounter=0;
	unsigned long objectValuesCounter=0;
	while(objectKeys[objectKeysCounter++]!=NULL){			// following the rules of ecma script skipping undefined objects
		if(objectValues[objectValuesCounter]!=UNDEFINED)
			finalObjectSize+=stringCounter(objectKeys[objectKeysCounter-1])+stringCounter((objectValues[objectValuesCounter]==NULL)? (unsigned char*)"null" : objectValues[objectValuesCounter++])+4;				// taking acount for the "":,
		else
			objectValuesCounter++;
	}	
	finalObject=(unsigned char*)calloc(finalObjectSize,sizeof(unsigned char));
	_CS(finalObject,(unsigned char*)"{");
	objectKeysCounter=0;
	objectValuesCounter=0;
	while(objectKeys[objectKeysCounter++]!=NULL){			// following the rules of ecma script skipping undefined objects
		if(objectValues[objectValuesCounter]!=UNDEFINED){
			_CS(finalObject,(unsigned char*)"\"");
			_CS(finalObject,objectKeys[objectKeysCounter-1]);
			_CS(finalObject,(unsigned char*)"\":");
			_CS(finalObject,((objectValues[objectValuesCounter]==NULL)? (unsigned char*)"null" : objectValues[objectValuesCounter++]));
			_CS(finalObject,(unsigned char*)",");
		}
		else
			objectValuesCounter++;
	}
	finalObject[stringCounter(finalObject)-1]=0x7D;
	return finalObject;
}

unsigned char **makeJsonObjectKeys=NULL;
unsigned char **makeJsonObjectValues=NULL;

unsigned char jsonObjectKeysInitializer=0;
unsigned short jsonObjectKeysCounter=0;
unsigned char **jsonObjectKeys(void){
	jsonObjectKeysInitializer=0;
	jsonObjectKeysCounter=0;
	return makeJsonObjectKeys;
}

template<typename T,typename... Types>
unsigned char **jsonObjectKeys(T key,Types... keys){
	if(!jsonObjectKeysInitializer){
		jsonObjectKeysInitializer=1;
		if(makeJsonObjectKeys!=NULL)
			free(makeJsonObjectKeys);
		makeJsonObjectKeys=(unsigned char **)calloc((sizeof...(Types)+2),sizeof(unsigned char*));
	}
	makeJsonObjectKeys[jsonObjectKeysCounter++]=(unsigned char*)key;
	return jsonObjectKeys(keys...);
}

unsigned char jsonObjectValuesInitializer=0;
unsigned short jsonObjectValuesCounter=0;
unsigned char **jsonObjectValues(void){
	jsonObjectValuesInitializer=0;
	jsonObjectValuesCounter=0;
	return makeJsonObjectValues;
}

template<typename T,typename... Types>
unsigned char **jsonObjectValues(T value,Types... values){
	if(!jsonObjectValuesInitializer){
		jsonObjectValuesInitializer=1;
		if(makeJsonObjectValues!=NULL)
			free(makeJsonObjectValues);
		makeJsonObjectValues=(unsigned char **)calloc((sizeof...(Types)+2),sizeof(unsigned char*));
	}
	makeJsonObjectValues[jsonObjectValuesCounter++]=(unsigned char*)value;
	return jsonObjectValues(values...);
}

#define JSON_KEYS jsonObjectKeys
#define JSON_VALUES jsonObjectValues








unsigned char inverseBase64Table(unsigned char transBuf){
	return (transBuf-((65*((transBuf<0x5B)&&(transBuf>0x40)))|(71*((transBuf>0x60)&&(transBuf<0x7B)))|(-4*((transBuf>0x2F)&&(transBuf<0x3A)))|(-19*(transBuf==0x2B))|(-16*(transBuf==0x2F))))*(transBuf!=0x3D);//(char64-((0x41*(char64<26))|(71*((char64>25)&&(char64<52)))|(-4*((char64>51)&&(char64<62)))))
}

unsigned char *base64Decode(unsigned char *base64Text){
	unsigned char *startAddress=base64Text;
	unsigned char *lastAddress=startAddress+stringCounter(startAddress);
	unsigned char *base256Text=base64Text;
	unsigned short base64Counter=0;
	while(base64Text[base64Counter]){
		unsigned char base256Buffer[4]={inverseBase64Table(base64Text[base64Counter++]),inverseBase64Table(base64Text[base64Counter++]),inverseBase64Table(base64Text[base64Counter++]),inverseBase64Table(base64Text[base64Counter++])};
		*base256Text=(base256Buffer[0]<<2)|((base256Buffer[1]&0x30)>>4);
		base256Text++;
		*base256Text=(base256Buffer[1]<<4)|((base256Buffer[2]&0x3C)>>2);
		base256Text++;
		*base256Text=(base256Buffer[2]<<6)|base256Buffer[3];
		base256Text++;
	}
	while(base256Text<lastAddress){
		*base256Text=0;
		base256Text++;
	}
	return startAddress;
}

unsigned char base64Table(unsigned char transBuf){
		return transBuf+((65*(transBuf<26))|(71*((transBuf>25)&&(transBuf<52)))|(-4*((transBuf>51)&&(transBuf<62)))|(-19*(transBuf==62))|(-16*(transBuf==63)));
}
unsigned short base64(unsigned char *rawData, unsigned char *base64Text) {
	static unsigned short lastBase64Length;
	unsigned char *clearText=base64Text;
	while(lastBase64Length){
		*clearText=0;
		clearText++;
		lastBase64Length--;
	}
	unsigned short rawDataLength = stringCounter(rawData);
	unsigned char paddingCount = rawDataLength % 3;
	rawDataLength -= paddingCount;
	rawDataLength *= 1.3334f;
	unsigned short base64Counter = 0;
	unsigned char *rawData1 = rawData + 1;
	unsigned char *rawData2 = rawData + 2;
	while (base64Counter < rawDataLength) {
		base64Text[base64Counter++] = base64Table((*rawData) >> 2);
		base64Text[base64Counter++] = base64Table((((*rawData) & 0x03) << 4) | ((*rawData1) >> 4));
		base64Text[base64Counter++] = base64Table((((*rawData1) & 0x0F) << 2) | (((*rawData2) & 192) >> 6));
		base64Text[base64Counter++] = base64Table((*rawData2) & 0x3F);
		rawData += 3;
		rawData1 += 3; 
		rawData2 += 3;
	}
	if (paddingCount == 2) {
		base64Text[base64Counter++] = base64Table((*rawData) >> 2);
		base64Text[base64Counter++] = base64Table((((*rawData) & 0x03) << 4) | ((*rawData1) >> 4));
		base64Text[base64Counter++] = base64Table(((*rawData1) & 0x0F) << 2);
		base64Text[base64Counter++] = 0x3D;
	}
	else if (paddingCount == 1) {
		base64Text[base64Counter++] = base64Table((*rawData) >> 2);
		base64Text[base64Counter++] = base64Table(((*rawData) & 0x03) << 4);
		base64Text[base64Counter++] = 0x3D;
		base64Text[base64Counter++] = 0x3D;
	}
	lastBase64Length=base64Counter;
	return base64Counter;
}


unsigned char *base64Encode(unsigned char *rawData, unsigned char *base64Text=NULL,unsigned short rawDataLength=0) {
	rawDataLength = rawDataLength?rawDataLength:stringCounter(rawData);

  static unsigned char *base64TextBuffer=NULL;
  if(base64TextBuffer!=NULL){
    free(base64TextBuffer);
    base64TextBuffer=NULL;
  }

  base64Text=base64Text?base64Text:(base64TextBuffer=(unsigned char*)calloc((rawDataLength*1.3334f)+8,sizeof(unsigned char)));

	unsigned char paddingCount = rawDataLength % 3;
	rawDataLength -= paddingCount;
	rawDataLength *= 1.3334f;
	unsigned short base64Counter = 0;
	unsigned char *rawData1 = rawData + 1;
	unsigned char *rawData2 = rawData + 2;
	while (base64Counter < rawDataLength) {
		base64Text[base64Counter++] = base64Table((*rawData) >> 2);
		base64Text[base64Counter++] = base64Table((((*rawData) & 0x03) << 4) | ((*rawData1) >> 4));
		base64Text[base64Counter++] = base64Table((((*rawData1) & 0x0F) << 2) | (((*rawData2) & 192) >> 6));
		base64Text[base64Counter++] = base64Table((*rawData2) & 0x3F);
		rawData += 3;
		rawData1 += 3; 
		rawData2 += 3;
	}
	if (paddingCount == 2) {
		base64Text[base64Counter++] = base64Table((*rawData) >> 2);
		base64Text[base64Counter++] = base64Table((((*rawData) & 0x03) << 4) | ((*rawData1) >> 4));
		base64Text[base64Counter++] = base64Table(((*rawData1) & 0x0F) << 2);
		base64Text[base64Counter++] = 0x3D;
	}
	else if (paddingCount == 1) {
		base64Text[base64Counter++] = base64Table((*rawData) >> 2);
		base64Text[base64Counter++] = base64Table(((*rawData) & 0x03) << 4);
		base64Text[base64Counter++] = 0x3D;
		base64Text[base64Counter++] = 0x3D;
	}
	return base64Text;
}




unsigned char responeseHeaders=0;
#define fecthHeadersEnable() responeseHeaders=1
#define fetchHeadersDisable() responeseHeaders=0


unsigned char *urlEncodeReturnStr=(unsigned char*)malloc(1);
unsigned char *urlEncode(unsigned char *originalUrl) {
	unsigned char urlSpecialChars[23]="-._~:/?#[]@!$&'()*+,;=";
	unsigned char specialCharsCount=0;
	during(stringCounter(originalUrl),(argLoop index){
		specialCharsCount+=included(originalUrl[index],urlSpecialChars);
	});
	free(urlEncodeReturnStr);
	specialCharsCount=stringCounter(originalUrl)+(specialCharsCount*2)+1;
	urlEncodeReturnStr=(unsigned char*)calloc(specialCharsCount,sizeof(unsigned char));
	// _CS(urlEncodeReturnStr,$(specialCharsCount));
	unsigned char charToStr[2]="";
	during(stringCounter(originalUrl),(argLoop index){
		charToStr[0]=originalUrl[index];
		_CS(urlEncodeReturnStr,included(originalUrl[index],urlSpecialChars)?($("%",(intToHexaDecimal(originalUrl[index])+2))):(charToStr));
		// console.log(" >>> ",$("%",(intToHexaDecimal(originalUrl[index])+2)));
	});
	return urlEncodeReturnStr;
}

struct httpLink{
	unsigned char *domain=(unsigned char*)calloc(40,sizeof(unsigned char));
	unsigned short port;
	unsigned char secure;
	unsigned char *userName=(unsigned char*)calloc(30,sizeof(unsigned char));
	unsigned char *password=(unsigned char*)calloc(30,sizeof(unsigned char));
	unsigned char authDefined=0;
	unsigned char *requestPath=UNDEFINED;
};

struct httpLink urlBreakDown(unsigned char *httpRequest){
	struct httpLink urlParameters;
	unsigned short httpRequestLength=stringCounter(httpRequest);

	unsigned short requestPathLocatorLimit=httpRequestLength;
	unsigned short requestPathLocator=0;
	unsigned char slashCounter=0;
	while(((slashCounter+=(httpRequest[requestPathLocator++]==0x2F))<3)&&(requestPathLocatorLimit--));
	if(slashCounter!=3)
		urlParameters.requestPath=UNDEFINED;
	else
		urlParameters.requestPath=(httpRequest+requestPathLocator-1);

	urlParameters.secure=(httpRequest[4]==0x73);
	urlParameters.port=80+((443-80)*(httpRequest[4]==0x73));
	unsigned short urlStartLocation=7+urlParameters.secure;

	while((--httpRequestLength)&&(httpRequest[httpRequestLength]!=0x40));
	if(httpRequestLength){			//get basic auth
		urlParameters.authDefined=1;
		unsigned char authCharCounter=0;
		while(httpRequest[urlStartLocation]!=0x3A){	
			urlParameters.userName[authCharCounter++]=httpRequest[urlStartLocation++];
		}
		urlStartLocation++;
		authCharCounter=0;
		while(httpRequest[urlStartLocation]!=0x40){
			urlParameters.password[authCharCounter++]=httpRequest[urlStartLocation++];
		}
		urlStartLocation=httpRequestLength+1;
	}

	unsigned short domainStartLocation=urlStartLocation;
	while((httpRequest[urlStartLocation]!=0x3A)&&(httpRequest[urlStartLocation++]!=0x2F));		//trying to locate : or /
	if(httpRequest[(urlStartLocation)]==0x3A)
		urlParameters.port=strint(httpRequest+urlStartLocation+1);
	unsigned char domainFillCounter=0;
	while(domainStartLocation<(urlStartLocation-=(httpRequest[(urlStartLocation-1)]==0x2F))){
		urlParameters.domain[domainFillCounter++]=httpRequest[domainStartLocation++];
	}
	if(!urlParameters.port)
		urlParameters.port=80+((443-80)*(httpRequest[4]==0x73));
	return urlParameters;
}

unsigned short fetchMemoryLimiter=4069;

unsigned char *fetch(unsigned char *httpRequest,unsigned char *requestBody,unsigned char *responseBuffer){
	unsigned char* returnedBuffer=responseBuffer;
	struct httpLink urlParameters=urlBreakDown(httpRequest);
	unsigned char *requestBodyLocation=_CS(responseBuffer,(unsigned char*)((requestBody==UNDEFINED)?("GET "):("POST ")));
	if(urlParameters.requestPath!=UNDEFINED)
		urlEncode(_CS(requestBodyLocation,urlParameters.requestPath)+4+(requestBody!=UNDEFINED));
	_CS(responseBuffer,(unsigned char*)((urlParameters.requestPath==UNDEFINED)?("/"):("")));
	_CS(responseBuffer,(unsigned char*)" HTTP/1.1\r\n");
	_CS(responseBuffer,(unsigned char*)"Host: ");
	_CS(responseBuffer,urlParameters.domain);
	if(urlParameters.port!=80+((443-80)*(httpRequest[4]==0x73))){
		_CS(responseBuffer,(unsigned char*)":");
		_CS(responseBuffer,inttostring((unsigned long)(urlParameters.port)));
	}
	_CS(responseBuffer,(unsigned char*)"\r\n");
	_CS(responseBuffer,(unsigned char*)"User-Agent: PostmanRuntime/7.29.0\r\n");
	_CS(responseBuffer,(unsigned char*)"Accept: */*\r\n");
	_CS(responseBuffer,(unsigned char*)"Connection: keep-alive\r\n");
	if(requestBody!=UNDEFINED){
		_CS(responseBuffer,(unsigned char*)"Content-Type: application/json\r\n");
		_CS(responseBuffer,(unsigned char*)"Content-Length: ");
		_CS(responseBuffer,(unsigned char*)inttostring(stringCounter(requestBody)));
		_CS(responseBuffer,(unsigned char*)"\r\n\r\n");
		_CS(responseBuffer,requestBody);
		_CS(responseBuffer,(unsigned char*)"\r\n");
	}
	_CS(responseBuffer,(unsigned char*)"\r\n");

	// console.log(" >> ",responseBuffer,"\n---------------\n");

	if(urlParameters.secure){
		WiFiClientSecure client;
		client.setInsecure();
		if(!client.connect((char*)urlParameters.domain,urlParameters.port)){
			return UNDEFINED;
		}
		client.println((char*)responseBuffer);
		CLR(responseBuffer);
		while(!client.available());
		unsigned char *makeStr=responseBuffer;
		unsigned long memoryLimter=0;
		if(!responeseHeaders){
			unsigned char headersPreset[5]="\r\n\r\n";
			unsigned char passedHeadersPreset=0;
			while(passedHeadersPreset!=4)
				passedHeadersPreset*=(client.read()==headersPreset[passedHeadersPreset++]);
		}
		while(client.available()){
			if(memoryLimter<fetchMemoryLimiter){
				*makeStr=client.read();
				makeStr++;
			}
			else{
				client.read();
			}
			memoryLimter++;
		}
		makeStr=responseBuffer;
	}
	else{
		WiFiClient client;
		if(!client.connect((char*)urlParameters.domain,urlParameters.port)){
			return UNDEFINED;
		}
		client.write((char*)responseBuffer);
		CLR(responseBuffer);
		while(!client.available());
		unsigned char *makeStr=responseBuffer;
		unsigned long memoryLimter=0;
		if(!responeseHeaders){
			unsigned char headersPreset[5]="\r\n\r\n";
			unsigned char passedHeadersPreset=0;
			while(passedHeadersPreset!=4)
				passedHeadersPreset*=(client.read()==headersPreset[passedHeadersPreset++]);
		}
		while(client.available()){
			if(memoryLimter<fetchMemoryLimiter){
				*makeStr=client.read();
				makeStr++;
			}
			else{
				client.read();
			}
			memoryLimter++;
		}
		makeStr=responseBuffer;
	}
	// if(equalStrings((unsigned char*)"\r\n\r\n",returnedBuffer+(stringCounter(returnedBuffer)-4)))
	// 	CLR(returnedBuffer+(stringCounter(returnedBuffer)-4));

	return returnedBuffer;
}

unsigned char *fetchDynamicBuffer=(unsigned char*)malloc(1);
template <typename httpRequest_t,typename requestBody_t>
unsigned char *fetch(httpRequest_t httpRequest,requestBody_t requestBody){
	free(fetchDynamicBuffer);
	fetchDynamicBuffer=(unsigned char*)calloc(fetchMemoryLimiter+1,sizeof(unsigned char));
	return fetch((unsigned char*)httpRequest,(unsigned char*)requestBody,fetchDynamicBuffer);
}

template <typename httpRequest_t>
unsigned char *fetch(httpRequest_t httpRequest){
	free(fetchDynamicBuffer);
	fetchDynamicBuffer=(unsigned char*)calloc(fetchMemoryLimiter+1,sizeof(unsigned char));
	return fetch((unsigned char*)httpRequest,UNDEFINED,fetchDynamicBuffer);
}

