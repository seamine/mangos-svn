/* 
 * Copyright (C) 2005,2006,2007 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef MANGOS_MAIL_H
#define MANGOS_MAIL_H

#include <map>

#define MAIL_BODY_ITEM_TEMPLATE 8383                        // - plain letter, A Dusty Unsent Letter: 889

enum MAIL_RESPONSE
{
    MAIL_OK                 = 0,
    MAIL_MONEY_TAKEN        = 1,
    MAIL_ITEM_TAKEN         = 2,
    MAIL_RETURNED_TO_SENDER = 3,
    MAIL_DELETED            = 4,
    MAIL_MADE_PERMANENT     = 5
};

enum MAIL_ERRORS
{
    MAIL_ERR_BAG_FULL                  = 1,
    MAIL_ERR_CANNOT_SEND_TO_SELF       = 2,
    MAIL_ERR_NOT_ENOUGH_MONEY          = 3,
    MAIL_ERR_RECIPIENT_NOT_FOUND       = 4,
    MAIL_ERR_NOT_YOUR_TEAM             = 5,
    MAIL_ERR_INTERNAL_ERROR            = 6,
    MAIL_ERR_DISABLED_FOR_TRIAL_ACC    = 14,
    MAIL_ERR_RECIPIENT_CAP_REACHED     = 15,
    MAIL_ERR_CANT_SEND_WRAPPED_COD     = 16,
    MAIL_ERR_MAIL_AND_CHAT_SUSPENDED   = 17
};

enum MAIL_CHECKED
{
    NOT_READ            = 0,
    READ                = 1,
    AUCTION_CHECKED     = 4,
    COD_PAYMENT_CHECKED = 8,
    RETURNED_CHECKED    = 16
};

enum MailMessageType
{
    MAIL_NORMAL         = 0,
    MAIL_AUCTION        = 2,
    MAIL_CREATURE       = 3,                                // client send CMSG_CREATURE_QUERY on this mailmessagetype
    MAIL_GAMEOBJECT     = 4,                                // client send CMSG_GAMEOBJECT_QUERY on this mailmessagetype
    MAIL_ITEM           = 5,                                // client send CMSG_ITEM_QUERY on this mailmessagetype
};

enum Mail_state
{
    MAIL_STATE_UNCHANGED = 1,
    MAIL_STATE_CHANGED   = 2,
    MAIL_STATE_DELETED   = 3
};

enum MailAuctionAnswers
{
    AUCTION_OUTBIDDED           = 0,
    AUCTION_WON                 = 1,
    AUCTION_SUCCESSFUL          = 2,
    AUCTION_EXPIRED             = 3,
    AUCTION_CANCELLED_TO_BIDDER = 4,
    AUCTION_CANCELED            = 5
};

// gathered from Stationery.dbc
enum MailStationery
{
    MAIL_STATIONERY_UNKNOWN = 0x01,
    MAIL_STATIONERY_NORMAL  = 0x29,
    MAIL_STATIONERY_GM      = 0x3D,
    MAIL_STATIONERY_AUCTION = 0x3E,
    MAIL_STATIONERY_VAL     = 0x40,
    MAIL_STATIONERY_CHR     = 0x41
};

struct MailItemInfo
{
    uint32 item_guid;
    uint32 item_template;
};

struct MailItem
{
    MailItem() : item(NULL) {}
    uint8 item_slot;                                    // slot in mail
    uint16 item_pos;                                    // inventory pos
    uint64 item_guid;                                   // item guid
    uint32 item_template;                               // item entry
    Item *item;                                         // item pointer
    
    void deleteItem()
    {
        if(item)
        {
            delete item;
            item=NULL;
        }
    }
};

typedef std::map<uint64, MailItem> MailItemMap;


struct MailItemsInfo
{
    MailItemMap i_MailItemMap;                            // Keep the items in a map to avoid duplicate guids (which can happen)

    MailItemMap::iterator begin() { return i_MailItemMap.begin(); }
    MailItemMap::iterator end() { return i_MailItemMap.end(); }

    void AddItem(uint32 guid, uint32 _template, Item *item, uint8 slot = 0, uint16 pos = 0)
    {
        MailItem mailItem;
        mailItem.item_slot = slot;
        mailItem.item_pos = pos;
        mailItem.item_guid = guid;
        mailItem.item_template = _template;
        mailItem.item = item;
        i_MailItemMap[guid] = mailItem;
    }

    void AddItem(uint64 guid, uint8 slot = 0)
    {
        MailItem mailItem;
        mailItem.item_guid = guid;
        mailItem.item_slot = slot;
        i_MailItemMap[guid] = mailItem;
    }
    
    bool hasGuid(uint64 pGuid)
    {
        return(i_MailItemMap.find(pGuid) != i_MailItemMap.end());
    }

    MailItem& getMailItemInfo(uint64 pGuid)
    {
        return i_MailItemMap.find(pGuid)->second;
    }

    uint8 size()
    {
        return i_MailItemMap.size();
    }

    void deleteIncludedItems()
    {
        for(MailItemMap::iterator mailItemIter = begin(); mailItemIter != end(); ++mailItemIter)
        {
            MailItem& mailItem = mailItemIter->second;
            mailItem.deleteItem();
        }
    }
};

struct Mail
{
    uint32 messageID;
    uint8 messageType;
    uint8 stationery;
    uint32 sender;
    uint32 receiver;
    std::string subject;
    uint32 itemTextId;
    std::vector<MailItemInfo> items;
    std::vector<uint32> removedItems;
    time_t expire_time;
    time_t deliver_time;
    uint32 money;
    uint32 COD;
    uint32 checked;
    Mail_state state;

    void AddItem(uint32 itemId, uint32 item_template)
    {
        MailItemInfo mii;
        mii.item_guid = itemId;
        mii.item_template = item_template;
        items.push_back(mii);
    }

    void AddAllItems(MailItemsInfo& pMailItemsInfo)
    {
        for(MailItemMap::iterator mailItemIter = pMailItemsInfo.begin(); mailItemIter != pMailItemsInfo.end(); ++mailItemIter)
        {
            MailItem& mailItem = mailItemIter->second;
            AddItem(GUID_LOPART(mailItem.item_guid), mailItem.item_template);
        }
    }

    bool RemoveItem(uint32 itemId)
    {
        for(std::vector<MailItemInfo>::iterator itr = items.begin(); itr != items.end(); ++itr)
        {
            if(itr->item_guid == itemId)
            {
                items.erase(itr);
                return true;
            }
        }
        return false;
    }

    bool HasItems() { return items.empty() ? false : true; }
};

#endif
