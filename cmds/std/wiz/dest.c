/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : dest.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-7-5
 * Note   : dest 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <feature.h>
#include <message.h>

inherit COMMAND;

string help = @HELP
	標準 dest 指令。
HELP;

private void command(object me, string arg)
{
	int dest_all;
	string str, ob_idname, amount, total_amount, location;
	object ob, env;
	
	if( !is_command() ) return;

	if( !arg ) return tell(me, "指令格式：dest <物件名||檔案名>。\n", CMDMSG);
	
	if( sscanf(arg, "%s %s", amount, arg) == 2 )
	{
		// 如果 amount 並不是數字單位
		if( to_float(amount) == 0. )
		{	
			if( lower_case(amount) == "all" )
				dest_all = 1;
			else
			{
				arg = amount + " " + arg;
				amount = "1";
			}	
		}	
		else if( count(amount, "<", 1) ) amount = "1";
	}
	else amount = "1";
	
	ob = present(arg, me) || present(arg, environment(me));

	if( !objectp(ob) )
	{
		str = resolve_path(me, arg);
		ob = find_object(str) || find_object(str+".c") || find_object(str+".o");
	}

	if( !objectp(ob) )
		return tell(me, "沒有 "+arg+" 這個物件或檔案。\n", CMDMSG);
	
	ob_idname = ob->query_idname()||file_name(ob);
	
	if( userp(ob) )
		return tell(me, pnoun(2, me)+"無法摧毀一個使用者物件。\n", CMDMSG);
	
	total_amount = query_temp("amount",ob) || "1";
	
	env = environment(ob);
	
	if( env == me )
		location = "身上";
	else if( env == environment(me) )
		location = "地上";
	else
		location = "記憶體中";
	
	if( dest_all || count(total_amount, "<=", amount) )
	{	
		string unit = query("unit",ob)||"個";
		
		if( destruct(ob) )
			return msg("$ME使出"HIW"百"NOR WHT"裂"HIW"掌"NOR"將"+location+(total_amount==" 1"?"的 "+NUMBER_D->number_symbol(total_amount)+" "+unit:"全部的")+"「"+ob_idname+"」撕成了碎片。\n", me, 0, 1, ENVMSG);
			
		return tell(me, "摧毀物件 "+arg+" 失敗。\n", CMDMSG);
	}
	else
	{
		msg("$ME使出"HIW"百"NOR WHT"裂"HIW"掌"NOR"將"+location+"的 "+NUMBER_D->number_symbol(amount)+" "+(query("unit",ob)||"個")+"「"+ob_idname+"」撕成了碎片。\n", me, 0, 1, ENVMSG);
		
		if( env->is_living() )
			addn_temp("loading/cur", -to_int(count(amount,"*",query("mass",ob))), env);

		set_temp("amount", count(total_amount, "-", amount), ob);
		return;
	}
}