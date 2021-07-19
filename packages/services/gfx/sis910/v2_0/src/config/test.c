#include <stdio.h>
#include <stdlib.h>
#include "config.h"

void test_hash()
{
    hash_table *pt;
    pt = hashConstructTable(31);
    hashInsert("nifei","goodman",pt);
    hashInsert("nifei1","goodman1",pt);
    hashInsert("nifei2","goodman2",pt);

    char* value = hashLookup("nifei",pt);
    printf("the value is %s\n",value);
    hashFreeTable(pt,NULL);
}
int main()
{
    struct configFile *myConfig;
    int someIntValue;
    char *someStringValue;

    test_hash();
    myConfig = readConfigFile("test.cfg");
    if (!myConfig) {
        printf("can't open this file\n");
        exit(-1);
    }
    someStringValue = getConfigString(myConfig, "file browser", "key1");
    if (!someStringValue) {
        printf("don't exit this key\n");
        exit(-1);
    } else {
        printf("the key1 value is %s\n", someStringValue);
    }

    someIntValue = getConfigInt(myConfig, "text reader", "key1");
    printf("someIntValue = %d\n", someIntValue);

    printConfigFile(myConfig);

    setConfigString(myConfig, "file browser", "key1", "nifeinifeinifeinifeinifeinifeinifeinifeinifeinifeinnifeinifeinifeinifei");
    setConfigInt(myConfig, "text reader", "key2", 200);


    printConfigFile(myConfig);
    saveConfigFile(myConfig, "test1.cfg");
    unloadConfigFile(myConfig);


}
