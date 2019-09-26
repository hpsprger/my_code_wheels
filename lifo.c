#include<stdio.h>
#include<string.h>


#define SUCCESS (0x0)
#define FAIL    (0x1)

#define EMPTY   (0x2)
#define FULL    (0x3)
#define NORMAL  (0x4)

#define LIFO_DATA_LEN (0x1000)

typedef struct _LIFO {
    unsigned int data[LIFO_DATA_LEN];
    unsigned int index;
    unsigned int status; 
} LIFO;

int LifoInit(LIFO *pLifo)
{
    if (NULL == pLifo) {
        return FAIL;
    }

    (void)memset(pLifo, 0, sizeof(LIFO));
    return SUCCESS;
}

int LifoPush(LIFO *pLifo, unsigned int data)
{
    if (NULL == pLifo) {
        return FAIL;
    }

    if (FULL != pLifo->status) {
        pLifo->data[pLifo->index++] = data;
        if (pLifo->index == LIFO_DATA_LEN) {
            pLifo->status = FULL;  
        } else {
           pLifo->status = NORMAL;
        }
    } else {
        return FAIL;
    }

    return SUCCESS;
}

int LifoPop(LIFO *pLifo, unsigned int *pData)
{
    if (NULL == pLifo) {
        return FAIL;
    }

    if (EMPTY != pLifo->status) {
        *pData=pLifo->data[--pLifo->index];
        if (pLifo->index == LIFO_DATA_LEN) {
            pLifo->status = EMPTY;  
        } else {
           pLifo->status = NORMAL;
        }
    } else {
        return FAIL;
    }

    return SUCCESS;
}

unsigned int GetSum(LIFO *pLifo)
{
    unsigned long sum = 0;
    unsigned int index;

    if (NULL == pLifo) {
        return FAIL;
    }
    
    for (index = 0; index < pLifo->index; index++) {
        sum += (unsigned long)pLifo->data[index];
    }

    return sum;
}


int main(void)
{
    return 0;
}

