/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"
#include <inttypes.h>
#include <errno.h>
#define MIN_BLOCK_SIZE 32
#define DROW 16
#define SROW 8
/* Read and Write a row of data into address hp */
#define PUT(hp, var) (*(sf_block_info)(hp)=(*var))
void* increaseHeap();
int initHeap();
int testFreePtr(sf_header* ptr);
sf_header* allocateBLock(sf_header* ptr, size_t size);
sf_header* freeBLock(sf_header* ptr);
sf_header* coalescing(sf_header* header); 
sf_header* split(sf_header* header);
sf_footer* getPrevFooter(sf_header* header);
sf_header* getNextHeader(sf_header* header);
sf_header* getPrevHeader(sf_header* header);
sf_header* convertToFree(sf_header* header);
sf_header* converToAllocate(sf_header* header);
sf_header* removeFLByHeader(sf_header* header);
int isPrevAllocate(sf_header* header);
int isNextAllocate(sf_header* header);
sf_header* updateBlock(sf_header* header, int allocated);
sf_footer* getFooter(sf_header* header);
size_t getFooterSize(sf_footer* footer);
size_t getHeaderSize(sf_header* header);
int padding(size_t request);
sf_free_list_node* addSize(size_t size);
sf_header* addFL(sf_header* header, sf_free_list_node* node);
sf_free_list_node* removeFL(sf_free_list_node* node);
sf_block_info* createInfo(sf_block_info* info, unsigned rs, unsigned bs, unsigned zeros, unsigned pa, unsigned a);
//sf_block_info* createInfo(sf_block_info* info, int rs, int bs, int zeros, int pa, int a);
void *sf_malloc(size_t size) 
{
    if(size==0){return NULL;}
    if((sf_mem_end()-sf_mem_start())==0)
    {
        initHeap();
    }
    int padSize=padding(size);
    int indicator=0;
    while(!indicator)//while did not find suitable size of free block in FL
    {
        //sf_show_free_lists();
        sf_free_list_node* currentNode=sf_free_list_head.next;
        while(currentNode!=&sf_free_list_head) //while not equal to itself
        {
            if(padSize<=(currentNode->size))//find suitable node size in FL
            {
                sf_header* hd=currentNode->head.links.next;
                if(hd!=&currentNode->head)//if list is not empty
                {
                    //successfully allocate a free block
                    removeFL(currentNode);//allocate free block and delete it from FL
                    createInfo(&hd->info,size,getHeaderSize(hd),0,isPrevAllocate(hd),1);
                    converToAllocate(hd);
                    split(hd);//try split if possible
                    //return hd;
                    sf_header* returnHeader=(sf_header*)((char*)hd+8);
                    return returnHeader;
                    //allocateBLock(hd,size);
                    break;
                }
            }
            currentNode=currentNode->next;
        }
        if(increaseHeap()==NULL)//increase heap by 1 page (4096)
        {
            sf_errno=ENOMEM;
            return NULL;
        }
     }

    return NULL;
}

void sf_free(void *ptr) {
    ptr=(sf_header*)((char*)ptr-8);
    testFreePtr(ptr);
    freeBLock((sf_header*)ptr);
    return;
}

void *sf_realloc(void *ptr, size_t rsize) {
    ptr=(sf_header*)((char*)ptr-8);
    testFreePtr(ptr);
    if(rsize==0)
    {
        ptr=(sf_header*)((char*)ptr+8);
        sf_free(ptr);
        return NULL;
    }
    if((rsize+8)>getHeaderSize((sf_header*)ptr))//greater 
    {
        sf_header* dest=sf_malloc(rsize);
        if(dest==NULL){return NULL;}
        dest=(sf_header*)((char*)dest-8);
        memcpy(((char*)dest+8),((char*)ptr+8),(((sf_header*)ptr)->info.requested_size));
        createInfo(&((sf_header*)dest)->info,rsize,getHeaderSize((sf_header*)dest),0,isPrevAllocate((sf_header*)dest),1);
        converToAllocate((sf_header*)dest);
        sf_free((sf_header*)((char*)ptr+8));
        sf_header* returnHeader=(sf_header*)((char*)dest+8);
        return returnHeader;
    }
    else//smaller
    {
        // if(getHeaderSize(ptr)-(rsize+SROW)<MIN_BLOCK_SIZE)//no split
        // {
        //     createInfo(&((sf_header*)ptr)->info,rsize,getHeaderSize((sf_header*)ptr),0,isPrevAllocate((sf_header*)ptr),1);
        //     converToAllocate((sf_header*)ptr);
        //     memcpy(ptr,((char*)ptr+SROW),rsize);
        //     return ptr;
        // }
        // else//split
        // {
        //     createInfo(&((sf_header*)ptr)->info,rsize,getHeaderSize((sf_header*)ptr),0,isPrevAllocate((sf_header*)ptr),1);
        //     converToAllocate((sf_header*)ptr);
        //     ptr=split((sf_header*)ptr);
        //     memcpy(ptr,((char*)ptr+SROW),rsize);   
        //     return ptr;   
        // }
        createInfo(&((sf_header*)ptr)->info,rsize,getHeaderSize((sf_header*)ptr),0,isPrevAllocate((sf_header*)ptr),1);
        converToAllocate((sf_header*)ptr);
        ptr=split((sf_header*)ptr);
        memcpy(((char*)ptr+8),((char*)ptr+8),rsize);
        sf_header* returnHeader=(sf_header*)((char*)ptr+8);   
        return returnHeader;         
    }
    return NULL;
}
sf_block_info* createInfo(sf_block_info* info, unsigned rs, unsigned bs, unsigned zeros, unsigned pa, unsigned a)
{
    info->allocated=a;
    info->prev_allocated=pa;
    info->two_zeroes=zeros;
    info->block_size=(bs>>4);
    info->requested_size=rs;
    return info;
}
//given block header allocate the block
sf_header* allocateBLock(sf_header* ptr, size_t size)
{
    removeFL(addSize(getHeaderSize(ptr)));//allocate free block and delete it from FL
    createInfo(&ptr->info,size,getHeaderSize(ptr),0,isPrevAllocate(ptr),1);
    converToAllocate(ptr);
    split(ptr);//try split if possible
    return ptr;
}
//given block header free the block
sf_header* freeBLock(sf_header* ptr)
{
    createInfo(&ptr->info,0,getHeaderSize(ptr),0,isPrevAllocate(ptr),0);
    convertToFree(ptr);
    ptr=coalescing(ptr);
    addFL(ptr,addSize(getHeaderSize(ptr)));
    return ptr;
}
//test free ptr validity
int testFreePtr(sf_header* ptr)
{
        if(ptr==NULL)
    {
        abort();
    }
    else if(ptr<(sf_header*)((char*)sf_mem_start()+(5*8))||ptr>(sf_header*)((char*)sf_mem_end()-8))
    {
        abort();
    }
    // else if((getFooter((sf_header*)ptr)->info.allocated==0)||((sf_header*)ptr)->info.allocated)
    else if(((sf_header*)ptr)->info.allocated==0)
    {
        abort();
    }
    else if((getHeaderSize((sf_header*)ptr)%16!=0)||(getHeaderSize((sf_header*)ptr)<32))
    {
        abort();
    }
    else if(((sf_header*)ptr)->info.requested_size+8>(getHeaderSize((sf_header*)ptr)))
    {
        abort();
    }
    else if((isPrevAllocate((sf_header*)ptr)==0)&&getPrevHeader((sf_header*)ptr)->info.allocated!=0&&getFooter(getPrevHeader((sf_header*)ptr))->info.allocated!=0)
    {
        abort();
    }
    return 1;
}
/**
 *coalescing free blocks according to 4 cases in book
 *@param header, header of current free block
 *@return header, header pointing to coalescing free block
 */
sf_header* coalescing(sf_header* header)
{
    int next_al=0, prev_al=0;
    sf_header* next_header=getNextHeader(header);//next header
    sf_header* prev_header=getPrevHeader(header);//prev header
    prev_al=isPrevAllocate(header);//is prev allocated
    next_al=isNextAllocate(header); //is next allocated
    //1-allocate, 0-free
    if(prev_al&&next_al)//case 1 prev 1, next 1
    {
        return header;
    }
    else if(prev_al&&!next_al)//case 2 prev 1, next 0;
    {
        //removeFL(addSize(getHeaderSize(next_header)));
        removeFLByHeader(next_header);
        createInfo(&header->info,0,(getHeaderSize(header)+getHeaderSize(next_header)),0,prev_al,0);
        header= convertToFree(header);
    }
    else if(!prev_al&&next_al)//case 3 prev 0, next 1
    {
        //removeFL(addSize(getHeaderSize(prev_header)));
        removeFLByHeader(prev_header);
        createInfo(&prev_header->info,0,(getHeaderSize(header)+getHeaderSize(prev_header)),0,isPrevAllocate(prev_header),0);
        header=convertToFree(prev_header);
    }
    else if(!prev_al&&!next_al)//case 4 prev 0, next 0
    {
        // removeFL(addSize(getHeaderSize(next_header)));
        // removeFL(addSize(getHeaderSize(prev_header)));
        removeFLByHeader(prev_header);
        removeFLByHeader(next_header);
        createInfo(&prev_header->info,0,(getHeaderSize(header)+getHeaderSize(prev_header)+getHeaderSize(next_header)),0,isPrevAllocate(prev_header),0);
        header=convertToFree(prev_header);
    }
    return header;
}
//initialize heap
int initHeap()
{
    sf_prologue* prologue=(sf_prologue*)((char*)sf_mem_grow());//first time call grow no need fo coalscing
    prologue->padding=0; //first row of heap to 0s
    createInfo(&prologue->header.info,0,0,0,0,1); //create info for prologue header
    sf_footer* footer=(sf_footer*)((char*)prologue+(4*SROW));
    createInfo(&footer->info,0,0,0,0,1);//create info for prologue footer
    prologue->header.links.next=NULL;
    prologue->header.links.prev=NULL;
    sf_epilogue* epilogue=(sf_epilogue*)((char*)sf_mem_end()-SROW); 
    createInfo(&epilogue->footer.info,0,0,0,0,1); //create info for epilogue footer
    //need to delet
    //sf_show_block_info((&prologue->footer.info));
    //
    //make rest of free space to one single free block
    sf_header* header=(sf_header*)((char*)sf_mem_start()+(SROW*5)); //pointing at header after prologue footer
    createInfo(&header->info,0,(PAGE_SZ-(6*SROW)),0,1,0); //create info for header of size 4096-48
    // sf_header* freeBLock=convertToFree(header); //convert to free block
    // sf_free_list_node* node=addSize(getHeaderSize(freeBLock));
    // addFL(freeBLock, node);
    addFL(convertToFree(header),addSize(getHeaderSize(convertToFree(header))));
    //need to delet
    //printf("%zu\n", getHeaderSize(freeBLock));
    //
    return 1;//if success
}
//increase heap by 1 page
void* increaseHeap()
{
    sf_header* header=(sf_header*)((char*)sf_mem_end()-SROW);
    void* ptr=sf_mem_grow();
    if(ptr==NULL)
    {
        return ptr;
    }
    else
    {
    //sf_header* header=(sf_header*)((char*)sf_mem_grow()-SROW);
        sf_epilogue* epilogue=(sf_epilogue*)((char*)header+PAGE_SZ);
        createInfo(&header->info,0,(PAGE_SZ),0,isPrevAllocate(header),0);
        createInfo(&epilogue->footer.info,0,0,0,0,1);
        convertToFree(header);
        header=coalescing(header);
        sf_free_list_node* node=addSize(getHeaderSize(convertToFree(header)));
        addFL(header,node);
        return ptr;
    }
    return ptr;
}
//given header update prev allocate according to free or allocate
sf_header* updateBlock(sf_header* header,int prev_allocated)
{
    if(header->info.allocated)//this block is allocate NO footer
    {
        createInfo(&header->info,header->info.requested_size,getHeaderSize(header),0,prev_allocated,header->info.allocated);
        //sf_footer* footer=getFooter(header);
        //createInfo(&footer->info,0,0,0,0,0);
    }
    else
    {
        createInfo(&header->info,header->info.requested_size,getHeaderSize(header),0,prev_allocated,header->info.allocated);
        sf_footer* footer=getFooter(header);
        createInfo(&footer->info,footer->info.requested_size,getHeaderSize(header),0,prev_allocated,footer->info.allocated);
    }
    return header;
}
//recive header make it allocated block format
sf_header* converToAllocate(sf_header* header)
{
    createInfo(&header->info, header->info.requested_size,getHeaderSize(header), 0, isPrevAllocate(header), 1);
    header->payload=0;//init payload
    //sf_footer* footer=getFooter(header);//free footer=header+bs-SROW
    //createInfo(&footer->info,0,0,0,0,0);//empty footer for allocated block.
    sf_header* next_header=getNextHeader(header);
    updateBlock(next_header,1);
    return header;
}
//convert allocate block to free block
sf_header* convertToFree(sf_header* header)
{
    createInfo(&header->info,0, getHeaderSize(header), 0,isPrevAllocate(header),0);//free header
    header->links.next=header;//next and pre point to itself at beginning
    header->links.prev=header;
    sf_footer* footer=getFooter(header);//free footer=header+bs-SROW
    createInfo(&footer->info,0, getHeaderSize(header), 0,isPrevAllocate(header),0);//free footer
    sf_header* next_header=getNextHeader(header);
    updateBlock(next_header,0);
    return header;
}
//given footer, return footer block size (<<4)
size_t getFooterSize(sf_footer* footer)
{
    return (int)((footer->info.block_size)<<4);
}
//given header, return header block size (<<4)
size_t getHeaderSize(sf_header* header)
{
    return (int)((header->info.block_size)<<4);
}
//given header, return size node
sf_header* removeFLByHeader(sf_header* header)
{
    (header->links.prev)->links.next=header->links.next;//prev=nexr
    (header->links.next)->links.prev=header->links.prev;//next=prev
    return header;
}
//given header, return its footer(block has to be free block)
sf_footer* getFooter(sf_header* header)
{
    sf_footer* footer=(sf_footer*)((char*)header+getHeaderSize(header)-SROW);
    return footer;
}
//given header, return previous footer
sf_footer* getPrevFooter(sf_header* header)
{
    //sf_footer* prev_footer=(sf_footer*)((char*)header-SROW);
    return (sf_footer*)((char*)header-SROW);
}
//given header, return 1-prev allocate, 0-free
int isPrevAllocate(sf_header* header)
{
    int isAllocate=0;
    isAllocate=header->info.prev_allocated;
    return isAllocate;
}
//given header, return 1-next block allocated, 0-free
int isNextAllocate(sf_header* header)
{
    int isAllocate=0;
    isAllocate=getNextHeader(header)->info.allocated;
    return isAllocate;
}
//given header, return prev header
sf_header* getPrevHeader(sf_header* header)
{
    sf_footer* prevFooter=getPrevFooter(header);
    return (sf_header*)((char*)header-getFooterSize(prevFooter));
}
//given header, return next header
sf_header* getNextHeader(sf_header* header)
{
    //sf_footer* prev_footer=(sf_footer*)((char*)header-SROW);
    return (sf_header*)((char*)header+getHeaderSize(header));
}
//add new size node into FL if no dup
sf_free_list_node* addSize(size_t size)
{
    sf_free_list_node* currentNode=sf_free_list_head.next;
    while(currentNode!=&sf_free_list_head)
    {
        if(size==currentNode->size)//find dup
        {
            return currentNode;
        }
        if(size<(currentNode->size))//size<cur and did not find dup
        {
            return sf_add_free_list(size,currentNode);
        }
        currentNode=currentNode->next;
    }
    return sf_add_free_list(size,&sf_free_list_head);//size is the new biggest size among list add to the end
}
/**
 *add new free block into FL
 *@param header, header pointer to add
 *@return success, 1 success, 0 fail
 */
sf_header* addFL(sf_header* header, sf_free_list_node* node)
{

    //sf_free_list_node* currentNode=sf_free_list_head.next;
    //size_t size=getHeaderSize(header);
    // while(currentNode!=&sf_free_list_head) //while not equal to itself
    // {
    //     if(size==(currentNode->size))//find suitable node size in FL
    //     {

            sf_header* hd=&node->head;//set hd=head
            if((hd->links.next)==hd)//list is empty
            {
                header->links.next=hd->links.next;
                header->links.prev=hd;
                hd->links.prev=header;
                hd->links.next=header; 
            }
            else
            {
            header->links.next=hd->links.next;
            header->links.prev=hd;
            (hd->links.next)->links.prev=header;
            hd->links.next=header;
            }
    //     }
    //     currentNode=currentNode->next;//go to next
    // }
    return header;
}
//remove first header from size node
sf_free_list_node* removeFL(sf_free_list_node* node)
{
    sf_header* head=&node->head;//set head=head
    sf_header* hd=head->links.next;// hd = head.next
    head->links.next=hd->links.next; //head.next=hd.next
    (hd->links.next)->links.prev=head;//h.prev=hd.prev
    return node;
}
/**
 *padding request size to 32 or mult of 16, return that number
 *@param request, request size before padding
 *@return request, request size after padding
 */
int padding(size_t request)
{
    request=request+8;
    if((request)<=MIN_BLOCK_SIZE)
        {
            request=MIN_BLOCK_SIZE;
            return request;
        }
    else 
        {
            if(request%DROW==0)
            {
                return request;
            }
            else
            {
                //request+=(DROW-(request-((request%DROW)*DROW)));
                request+=(DROW-(request%DROW));
                return request;
            }
        }
    return 0;
}

/**
 *split block into two block according to total size and request size
 *@param total, total size of block allocated
 *@param request, padded request size
 *@return success, 1 if success, 0 fail.
 */
sf_header* split(sf_header* header)
{
    int bs=getHeaderSize(header);//block size
    int rs=header->info.requested_size;//request size
    int padSize=padding(rs);//padded request size
    if((bs-padSize)>=MIN_BLOCK_SIZE)//if intern fragmentation >= 32
    {
        createInfo(&header->info,rs,padSize,0,isPrevAllocate(header),1);//assign bs = padSize
        converToAllocate(header);
        sf_header* freeHeader=(sf_header*)((char*)header+padSize);//create new free header
        createInfo(&freeHeader->info,0,(bs-padSize),0,1,0);
        convertToFree(freeHeader);
        //addSize(getHeaderSize(freeHeader));
        freeHeader=coalescing(freeHeader);
        addFL(freeHeader,addSize(getHeaderSize(freeHeader)));//add new header to node list
        //return freeHeader;
        return header;
    }
    return header;
}