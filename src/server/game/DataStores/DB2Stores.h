/*
TER-Server
*/

#ifndef TRINITY_DB2STORES_H
#define TRINITY_DB2STORES_H

#include "DB2Store.h"
#include "DB2Structure.h"

#include <string> 
#include <list>

enum DB2Hash
{
    DB2_HASH_ITEM               = 0x50238EC2,
    DB2_HASH_ITEM_CURRENCY_COST = 0x6FE05AE9,
    DB2_HASH_ITEM_EXTENDED_COST = 0xBB858355,
    DB2_HASH_ITEM_SPARSE        = 0x919BE54E,
    DB2_HASH_KEYCHAIN           = 0x6D8A2694,
}; 

extern DB2Storage <ItemEntry> sItemStore;
extern DB2Storage <ItemCurrencyCostEntry> sItemCurrencyCostStore;
extern DB2Storage <ItemExtendedCostEntry> sItemExtendedCostStore;
extern DB2Storage <ItemSparseEntry> sItemSparseStore;
extern DB2Storage <KeyChainEntry> sKeyChainStore; 

void LoadDB2Stores(std::string const& dataPath);

#endif