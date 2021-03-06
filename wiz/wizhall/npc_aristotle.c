/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : npc_aristotle.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-08-23
 * Note   : 
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

/*
	系統預設之 NPC 反應函式
	void reply_emote(object me, string verb, string args);
	void reply_say(object me, string msg);
	void reply_catch_tell(object me, string verb, string args);
	void reply_get_object(object me, object ob, mixed amount);
	void reply_get_money(object me, string unit, mixed money);
*/

#include <ansi.h>
#include <inherit.h>
#include <gender.h>
#include <daemon.h>

inherit STANDARD_NPC;

void create()
{
	set_idname("aristotle","亞里斯多德");

	set("unit", "位");
	set("age", 62);
	set("gender", MALE_GENDER);
	set("position", "哲學家");

	startup_living();
}
