/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : buildterrain.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-08-22
 * Note   : 建造地形
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
#include <terrain.h>

inherit COMMAND;

string help = @HELP
        標準 buildterrain 指令。
HELP;

#define BUILDROAD_COST	10000

private void do_command(object me, string arg)
{
	string owner, arg2;
	array loc;
	object env = environment(me);
	
	if( !env->is_maproom() )
		return tell(me, "只有在地圖中才能夠建造地形。\n", CMDMSG);

	loc = query_temp("location", me);
	
	return tell(me, "目前暫不開放土地地形整地功能。\n", CMDMSG);

	if( !CITY_D->is_mayor_or_officer(loc[CITY], me) )
		return tell(me, pnoun(2, me)+"不是這座城市的市長或官員。\n", CMDMSG);

	owner = (MAP_D->query_map_system(loc))->query_coor_data(loc, "owner");

	return tell(me, "目前不開放建造地形。\n", CMDMSG);

	if( owner != "GOVERNMENT/"+loc[CITY] )
		return tell(me, "這塊土地並不是政府土地，無法用來建造地形。\n", CMDMSG);

	switch( (MAP_D->query_map_system(loc))->query_coor_data(loc, TYPE) )
	{
		case DOOR:
		case WALL:
			return tell(me, "這裡已經有其他地形或是建築物，無法在此建造地形。\n", CMDMSG);
	}

	sscanf(arg, "%s %s", arg, arg2);
	
	switch(arg)
	{
		case "forest":
			(MAP_D->query_map_system(loc))->delete_coor_data(loc);
			(MAP_D->query_map_system(loc))->set_coor_data(loc, TYPE, FOREST);
			(MAP_D->query_map_system(loc))->set_coor_icon(loc, HIG"＊"NOR);
			 msg("$ME在此開闢了森林地形「"HIG"＊"NOR"」。\n", me, 0, 1);
			 break;

		case "mountain":
			if( arg2 == "right" )
			{
				(MAP_D->query_map_system(loc))->set_coor_icon(loc, YEL"◣"NOR);
				msg("$ME在此開闢了山脈地形「"YEL"◣"NOR"」。\n", me, 0, 1);
			}
			else if( arg2 == "left" )
			{
				(MAP_D->query_map_system(loc))->set_coor_icon(loc, YEL"◢"NOR);
				msg("$ME在此開闢了山脈地形「"YEL"◢"NOR"」。\n", me, 0, 1);
			}
			else
				return tell(me, "請輸入欲開闢的山脈方向(right, left)。\n", CMDMSG);
				
			(MAP_D->query_map_system(loc))->delete_coor_data(loc);
			(MAP_D->query_map_system(loc))->set_coor_data(loc, TYPE, MOUNTAIN);
			break;

		case "river":
			switch(arg2)
			{
				case "north":
					(MAP_D->query_map_system(loc))->delete_coor_data(loc);
					(MAP_D->query_map_system(loc))->set_coor_data(loc, FLAGS, FLOW_NORTH);
					msg("$ME在此開闢了河流地形「"BBLU HIB"∼"NOR"」，流向往北。\n", me, 0, 1);
					break;
				case "south":
					(MAP_D->query_map_system(loc))->delete_coor_data(loc);
					(MAP_D->query_map_system(loc))->set_coor_data(loc, FLAGS, FLOW_SOUTH);
					msg("$ME在此開闢了河流地形「"BBLU HIB"∼"NOR"」，流向往南。\n", me, 0, 1);
					break;
				case "east":
					(MAP_D->query_map_system(loc))->delete_coor_data(loc);
					(MAP_D->query_map_system(loc))->set_coor_data(loc, FLAGS, FLOW_EAST);
					msg("$ME在此開闢了河流地形「"BBLU HIB"∼"NOR"」，流向往東。\n", me, 0, 1);
					break;
				case "west":
					(MAP_D->query_map_system(loc))->delete_coor_data(loc);
					(MAP_D->query_map_system(loc))->set_coor_data(loc, FLAGS, FLOW_WEST);
					msg("$ME在此開闢了河流地形「"BBLU HIB"∼"NOR"」，流向往西。\n", me, 0, 1);
					break;
				default:
					return tell(me, "請輸入欲開闢河流的流向(north, south, east, west)。\n", CMDMSG);
			}
			
			(MAP_D->query_map_system(loc))->set_coor_data(loc, TYPE, RIVER);
			(MAP_D->query_map_system(loc))->set_coor_icon(loc, BBLU HIB"∼"NOR);
			break;
		default:
			return tell(me, pnoun(2, me)+"想要建造何種地形？(forest, mountain, river)。\n", CMDMSG);
	}
}
