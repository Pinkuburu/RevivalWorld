/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : whereis.c
 * Author : Clode@RevivalWorld
 * Date   : 2002-8-14
 * Note   : whereis 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <feature.h>
#include <message.h>
#include <daemon.h>
#include <location.h>

inherit COMMAND;

string help = @HELP
	標準 whereis 指令
HELP;

private void whereis(string target_file, string dir, string ref *results, string ref *elem)
{
	string path;
	
	if( file_size(dir) != -2 || !arrayp(get_dir(dir)) )
		return;

	foreach(string file in get_dir(dir))
	{
		path = dir + file;

		if( file_size(path) == -2 ) whereis(target_file, path+"/", ref results, ref elem);
		
		else
		{
			if( file == target_file || file[0..<3] == target_file )
				results += ({ path });
				
			else if( file[0..(strlen(elem[0])-1)] == elem[0] )
			{
				int all_fit = 1;
			
				if( sizeof(elem) >= 2 
					&& file[<strlen(elem[<1])..<1] != elem[<1] 
					&& file[0..<3][<strlen(elem[<1])..<1] != elem[<1] )
				{
					all_fit = 0;
				}
				
				else if( sizeof(elem) > 2 )
				{
					foreach( string e in elem[1..<2] )
						if( strsrch(file, e) == -1 ) all_fit = 0;
				}
				
				if( all_fit )
					results += ({ path });
			}
		}
	}	
}
		
private void command(object me, string arg)
{
	int count;
	object ob;
	string msg;
	string *elem, *results = allocate(0);
	
	if( !is_command() ) return;
	
	if( !arg ) return tell(me, "請輸入欲搜尋的檔案名稱或玩家 ID。\n", CMDMSG);
	
	if( objectp(ob = find_player(arg)) )
	{
		string location;
		object env = environment(ob);
		
		if( env )
		{
			if( env->is_maproom() )
			{
				array loc = query_temp("location", ob);
				location = CITY_D->query_city_idname(loc[CITY])+NOR"第"+CHINESE_D->chinese_number(loc[NUM]+1)+"都會區，座標("+(loc[X]+1)+","+(loc[Y]+1)+")";
			}
			else
				location = query("short", env);
		}	
		
		return tell(me, ob->query_idname()+"目前所在位置："+location+"。\n", CMDMSG);
	}
	
	elem = explode(arg, "*");
	
	whereis(arg,"/", ref results, ref elem);
	
	if( !sizeof(results) ) 
		return tell(me, "沒有任何符合名稱條件的檔案。\n", CMDMSG);
		
	msg = "搜尋 "+arg+"，共有 "+sizeof(results)+" 個搜尋結果。\n";
	
	foreach( string r in results )
		msg += sprintf(HIY"%-3d -"NOR" %-s\n",++count, r);
	
	me->more(msg+"\n");
	
}