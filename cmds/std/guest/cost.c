/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : cost.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-28
 * Note   : cost 指令
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>
#include <message.h>

#define COST_TRIM       0
#define EXPT_TRIM       -40
inherit COMMAND;

string help = @HELP
        標準 cost 指令。
HELP;

private void command(object me, string arg)
{
	mapping sinfo, einfo;
	int i, cost, time_exp, count = 1;
	float etime, utime, stime;
	string msg;

	if( !is_command() ) return;
	if( !arg ) return tell(me, "指令格式﹕cost <指令>\n");
	sscanf(arg, "%d %s", count, arg);

	me = this_player(1);
	sinfo = rusage();
	cost = eval_cost();
	while(i < count) {
		time_exp += time_expression
		{
			me->force_me(arg);
		};
		// 此 40 為 force_me()中固定的消耗
		time_exp += EXPT_TRIM;
		cost += COST_TRIM;
		i++;
	}       
	cost -= eval_cost();
	einfo = rusage();
	utime = to_float(einfo["utime"]-sinfo["utime"])/1000000;
	stime = to_float(einfo["stime"]-sinfo["stime"])/1000000;
	etime = to_float(time_exp + EXPT_TRIM)/1000000;
	msg  = sprintf("執行 %d 次平均值：\n", count);
	msg += sprintf("效率評估: %.2f\n", to_float(cost)/count);
	msg += sprintf("系統時間: %.6f s\n", stime/count);
	msg += sprintf("使用時間: %.6f s\n", utime/count);
	msg += sprintf("運算時間: %.6f s\n", etime/count);
	tell(me, msg);
}

