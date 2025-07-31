#include "public.h"

#ifdef 	LOG_TAG
#undef	LOG_TAG
#endif
#define LOG_TAG    "main"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    hw_init();
    init_task_queue();
    task_init();    
    while (1)
    {
        execute_state_machine();
        scheduler();
    }
}



