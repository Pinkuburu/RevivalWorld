/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : room_s_kitchen.c
 * Author : tzj@RevivalWorld
 * Date   : 2006-09-09
 * Note   : 蘇菲亞家的廚房
 * Update :
 *  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit STANDARD_ROOM;

string help = @HELP
在這個平凡無奇的房間內並沒有任何特殊的資訊。
HELP;

void create()
{
	set("short", HIY"廚房"NOR);
	set("long",@TEXT
　　一走進這裡，烹調時產生的熱氣迎面而來，隨即聞到燉肉美味濃厚的香氣，
火爐上放置的鍋子正沸騰地咕嚕作響。此處廚具的擺放井然有序，想必經過一番
巧思整理。房內牆壁漆著讓人覺得溫暖的淡淡鵝黃色，但靠近爐子的牆壁稍微被
熏成棕色了。
TEXT);


	set("exits", ([
		//往東走是通往2F的樓梯及兩個後門
		"east"    : "/quest/suphia_and_benjamin/room_s_afterroom",
		
		//往南走是客廳
		"south"   : "/quest/suphia_and_benjamin/room_s_frontroom",
		
		//往西走是牆
		//"west"  : ,
		
		//往北走是牆
		//"north" : ,
		
	]));

/*	預留放NPC或物品的...
	set("objects", ([
		"/quest/suphia_and_benjamin/npc_suphia" : 1 ,
	]));
*/
	
	::reset_objects();
	replace_program(STANDARD_ROOM);
}
