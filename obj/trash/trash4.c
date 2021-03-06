/* This	program	is a part of RW	mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File	  : Trash.c
 * Author :
 * Date	  : 2003-5-3
 * Note	  : 垃圾
 * Update :
 *  o 2000-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit	STANDARD_OBJECT;

void create()
{
	object *user = users();
	int usersize = sizeof(user);

	if( usersize )
		set_idname(WHT"briefs"NOR, user[random(usersize)]->query_name()+"穿過的破內褲");
	else
		set_idname(WHT"briefs"NOR , "穿過的破內褲");

	set_temp("status", WHT"垃圾"NOR);

	if( this_object()->set_shadow_ob() ) return;

	set("unit", "個");
	set("value", -1);
	set("mass", 70);
	set("garbage", 1);
}

