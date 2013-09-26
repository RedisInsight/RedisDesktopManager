/* Simple _NonAppStop() implementation which can be linked to your 
 * NLM in order to keep the screen open when the NLM terminates
 * (the good old clib behaviour).
 * You dont have to call it, its done automatically from LibC.
 *
 * 2004-Aug-11  by Guenter Knauf 
 *
 * URL: http://www.gknw.net/development/mk_nlm/
 */
 
#include <stdio.h>
#include <screen.h>

void _NonAppStop()
{
    uint16_t row, col;
    
    GetScreenSize(&row, &col);
    gotorowcol(row-1, 0);
    /* pressanykey(); */
    printf("<Press any key to close screen> ");
    getcharacter();
}


