#include <stdlib.h>

#include "debug.h"
#include "hw1.h"
#include "const.h"
#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif
int myStrtoi(char* target);
int compareStr(char** argv, char* reserved, int index);
int myAtoi(char* target);
int keyVarify(char* target);
void setOption(int h, int u, int d, int c, int f, int factor, int k, int key,int p);
/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the content of three frames of audio data and
 * two annotation fields have been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variables "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 1 if validation succeeds and 0 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv)
{
    global_options=0;
    int returnValue=0;
    if(argc==1){
        returnValue=0;
        
        return returnValue; // no flag detect
    }
    char* h="-h";
    char* u="-u";
    char* d="-d";
    char* f="-f";
    char* p="-p";
    char* c="-c";
    char* k="-k";
    int index=1;
    int key=0;
    int ish=0;
    int isc=0;
    int isk=0;
    int iskey=0;
    int isu=0;
    int isd=0;
    int isf=0;
    int isp=0;
    ish=compareStr(argv, h, index);
    int total=0;
    if(ish==1)
    {
        returnValue=1;
        setOption(ish, isu, isd, isc, isf, 0, isk, 0,isp);
        //printf("%lx\n", global_options);
        return returnValue; //-h detect
    }
    isu=compareStr(argv, u, index);
    isd=compareStr(argv, d, index);
    if(isu==1||isd==1)
    {   
        if(argc==2)
        {
            returnValue=1;
            setOption(ish, isu, isd, isc, isf, 0, isk, 0,isp);
            //printf("%lx\n", global_options);
            return returnValue; //-u detect no optional arguments.
        }
        if(argc==3)
        {
            index=2;
            isf=compareStr(argv, f, index);
            if(isf==1)
            {
                returnValue=0;

                return returnValue; //-f without factor
            }
            index=2;
            isp=compareStr(argv, p, index);
            if(isp==1)
            {
                returnValue=1;
                setOption(ish, isu, isd, isc, isf, 0, isk, 0,isp);
                //printf("%lx\n", global_options);
                return returnValue; //-p detect
            }
            returnValue=0;
            return returnValue; // 3rd arg not f or p
        }
        if(argc==4)
        {
            index=2;
            isf=compareStr(argv, f, index);
            if(isf==1)
            {
                index=3;
                total=myAtoi(*(argv+index));
                if(total!=0) //if valid
                {
                    setOption(ish, isu, isd, isc, isf, total, isk, 0,isp);
                    //printf("%lx\n", global_options);
                    return 1;
                }
                else
                {
                    returnValue=0;
                    return returnValue; //invalid factor
                }    
            }
            returnValue=0;
            return returnValue; //-p is not possible in 4 args
        }
        if(argc==5)
        {
            index=2;
            isf=compareStr(argv, f, index);
            if(isf==1)
            {
                index=3;
                total=myAtoi(*(argv+index));
                if(total!=0) //if valid
                {
                   
                    index=4;
                    isp=compareStr(argv, p, index);
                    if(isp==1)
                    {
                        returnValue=1;
                        setOption(ish, isu, isd, isc, isf, total, isk, 0,isp);
                        //printf("%lx\n", global_options);
                        return returnValue; //-p confirm
                    }
                    else
                    {
                        returnValue=0;
                        return returnValue; //-p not confirm but has 5 args 
                    }
                    returnValue=0;
                    return returnValue;
                }
                returnValue=0;
                return returnValue; //invalid factor 
            }
            index=2;
            isp=compareStr(argv, p, index);
            if(isp==1)
            {
                index=3;
                isf=compareStr(argv, f, index);
                if(isf==1)
                {
                    index=4;
                    total=myAtoi(*(argv+index));
                    if(total!=0) //if valid
                    {
                        setOption(ish, isu, isd, isc, isf, total, isk, 0,isp);
                        returnValue=1;
                        //printf("%lx\n", global_options);
                        return returnValue;
                    }
                    else
                    {   
                        returnValue=0;
                        return returnValue; //invalid factor
                    }
                }
                returnValue=0;
                return returnValue;
            }
            returnValue=0;
            return returnValue;
        }
        returnValue=0;
        return returnValue;
    }
    isc=compareStr(argv, c, index);
    if(isc==1)
    {
        if(argc==2)
        {
            returnValue=0;
            return returnValue;
        }
        if(argc==3)
        {

                returnValue=0;
                return returnValue;
            
        }
        if(argc==4)
        {
            index=2;
            isk=compareStr(argv, k, index);
            if(isk==1)
            {
                /////////////////
                index=3;
                iskey=keyVarify(*(argv+ index));
                key=myStrtoi(*(argv+index));
               //myStrtoi(*(argv+index));
                if(iskey!=0)
                {
                    returnValue=1;
                    setOption(ish, isu, isd, isc, isf, total, isk, key,isp);
                    return returnValue;
                }
                else
                {
                    returnValue=0;
                    return returnValue;
                }
            }
            else
            {
                returnValue=0;
                return returnValue;
            }
        }
        if(argc==5)
        {
            index=2;
            isk=compareStr(argv, k, index);
            if(isk==1)
            {

                index=3;
                iskey=keyVarify(*(argv+index));
                key=myStrtoi(*(argv+index));
                if(iskey!=0)
                {
                    index=4;
                    isp=compareStr(argv,p,index);
                    if(isp==1)
                    {
                        setOption(ish, isu, isd, isc, isf, total, isk, key,isp);
                        returnValue=1;
                        return returnValue;
                    }
                    returnValue=0;
                    return returnValue;
                }
                else
                {
                    returnValue=0;
                    return returnValue;
                }
            }
            index=2;
            isp=compareStr(argv, p, index);
            if(isp==1)
            {
                index=3;
                isk=compareStr(argv, k, index);
                if(isk==1)
                {

                    index=4;
                    iskey=keyVarify(*(argv+index));
                    key=myStrtoi(*(argv+index));
                    if(iskey!=0)
                    {
                        returnValue=1;
                        setOption(ish, isu, isd, isc, isf, total, isk, key,isp);
                        return returnValue;
                    }
                    returnValue=0;
                    return returnValue;
                }
                returnValue=0;
                return returnValue;
            }
            returnValue=0;
            return returnValue;
        }

    }
    returnValue=0;
    return returnValue;
}
/**
 *compare two string whether equal, return 1 if equal, 0 otherwise
 **/
int compareStr(char** argv, char* reserved, int index)
{
    int i=0;
    int diff;
    int is=1;
    while(*(*(argv+index)+i)!='\0' || *(reserved+i)!='\0')
    {
        diff=(*(*(argv+index)+i)-*(reserved+i));
        //printf("%s\n",*(reserved+index)+i);
            if(diff!=0)
            {
                is=0;
                return is;
                
            }
        i++;
    }
    return is;
}
/**
 *@brief convert asci to int [1,1024], return 0 if invalid, int valid.
 */
int myAtoi(char* target)
{
    int size=0;

    while(*(target+size)!='\0')
    {
        if(*(target+size)<48 || *(target+size)>57)
        {

            return 0;
        }
        //printf("%d\n", *(target+size));
        size++;
    }
    //printf("%d\n", size);
    int mutiplyer=1;
    int total=0;
    //printf("%d\n", *(target+size-1)-48);
    while(size!=0)
    {
        total+=((*(target+size-1)-48)*mutiplyer);
        //printf("%d\n", *(target+size)-48);
        size--;
        mutiplyer=mutiplyer*10;
    }
    if(total>1024 || total < 0)
        return 0;
    return total;
}
/**
 *
 */
int keyVarify(char* target)
{
    int size=0;
    while(*(target+size)!='\0')
    {
        if(size>=8)
        {
            return 0;
        }
        if((*(target+size)>57||*(target+size)<48)
            &&(*(target+size)>70||*(target+size)<65)
            &&(*(target+size)>102||*(target+size)<97))
        {
            return 0;
        }
        size++;
    }
    return 1;
}
/**
 *
 */
int myStrtoi(char* target)
{
    int i;
    int size=0;
    int value=0;
    value=value<<8;
    int mask=0;
    mask=mask<<8;
    //printf("%d\n", value);
    while(*(target+size)!='\0')
    {
        size++;
    }
    int sizeCopy=size;
    for(i=0;i<sizeCopy;i++)
    {
        mask=0;
        mask=mask<<8;
        switch(*(target+i))
        {
            case '0':  
                mask=0;
                break;
            case '1':
                mask=1;
                break;
            case '2':  
                mask=2;
                break;
            case '3':
                mask=3;
                break;
            case '4':  
                mask=4;
                break;
            case '5':
                mask=5;
                break;
            case '6':  
                mask=6;
                break;
            case '7':
                mask=7;
                break;
            case '8':  
                mask=8;
                break;
            case '9':
                mask=9;
                break;
            case 'a':  
                mask=10;
                break;
            case 'A':
                mask=10;
                break;
            case 'b':  
                mask=11;
                break;
            case 'B':
                mask=11;
                break;
            case 'c':  
                mask=12;
                break;
            case 'C':
                mask=12;
                break;
            case 'd':  
                mask=13;
                break;
            case 'D':
                mask=13;
                break;
            case 'e':  
                mask=14;
                break;
            case 'E':
                mask=14;
                break;
            case 'f':  
                mask=15;
                break;
            case 'F':
                mask=15;
                break;
        }
        mask=mask<<(4*(size-1));
        value=value|mask;
        //printf("%x\n",value );
        size--;
    }
    //printf("%x\n", value);
    return value;
}
/**
 *
 */
void setOption(int h, int u, int d, int c, int f, int factor, int k, int key,int p)
{
    unsigned long mask=0;

    if(h==1)
    {
        global_options=(global_options+1)<<63;
        return;
    }
    if(u==1)
    {
        global_options=(global_options+1)<<62;
        if(p==1)
        {
            global_options=global_options|0x0800000000000000;
        }
        if(f==1)
        {
            factor-=1;
            mask=mask|factor;
            mask=(mask<<48);
            
            global_options=global_options|mask;
        }
        return;
    }
    if(d==1)
    {
        global_options=(global_options+1)<<61;
        if(p==1)
        {
            global_options=global_options|0x0800000000000000;
        }
        if(f==1)
        {
            factor-=1;
            mask=mask|factor;
            mask=mask<<48;
            global_options=global_options|mask;
        }
        return;
    }
    if(c==1)
    {
        global_options=(global_options+1)<<60;
        if(p==1)
        {
            global_options=global_options|0x0800000000000000;
        }
        global_options=global_options|key;
        //printf("%lx\n", global_options);
        return;
    }

}
/**
 * @brief  Recodes a Sun audio (.au) format audio stream, reading the stream
 * from standard input and writing the recoded stream to standard output.
 * @details  This function reads a sequence of bytes from the standard
 * input and interprets it as digital audio according to the Sun audio
 * (.au) format.  A selected transformation (determined by the global variable
 * "global_options") is applied to the audio stream and the transformed stream
 * is written to the standard output, again according to Sun audio format.
 *
 * @param  argv  Command-line arguments, for constructing modified annotation.
 * @return 1 if the recoding completed successfully, 0 otherwise.
 */
int recode(char **argv) {
    AUDIO_HEADER ah;

    //printf("%x\n", AUDIO_MAGIC);
    //int address;
    //address=&ahp;
    AUDIO_HEADER *ahp=&ah;
    //char* cp=(char*)ahp;
    ahp->magic_number=AUDIO_MAGIC;
    char* cp=(char*)ahp;
    //printf("%x\n", ahp->magic_number);
    printf("%x\n", *(cp+5));
    //read_header(&ahp);
    return 0;
}
int read_header(AUDIO_HEADER *hp)
{
    //int offset=0;
    int i;
    //char *c;
    //c=(char*)(hp);
    for(i=0;i<4;i++)
    {
        //c=getchar((hp->magic_number)+1);
        //puts((char)(hp->magic_number));
        //printf("%x\n", (hp->magic_number));
    }
    return 0;
}