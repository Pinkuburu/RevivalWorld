/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : land_dominium.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-1-21
 * Note   : 土地所有權狀
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <map.h>
#include <ansi.h>
#include <inherit.h>
#include <daemon.h>
#include <message.h>
#include <location.h>
#include <roommodule.h>

inherit STANDARD_OBJECT;

string DECLARATION = @TEXT


	             [1;37;43m 土 地 所 有 權 狀 [m
    利用這張土地所有權狀可自由認領一塊無人擁有的土地，認領後
的土地便由%^GENDER%^所擁有，認領土地後%^GENDER%^便可以開始利用此土地來創造財
屬於自己的財富，選擇一塊荒地後 claim 即可認領。      [1;31m╭─╮[m
                                                     [1;31m│核│[m
    擁有者：%^ID%^                             [1;31m│准│[m
    有效期限：永遠有效                               [1;31m╰─╯[m
    

TEXT;

void do_claim(object me, string arg);

mapping actions = 	
([
	"claim"			:(: do_claim :),
]);

void do_claim(object me, string arg)
{
	object env = environment(me);
	mapping data;
	string owner;
	array loc = query_temp("location", me);
	
	if( !(owner = query_temp("owner")) )
		return tell(me, "這是一張廢棄的土地所有權狀，"+pnoun(2, me)+"無法使用。\n", CMDMSG);
		
	if( owner != me->query_id(1) )
		return tell(me, "這張土地所有權狀是 "+capitalize(owner)+" 的，"+pnoun(2, me)+"無權使用。\n", CMDMSG);
	
	if( !arrayp(loc) || !env || !env->is_maproom() ) 
		return tell(me, pnoun(2, me)+"無法認領這塊土地。\n", CMDMSG);

	if( !CITY_D->is_city_location(loc) )
		return tell(me, pnoun(2, me)+"只能在城市區域內認領土地。\n", CMDMSG);

	if( loc[CITY] != query("city", me) )
		return tell(me, pnoun(2, me)+"只能在自己的城市裡認領土地。\n", CMDMSG);

	if( !(CITY_D->query_coor_data(loc, "region") & AGRICULTURE_REGION) )
		return tell(me, pnoun(2, me)+"只能認領已經經過市政府規劃的農業區。\n", CMDMSG);
	
	if( !mapp(data = CITY_D->query_coor_data(loc)) )
		return tell(me, "土地資料錯誤，請通知巫師處理。\n", CMDMSG);
	
	if( nullp(owner = data["owner"]) && nullp(data["ownership"]) )
	{
		if( CITY_D->set_coor_data(loc, "owner", me->query_id(1)) )
		{
			CITY_D->set_coor_icon(loc, BWHT"□"NOR);
			CITY_D->set_coor_data(loc, "claimed", 1);
			tell(me, pnoun(2, me)+"利用土地所有權狀認領了這塊土地。\n", CMDMSG);
			
			// 設定房地產資訊
			ESTATE_D->set_land_estate(me->query_id(1), loc);

			delete("flag/land_dominium", me);
			me->save();
			destruct(this_object());
		}
		else if( owner == me->query_id(1) )
			return tell(me, "這塊地已經是"+pnoun(2, me)+"的了。\n", CMDMSG);
	}
	else tell(me, "這塊地已經是別人的了，"+pnoun(2, me)+"無法認領。\n", CMDMSG);
}

// 在誰手上就是誰的
void set_owner()
{
	object env = environment(this_object());

	if( env && userp(env) )
	{
		set_idname(env->query_id()+"'s land dominium", env->query_name()+"的土地所有權狀");
		set("long", terminal_colour(DECLARATION, (["ID":sprintf("%-12s",ansi_capitalize(env->query_id())), "GENDER":(query("gender", env)=="男"?"你":"妳")])));
		set_temp("owner", env->query_id(1));
	}
}

void create()
{
	set_idname("droped land dominium", "廢棄的土地所有權狀");
	set_temp("keep", 2);
	set("long", terminal_colour(DECLARATION, (["ID":sprintf("%-12s","已廢棄"), "GENDER":"無"])));
	
	call_out( (: set_owner :), 1 );
	
        if( this_object()->set_shadow_ob() ) return;
	
	set("unit","張");
	set("mass",10);
	set("value", -1);
	set("flag/no_amount", 1);
        set("flag/no_drop", 1);
        set("flag/no_give", 1);
}
