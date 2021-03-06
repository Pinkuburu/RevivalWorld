/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : rmdir.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-8
 * Note   : rmdir 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
	標準 rmdir 指令。
HELP;

private void command(object me, string arg)
{

	if( !is_command() ) return;
	
	if( !arg )
		return tell(me, "請輸入路徑名稱。\n", CMDMSG);

	arg = resolve_path(me, arg);
	
	if( file_size(arg) != -2 )
		return tell(me, "沒有 "+arg+" 這個目錄。\n", CMDMSG);

	if( sizeof(get_dir(arg[<1]=='/'?arg:arg+"/")) != 0 )
		return tell(me, "請先將目錄 "+arg+" 內的目錄檔案刪除。\n", CMDMSG);

	if( rmdir(arg) )
		tell(me, "刪除目錄 "+arg+" 完畢。\n", CMDMSG);
	else
		tell(me, "你沒有權利刪除目錄 "+arg+"。\n", CMDMSG);
}