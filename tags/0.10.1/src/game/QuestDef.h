/* 
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
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

#ifndef MANGOSSERVER_QUEST_H
#define MANGOSSERVER_QUEST_H

#include "Platform/Define.h"
#include "Database/DatabaseEnv.h"

#include <string>
#include <vector>

class Player;

class ObjectMgr;

// (PLAYER_VISIBLE_ITEM_1_CREATOR - PLAYER_QUEST_LOG_1_1)/3
#define MAX_QUEST_LOG_SIZE 25

#define QUEST_OBJECTIVES_COUNT 4
#define QUEST_SOURCE_ITEM_IDS_COUNT 4
#define QUEST_REWARD_CHOICES_COUNT 6
#define QUEST_REWARDS_COUNT 4
#define QUEST_DEPLINK_COUNT 10
#define QUEST_REPUTATIONS_COUNT 5
#define QUEST_EMOTE_COUNT 4

enum QuestFailedReasons
{
    INVALIDREASON_DONT_HAVE_REQ                 = 0,
    INVALIDREASON_QUEST_FAILED_LOW_LEVEL        = 1,        //You are not high enough level for that quest.
    INVALIDREASON_QUEST_FAILED_WRONG_RACE       = 6,        //That quest is not available to your race.
    INVALIDREASON_QUEST_ALREADY_DONE            = 7,        //You have completed that quest.
    INVALIDREASON_QUEST_ONLY_ONE_TIMED          = 12,       //You can only be on one timed quest at a time.
    INVALIDREASON_QUEST_ALREADY_ON              = 13,       //You are already on that quest
    INVALIDREASON_QUEST_FAILED_EXPANSION        = 16,       //This quest requires an expansion enabled account.
    INVALIDREASON_QUEST_ALREADY_ON2             = 18,       //You are already on that quest
    INVALIDREASON_QUEST_FAILED_MISSING_ITEMS    = 21,       //You don't have the required items with you. Check storage.
    INVALIDREASON_QUEST_FAILED_NOT_ENOUGH_MONEY = 23,       //You don't have enough money for that quest.
    INVALIDREASON_DAILY_QUESTS_REMAINING        = 26,       //You have already completed 10 daily quests today
    INVALIDREASON_QUEST_FAILED_CAIS             = 27,       //You cannot complete quests once you have reached tired time
};

enum QuestShareMessages
{
    QUEST_PARTY_MSG_SHARING_QUEST   = 0,
    QUEST_PARTY_MSG_CANT_TAKE_QUEST = 1,
    QUEST_PARTY_MSG_ACCEPT_QUEST    = 2,
    QUEST_PARTY_MSG_REFUSE_QUEST    = 3,
    QUEST_PARTY_MSG_TOO_FAR         = 4,
    QUEST_PARTY_MSG_BUSY            = 5,
    QUEST_PARTY_MSG_LOG_FULL        = 6,
    QUEST_PARTY_MSG_HAVE_QUEST      = 7,
    QUEST_PARTY_MSG_FINISH_QUEST    = 8,
};

enum __QuestTradeSkill
{
    QUEST_TRSKILL_NONE           = 0,
    QUEST_TRSKILL_ALCHEMY        = 1,
    QUEST_TRSKILL_BLACKSMITHING  = 2,
    QUEST_TRSKILL_COOKING        = 3,
    QUEST_TRSKILL_ENCHANTING     = 4,
    QUEST_TRSKILL_ENGINEERING    = 5,
    QUEST_TRSKILL_FIRSTAID       = 6,
    QUEST_TRSKILL_HERBALISM      = 7,
    QUEST_TRSKILL_LEATHERWORKING = 8,
    QUEST_TRSKILL_POISONS        = 9,
    QUEST_TRSKILL_TAILORING      = 10,
    QUEST_TRSKILL_MINING         = 11,
    QUEST_TRSKILL_FISHING        = 12,
    QUEST_TRSKILL_SKINNING       = 13,
    QUEST_TRSKILL_JEWELCRAFTING  = 14,
};

enum QuestStatus
{
    QUEST_STATUS_NONE           = 0,
    QUEST_STATUS_COMPLETE       = 1,
    QUEST_STATUS_UNAVAILABLE    = 2,
    QUEST_STATUS_INCOMPLETE     = 3,
    QUEST_STATUS_AVAILABLE      = 4,
    MAX_QUEST_STATUS
};

enum __QuestGiverStatus
{
    DIALOG_STATUS_NONE                     = 0,
    DIALOG_STATUS_UNAVAILABLE              = 1,
    DIALOG_STATUS_CHAT                     = 2,
    DIALOG_STATUS_INCOMPLETE               = 3,
    DIALOG_STATUS_REWARD_REP               = 4,
    DIALOG_STATUS_AVAILABLE_REP            = 5,
    DIALOG_STATUS_AVAILABLE                = 6,
    DIALOG_STATUS_REWARD2                  = 7,             // not yellow dot on minimap
    DIALOG_STATUS_REWARD                   = 8              // yellow dot on minimap
};

enum __QuestFlags
{
    // Flags used at server and sended to client
    QUEST_FLAGS_STAY_ALIVE     = 1,                         // Not used currently
    QUEST_FLAGS_EVENT          = 2,                         // Not used currently
    QUEST_FLAGS_EXPLORATION    = 4,                         // Not used currently
    QUEST_FLAGS_UNK1           = 8,                         // Not used currently
    //QUEST_FLAGS_NONE2        = 16,                        // Not used currently
    QUEST_FLAGS_EPIC           = 32,                        // Not used currently: Unsure of content
    QUEST_FLAGS_RAID           = 64,                        // Not used currently
    QUEST_FLAGS_TBC            = 128,                       // Not used currently: Available if TBC expension enabled only
    QUEST_FLAGS_UNK2           = 256,                       // Not used currently: _DELIVER_MORE Quest needs more than normal _q-item_ drops from mobs
    QUEST_FLAGS_UNK3           = 512,                       // Not used currently: _REWARD_OUTSIDE Item rewards not given like in normal quests, Given somehow outside the quest or created by spell
    QUEST_FLAGS_UNK4           = 1024,                      // Not used currently: Unknown tbc flag
    QUEST_FLAGS_TBC_RACES      = 2048,                      // Not used currently: Bloodelf/draenei starting zone quests
    QUEST_FLAGS_DAILY          = 4096,                      // Used to know quest is Daily one

    // Mangos flags for set SpecialFlags in DB if required but used only at server
    QUEST_MANGOS_FLAGS_REPEATABLE           = 0x010000,     // Set by 1 in SpecialFlags from DB
    QUEST_MANGOS_FLAGS_EXPLORATION_OR_EVENT = 0x020000,     // Set by 2 in SpecialFlags from DB (if reequired area explore, spell SPELL_EFFECT_QUEST_COMPLETE casting, table `*_script` command SCRIPT_COMMAND_QUEST_EXPLORED use, set from script DLL)
    QUEST_MANGOS_FLAGS_DB_ALLOWED = 0xFFFF | QUEST_MANGOS_FLAGS_REPEATABLE | QUEST_MANGOS_FLAGS_EXPLORATION_OR_EVENT,

    // Mangos flags for internal use only
    QUEST_MANGOS_FLAGS_DELIVER              = 0x040000,     // Internal flag computed only
    QUEST_MANGOS_FLAGS_SPEAKTO              = 0x080000,     // Internal flag computed only
    QUEST_MANGOS_FLAGS_KILL_OR_CAST         = 0x100000,     // Internal flag computed only
    QUEST_MANGOS_FLAGS_TIMED                = 0x200000,     // Internal flag computed only
};

struct QuestLocale
{
    std::vector<std::string> Title;
    std::vector<std::string> Details;
    std::vector<std::string> Objectives;
    std::vector<std::string> OfferRewardText;
    std::vector<std::string> RequestItemsText;
    std::vector<std::string> EndText;
    std::vector<std::string> ObjectiveText[QUEST_OBJECTIVES_COUNT];
};

// This Quest class provides a convenient way to access a few pretotaled (cached) quest details,
// all base quest information, and any utility functions such as generating the amount of
// xp to give
class Quest
{
    friend class ObjectMgr;
    public:
        Quest(Field * questRecord);
        uint32 XPValue( Player *pPlayer ) const;

        bool HasFlag( uint32 flag ) const { return ( QuestFlags & flag ) != 0; }
        void SetFlag( uint32 flag ) { QuestFlags |= flag; }

        // table data accessors:
        uint32 GetQuestId() const { return QuestId; }
        int32  GetZoneOrSort() const { return ZoneOrSort; }
        uint32 GetMinLevel() const { return MinLevel; }
        uint32 GetQuestLevel() const { return QuestLevel; }
        uint32 GetType() const { return Type; }
        uint32 GetRequiredRaces() const { return RequiredRaces; }
        uint32 GetRequiredSkillValue() const { return RequiredSkillValue; }
        uint32 GetRepObjectiveFaction() const { return RepObjectiveFaction; }
        int32  GetRepObjectiveValue() const { return RepObjectiveValue; }
        uint32 GetRequiredMinRepFaction() const { return RequiredMinRepFaction; }
        int32  GetRequiredMinRepValue() const { return RequiredMinRepValue; }
        uint32 GetRequiredMaxRepFaction() const { return RequiredMaxRepFaction; }
        int32  GetRequiredMaxRepValue() const { return RequiredMaxRepValue; }
        uint32 GetSuggestedPlayers() const { return SuggestedPlayers; }
        uint32 GetLimitTime() const { return LimitTime; }
        int32  GetPrevQuestId() const { return PrevQuestId; }
        int32  GetNextQuestId() const { return NextQuestId; }
        int32  GetExclusiveGroup() const { return ExclusiveGroup; }
        uint32 GetNextQuestInChain() const { return NextQuestInChain; }
        uint32 GetSrcItemId() const { return SrcItemId; }
        uint32 GetSrcItemCount() const { return SrcItemCount; }
        uint32 GetSrcSpell() const { return SrcSpell; }
        std::string GetTitle() const { return Title; }
        std::string GetDetails() const { return Details; }
        std::string GetObjectives() const { return Objectives; }
        std::string GetOfferRewardText() const { return OfferRewardText; }
        std::string GetRequestItemsText() const { return RequestItemsText; }
        std::string GetEndText() const { return EndText; }
        int32  GetRewOrReqMoney() const;
        uint32 GetRewMoneyMaxLevel() const { return RewMoneyMaxLevel; }
                                                            // use in XP calculation at client
        uint32 GetRewSpell() const { return RewSpell; }
        uint32 GetPointMapId() const { return PointMapId; }
        float  GetPointX() const { return PointX; }
        float  GetPointY() const { return PointY; }
        uint32 GetPointOpt() const { return PointOpt; }
        uint32 GetIncompleteEmote() const { return IncompleteEmote; }
        uint32 GetCompleteEmote() const { return CompleteEmote; }
        uint32 GetQuestStartScript() const { return QuestStartScript; }
        uint32 GetQuestCompleteScript() const { return QuestCompleteScript; }
        bool   IsRepeatable() const { return QuestFlags & QUEST_MANGOS_FLAGS_REPEATABLE; }
        bool   IsAutoComplete() const { return Objectives.empty(); }
        uint32 GetFlags() const { return QuestFlags; }
        bool   IsDaily() const { return QuestFlags & QUEST_FLAGS_DAILY; }

        // multiple values
        std::string ObjectiveText[QUEST_OBJECTIVES_COUNT];
        uint32 ReqItemId[QUEST_OBJECTIVES_COUNT];
        uint32 ReqItemCount[QUEST_OBJECTIVES_COUNT];
        uint32 ReqSourceId[QUEST_SOURCE_ITEM_IDS_COUNT];
        uint32 ReqSourceCount[QUEST_SOURCE_ITEM_IDS_COUNT];
        uint32 ReqSourceRef[QUEST_SOURCE_ITEM_IDS_COUNT];
        int32  ReqCreatureOrGOId[QUEST_OBJECTIVES_COUNT];   // >0 Creature <0 Gameobject
        uint32 ReqCreatureOrGOCount[QUEST_OBJECTIVES_COUNT];
        uint32 ReqSpell[QUEST_OBJECTIVES_COUNT];
        uint32 RewChoiceItemId[QUEST_REWARD_CHOICES_COUNT];
        uint32 RewChoiceItemCount[QUEST_REWARD_CHOICES_COUNT];
        uint32 RewItemId[QUEST_REWARDS_COUNT];
        uint32 RewItemCount[QUEST_REWARDS_COUNT];
        uint32 RewRepFaction[QUEST_REPUTATIONS_COUNT];
        int32  RewRepValue[QUEST_REPUTATIONS_COUNT];
        uint32 DetailsEmote[QUEST_EMOTE_COUNT];
        uint32 OfferRewardEmote[QUEST_EMOTE_COUNT];

        uint32 GetReqItemsCount() const { return m_reqitemscount; }
        uint32 GetReqCreatureOrGOcount() const { return m_reqCreatureOrGOcount; }
        uint32 GetRewChoiceItemsCount() const { return m_rewchoiceitemscount; }
        uint32 GetRewItemsCount() const { return m_rewitemscount; }

        typedef std::vector<int32> PrevQuests;
        PrevQuests prevQuests;
        typedef std::vector<uint32> PrevChainQuests;
        PrevChainQuests prevChainQuests;

        // cached data
    private:
        uint32 m_reqitemscount;
        uint32 m_reqCreatureOrGOcount;
        uint32 m_rewchoiceitemscount;
        uint32 m_rewitemscount;

        // table data
    protected:
        uint32 QuestId;
        int32  ZoneOrSort;
        uint32 MinLevel;
        uint32 QuestLevel;
        uint32 Type;
        uint32 RequiredRaces;
        uint32 RequiredSkillValue;
        uint32 RepObjectiveFaction;
        int32  RepObjectiveValue;
        uint32 RequiredMinRepFaction;
        int32  RequiredMinRepValue;
        uint32 RequiredMaxRepFaction;
        int32  RequiredMaxRepValue;
        uint32 SuggestedPlayers;
        uint32 LimitTime;
        uint32 QuestFlags;
        int32  PrevQuestId;
        int32  NextQuestId;
        int32  ExclusiveGroup;
        uint32 NextQuestInChain;
        uint32 SrcItemId;
        uint32 SrcItemCount;
        uint32 SrcSpell;
        std::string Title;
        std::string Details;
        std::string Objectives;
        std::string OfferRewardText;
        std::string RequestItemsText;
        std::string EndText;
        int32  RewOrReqMoney;
        uint32 RewMoneyMaxLevel;
        uint32 RewSpell;
        uint32 PointMapId;
        float  PointX;
        float  PointY;
        uint32 PointOpt;
        uint32 IncompleteEmote;
        uint32 CompleteEmote;
        uint32 QuestStartScript;
        uint32 QuestCompleteScript;
};

enum QuestUpdateState
{
    QUEST_UNCHANGED = 0,
    QUEST_CHANGED = 1,
    QUEST_NEW = 2
};

struct QuestStatusData
{
    QuestStatusData()
        : m_status(QUEST_STATUS_NONE),m_rewarded(false),
        m_explored(false), m_timer(0), uState(QUEST_NEW)
    {
        memset(m_itemcount,    0, QUEST_OBJECTIVES_COUNT * sizeof(uint32));
        memset(m_creatureOrGOcount, 0, QUEST_OBJECTIVES_COUNT * sizeof(uint32));
    }

    QuestStatus m_status;
    bool m_rewarded;
    bool m_explored;
    uint32 m_timer;
    QuestUpdateState uState;

    uint32 m_itemcount[ QUEST_OBJECTIVES_COUNT ];
    uint32 m_creatureOrGOcount[ QUEST_OBJECTIVES_COUNT ];
};
#endif
