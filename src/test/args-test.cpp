#include "gtest/gtest.h"
extern "C"
{
#include "dataArgs.h"
#include "dataString.h"
}
static int mem;
extern DMEM_STATE DMEMS;
TEST(args_test, test1)
{
    mem = DMEMS.blk_num;
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"a", 1);
    int a = args_getInt(args, (char *)"a");
    EXPECT_EQ(a, 1);
    args_deinit(args);
}
TEST(args_test, test2)
{
    Args *args = New_args(NULL);
    float floatOut = 0;
    int64_t int64Out = 0;
    void *pointer = NULL;
    char *strOut = NULL;
    args_setFloatWithDefaultName(args, 24.5);
    args_setInt(args, (char *)"int64Test", (int64_t)22221);
    args_setPtr(args, (char *)"pointerTest", (void *)2222322);
    args_setStr(args, (char *)"strTest", (char *)"teeeds");

    floatOut = args_getFloatByIndex(args, 0);
    int64Out = args_getInt(args, (char *)"int64Test");
    pointer = args_getPtr(args, (char *)"pointerTest");
    strOut = args_getStr(args, (char *)"strTest");

    EXPECT_EQ(floatOut, 24.5);
    EXPECT_EQ(int64Out, 22221);
    EXPECT_EQ((uint64_t)pointer, 2222322);
    EXPECT_EQ(1, strEqu((char *)"teeeds", strOut));
    EXPECT_EQ(0, strcmp("int", args_getType(args, (char *)"int64Test")));
    EXPECT_EQ(0, strcmp("pointer", args_getType(args, (char *)"pointerTest")));
    EXPECT_EQ(0, strcmp("str", args_getType(args, (char *)"strTest")));
    args_deinit(args);
}
TEST(args_test, test3)
{
    Args *args1 = New_args(NULL);
    Args *args2 = New_args(NULL);

    args_setFloat(args1, (char *)"argtest1", 2.883);
    args_setStr(args1, (char *)"strtest1", (char *)"teststr");
    args_copyArgByName(args1, (char *)"argtest1", args2);
    args_copyArgByName(args1, (char *)"strtest1", args2);

    EXPECT_EQ(2.8830f, args_getFloat(args1, (char *)"argtest1"));
    args_deinit(args1);
    args_deinit(args2);
}
TEST(args_test, test4)
{
    int32_t testint = 12333;
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"testint", testint);
    EXPECT_EQ(12333, args_getInt(args, (char *)"testint"));
    args_deinit(args);
}
TEST(args_test, test5)
{
    int32_t testInt = 124;
    Args *args = New_args(NULL);
    args_bind(args, (char *)"int", (char *)"testInt", &testInt);
    char *type = args_getType(args, (char *)"testInt");
    args_print(args, (char *)"testInt");
    EXPECT_EQ(1, strEqu((char *)"124", args_print(args, (char *)"testInt")));
    args_deinit(args);
}
TEST(args_test, test6)
{
    Args *args = New_args(NULL);
    args_setFloat(args, (char *)"testfloat", 1.42);
    EXPECT_TRUE(strEqu((char *)"1.420000", args_print(args, (char *)"testfloat")));
    args_deinit(args);
}
TEST(args_test, test7)
{
    Args *args = New_args(NULL);
    float floatBindTest = 2.314;
    args_bind(args, (char *)"float", (char *)"floatBind", &floatBindTest);
    EXPECT_TRUE(strEqu((char *)"2.314000", args_print(args, (char *)"floatBind")));
    args_deinit(args);
}
TEST(args_test, test8)
{
    Args *args = New_args(NULL);
    args_setStr(args, (char *)"testString", (char *)"test string print");
    EXPECT_TRUE(strEqu((char *)"test string print", args_print(args, (char *)"testString")));
    args_deinit(args);
}
TEST(args_test, test9)
{
    Args *args = New_args(NULL);
    char strBindTest[] = "test string bind";
    args_bindStr(args, (char *)"testStringBind", (char **)&strBindTest);
    EXPECT_TRUE(strEqu((char *)"test string bind", args_print(args, (char *)"testStringBind")));
    args_deinit(args);
}
TEST(args_test, test10)
{
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"testInt", 1);
    char *printOutBefore = args_print(args, (char *)"testInt");
    EXPECT_TRUE(strEqu(printOutBefore, (char *)"1"));
    args_set(args, (char *)"testInt", (char *)"4");
    char *printOutAfter = args_print(args, (char *)"testInt");
    EXPECT_TRUE(strEqu(printOutAfter, (char *)"4"));
    args_deinit(args);
}
TEST(args_test, test11)
{
    Args *args = New_args(NULL);
    char testStrBind[256] = "testtest";
    args_bindStr(args, (char *)"testStr", (char **)&testStrBind);
    char *printOutBefore = args_print(args, (char *)"testStr");
    EXPECT_TRUE(strEqu(printOutBefore, (char *)"testtest"));
    args_set(args, (char *)"testStr", (char *)"ttww");
    char *printOutAfter = args_print(args, (char *)"testStr");
    EXPECT_TRUE(strEqu(printOutAfter, (char *)"ttww"));
    args_deinit(args);
}
TEST(args_test, test12)
{
    Args *args = New_args(NULL);
    args_setInt(args, (char *)"test", 13);
    args_setInt(args, (char *)"kw", 25);
    EXPECT_EQ(2, args_getSize(args));
    EXPECT_EQ(13, (int)args_getInt(args, (char *)"test"));
    args_removeArg(args, (char *)"test");
    EXPECT_EQ(1, args_getSize(args));
    EXPECT_EQ(-999999999, (int)args_getInt(args, (char *)"test"));
    args_deinit(args);
}

extern DMEM_STATE DMEMS;
TEST(args_test, mem)
{
    EXPECT_EQ(DMEMS.blk_num, mem);
}
