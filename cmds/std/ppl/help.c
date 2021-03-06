/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : help.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-13
 * Note   : help 指令, 無聊再來把這個弄成menu吧..
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#define HELP_DIR	"/doc/help/"

#include <ansi.h>
#include <help.h>
#include <feature.h>
#include <daemon.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
說明指令

這個指令可以讓使用者取得使用各項功能的說明
讓使用者能更輕易的熟悉重生的世界的操作方式

指令格式:
help			說明主題列表
help <說明主題>		取得有關 <說明主題> 的進一步資訊

相關指令:
HELP;

private varargs string help_style(string title, string arg, string content, string lastmodify)
{
	return sprintf(
		" %-52s%24s\n" 
		WHT"┌─────────────────────────────────────┐\n"NOR // 78 字元寬
		"\n%s\n"
		WHT"└─────────────────────────────────────┘\n"NOR + (!undefinedp(lastmodify)?CYN"%77s\n"NOR:"") ,
		HIW+title+NOR + " - ", NOR WHT"["HIW + arg + NOR WHT"]"NOR, arg=="color"?content:ansi(content), "最後更新時間 "+lastmodify);
}

private void do_command(object me, string arg)
{
	string help, cmd_alias;
	mapping info;
	object ob, env = environment(me);
	function fp;

	if( !arg ) arg = "topics";

	if( !env )
		return tell(me, pnoun(2, me)+"的角色在奇怪的地方，請通知巫師處理。\n");

	// 環境說明
	if( arg == "here" )
	{
		array loc = query_temp("location", me);

		if( !env )
			return tell(me, pnoun(2, me)+"目前並不身處於任何環境之中。\n");

		// 地圖座標說明
		if( env->is_maproom() )
			return me->more((MAP_D->query_map_system(loc))->query_coor_help(loc) || "這個地方並沒有什麼特別的說明。\n");

		// 模組房間功能說明
		else if( mapp(info = env->query_action_info(query("function", env))) && mapp(info["help"]) && info["help"]["topics"])
			return me->more(info["help"]["topics"]);

		// 一般房間功能說明	
		else
			return me->more(fetch_variable("help", env) || "這個地方並沒有什麼特別的說明。\n");
	}

	if( query("help/"+arg, env) )
		return me->more(help_style("物件說明", arg, query("help/"+arg, env)));

	// 物件說明
	if( objectp(ob = present(arg)) && stringp(fetch_variable("help", ob)) )
		return me->more(help_style("物件說明", arg, fetch_variable("help", ob)));

	// 模組指令說明
	if( mapp(info = env->query_action_info(query("function", env))) && mapp(info["help"]) && info["help"][arg] )
		return me->more(help_style("模組指令說明", arg, info["help"][arg]));

	// 一般說明文件檔
	if( file_size(HELP_DIR+arg) > 0 )
	{
		string title, content;
		help = read_file(HELP_DIR + arg);
		if( sscanf(help, "%s|\n%s", title, content) )
			help = help_style(title, arg, content, TIME_D->replace_ctime(stat(HELP_DIR+arg)[1]));
		return me->more(help);
		//return me->more_file(HELP_DIR+arg);
	}
	if( stringp(cmd_alias = COMMAND_D->default_alias(arg)) )
		arg = explode(cmd_alias, " ")[0];

	// 玩家系統指令說明
	if( functionp(fp = COMMAND_D->find_ppl_command(arg)) )
	{
		help = fetch_variable("help", function_owner(fp)) || "此指令並無任何說明。\n";
		return me->more(help_style("玩家指令說明", arg, help));
	}
	if( wizardp(me) )
	{
		// 巫師系統指令說明
		if( functionp(fp = COMMAND_D->find_wiz_command(arg)) )
		{
			help = fetch_variable("help", function_owner(fp)) || "此指令並無任何說明。\n";
			return me->more(help_style("巫師指令說明", arg, help));
		}

		if( arg[<2..<1] == "()" ) arg = arg[0..<3];

		// MudOS 文件說明
		foreach(string dir in SEARCH_PATHS)
			if( file_size(dir+arg) > 0 )
				return me->more_file(dir+arg);
	}

	tell(me, "沒有 "+arg+" 的相關說明。\n");
}
