#include<stdio.h>
#include<string.h>


#define SUCCESS (0x0)
#define FAIL    (0x1)

#define EMPTY   (0x2)
#define FULL    (0x3)
#define NORMAL  (0x4)

#define FIFO_DATA_LEN (0x1000)

typedef struct _FIFO {
    unsigned int data[FIFO_DATA_LEN];
    unsigned int rIndex;
    unsigned int wIndex;
    unsigned int status; 
} FIFO;

int FifoInit(FIFO *pFifo)
{
    if (NULL == pFifo) {
        return FAIL;
    }

    (void)memset(pFifo, 0, sizeof(FIFO));
    return SUCCESS;
}

int FifoPush(FIFO *pFifo, unsigned int data)
{
    if (NULL == pFifo) {
        return FAIL;
    }

    if (FULL != pFifo->status) {
        pFifo->data[pFifo->wIndex++] = data;
        pFifo->wIndex %= FIFO_DATA_LEN;
        if (pFifo->wIndex == pFifo->rIndex) {
            pFifo->status = FULL;  
        } else {
           pFifo->status = NORMAL;
        }
    } else {
        return FAIL;
    }

    return SUCCESS;
}

int FifoPop(FIFO *pFifo, unsigned int *pData)
{
    if (NULL == pFifo) {
        return FAIL;
    }

    if (EMPTY != pFifo->status) {
        *pData=pFifo->data[pFifo->rIndex++];
        pFifo->rIndex %= FIFO_DATA_LEN;  
        if (pFifo->wIndex == pFifo->rIndex) {
            pFifo->status = EMPTY;  
        } else {
           pFifo->status = NORMAL;
        }
    } else {
        return FAIL;
    }

    return SUCCESS;
}

unsigned int GetSum(FIFO *pFifo)
{
    unsigned long sum = 0;
    unsigned int index;

    if (NULL == pFifo) {
        return FAIL;
    }
    
    for (index = pFifo->rIndex; index != pFifo->wIndex; index++) {
        index %= FIFO_DATA_LEN;
        sum += (unsigned long)pFifo->data[index];
    }

    return sum;
}

