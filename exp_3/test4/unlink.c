#include "init.h"
int main()
{
    sem_unlink("sender");
    sem_unlink("receiver1");
    sem_unlink("receiver2");
    sem_unlink("mutex"); 
    return 0;
}