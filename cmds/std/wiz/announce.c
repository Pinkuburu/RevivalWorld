/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : announce.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-9-12
 * Note   : announce 指令
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
指令格式：announce <公告訊息>

可以對玩家發出強制的公告訊息，以通知玩家
重要事項。
HELP;

private void command(object me, string arg)
{
	if( !is_command() ) return;
	if( !arg )
		return tell(me, "指令格式：announce <公告訊息>\n", CMDMSG);
	CHANNEL_D->announce(me, arg);
	return;
}