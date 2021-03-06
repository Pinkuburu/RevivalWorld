/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : wiz_login_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2000-12-29
 * Note   : 巫師登錄精靈
 * Update :
 *  o 2001-02-09 Clode 刪除 port 分法, 改進一些程式寫法
 *  o 2001-07-20 Clode 重寫 repeat login id 的檢查
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <login.h>
#include <daemon.h>
#include <object.h>
#include <secure.h>
#include <mudlib.h>
#include <message.h>

#define INITIALIZE		1
#define INPUT_ID		2
#define INPUT_PASSWORD		3
#define RECONNECT		4
#define OLD_PLAYER		5
#define ENTER_GAME		6

private nomask varargs void logon_handle(int state, object ob, string arg)
{
	ob->reset_time_out(); // 重設Time_out時間

	if( arg && query("encode/gb", ob) ) arg = G2B(arg);
	
	switch(state)
	{
	/* 啟始 Login */
	case INITIALIZE:
		{
			ob->directly_receive("Current charset is " + (query("encode/gb", ob) ? "Simplified Chinese (GB)":"Traditional Chinese (BIG5)") + "\n");
			ob->directly_receive("Please input GB/BIG5 to change charset or directly login user.\n");

			tell(ob, "您的巫師英文名字：", CLIENTIGNORE);
			input_to( (: logon_handle, INPUT_ID, ob :) );

			return;
		}
		
	/* 輸入 ID */
	case INPUT_ID:
		{	
			int level_num;
			
			if( !arg || arg == "" )
				return logon_handle(INITIALIZE, ob);
			
			arg = remove_ansi(remove_fringe_blanks(lower_case(arg)));
			
			if( arg == "gb" )
			{
				set("encode/gb", 1, ob);
				return this_object()->logon(ob);
			}
			else if( arg == "big5" )
			{
				delete("encode/gb", ob);
				return this_object()->logon(ob);
			}

			if( file_size(user_data(arg)) <= 0 )
			{
				tell(ob, "本遊戲沒有 "+arg+" 這位巫師。\n", CONNECTFAILED);
				return logon_handle(INITIALIZE, ob);
			}
			
			level_num = SECURE_D->level_num(arg);
			
			if( level_num < GUEST )
			{
				tell(ob, HIG"只有巫師才能從連接埠 "+WIZ_PORT+" 登入，玩家請從 "+PPL_PORT+" 登入。\n"NOR, CONNECTFAILED);
				return logon_handle(INITIALIZE, ob);
			}
			
			if( level_num < LOGIN_D->query_wiz_lock() )
			{
				tell(ob, HIY"系統更新維護中，目前僅允許權限在 "+SECURE_D->level_num_to_level_name(LOGIN_D->query_wiz_lock())+" 以上的使用者連線。\n"NOR, CONNECTFAILED);
				return logon_handle(INITIALIZE, ob);
			}
			
			if( LOGIN_D->is_repeat_login(arg) )
			{
				tell(ob, "抱歉您輸入的 "+capitalize(arg)+" 正在被登錄中，請勿重覆登錄。\n", CONNECTFAILED);
				return logon_handle(INITIALIZE, ob);
			}
			else
				set("id", arg, ob);

			tell(ob, "請輸入密碼：", CLIENTIGNORE);
			input_to( (: logon_handle, INPUT_PASSWORD, ob :), 1 );
			return;           
		}

	/* 輸入密碼 */
	case INPUT_PASSWORD:
		{
			string password = PASSWORD_D->query_password(query("id", ob));

			if( !arg || crypt(arg, password) != password )
			{
				tell(ob, "密碼輸入錯誤。\n", CONNECTFAILED);
					
				if( LOGIN_D->input_wrong_password(ob) >= 3 )
				{
					tell(ob, "你已經輸入錯誤密碼三次，請重新登入再試。\n", CONNECTFAILED);
					destruct(ob); return;
				}
				
				tell(ob, "請輸入正確密碼：", CLIENTIGNORE);
				input_to( (: logon_handle, INPUT_PASSWORD, ob :), 1 ); return;
			}

			// 若在遊戲中找到相同 ID 使用者物件
			if( find_player(query("id", ob)) )
			{
				set("player", find_player(query("id", ob)), ob);
				
				// 若為互動物件, 則詢問是否取代
				if( interactive(query("player", ob)) )
				{
					tell(ob, "你想取代目前在遊戲中相同的人物嗎(Y/N)？\n", CMDMSG);
					input_to( (: logon_handle, RECONNECT, ob :) );return;
				}
				// 直接重新連線取代物件
				else
					logon_handle(RECONNECT, ob, "y");return;
			}

			/* 密碼檢查正確, 進入遊戲 */
			return logon_handle(OLD_PLAYER, ob);
		}

	/* 舊玩家登錄 */
	case OLD_PLAYER:
		{
			object user_ob = load_user(query("id", ob));
			
			if( !objectp(user_ob) )
			{
				object login_ob = new(LOGIN_OB);
				
				tell(ob, "無法載入 "+capitalize(query("id", ob))+" 的儲存檔，請聯絡巫師處理。\n", CONNECTFAILED);
				exec(login_ob, ob);
				destruct(ob);
				logon_handle(INITIALIZE, login_ob);return;
			}

			set_temp("login/ident", query("ident", ob), user_ob);
			set_temp("login/remoteport", query("remote_port", ob), user_ob);
			set_temp("login/terminaltype", query("terminaltype", ob), user_ob);
			set_temp("windowsize", query("windowsize", ob) || DEFAULT_WINDOW_SIZE, user_ob);

			if( query("encode/gb", ob) )
				set("encode/gb", 1, user_ob);
			else
				delete("encode/gb", user_ob);

			// 由 LOGIN_OB 切換至 USER_OB
			exec( user_ob , ob );
			destruct(ob);

			return logon_handle(ENTER_GAME, user_ob);
		}

	/* 重新連線 */
	case RECONNECT:
		{
			object reconnect_ob;
			
			if( !arg || remove_ansi(remove_fringe_blanks(lower_case(arg))) != "y" )
			{
				tell(ob, "您決定不取代遊戲中相同的人物。\n", CONNECTFAILED);
				ob->reset_database();
				
				logon_handle(INITIALIZE, ob);return;
			}

			if( !objectp(reconnect_ob = query("player", ob)) )
			{
				tell(ob, "原本遊戲中的人物已經消失，請重新登入。\n", CONNECTFAILED);
				ob->reset_database();
				logon_handle(INITIALIZE, ob);return;
			}
				
			if( !interactive(reconnect_ob) )
				reconnect_ob->reconnect();
			else
			{
				object temp = new(VOID_OB);
				
				tell(reconnect_ob, "另外一位使用者自 "+query_ip_name(ob)+" 連線取代了你。\n", ETCMSG);
				
				/* 先將原本的玩家轉換至一個暫時 USER_OB 再將之摧毀 */
				exec(temp, reconnect_ob);
				destruct(temp);
			}

			set_temp("reconnect", 1, reconnect_ob);
			set_temp("login/ident", query("ident", ob), reconnect_ob);
			set_temp("login/remoteport", query("remote_port", ob), reconnect_ob);
			set_temp("windowsize", query("windowsize", ob) || DEFAULT_WINDOW_SIZE, reconnect_ob);
			set_temp("login/terminaltype", query("terminaltype", ob), reconnect_ob);

			if( query("encode/gb", ob) )
				set("encode/gb", 1, reconnect_ob);
			else
				delete("encode/gb", reconnect_ob);

			/* 取代舊 user_ob */
			exec( reconnect_ob , ob );
			destruct(ob);

			return logon_handle(ENTER_GAME, reconnect_ob);
		}

	/* 進入遊戲 */
	case ENTER_GAME:
		{	
			// 進入遊戲
			LOGIN_D->enter_game(ob);
			return;

		}/* ENTER_GAME */

	default: return logon_handle(INITIALIZE, ob);
	
	}/* END SWITCH */
}

void wait_for_login(object login_ob, string arg)
{
	tell(login_ob, "載入中，請稍後 ...\n", CMDMSG);
	input_to( (: wait_for_login, login_ob :) );
}
nomask void logon(object login_ob)
{
	/* 載入系統 */
	if( !SYSTEM_D->valid_login() )
	{
		tell(login_ob, HIY + MUD_FULL_NAME + "載入中，請稍後 ...\n"NOR, CMDMSG);
		SYSTEM_D->add_loginuser( login_ob );
		input_to( (: wait_for_login, login_ob :) );
		return;
	}

	if( !LOGIN_D->check_login_attacker(query_ip_number(login_ob)) )
	{
		login_ob->directly_receive(HIR"\n很抱歉，這個 IP["+query_ip_number(login_ob)+"] 的登入動作太過頻繁，請稍候再試。\n"NOR);
		CHANNEL_D->channel_broadcast("sys", "登入："+query_ip_number(login_ob)+" 的登入動作太過頻繁");
		destruct(login_ob);
		return;
	}

	LOGIN_D->show_login_message(login_ob);
	logon_handle(INITIALIZE, login_ob);
}
int remove()
{
	foreach(object user in users())
	{
		if( !user->is_login_ob() ) continue;
		tell(user, HIR"\n很抱歉，登錄系統更新，麻煩您再重新登錄。\n"NOR, CONNECTFAILED);
		flush_messages(user);
		destruct(user);
	}
}

string query_name()
{
	return "巫師登入系統(WIZ_LOGIN_D)";
}
