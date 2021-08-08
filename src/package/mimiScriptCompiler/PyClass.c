#include "BaseObj.h"
#include "SysObj.h"
#include "dataStrs.h"
#include "MimiObj.h"
#include "Compiler.h"
#include "PyMethod.h"
#include "PyClass.h"
#include "Compiler.h"
#include <stdio.h>

void PyClass_setSuper(MimiObj *self, char *superClassName)
{
    obj_setStr(self, "superClassName", superClassName);
}

static void setSuper(MimiObj *self, Args *args)
{
    char *superClassName = args_getStr(args, "superClassName");
    PyClass_setSuper(self, superClassName);
}

int __foreach_pyMethod_writeEachMethodFun(Arg *argEach, Args *handleArgs)
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
static void pyClass_writeMethodFun(MimiObj *pyClass, FILE *fp)
{
    Args *handleArgs = New_args(NULL);
    args_setPtr(handleArgs, "fp", fp);
    args_foreach(pyClass->attributeList, __foreach_pyMethod_writeEachMethodFun, handleArgs);
}

int __foreach_pyMethod_writeEachMethodDefine(Arg *argEach, Args *handleArgs)
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
static void pyClass_writeNewFun(MimiObj *pyClass, FILE *fp)
{
    char *name = obj_getStr(pyClass, "__name");
    Args *buffs = New_strBuff();
    char *superClassName = obj_getStr(pyClass, "superClassName");
    char *newFunDeclearation = args_getBuff(buffs, 512);
    char *deriveCmd = args_getBuff(buffs, 512);
    char *returnCmd = args_getBuff(buffs, 512);
    char *endLine = args_getBuff(buffs, 512);

    sprintf(newFunDeclearation, "MimiObj *New_%s(Args *args){\n", name);
    sprintf(deriveCmd, "    MimiObj *self = New_%s(args);\n", superClassName);

    fpusWithInfo(newFunDeclearation, fp);
    fpusWithInfo(deriveCmd, fp);

    Args *handleArgs = New_args(NULL);
    args_setPtr(handleArgs, "fp", fp);
    args_foreach(pyClass->attributeList, __foreach_pyMethod_writeEachMethodDefine, handleArgs);

    sprintf(returnCmd, "    return self;\n");
    sprintf(endLine, "}\n");

    fpusWithInfo(returnCmd, fp);
    fpusWithInfo(endLine, fp);
    args_deinit(buffs);
}

void pyClass_gnenrateDefineMethodFun(MimiObj *pyClass, FILE *fp)
{
    Args *handleArgs = New_args(NULL);
    args_setPtr(handleArgs, "fp", fp);
    args_foreach(pyClass->attributeList, __foreach_pyMethod_writeEachMethodDefine, handleArgs);
}

/* main operation */
void pyClass_writeOneClassSourceFile(MimiObj *pyClass, char *path)
{
    Args *buffs = New_args(NULL);
    char *name = obj_getStr(pyClass, "__name");
    char *superClassName = obj_getStr(pyClass, "superClassName");
    char *fileName = strsAppend(buffs, name, "-api.c");
    char *filePath = strsAppend(buffs, path, fileName);
    char *includeSuperClass = args_getBuff(buffs, 512);
    char *includeImpl = args_getBuff(buffs, 512);

    FILE *fp = fopen(filePath, "w+");
    printf("\n--------[%s]--------\n", filePath);
    sprintf(includeSuperClass, "#include \"%s.h\"\n", superClassName);
    sprintf(includeImpl, "#include \"%s.h\"\n", name);
    fpusWithInfo("/* Warning!!! Don't modify this file!!!*/\n", fp);
    fpusWithInfo(includeSuperClass, fp);
    fpusWithInfo(includeImpl, fp);
    fpusWithInfo("#include <stdio.h>\n", fp);
    fpusWithInfo("#include \"BaseObj.h\"\n", fp);
    fpusWithInfo("\n", fp);

    pyClass_writeMethodFun(pyClass, fp);
    pyClass_writeNewFun(pyClass, fp);

    args_deinit(buffs);
    fclose(fp);
}

int __foreach_pyMethod_writeEachMethodDeclear(Arg *argEach, Args *handleArgs)
{
    FILE *fp = args_getPtr(handleArgs, "fp");
    char *type = arg_getType(argEach);
    if (strEqu(type, "_class-PyMethod"))
    {
        MimiObj *pyMethod = arg_getPtr(argEach);
        pyMethod_writeMethodDeclearMain(pyMethod, fp);
    }
    return 0;
}

void pyClass_writeClassHeadFileMain(MimiObj *pyClass, char *path)
{
    Args *buffs = New_args(NULL);
    char *newFunDeclearation = args_getBuff(buffs, 512);
    char *name = obj_getStr(pyClass, "__name");
    char *fileName = strsAppend(buffs, name, ".h");
    char *filePath = strsAppend(buffs, path, fileName);
    char *ifndef = args_getBuff(buffs, 512);
    char *define = args_getBuff(buffs, 512);

    printf("\n--------[%s]--------\n", filePath);
    FILE *fp = fopen(filePath, "w+");

    fpusWithInfo("/* Warning!!! Don't modify this file!!!*/\n", fp);
    sprintf(ifndef, "#ifndef __%s__H\n", name);
    sprintf(define, "#define __%s__H\n", name);

    fpusWithInfo(ifndef, fp);
    fpusWithInfo(define, fp);
    fpusWithInfo("#include \"MimiObj.h\"\n", fp);
    fpusWithInfo("#include <stdio.h>\n", fp);
    fpusWithInfo("#include <stdlib.h>\n", fp);

    sprintf(newFunDeclearation, "MimiObj *New_%s(Args *args);\n", name);
    fpusWithInfo(newFunDeclearation, fp);

    Args *handleArgs = New_args(NULL);
    args_setPtr(handleArgs, "fp", fp);
    args_foreach(pyClass->attributeList, __foreach_pyMethod_writeEachMethodDeclear, handleArgs);

    fpusWithInfo("#endif\n", fp);

    args_deinit(buffs);
    fclose(fp);
}

MimiObj *New_PyClass(Args *args)
{
    MimiObj *self = New_SysObj(args);
    class_defineMethod(self, "setSuper(superClassName:str)", setSuper);
    return self;
}
