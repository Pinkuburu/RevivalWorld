/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : recall.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-2-2
 * Note   : recall 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <feature.h>
#include <object.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
這個指令可以讓您在迷路的時候快速地回到巫師大廳


HELP;

#define WIZ_HALL_UP	"/wiz/wizhall/room_wizhall_7"

private void do_command(object me, string arg)
{
	object recallroom;
	object env = environment(me);
	object *inv;

	// 是犯人
	if( query("prisoner", me) )
	{
		msg("$ME的手腳被鐵鍊銬住，無法自由行動。\n", me, 0, 1);
		return;
	}

	//忙碌中不能下指令
	if( me->is_delaying() )
	{
		tell(me, me->query_delay_msg());
		return me->show_prompt();
	}

	if( wizardp(me) )
		recallroom = load_object(WIZ_HALL_UP);
	else
		recallroom = load_object(WIZ_HALL);
		
	inv = all_inventory(recallroom);

	if( env && env == recallroom ) 
		return tell(me, pnoun(2, me)+"已經在"+query("short", env)+"了。\n");

	msg("$ME舉起右手在面前極速畫了一個圓，竟劃破了面前的空間，只見圓內的空間真空將$ME給吸了進去。\n", me, 0, 1);

	me->move(recallroom);

	inv = filter_array(inv, (: userp($1) :));

	if( sizeof(inv) )
		msg("$ME從"+query("short", recallroom)+"的角落空間裂縫中鑽出，嚇了$YOU一跳。\n", me, inv[random(sizeof(inv))], 1);
	else
		msg("$ME從"+query("short", recallroom)+"的角落空間裂縫中鑽出。\n", me, 0, 1);
}