/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : buildbridge.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-05-03
 * Note   : 建造橋樑
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <map.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>
#include <location.h>

inherit COMMAND;

string help = @HELP
    造橋指令，造橋費用每段為 2,000,000
buildbridge north	- 往北方建造橋樑
buildbridge south	- 往南方建造橋樑
buildbridge east	- 往東方建造橋樑
buildbridge west	- 往西方建造橋樑
buildbridge west -d	- 拆除在西方的橋樑
HELP;

#define BUILDBRIDGE_COST	2000000

private void do_command(object me, string arg)
{
	string name;
	array loc;
	object env = environment(me);
	
	if( !env->is_maproom() )
		return tell(me, "只有在地圖中才能夠建造橋樑。\n");

	loc = query_temp("location", me);

	if( !CITY_D->is_mayor_or_officer(loc[CITY], me) )
		return tell(me, pnoun(2, me)+"不是這座城市的市長或官員。\n");

	if( !arg )
		return tell(me, "請輸入正確的橋樑建造方向(north, south, west, east)。\n");

	sscanf(arg, "%s %s", arg, name);
	
	switch(arg)
	{
		case "north":
			loc = arrange_city_location(loc[X], loc[Y]-1, loc[CITY], loc[NUM]);
			break;
		case "south":
			loc = arrange_city_location(loc[X], loc[Y]+1, loc[CITY], loc[NUM]);
			break;
		case "west":
			loc = arrange_city_location(loc[X]-1, loc[Y], loc[CITY], loc[NUM]);
			break;
		case "east":
			loc = arrange_city_location(loc[X]+1, loc[Y], loc[CITY], loc[NUM]);
			break;
		default:
			return tell(me, "請輸入正確的橋樑建造方向(north, south, west, east)。\n");
	}

	if( name == "-d" )
	{
		if( (MAP_D->query_map_system(loc))->query_coor_data(loc, TYPE) != BRIDGE )
			return tell(me, "那個方向並沒有橋樑。\n");

		(MAP_D->query_map_system(loc))->set_coor_data(loc, TYPE, RIVER); 
		(MAP_D->query_map_system(loc))->set_coor_icon(loc, BBLU HIB"∼"NOR);
		(MAP_D->query_map_system(loc))->delete_coor_data(loc, "owner");
		msg("$ME拆除掉了在 "+arg+" 方向的橋樑。\n", me, 0, 1);
		return;
	}

	if( (MAP_D->query_map_system(loc))->query_coor_data(loc, TYPE) != RIVER )
		return tell(me, "那個方向並沒有河流。\n");

	if( name )
	{
		if( noansi_strlen(name) > 14 )
			return tell(me, "橋樑名稱最長不能超過七個中文字。\n");

		name = kill_repeat_ansi(name+NOR);
		
		if( (MAP_D->query_map_system(loc))->query_coor_data(loc, TYPE) == BRIDGE && (MAP_D->query_map_system(loc))->query_coor_data(loc, "bridgename") == name )
			return tell(me, "這塊土地上已經建造了橋樑「"+arg+"」。\n");
		
		if( CITY_D->city_exist(loc[CITY]) )
		{
			CITY_D->change_assets(loc[CITY], -BUILDBRIDGE_COST);
			
			msg("$ME花費了市府資產 $"+MONEY_D->city_to_money_unit(loc[CITY])+" "+NUMBER_D->number_symbol(BUILDBRIDGE_COST)+"，在 "+arg+" 方向開闢了一座新橋樑，名叫「"+name+"」。\n", me, 0, 1);
		
			CITY_D->set_coor_data(loc, "owner", "GOVERNMENT/"+loc[CITY]);
		}
		else
			msg("$ME在 "+arg+" 方向開闢了一條新橋樑，名叫「"+name+"」。\n", me, 0, 1);
		(MAP_D->query_map_system(loc))->set_coor_data(loc, "bridgename", name);
	}
	else
	{
		if( CITY_D->query_coor_data(loc, TYPE) == BRIDGE && !CITY_D->query_coor_data(loc, "roadname") )
			return tell(me, "這塊土地上已經建造了橋樑。\n");
		
		if( CITY_D->city_exist(loc[CITY]) )
		{
			CITY_D->change_assets(loc[CITY], -BUILDBRIDGE_COST);
			
			msg("$ME花費了市府資產 $"+MONEY_D->city_to_money_unit(loc[CITY])+" "+NUMBER_D->number_symbol(BUILDBRIDGE_COST)+"，在 "+arg+" 方向開闢了一座新橋樑。\n", me, 0, 1);
		
			CITY_D->set_coor_data(loc, "owner", "GOVERNMENT/"+loc[CITY]);
		}
		else
			msg("$ME在 "+arg+" 方向開闢了一條新橋樑。\n", me, 0, 1);
	}

	(MAP_D->query_map_system(loc))->set_coor_data(loc, TYPE, BRIDGE); 
	(MAP_D->query_map_system(loc))->set_coor_icon(loc, BRED HIR"井"NOR);
	
}
