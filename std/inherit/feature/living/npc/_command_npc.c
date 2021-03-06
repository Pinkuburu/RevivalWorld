/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _command_npc.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-4
 * Note   : 電腦生物指令系統
 * Update :
 *  o 2001-07-08 Clode 整理 action 程式碼
 *
 * P.S Action 之 fetch_variable 需重寫
 -----------------------------------------
 */

#include <daemon.h>
#include <message.h>
#include <function.h>

private nomask void initialize_actions();
private nomask void do_action(mapping fit_actions, string verb, string arg);
private void evaluate_command(function command_fp, string verb, string arg);

private void evaluate_command(function command_fp, string verb, string arg)
{
	if( functionp(command_fp) & FP_OWNER_DESTED )
		return tell(this_object(), "「"+verb+"」之指令函式指標擁有者已被摧毀，請通知巫師處理。\n"); 
		
	evaluate(command_fp, this_object(), arg);
}

private nomask int process_command(string verb, string arg)
{
	function command_fp;
	mapping all_actions;
	object ob;
	object env = environment();
	object *env_action_ob;
	mapping inv_ob_actions;
	
	all_actions = this_object()->query_actions() || allocate_mapping(0);

	if( objectp(env) )
	{
		env_action_ob = present_objects(this_object());

		if( !env->is_maproom() )
			env_action_ob += ({ env });
		
		// 特殊出口指令簡化
		if( !arg && query("exits/"+verb, env) )
		{
			arg = verb;
			verb = "go";
		}
	}

	if( !mapp(all_actions[verb]) )
		all_actions[verb] = allocate_mapping(0);

	/* 將所有 actions 整合起來 */
	if( sizeof(env_action_ob) )
	foreach( ob in env_action_ob )
		if( !userp(ob) && mapp(inv_ob_actions = ob->query_actions()) && functionp(inv_ob_actions[verb]) )
			all_actions[verb] += ([ ob : inv_ob_actions[verb] ]);
	
	// 執行 Action
	if( sizeof(all_actions[verb]) )
	{
		do_action(all_actions[verb], verb, arg);
		return 1;
	}
	// 執行系統指令
	else if( functionp(command_fp = COMMAND_D->find_npc_command(verb)) )
	{	
		evaluate_command(command_fp, verb, arg);
		return 1;
	}
	else 
		return 0;
}

void do_command(string cmd)
{
	string verb;
	string args;
	
	if( !cmd || !cmd[0] || query_temp("disable/cmd") ) return;
	
	if( sscanf(cmd, "%s %s", verb, args) != 2 )
		verb = cmd;
	
	set_this_player(this_object());

	if( !process_command(verb, args) )
	{
		process_command("say", "抱歉，我不清楚「"+cmd+"」該怎麼做。");
	}
}