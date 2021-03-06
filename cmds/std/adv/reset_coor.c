/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : reset_coor.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-09-16
 * Note   : reset_coor 指令
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <map.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
	標準 reset_coor 指令。
HELP;

private void command(object me, string arg)
{	
	array loc;
	object env = environment(me);
	
	if( !is_command() ) return;
	
	if( !env->is_maproom() )
		return tell(me, "必須在地圖系統上才能夠重設座標資料。\n", CMDMSG);
	
	loc = query_temp("location", me);

	switch( CITY_D->query_coor_data(loc, TYPE) )
	{
		case MOUNTAIN:
		case FOREST:
		case RIVER:
			return tell(me, "這塊地是自然地形，無法重設座標資料。\n", CMDMSG);
	}
		
	CITY_D->delete_coor_data(loc);
	CITY_D->delete_coor_icon(loc);
	ESTATE_D->remove_estate(loc);

	tell(me, "重設座標資料完畢。\n", CMDMSG);
}
