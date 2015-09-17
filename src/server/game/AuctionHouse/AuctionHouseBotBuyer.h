
#ifndef AUCTION_HOUSE_BOT_BUYER_H
#define AUCTION_HOUSE_BOT_BUYER_H

#include "Define.h"
#include "AuctionHouseMgr.h"
#include "AuctionHouseBot.h"

struct BuyerAuctionEval
{
    BuyerAuctionEval(): AuctionId(0), LastChecked(0), LastExist(0) {}

    uint32  AuctionId;
    time_t  LastChecked;
    time_t  LastExist;
};

struct BuyerItemInfo
{
    BuyerItemInfo(): ItemCount(0), BuyPrice(0), BidPrice(0), MinBuyPrice(0), MinBidPrice(0) {}

    uint32  ItemCount;
    double  BuyPrice;
    double  BidPrice;
    uint32  MinBuyPrice;
    uint32  MinBidPrice;
};

typedef std::map<uint32, BuyerItemInfo> BuyerItemInfoMap;
typedef std::map<uint32, BuyerAuctionEval> CheckEntryMap;

struct BuyerConfiguration
{
    BuyerConfiguration(): _houseType(AUCTION_HOUSE_NEUTRAL) {}

    void Initialize(AuctionHouseType houseType)
    {
        _houseType = houseType;
    }

    AuctionHouseType GetHouseType() const { return _houseType; }

    BuyerItemInfoMap SameItemInfo;
    CheckEntryMap CheckedEntry;
    uint32 FactionChance;
    bool BuyerEnabled;
    uint32 BuyerPriceRatio;

private:
    AuctionHouseType _houseType;
};

// This class handle all Buyer method
// (holder of AuctionBotConfig for each auction house type)
class AuctionBotBuyer : public AuctionBotAgent
{
public:
    AuctionBotBuyer();
    ~AuctionBotBuyer() override;

    bool Initialize() override;
    bool Update(AuctionHouseType houseType) override;

    void LoadConfig();
    void AddNewAuctionBuyerBotBid(BuyerConfiguration& config);

private:
    uint32 _checkInterval;
    BuyerConfiguration _houseConfig[MAX_AUCTION_HOUSE_TYPE];

    void LoadBuyerValues(BuyerConfiguration& config);
    bool IsBuyableEntry(uint32 buyoutPrice, double inGameBuyPrice, double maxBuyablePrice, uint32 minBuyPrice, uint32 maxChance, uint32 chanceRatio);
    bool IsBidableEntry(uint32 bidPrice, double inGameBuyPrice, double maxBidablePrice, uint32 minBidPrice, uint32 maxChance, uint32 chanceRatio);
    void PlaceBidToEntry(AuctionEntry* auction, uint32 bidPrice);
    void BuyEntry(AuctionEntry* auction);
    void PrepareListOfEntry(BuyerConfiguration& config);
    uint32 GetBuyableEntry(BuyerConfiguration& config);
};

#endif