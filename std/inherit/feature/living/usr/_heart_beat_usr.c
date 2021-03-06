/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _heart_beat_usr.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-8-26
 * Note   : 使用者心跳
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <message.h>

private nosave int heartbeat_tick = random(2048);

private void delay_decay();
private void process_heartbeat_command_decay();

// Status 變化
private nomask void process_heartbeat_status()
{
	mapping status = query_temp("status");

	if( !mapp(status) || !sizeof(status) ) return;

	foreach(string key, int time in status)
	{
		if( time == -1 ) continue;

		if( --time <= 0 )
			map_delete(status, key);
		else
			status[key] = time;
	}

	if( !sizeof(status) )
		delete_temp("status");
	else
		set_temp("status", status);
}

// Condition 變化
private nomask void process_heartbeat_condition()
{
	mapping condition;

	if( mapp(condition = query("condition")) )
	{
		object me = this_object();
		
		foreach( string key, mapping value in condition )
		{
			if( value["time"] <= 0 )
			{
				catch(replace_string(key, "#","/")->stop_effect(this_object()));
				delete("condition/"+key);
				
				// 檢查是否爆表
				if( me->query_stamina_cur() > me->query_stamina_max() )
					me->set_stamina_full();
				if( me->query_health_cur() > me->query_health_max() )
					me->set_health_full();
				if( me->query_energy_cur() > me->query_energy_max() )
					me->set_energy_full();

				continue;
			}

			if( value["heart_beat"] > 0 && !(value["time"] % value["heart_beat"]) )
				catch(replace_string(key, "#","/")->heart_beat_effect(this_object()));

			addn("condition/"+key+"/time", -1);
		}
	}	
}

// 每 1 秒呼叫一次
nomask void heart_beat()
{
	object me = this_object();

	heartbeat_tick++;

	// 每秒處理一次 Condition
	process_heartbeat_condition();

	// 每秒處理一次 Status
	process_heartbeat_status();

	// 每秒處理一次 Delay Decay
	delay_decay();
	
	// 每秒處理一次 Command Decay
	process_heartbeat_command_decay();

	// 每兩秒
	if( !((heartbeat_tick+1) % 2) )
	{
		if( !COMBAT_D->in_fight(me) )
		{		
			if( !me->is_stamina_full() )
				me->earn_stamina(me->stamina_regen());
			if( !me->is_health_full() )
				me->earn_health(me->health_regen());
			if( !me->is_energy_full() )
				me->earn_energy(me->energy_regen());
		}

		// 若在昏倒狀態	
		if( query("faint") )
		{
			float recover_speed = 1 + me->query_skill_level("consciousness")/100.;
			
			string msg = "";
			
			msg += "體力 "+NOR GRN+me->query_stamina_cur()+NOR"/"HIG+me->query_stamina_max()+NOR" ";
			msg += "生命 "+NOR YEL+me->query_health_cur()+NOR"/"HIY+me->query_health_max()+NOR" ";
			msg += "精神 "+NOR BLU+me->query_energy_cur()+NOR"/"HIB+me->query_energy_max()+NOR;
	
			// 閃避 block msg
			delete_temp("disable/msg");
			tell(me, sprintf(HIW"☆"NOR"☆"WHT"☆ %s "NOR WHT"☆"NOR"☆"HIW"☆\n"NOR, msg));
			set_temp("disable/msg",1);
				
			if( me->query_stamina_cur()*recover_speed >= me->query_stamina_max()/2 )
				me->wakeup();
		}

		if( me->query_food_cur() > 0 )
			me->cost_food(1);
		if( me->query_drink_cur() > 0 )
			me->cost_drink(1);
	}
	
	// 每六秒
	if( !((heartbeat_tick+2) % 6) )
	{
		if( interactive(me) && query_idle(me) > 300 )
		{
			if( !query_temp("startidle") )
			{
				tell(me, pnoun(2, me)+"已經連續發呆超過五分鐘，將開始累積發呆技能經驗值。\n");
				set_temp("startidle", 1);
			}
			me->add_skill_exp("idle", pow(query_idle(me), 0.3));
		}
		else if( query_temp("startidle") ) 
			delete_temp("startidle");
	}
	
	// 防斷線機制(每 60 秒送出一次 NOR 色碼)
	if( !((heartbeat_tick+3) % 60) )
	{
		receive(NOR);
	}

	// 遊戲時間 1 小時(即實際時間 120 秒)紀錄一次年齡
	if( !((heartbeat_tick+4) % 120) )
	{
		if( addn("age_hour", 1) == 8760 )
			BIRTHDAY_D->grow_up(me);
	}

	
	// 600 秒儲存一次玩家檔案, 
	if( !((heartbeat_tick+5) % 600) )
	{
		object labor_ob;
		string labor, *labors = query("hirelabors");
		string mailfile = read_file(user_mail(me->query_id(1)));
		
		addn("total_online_time", time() - query("last_on/time"));

		// 紀錄最後一次在線上的時間, 根據此資料由 CLEAN_D 處理帳號刪除
		set("last_on/time", time());

		if( environment() )
			set("quit_place", copy(query_temp("location")) || base_name(environment()));

		me->save();
		
		if( sizeof(labors) )
		foreach(labor in labors)
			if( file_exists(labor) && objectp(labor_ob = load_object(labor)) )
				labor_ob->save();
		
		//檢查是否有個人信件
		if( stringp(mailfile) )
		{
			int unread;
			mapping maildata = restore_variable(mailfile);
			
			if( mapp(maildata) )
			foreach(int time, mapping mail in maildata)
				if( undefinedp(mail["read"]) )
					unread++;
			
			if( unread > 0 )
				tell(me, HIG"信"NOR GRN"件"NOR"："+pnoun(2, me)+"共有 "+unread+" 封信件未讀，請到政府開設的郵局閱讀信件。\n");
		}
	}
	
	// 每一小時更新一次排行榜紀錄
	if( !((heartbeat_tick+6) % 3600) )
		TOP_D->update_top_rich(me);

	// 每一小時更新一次排行榜紀錄
	if( !((heartbeat_tick+7) % 3600) )
		TOP_D->update_top_social_exp(me);
		
	// 每三小時更新一次排行榜紀錄
	if( !((heartbeat_tick+8) % 10800) )
		TOP_D->update_top_skill(me);		
	
}

nomask void remove()
{
	CHANNEL_D->remove_user(this_object());
}
