/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : equip.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-01-21
 * Note   : 
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <equipment.h>

inherit COMMAND;

string help = @HELP
    裝備物品的指令，無論是武器、衣物、座騎、裝飾品、團體武裝都
是利用此指令進行裝備動作。

equip '物品'		裝備某項物品

HELP;

private void do_command(object me, string arg)
{
	int status;
	object ob;

	if( !arg )
	{
		string msg;
		object equipments = me->query_equipment_objects();
		
		if( !sizeof(equipments) )
			return tell(me, pnoun(2, me)+"目前身上沒有任何裝備。\n");

		msg = pnoun(2, me)+"目前身上的裝備如下：\n";
		
		foreach(ob in me->query_equipment_objects())
			msg += "．"HIW"["NOR+ob->query_part_name()+HIW"] "+ob->query_idname()+"\n"NOR;
			
		return tell(me, msg+"\n");
	}
		
	if( !objectp(ob = present(arg, me)) )
		return tell(me, pnoun(2, me)+"的身上並沒有 "+arg+" 這個物品。\n");
	
	if( me->is_equipping_object(ob) )
		return tell(me, pnoun(2, me)+"已經將"+ob->query_idname()+"裝備在「"+ob->query_part_name()+"」的部位上了。\n");

	if( !me->equip(ob, &status) )
	{
		switch(status)
		{
			// 1: 此物件不是裝備
			// 2: 無法裝備在此物件上
			// 3: 不知此物件該裝備在何處
			// 4: 已經有同種類的裝備
			// 5: 已經裝備在其他的部位上
			
			case 1:	return tell(me, ob->query_idname()+"無法用來裝備。\n"); break;
			case 2: return tell(me, pnoun(2, me)+"似乎沒有足夠的能力來裝備這個物品。\n"); break;
			case 3: return tell(me, pnoun(2, me)+"不瞭解"+ob->query_idname()+"該裝備在何處。\n"); break;
			case 4: return tell(me, ob->query_idname()+"必須裝備在「"+ob->query_part_name()+"」部位上，但與"+pnoun(2, me)+"身上某個裝備中的物品發生衝突。\n"); break;
			case 5: return tell(me, ob->query_idname()+"已經裝備在其他的部位上了。\n"); break;
		}
	}
	else
		msg("$ME將$YOU裝備在「"+ob->query_part_name()+"」的部位上。\n", me, ob, 1);
}
