/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : Lock.c
 * Author : 
 * Date   : 2003-05-02
 * Note   : 鎖門指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <map.h>
#include <lock.h>
#include <feature.h>
#include <message.h>
#include <daemon.h>
#include <location.h>

inherit COMMAND;

string help = @HELP
這指令可讓你鎖上屬於你建築物的門，讓外人無法進出被你鎖上的門。

lock '方向'		- 將某個方向的門鎖上
(north, south, east, west, northeast, northwest, southeast, southwest, up, down)
HELP;

private void do_command(object me, string arg)
{
	int locktype;
	string option;
        mapping exits;
        mapping a = ([
        "west"  : "east",
        "east"  : "west",
        "north" : "south",
        "south" : "north",
        "northeast" : "southwest",
        "northwest" : "southeast",
        "southeast" : "northwest",
        "southwest" : "northeast",
        "up"    : "down",
        "down"  : "up"
        ]);
        object env, to;

        env = environment(me);

	if( !arg )
		return tell(me, pnoun(2, me)+"想要將哪一個方向的門上鎖？\n");

	sscanf(arg, "%s %s", arg, option);

	switch(option)
	{
		case "wiz":
			if( wizardp(me) )
				locktype = WIZLOCKED;
			else
				locktype = LOCKED;
			break;
		default:
			locktype = LOCKED;
	}

	if( !env->is_maproom() )
	{
	        if( !wizardp(me) && query("owner", env) != me->query_id(1) )
	                return tell(me, "這裡並不是你的建築物，無法上鎖。\n");

	        if( !mapp(exits = query("exits", env)) )
	                return tell(me, "這裡並沒有任何出口，要怎麼上鎖呢？\n");
	
	        if( !exits[arg] )
	                return tell(me, "這裡並沒有這個方向，無法往這方向上鎖。\n");

	        if( arrayp(exits[arg]) )
	        {
	        	set("lock/"+arg, query("lock/"+arg, env) | locktype, env);
	        	CITY_D->set_coor_data(exits[arg], "lock", CITY_D->query_coor_data(exits[arg], "lock") | locktype);
	        } 
	        else 
	        {
	                to = load_object(exits[arg]);
	                set("lock/"+arg, query("lock/"+arg, env) | locktype, env);
	                set("lock/"+a[arg], query("lock/"+a[arg], to) | locktype, to);
	                broadcast(to, "似乎有人把 "+a[arg] +"方向的門鎖了起來。\n");
	        }
	        
	        tell(me, pnoun(2, me)+"把往 "+capitalize(arg)+" 的方向的門鎖了起來。\n");
	}
	else
	{
		int x, y, num;
		string city;
		array loc = query_temp("location", me);
		
		x = loc[X];
		y = loc[Y];
		city = loc[CITY];
		num = loc[NUM];

		if( !CITY_D->is_city_location(loc) )
			return tell(me, "這附近沒有門可以讓"+pnoun(2, me)+"上鎖。\n");

		switch(arg)
		{
			case "north":
				loc = arrange_city_location(x, --y, city, num);
				break;
			case "south":
				loc = arrange_city_location(x, ++y, city, num);
				break;
			case "east":
				loc = arrange_city_location(++x, y, city, num);
				break;
			case "west":
				loc = arrange_city_location(--x, y, city, num);
				break;
			case "northeast":
				loc = arrange_city_location(++x, --y, city, num);
				break;
			case "northwest":
				loc = arrange_city_location(--x, --y, city, num);
				break;
			case "southeast":
				loc = arrange_city_location(++x, ++y, city, num);
				break;
			case "southwest":
				loc = arrange_city_location(--x, ++y, city, num);
				break;
			default:
				return tell(me, pnoun(2, me)+"想要將哪一個方向的門上鎖？\n");
		}

		if( !CITY_D->valid_coordinate(x, y, city, num) )
			return tell(me, "那個方向是不合理的座標。\n");

		if( !wizardp(me) && CITY_D->query_coor_data(loc, "owner") != me->query_id(1) )
			return tell(me, "這裡並不是你的建築物，無法上鎖。\n");
		
		if( CITY_D->query_coor_data(loc, TYPE) != DOOR )
			return tell(me, "那裡並沒有門，無法上鎖。\n");
		
		if( ESTATE_D->query_loc_estate(loc)["type"] == "land" )
			return tell(me, "只有一扇門你要鎖誰？\n");

		CITY_D->set_coor_data(loc, "lock", CITY_D->query_coor_data(loc, "lock") | locktype);
		tell(me, pnoun(2, me)+"把往 "+capitalize(arg)+" 的方向的門鎖了起來。\n");
	}
}


