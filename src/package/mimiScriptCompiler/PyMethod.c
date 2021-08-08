#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"
#include "dataStrs.h"
#include "Compiler.h"

void pyMethod_writeOneMethodDefine(MimiObj *pyMethod, FILE *fp)
{
    Args *buffs = New_strBuff();
    MimiObj *pyClass = obj_getPtr(pyMethod, "context");
    char *className = obj_getStr(pyClass, "name");
    char *methodName = obj_getStr(pyMethod, "name");
    char *typeList = obj_getStr(pyMethod, "typeList");
    char *returnType = obj_getStr(pyMethod, "returnType");

    if ((NULL == typeList) && (NULL == returnType))
    {
        char *defineMethod = args_getBuff(buffs, 512);
        sprintf(defineMethod, "    class_defineMethod(self, \"%s()\", %s_%sMethod);\n",
                methodName,
                className,
                methodName);
        fpusWithInfo(defineMethod, fp);
        goto exit;
    }

    if (NULL == typeList)
    {
        char *defineMethod = args_getBuff(buffs, 512);
        sprintf(defineMethod, "    class_defineMethod(self, \"%s()->%s\", %s_%sMethod);\n", methodName,
                returnType,
                className,
                methodName);
        fpusWithInfo(defineMethod, fp);
        goto exit;
    }

    if (NULL == returnType)
    {
        char *defineMethod = args_getBuff(buffs, 512);
        sprintf(defineMethod, "    class_defineMethod(self, \"%s(%s)\", %s_%sMethod);\n",
                methodName,
                typeList,
                className,
                methodName);
        fpusWithInfo(defineMethod, fp);
        goto exit;
    }

    char *defineMethod = args_getBuff(buffs, 512);
    sprintf(defineMethod, "    class_defineMethod(self, \"%s(%s)->%s\", %s_%sMethod);\n", methodName,
            typeList,
            returnType,
            className,
            methodName);
    fpusWithInfo(defineMethod, fp);
    goto exit;
exit:
    args_deinit(buffs);
    return;
}

int pyMethod_writeEachMethodDefine(Arg *argEach, Args *handleArgs)
{
    FILE *fp = args_getPtr(handleArgs, "fp");
    char *type = arg_getType(argEach);
    if (strEqu(type, "_class-PyMethod"))
    {
        MimiObj *pyMethod = arg_getPtr(argEach);
        pyMethod_writeOneMethodDefine(pyMethod, fp);
    }
    return 0;
}



char *getTypeInC(Args *buffs, char *argType)
{
    if (strEqu(argType, "int"))
    {
        return strsCopy(buffs, "int");
    }
    if (strEqu(argType, "float"))
    {
        return strsCopy(buffs, "float");
    }
    if (strEqu(argType, "pointer"))
    {
        return strsCopy(buffs, "void *");
    }
    if (strEqu(argType, "str"))
    {
        return strsCopy(buffs, "char *");
    }
    return NULL;
}

char *getGetFunName(Args *buffs, char *argType)
{
    if (strEqu(argType, "int"))
    {
        return strsCopy(buffs, "args_getInt");
    }
    if (strEqu(argType, "float"))
    {
        return strsCopy(buffs, "args_getFloat");
    }
    if (strEqu(argType, "pointer"))
    {
        return strsCopy(buffs, "args_getPtr");
    }
    if (strEqu(argType, "str"))
    {
        return strsCopy(buffs, "args_getStr");
    }
    return NULL;
}

int getArgNum(char *typeList)
{
    int argNum = 1 + strCountSign(typeList, ',');
    if (!strIsContain(typeList, ':'))
    {
        argNum = 0;
    }
    return argNum;
}

char *getReturnFunName(Args *buffs, char *returnType)
{
    if (strEqu(returnType, "int"))
    {
        return strsCopy(buffs, "method_returnInt");
    }
    if (strEqu(returnType, "float"))
    {
        return strsCopy(buffs, "method_returnFloat");
    }
    if (strEqu(returnType, "pointer"))
    {
        return strsCopy(buffs, "method_returnPtr");
    }
    if (strEqu(returnType, "str"))
    {
        return strsCopy(buffs, "method_returnStr");
    }
    return NULL;
}

void pyMethod_writeMethodFunMain(MimiObj *pyMethod, FILE *fp)
{
    Args *buffs = New_strBuff();
    MimiObj *pyClass = obj_getPtr(pyMethod, "context");
    char *className = obj_getStr(pyClass, "name");
    char *methodName = obj_getStr(pyMethod, "name");
    char *typeList = obj_getStr(pyMethod, "typeList");
    char *returnType = obj_getStr(pyMethod, "returnType");
    char *returnTypeInC = getTypeInC(buffs, returnType);
    char *returnFunName = getReturnFunName(buffs, returnType);

    char *methodFunName = args_getBuff(buffs, 256);
    sprintf(methodFunName, "void %s_%sMethod(MimiObj *self, Args *args){\n",
            className,
            methodName);
    fpusWithInfo(methodFunName, fp);

    int argNum = getArgNum(typeList);

    char *typeListBuff = strsCopy(buffs, typeList);
    char *localCallArglist = strsCopy(buffs, "");
    for (int i = 0; i < argNum; i++)
    {
        char *argDeclearation = strsPopToken(buffs, typeListBuff, ',');
        char *argName = strsGetFirstToken(buffs, argDeclearation, ':');
        char *argType = strsGetLastToken(buffs, argDeclearation, ':');
        char *argTypeInC = getTypeInC(buffs, argType);
        char *getFunName = getGetFunName(buffs, argType);
        localCallArglist = strsAppend(buffs, localCallArglist, argName);
        if (i != argNum - 1)
        {
            localCallArglist = strsAppend(buffs, localCallArglist, " ,");
        }

        char *transferArgCmd = args_getBuff(buffs, 256);
        sprintf(transferArgCmd, "    %s %s = %s(args, \"%s\");\n",
                argTypeInC,
                argName,
                getFunName,
                argName);

        fpusWithInfo(transferArgCmd, fp);
    }

    char *devideSign = NULL;
    if (argNum > 0)
    {
        devideSign = strsCopy(buffs, ", ");
    }
    if (argNum == 0)
    {
        devideSign = strsCopy(buffs, "");
    }
    /* get callLocalFunCmd */
    char *callLocalFunCmd = args_getBuff(buffs, 256);
    if (NULL == returnType)
    {
        sprintf(callLocalFunCmd, "    %s_%s(self%s%s);\n",
                className,
                methodName,
                devideSign,
                localCallArglist);
    }
    if (NULL != returnType)
    {
        sprintf(callLocalFunCmd, "    %s res = %s_%s(self%s%s);\n",
                returnTypeInC,
                className,
                methodName,
                devideSign,
                localCallArglist);
    }
    fpusWithInfo(callLocalFunCmd, fp);

    /* get call method return commond */
    char *callMethodReturnCmd = args_getBuff(buffs, 256);
    if (NULL != returnType)
    {
        sprintf(callMethodReturnCmd, "    %s(args, res);\n",
                returnFunName);
        fpusWithInfo(callMethodReturnCmd, fp);
    }

    fpusWithInfo("}\n\n", fp);
    arg_deinit(buffs);
}


void pyMethod_writeMethodDeclearMain(MimiObj *pyMethod, FILE *fp)
{
    Args *buffs = New_strBuff();
    MimiObj *pyClass = obj_getPtr(pyMethod, "context");
    char *className = obj_getStr(pyClass, "name");
    char *methodName = obj_getStr(pyMethod, "name");
    char *typeList = obj_getStr(pyMethod, "typeList");
    char *returnType = obj_getStr(pyMethod, "returnType");
    char *methodDeclear = args_getBuff(buffs, 512);
    char *returnTypeInC = getTypeInC(buffs, returnType);
    char *typeListInC = NULL;
    sprintf(methodDeclear, "%s %s_%s(%s);\n",
            returnTypeInC,
            className,
            methodName,
            typeListInC);
    fpusWithInfo(methodDeclear, fp);
    arg_deinit(buffs);
}

int pyMethod_writeEachMethodDeclear(Arg *argEach, Args *handleArgs)
{
    FILE *fp = args_getPtr(handleArgs, "fp");
    char *type = arg_getType(argEach);
    if (strEqu(type, "_class-PyMethod"))
    {
        MimiObj *pyMethod = arg_getPtr(argEach);
        pyMethod_writeMethodDeclearMain(pyMethod, fp);
    }
}

int pyMethod_writeEachMethodFun(Arg *argEach, Args *handleArgs)
{
    FILE *fp = args_getPtr(handleArgs, "fp");
    char *type = arg_getType(argEach);
    if (strEqu(type, "_class-PyMethod"))
    {
        MimiObj *pyMethod = arg_getPtr(argEach);
        pyMethod_writeMethodFunMain(pyMethod, fp);
    }
    return 0;
}