/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : master.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-08-24
 * Note   : 新手指導員板
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit BULLETIN_BOARD;

void create()
{
	set_idname(HIB"master"NOR BLU" board"NOR, HIB"新手"NOR BLU"指導"NOR);
	
	// 利用 shadow_ob 技術來做分散式公佈欄, 即時資料皆儲存於 shadow_ob 上
	if( clonep() ) 
		set("shadow_ob", find_object(base_name(this_object())));
	
	else
	{
		set("boardid", "master");
		set("capacity", 2000);
		
		initialize_board();
	}
	startup_action();
}
