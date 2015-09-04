
#include "Log.h"
#include "Item.h"
#include "ItemPrototype.h"
#include "AuctionHouseBotBuyer.h"

AuctionBotBuyer::AuctionBotBuyer()
{
    // Define faction for our main data class.
    for (int i = 0; i < MAX_AUCTION_HOUSE_TYPE; ++i)
        _houseConfig[i].Initialize(AuctionHouseType(i));
}

AuctionBotBuyer::~AuctionBotBuyer()
{
}

bool AuctionBotBuyer::Initialize()
{
    LoadConfig();

    bool activeHouse = false;
    for (int i = 0; i < MAX_AUCTION_HOUSE_TYPE; ++i)
    {
        if (_houseConfig[i].BuyerEnabled)
        {
            activeHouse = true;
            break;
        }
    }

    if (!activeHouse)
        return false;

    //load Check interval
    _checkInterval = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_BUYER_RECHECK_INTERVAL) * MINUTE;
	sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot buyer interval between 2 check = %u", _checkInterval);
    return true;
}

void AuctionBotBuyer::LoadBuyerValues(BuyerConfiguration& config)
{
    uint32 factionChance;

    switch (config.GetHouseType())
    {
        case AUCTION_HOUSE_ALLIANCE:
            config.BuyerPriceRatio = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_ALLIANCE_PRICE_RATIO) + 50;
            factionChance = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_BUYER_CHANCE_RATIO_ALLIANCE);
            break;
        case AUCTION_HOUSE_HORDE:
            config.BuyerPriceRatio = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_HORDE_PRICE_RATIO) + 50;
            factionChance = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_BUYER_CHANCE_RATIO_HORDE);
            break;
        default:
            config.BuyerPriceRatio = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_NEUTRAL_PRICE_RATIO) + 50;
            factionChance = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_BUYER_CHANCE_RATIO_NEUTRAL);
            break;
    }

    config.FactionChance = 5000 * factionChance;
}

void AuctionBotBuyer::LoadConfig()
{
    for (int i = 0; i < MAX_AUCTION_HOUSE_TYPE; ++i)
    {
        _houseConfig[i].BuyerEnabled = sAuctionBotConfig->GetConfigBuyerEnabled(AuctionHouseType(i));
        if (_houseConfig[i].BuyerEnabled)
            LoadBuyerValues(_houseConfig[i]);
    }
}

uint32 AuctionBotBuyer::GetBuyableEntry(BuyerConfiguration& config)
{
    config.SameItemInfo.clear();
    uint32 count = 0;
    time_t now = time(nullptr);

    AuctionHouseObject* house = sAuctionMgr->GetAuctionsMap(config.GetHouseType());
    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = house->GetAuctionsBegin(); itr != house->GetAuctionsEnd(); ++itr)
    {
        AuctionEntry* entry = itr->second;
        Item* item = sAuctionMgr->GetAItem(entry->itemGUIDLow);
        if (item)
        {
            ItemTemplate const * prototype = item->GetTemplate();
            if (prototype)
            {
                ++config.SameItemInfo[item->GetEntry()].ItemCount;    // Structure constructor will make sure Element are correctly initialized if entry is created here.
                config.SameItemInfo[item->GetEntry()].BuyPrice = config.SameItemInfo[item->GetEntry()].BuyPrice + (itr->second->buyout / item->GetCount());
                config.SameItemInfo[item->GetEntry()].BidPrice = config.SameItemInfo[item->GetEntry()].BidPrice + (itr->second->startbid / item->GetCount());
                if (itr->second->buyout != 0)
                {
                    if (itr->second->buyout / item->GetCount() < config.SameItemInfo[item->GetEntry()].MinBuyPrice)
                        config.SameItemInfo[item->GetEntry()].MinBuyPrice = itr->second->buyout / item->GetCount();
                    else if (config.SameItemInfo[item->GetEntry()].MinBuyPrice == 0)
                        config.SameItemInfo[item->GetEntry()].MinBuyPrice = itr->second->buyout / item->GetCount();
                }
                if (itr->second->startbid / item->GetCount() < config.SameItemInfo[item->GetEntry()].MinBidPrice)
                    config.SameItemInfo[item->GetEntry()].MinBidPrice = itr->second->startbid / item->GetCount();
                else if (config.SameItemInfo[item->GetEntry()].MinBidPrice == 0)
                    config.SameItemInfo[item->GetEntry()].MinBidPrice = itr->second->startbid / item->GetCount();

                if (!entry->owner)
                {

                    if (entry->bid != 0 && entry->bidder) // Add bid by player
                    {
                        config.CheckedEntry[entry->Id].LastExist = now;
                        config.CheckedEntry[entry->Id].AuctionId = entry->Id;
                        ++count;
                    }
                }
                else
                {
                    if (entry->bid != 0)
                    {
                        if (entry->bidder)
                        {
                            config.CheckedEntry[entry->Id].LastExist = now;
                            config.CheckedEntry[entry->Id].AuctionId = entry->Id;
                            ++count;
                        }
                    }
                    else
                    {
                        config.CheckedEntry[entry->Id].LastExist = now;
                        config.CheckedEntry[entry->Id].AuctionId = entry->Id;
                        ++count;
                    }
                }
            }
        }
    }

	sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: %u items added to buyable vector for ah type: %u", count, config.GetHouseType());
	sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: SameItemInfo size = %u", (uint32)config.SameItemInfo.size());
    return count;
}

void AuctionBotBuyer::PrepareListOfEntry(BuyerConfiguration& config)
{
    time_t now = time(nullptr) - 5;

    for (CheckEntryMap::iterator itr = config.CheckedEntry.begin(); itr != config.CheckedEntry.end();)
    {
        if (itr->second.LastExist < (now - 5))
            config.CheckedEntry.erase(itr++);
        else
            ++itr;
    }

	sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: CheckedEntry size = %u", (uint32)config.CheckedEntry.size());
}

bool AuctionBotBuyer::IsBuyableEntry(uint32 buyoutPrice, double inGameBuyPrice, double maxBuyablePrice, uint32 minBuyPrice, uint32 maxChance, uint32 chanceRatio)
{
    double ratio = 0;
    uint32 chance = 0;

    if (buyoutPrice <= minBuyPrice)
    {
        if (buyoutPrice <= maxBuyablePrice)
            chance = maxChance;
        else
        {

            if (buyoutPrice > 0 && maxBuyablePrice > 0)
            {
                ratio = buyoutPrice / maxBuyablePrice;
                if (ratio < 10)
                    chance = maxChance - (ratio * maxChance / 10);
                else
                    chance = 1;
            }
        }
    }
    else if (buyoutPrice <= inGameBuyPrice)
    {
        if (buyoutPrice <= maxBuyablePrice)
            chance = maxChance / 5;
        else
        {

            if (buyoutPrice > 0 && maxBuyablePrice > 0)
            {
                ratio = buyoutPrice / maxBuyablePrice;
                if (ratio < 10)
                    chance = (maxChance / 5) - (ratio * maxChance / 50);
                else
                    chance = 1;
            }
        }
    }
    else if (buyoutPrice <= maxBuyablePrice)
        chance = maxChance / 10;
    else
    {
        if (buyoutPrice > 0 && maxBuyablePrice > 0)
        {
            ratio = buyoutPrice / maxBuyablePrice;
            if (ratio < 10)
                chance = (maxChance / 5) - (ratio* maxChance / 50);
            else
                chance = 0;
        }
        else
            chance = 0;
    }

    if (urand(1, chanceRatio) <= chance)
    {
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: WIN BUY! Chance = %u, num = %u.", chance, chanceRatio);
        return true;
    }
    else
    {
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: LOOSE BUY! Chance = %u, num = %u.", chance, chanceRatio);
        return false;
    }
}

bool AuctionBotBuyer::IsBidableEntry(uint32 bidPrice, double inGameBuyPrice, double maxBidablePrice, uint32 minBidPrice, uint32 maxChance, uint32 chanceRatio)
{
    double ratio = 0;
    uint32 chance = 0;

    if (bidPrice <= minBidPrice)
    {
        if (inGameBuyPrice != 0 && bidPrice < inGameBuyPrice - (inGameBuyPrice / 30))
            chance = maxChance;
        else
        {
            if (bidPrice < maxBidablePrice)
            {
                ratio = maxBidablePrice / bidPrice;
                if (ratio < 3)
                    chance = maxChance / 500 * ratio;
                else
                    chance = maxChance / 500;
            }
        }
    }
    else if (bidPrice < (inGameBuyPrice - (inGameBuyPrice / 30)))
        chance = (maxChance / 10);
    else
    {
        if (bidPrice < maxBidablePrice)
        {
            ratio = maxBidablePrice / bidPrice;
            if (ratio < 4)
                chance = maxChance / 1000 * ratio;
            else
                chance = maxChance / 1000;
        }
    }

    if (urand(1, chanceRatio) <= chance)
    {
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: WIN BID! Chance = %u, num = %u.", chance, chanceRatio);
        return true;
    }
    else
    {
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: LOOSE BID! Chance = %u, num = %u.", chance, chanceRatio);
        return false;
    }
}

void AuctionBotBuyer::PlaceBidToEntry(AuctionEntry* auction, uint32 bidPrice)
{
	sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: Bid placed to entry %u, %.2fg", auction->Id, float(bidPrice) / 10000.0f);
    auction->bid = bidPrice;
}

void AuctionBotBuyer::BuyEntry(AuctionEntry* auction)
{
	sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: Entry %u bought at %.2fg", auction->Id, float(auction->buyout) / 10000.0f);
    auction->bid = auction->buyout;
}

void AuctionBotBuyer::AddNewAuctionBuyerBotBid(BuyerConfiguration& config)
{
    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config.GetHouseType());

    PrepareListOfEntry(config);

    time_t now = time(nullptr);
    uint32 buyCycles;
    if (config.CheckedEntry.size() > sAuctionBotConfig->GetItemPerCycleBoost())
    {
        buyCycles = sAuctionBotConfig->GetItemPerCycleBoost();
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: Boost value used for Buyer! (if this happens often adjust both ItemsPerCycle in worldserver.conf)");
    }
    else
        buyCycles = sAuctionBotConfig->GetItemPerCycleNormal();

    for (CheckEntryMap::iterator itr = config.CheckedEntry.begin(); itr != config.CheckedEntry.end();)
    {
        AuctionEntry* auction = auctionHouse->GetAuction(itr->second.AuctionId);
        if (!auction) // is auction not active now
        {
			sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: Entry %u on ah %u doesn't exists, perhaps bought already?",
                itr->second.AuctionId, auction->GetHouseId());

            config.CheckedEntry.erase(itr++);
            continue;
        }

        if (itr->second.LastChecked != 0 && (now - itr->second.LastChecked) <= _checkInterval)
        {
			sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: In time interval wait for entry %u!", auction->Id);
            ++itr;
            continue;
        }

        if (buyCycles == 0)
            break;

        uint32 maxChance = 5000;

        Item* item = sAuctionMgr->GetAItem(auction->itemGUIDLow);
        if (!item) // auction item not accessible, possible auction in payment pending mode
        {
            config.CheckedEntry.erase(itr++);
            continue;
        }

        ItemTemplate const* prototype = item->GetTemplate();

        uint32 basePrice = sAuctionBotConfig->GetConfig(CONFIG_AHBOT_BUYPRICE_BUYER) ? prototype->BuyPrice : prototype->SellPrice;
        basePrice *= item->GetCount();

        double maxBuyablePrice = (basePrice * config.BuyerPriceRatio) / 100;
        BuyerItemInfoMap::iterator sameItemItr = config.SameItemInfo.find(item->GetEntry());
        uint32 buyoutPrice = auction->buyout / item->GetCount();

        uint32 bidPrice;
        uint32 bidPriceByItem;
        if (auction->bid >= auction->startbid)
        {
            bidPrice = auction->GetAuctionOutBid();
            bidPriceByItem = auction->bid / item->GetCount();
        }
        else
        {
            bidPrice = auction->startbid;
            bidPriceByItem = auction->startbid / item->GetCount();
        }

        double inGameBuyPrice;
        double inGameBidPrice;
        if (sameItemItr == config.SameItemInfo.end())
        {
            inGameBuyPrice = 0;
            inGameBidPrice = 0;
        }
        else
        {
            if (sameItemItr->second.ItemCount == 1)
                maxBuyablePrice = maxBuyablePrice * 5; // if only one item exist can be bought if the price is high too.
            inGameBuyPrice = sameItemItr->second.BuyPrice / sameItemItr->second.ItemCount;
            inGameBidPrice = sameItemItr->second.BidPrice / sameItemItr->second.ItemCount;
        }

        double maxBidablePrice = maxBuyablePrice - (maxBuyablePrice / 30); // Max Bidable price defined to 70% of max buyable price

		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: Auction added with data:");
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: MaxPrice of Entry %u is %.1fg.", itr->second.AuctionId, maxBuyablePrice / 10000);
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: GamePrice buy=%.1fg, bid=%.1fg.", inGameBuyPrice / 10000, inGameBidPrice / 10000);
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: Minimal price see in AH Buy=%ug, Bid=%ug.",
            sameItemItr->second.MinBuyPrice / 10000, sameItemItr->second.MinBidPrice / 10000);
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: Actual Entry price,  Buy=%ug, Bid=%ug.", buyoutPrice / 10000, bidPrice / 10000);

        if (!auction->owner)                // Original auction owner
            maxChance = maxChance / 5;      // if Owner is AHBot this mean player placed bid on this auction. We divide by 5 chance for AhBuyer to place bid on it. (This make more challenge than ignore entry)
        if (auction->buyout != 0)           // Is the item directly buyable?
        {
            if (IsBuyableEntry(buyoutPrice, inGameBuyPrice, maxBuyablePrice, sameItemItr->second.MinBuyPrice, maxChance, config.FactionChance))
            {
                if (IsBidableEntry(bidPriceByItem, inGameBuyPrice, maxBidablePrice, sameItemItr->second.MinBidPrice, maxChance / 2, config.FactionChance))
                {
                    if (urand(0, 5) == 0)
                        PlaceBidToEntry(auction, bidPrice);
                    else
                        BuyEntry(auction);
                }
                else
                    BuyEntry(auction);
            }
            else if (IsBidableEntry(bidPriceByItem, inGameBuyPrice, maxBidablePrice, sameItemItr->second.MinBidPrice, maxChance / 2, config.FactionChance))
                PlaceBidToEntry(auction, bidPrice);
        }
        else if (IsBidableEntry(bidPriceByItem, inGameBuyPrice, maxBidablePrice, sameItemItr->second.MinBidPrice, maxChance, config.FactionChance))
            PlaceBidToEntry(auction, bidPrice);

        itr->second.LastChecked = now;
        --buyCycles;

        ++itr;
    }
}

bool AuctionBotBuyer::Update(AuctionHouseType houseType)
{
    if (sAuctionBotConfig->GetConfigBuyerEnabled(houseType))
    {
		sLog->outInfo(LOG_FILTER_SERVER_LOADING, "AHBot: %s buying ...", AuctionBotConfig::GetHouseTypeName(houseType));
        if (GetBuyableEntry(_houseConfig[houseType]) > 0)
            AddNewAuctionBuyerBotBid(_houseConfig[houseType]);
        return true;
    }

    return false;
}