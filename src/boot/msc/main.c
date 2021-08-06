#include <stdio.h>
#include <stdlib.h>
#include "sysObj.h"
#include "MimiObj.h"
#include "dataMemory.h"

#include "CompilerClass.h"
extern DMEM_STATE DMEMS;
int main()
{
    char inputBuff[256] = {0};
    MimiObj *msc = newRootObj("msc", New_Compiler);
    obj_run(msc, "build('../mimiscript-python-api/mimiscript-api.py')");

    printf("memory used most = %d \r\n", DMEMS.maxNum);
    // return 0;

    while (1)
    {
        fgets(inputBuff, sizeof(inputBuff), stdin);

        /* run mimiScript and get res */
        Args *resArgs = obj_runDirect(msc, inputBuff);

        /* get system output of mimiScript*/
        char *sysOut = args_getStr(resArgs, "sysOut");

        if (NULL != sysOut)
        {
            /* print out the system output */
            printf("%s\r\n", sysOut);
        }

        /* deinit the res */
        args_deinit(resArgs);
    }
}
