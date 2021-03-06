/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : set.c
 * Author : Cwlin@RevivalWorld
 * Date   : 2003-06-07
 * Note   : 
 * Update :
 *  o 2003-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <feature.h>
#include <message.h>
#include <daemon.h>
#include <type.h>

inherit COMMAND;

#define VAR_TYPE        0
#define VAR_REGEXP      1
#define VAR_LEVEL	2
#define VAR_COMMENT     3

// 參數型態
#define TYPE_BOOLEAN	0
#define	TYPE_INTEGER	1
#define TYPE_STRING	2

// 使用權限
#define LV_PLAYER	0
#define LV_WIZARD	1

string help = @HELP
設定環境變數

這個指令用來設定環境變數，可讓使用者自由調整更改喜好且習慣的操作環境。

指令格式:
set			顯示所有可供更改的環境變數與設定值
set -l			顯示所有可供更改的環境變數與說明
set <變數> <值>		將環境變數 <變數> 的值更改為 <值>
set -d <變數>		將環境變數 <變數> 更改為系統預設值

相關指令: chfn, alias,
HELP;
private nosave mapping ENV_VARS = 
([
//	變數名稱	: ({ 參數型態, 參數格式(正規表示法), 權限, 說明 }),
	"no_map"	: ({ TYPE_BOOLEAN, "[0-1]", 	LV_PLAYER, "開關即時地圖顯示 (0: 開啟, 1: 關閉)" }),
	"rumor"		: ({ TYPE_INTEGER, "[0-2]", 	LV_PLAYER, "謠言頻道匿名模式 (0: 匿名, 1: 冒名, 2: 玩家編號)" }),
	"invis"		: ({ TYPE_BOOLEAN, "[0-1]", 	LV_WIZARD, "隱形模式 (0: 出現, 1: 隱形)" }),
	"no_weather"	: ({ TYPE_BOOLEAN, "[0-1]", 	LV_WIZARD, "不受天氣影響 (0: 受影響, 1: 不受影響)" }),
        "timezone"	: ({ TYPE_STRING,  "[a-zA-Z]", 	LV_PLAYER, "時區" }),
        "edit_prompt"	: ({ TYPE_BOOLEAN, "[0-1]", 	LV_PLAYER, "使用文字編輯器時顯示命令提示字元 (0: 不顯示, 1: 顯示)" }),
        "no_player_city": ({ TYPE_BOOLEAN, "[0-1]", 	LV_PLAYER, "玩家名稱之前的城市名稱顯示 (0: 顯示, 1:不顯示)" }),
 //       "no_id_color"	: ({ TYPE_BOOLEAN, "[0-1]",	LV_PLAYER, "角色名稱之彩色顯示(0: 彩色, 1: 黑白)" }),
 //       "no_color"	: ({ TYPE_BOOLEAN, "[0-1]",	LV_PLAYER, "所有文字之彩色顯示(0: 彩色, 1: 黑白)" }),
]);

private void command(object me, string arg)
{
	mixed val;
	string var, msg;
	mapping myvars;
	array var_data;

	if( !is_command() ) return;

	if( !myvars = query("env", me) ) myvars = allocate_mapping(0);

	// 顯示目前的設定
	if( !arg || arg == "-l" )
	{
		msg = pnoun(2, me) + "目前允許設定的環境變數如下：\n";
		foreach(var, var_data in ENV_VARS)
		{
			if( var_data[VAR_LEVEL] == LV_WIZARD && !wizardp(me) ) continue;

			if( arg != "-l" )
			{
				val = myvars[var];
				if( var_data[VAR_TYPE] == TYPE_BOOLEAN ) val = (val ? "是":"否");
				if( var_data[VAR_TYPE] == TYPE_STRING ) val = (val ? val:"未設定");
			}
			else val = var_data[VAR_COMMENT];
			
			msg += sprintf(HIW BLK" ["HIW"%-|16s"BLK"]"NOR"\t\t -> " + val + "\n", var);
		}
		return tell(me, msg);
	}

	if( sscanf(arg, "%s %s", var, val) != 2 )
		return tell(me, "指令格式錯誤，請參考 help set 以取得更多的使用資訊。\n");

	if( var == "-d" )
	{
		var = val;
		val = "0";
	}

	if( undefinedp(var_data = ENV_VARS[var]) )
		return tell(me, "沒有 " + var + " 這種環境變數。\n");

	if( var_data[VAR_LEVEL] == LV_WIZARD && !wizardp(me) )
		return tell(me, "沒有 " + var + " 這種環境變數。\n");

	if( val != "0" && !regexp(val, var_data[VAR_REGEXP]) )
		return tell(me, var + " 環境變數不允許使用 " + val + " 這個值。\n" );

	switch(var_data[VAR_TYPE])
	{
		case TYPE_BOOLEAN:
			val = to_int(val);
			val = (val ? 1:0);
			if( !val ) map_delete(myvars, var);
			else myvars[var] = val;
			val = (val ? "是":"否");
			break;
		case TYPE_INTEGER:
			val = to_int(val);
			if( !val ) map_delete(myvars, var);
			else myvars[var] = val;
			break;
		case TYPE_STRING:
			val = sprintf("%s", val);
			if( val == "0" ) map_delete(myvars, var);
			else myvars[var] = val;
			break;
		default:break;
	}

	if( !sizeof(myvars) ) myvars = 0;
	set("env", myvars, me);

	return tell(me, "環境變數 " + var + " = " + val + " 設定完成。\n");
}
 
