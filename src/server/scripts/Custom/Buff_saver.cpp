#include "ScriptMgr.h"
#include "GossipDef.h"
 
#include "string"
 
uint32 BuffList[] = {72525, 71195, 71193, 71188, 62276, 63364, 48869, 69787, 2583, 90007, 47008, 26662, 62555, 27680};

 
class Buff_saver : public CreatureScript
{
  public: Buff_saver() : CreatureScript("Buff_saver") {}
 
  bool OnGossipHello(Player* players, Creature* creature)
  {
    players->ADD_GOSSIP_ITEM(GOSSIP_ICON_BATTLE, "Сохрани мои Бафы!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    players->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT_18, "Верни мне мои Бафы!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
    players->SEND_GOSSIP_MENU(100350, creature->GetGUID());
 
    return true;
  }
 
  bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
  {
	int SpellIs[100] = {};
    QueryResult buff_check = WorldDatabase.PQuery("SELECT player_guid FROM active_buff_list WHERE player_guid = %i;", player->GetGUID());

      time_t t = time(NULL);
	  
      switch(action)
      {
          case GOSSIP_ACTION_INFO_DEF+1:
            if(!buff_check)
            {
              for(int i = 0; i < 16; i++)
              {
                if(player->GetAura(BuffList[i]))
                  SpellIs[i] = BuffList[i];
 
                player->RemoveAura(BuffList[i]);
              }
 
			  WorldDatabase.PQuery("INSERT INTO active_buff_list VALUE(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i);", player->GetGUID(), SpellIs[0], SpellIs[1], SpellIs[2], SpellIs[3], SpellIs[4], SpellIs[5], SpellIs[6], SpellIs[7], SpellIs[8], SpellIs[9], SpellIs[10], SpellIs[11], SpellIs[12], SpellIs[13], SpellIs[15]);            
              WorldDatabase.PQuery("INSERT INTO played_loged VALUE(4, %i, '%s', '%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i');", t, player->GetName(), SpellIs[0], SpellIs[1], SpellIs[2], SpellIs[3], SpellIs[4], SpellIs[5], SpellIs[6], SpellIs[7], SpellIs[8], SpellIs[9], SpellIs[10], SpellIs[11], SpellIs[12], SpellIs[13], SpellIs[15]);
			  creature->MonsterWhisper("Ваши бафы были успешно сохранены", player->GetGUID());
			}
			else
			  creature->MonsterWhisper("У вас уже есть сохроненные бафы!", player->GetGUID());
          break;
 
          case GOSSIP_ACTION_INFO_DEF+2:
            if(buff_check)
            {
              QueryResult result_buff = WorldDatabase.PQuery("SELECT * FROM active_buff_list WHERE player_guid = %i;", player->GetGUID());
 
              Field* field_buff = result_buff->Fetch();
 
              for(int i = 1; i < 16; i++)
			  {
               player->AddAura(field_buff[i].GetInt32(), player);
				
			  }
				WorldDatabase.PQuery("DELETE FROM active_buff_list WHERE player_guid = %i;", player->GetGUID());
                WorldDatabase.PQuery("INSERT INTO played_loged VALUE(5, %i, '%s', 'Restored');", t, player->GetName());
			    creature->MonsterWhisper("Вот ваши бафы!", player->GetGUID());
            }
			else
			  creature->MonsterWhisper("У вас нету сохраненных бафов!", player->GetGUID());
          break;
      }
 
      player->CLOSE_GOSSIP_MENU();
      return true;
  }
};
 
void AddSC_Buff_saver()
{
    new Buff_saver();
}