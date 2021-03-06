/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : quit.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-5
 * Note   : quit 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
離線指令

離開重生的世界
HELP;

private void do_command(object me, string arg)
{
	if( me->is_fighting() )
		return tell(me,"你正在戰鬥中，無法離線！\n");
	if( me != this_player(1) ) 
		tell(me, this_player(1)->query_idname()+"強迫你離開遊戲。\n", CMDMSG);

	if( me->is_delaying() )
		return tell(me, me->query_delay_msg(), CMDMSG);

     	catch{"/cmds/std/wiz/snoop.c"->remove_user(me);};

	LOGOUT_D->quit(me, arg);
}
