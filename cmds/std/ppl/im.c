/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : im.c
 * Author : Whatup@RevivalWorld
 * Date   : 2002-02-01
 * Note   : Im 指令。
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */
#define IM_D "/system/daemons/im_d.c"

#include <feature.h>
#include <message.h>

inherit COMMAND;

void who_list(object me);
void register(object me,string pro);
void help(object me);

string help = @HELP

指令格式 : im [各項參數]
            
    這個指令可以讓你登入網路上流行的即時通訊軟體。目前支援的         
即時通訊軟體有 MSN 、 YAHOO 這二項。

    不管你是第幾次使用，要使用 im 時，必需先打 im login 作
登入的動作。

    因為網路傳輸的關系，有些訊息可能會延遲一些時間才會收到。
     
     im login           - 登入 im 系統。
     im quit            - 登出 im 系統。
     im who             - 列出你的好友名單。
     im register msn    - 註冊 msn 帳號。
     im register yahoo  - 註冊 yahoo 帳號。
     im add <protocol> <account> - 新增好友。
     im remove <account>- 移除好友。
     im no              - 不同意他人把你加入好友名單。
     im yes             - 同意他人把你加入好友名單。
     im nick <nick>     - 修改 msn 的暱稱。
     im tell <id> <msg> - 傳訊給某人。

HELP;

private void command(object me, string arg)
{
	string who,msg;

	if( !is_command() ) return;

	if(!arg) return tell( me, help);

	if(arg == "login") 
	{
		if( query_temp("im_fd", me) 
		&& IM_D->query_users()[query_temp("im_fd", me)]["obj"] == me)	
			return tell("\n[IM Message]:你正在登入中，請用 im quit 離線。\n");

		tell( me, "\n[IM Message]:開始登入即時通訊精靈。\n");
		IM_D->login_irc(me);
		return;
	}

	if(!undefinedp(query_temp("im_fd", me))) 
	{
		if(arg == "who")
		{ 
			who_list(me);
			return ;
		}
		
		if(arg == "quit") 
		{
                        IM_D->send_command(query_temp("im_fd", me),"QUIT\r\n");
			delete_temp("im_fd", me);
			tell( me, "\n[IM Message]:即時通訊精靈離線了。\n");
			return;
		}

		if(sscanf(arg,"tell %s %s",who,msg) == 2)
			return IM_D->process_send_msg(me,who,msg);

		if(arg == "register msn") 
		{	
			register(me,"MSN");
			return;
		}
		
		if(arg == "register yahoo") 
		{
			register(me,"YAHOO");
			return;
		}
		
		if(sscanf(arg,"add %s %s",who,msg) == 2)
		{
			if(who != "msn" && who != "yahoo")
				return tell( me, "\n[IM Message]:目前只能加入 msn 或是 yahoo 的好友名單。\n");

			IM_D->send_command(query_temp("im_fd", me), "PRIVMSG #bitlbee :add "+who+" "+msg);

			return tell( me, "\n[IM Message]:你把 "+msg +" 加入你的 "+who +" 好友名單。\n");
		}

		if(sscanf(arg,"remove %s",who) == 1)
		{
			int fd = query_temp("im_fd", me);

			if(!undefinedp(IM_D->query_users()[fd]["list"]) 
			&& !undefinedp(IM_D->query_users()[fd]["list"][who]))
			{
				IM_D->send_command(fd,  "PRIVMSG #bitlbee :remove "+who);
				return tell( me, "\n[IM Message]:你把 "+who +" 從你的好友名單刪除。\n");
			}
			return tell( me, "\n[IM Message]:沒有這個人哦！\n");
		}

		if(arg == "yes") 
		{
			IM_D->send_command(query_temp("im_fd", me), "PRIVMSG #bitlbee : yes");
			return tell( me, "\n[IM Message]:你同意他把你加入他的好友名單。\n");
		}

		if(arg == "no")
		{
			IM_D->send_command(query_temp("im_fd", me),  "PRIVMSG #bitlbee : no");
			return tell( me, "\n[IM Message]:你不同意他加你進入他的好友名單。\n");
		}

		if(sscanf(arg,"nick %s",msg))
		{
			tell( me, "\n[IM Message]:你將名字改為："+msg+"。\n");
			IM_D->send_command(query_temp("im_fd", me),  "PRIVMSG #bitlbee :nick msn \""+msg+"\"");
			return;
		}
		tell( me,"\n[IM Message]:沒有這項功能哦。\n");
	}
	else 
	return tell( me,"你並沒有登入\n");
	return;
}

void who_list(object me)
{
	mapping map = IM_D->query_users();
	tell( me,"好友名單列表：\n");

	foreach(int fd,mapping m in map)
	{
		if(m["obj"] == me && me->query_id(1) == m["id"] )
		{
			foreach(string k,string* v in m["list"])
			{
				if(v[2] == m["id"] || v[2] == "root") continue;
			tell( me, sprintf("(%-4s)[%-10s] %-70s \n",v[3]=="G"?"離開":"線上",v[2],v[4]+"("+v[0]+"@"+v[1]+")"));
			}
		}
	}
	return;
}

void register(object me,string pro)
{
	tell( me,"[IM Message] 請輸入你的 "+pro+" 帳號，或是按 . 取消:");
	input_to("register_step1",me, pro);
	return;
}

void register_step1(string arg,object me,string pro)
{
	if(arg[0] == '.') 
	{
		tell( me, "[IM Message] 取消設定。\n");
		return;
	}

	if(strlen(arg) < 5) 
	{
		tell( me, "[IM Message] 請輸入你的 "+pro+" 帳號，或是按 . 取消:");
		input_to("register_step1",me,pro);
	}

	tell( me, "[IM Message] 請輸入你的 "+pro+" 密碼，或是按 . 取消:");
	input_to("register_step2",me,pro,arg);
}

void register_step2(string arg,object me,string pro,string account)
{
	if(arg[0] == '.') 
	{
		tell( me, "[IM Message] 取消設定。\n");
		return;
	}

	if(strlen(arg) < 5) 
	{
		tell( me, "[IM Message] 請輸入你的 "+pro+" 密碼，或是按 . 取消:");
		input_to("register_step2",me,pro,account);
	}

	IM_D->del_account( query_temp("im_fd", me), pro);
	IM_D->register_account( query_temp("im_fd", me),pro,account,arg);
	tell( me, "[IM Message] "+pro+" 帳號新增或修改完畢。\n");
}
