/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : enterprise.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-04-06
 * Note   : 企業資訊
 * Update :
 *  o 2003-00-00  
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <feature.h>
#include <daemon.h>

inherit COMMAND;

#define REGISTER_ENTERPRISE_MONEY	"10000000"

string help = @HELP
企業管理指令，用法如下：
  enterprise			- 列出目前所有企業資訊
  enterprise 企業編號		- 查詢某企業的詳細資料
  enterprise register 名稱	- 花費 $RW 10,000,000 登記註冊國際企業集團(名稱可使用色彩)
  enterprise unregister 名稱	- 註銷解散國際企業集團(總裁指令)
  enterprise invite 'ID'	- 邀請其他玩家進入企業集團(總裁指令)
  enterprise kickout 'ID'	- 將某位企業成員踢出企業集團(總裁指令)
  enterprise join		- 接受邀請進入企業集團
  enterprise leave		- 離開企業組織
HELP;

private void do_command(object me, string arg)
{
	int strlen, i;
	string enterprise_color_id, enterprise_id;
	string inviteid, kickoutid;
	string myid = me->query_id(1);
	string myenterprise = query("enterprise", me);
	string *enterprise_stockvalue_sort;
	if( !arg )
	{
		
		string msg = "";
		msg += "   企業名稱            總裁           成員    股價     成員資產總值\n";
		msg += WHT"─────────────────────────────────────\n"NOR;
		
		enterprise_stockvalue_sort = sort_array(ENTERPRISE_D->query_all_enterprises(), (: ENTERPRISE_D->query_enterprise_info($2, "stockvalue") > ENTERPRISE_D->query_enterprise_info($1, "stockvalue") ? 1 : -1 :));
		
		foreach(enterprise_id in ENTERPRISE_D->query_all_enterprises())
		{	
			msg += sprintf("%-3s%-20s"HIM"%-15s"HIG"%-4d"HIC"%8.2f"NOR RED"("HIR"%-2d"NOR RED") "HIY"%s\n"NOR
				,++i+"."
				,ENTERPRISE_D->query_enterprise_color_id(enterprise_id)
				,capitalize(ENTERPRISE_D->query_enterprise_info(enterprise_id, "president"))
				,sizeof(ENTERPRISE_D->query_enterprise_info(enterprise_id, "member"))
				,ENTERPRISE_D->query_enterprise_info(enterprise_id, "stockvalue")+0.
				,member_array(enterprise_id, enterprise_stockvalue_sort)+1
				,money("RW", ENTERPRISE_D->query_enterprise_allmembers_assets(enterprise_id)));
		}
		msg += WHT"─────────────────────────────────────\n"NOR;
		msg += "輸入 enterprise '企業編號' 查詢細節資料\n";
		return tell(me, msg);	
	}

	if( (i = to_int(arg)) )
	{
		array estates;
		string president, msg, membermsg;
		string *allenterprises = ENTERPRISE_D->query_all_enterprises();
		
		if( i<=0 || i>sizeof(allenterprises) )
			return tell(me, "輸入編號錯誤。\n");
		
		arg = allenterprises[i-1];

		estates = ESTATE_D->query_player_estate("ENTERPRISE/"+arg);
		president = ENTERPRISE_D->query_president(arg);
		msg = "「"+ENTERPRISE_D->query_enterprise_color_id(arg)+"」企業集團的詳細資料如下：\n";
		membermsg = "";

		msg += WHT"─────────────────────────────────────\n"NOR;
		msg += "總裁     "HIW+(president ? capitalize(president) : "無")+NOR"\n";
		msg += "成立時間 "HIC+TIME_D->replace_ctime(ENTERPRISE_D->query_enterprise_info(arg, "registertime"))[0..9]+NOR+"\n";
		msg += "企業資產 "HIY+money("RW", ENTERPRISE_D->query_enterprise_assets(arg))+"\n"NOR;
		msg += "可用資金 "HIY+money("RW", ENTERPRISE_D->query_money(arg))+"\n"NOR;
		msg += "企業成員\n"HIG"  名稱         城市        企業投資            個人資產            加入時間\n"NOR;

		foreach(string memberid, mapping data in ENTERPRISE_D->query_enterprise_info(arg, "member"))
		{
			if( memberid == president )
				membermsg = sprintf("  %-13s%-12s%-20s%-20s%s\n"NOR, capitalize(memberid), CITY_D->query_city_id(CITY_D->query_player_city(memberid)), NUMBER_D->number_symbol(data["invest"]), NUMBER_D->number_symbol(MONEY_D->query_lazy_assets(memberid)), TIME_D->replace_ctime(data["jointime"])[0..9]) + membermsg;
			else
				membermsg += sprintf("  %-13s%-12s%-20s%-20s%s\n"NOR, capitalize(memberid), CITY_D->query_city_id(CITY_D->query_player_city(memberid)), NUMBER_D->number_symbol(data["invest"]), NUMBER_D->number_symbol(MONEY_D->query_lazy_assets(memberid)), TIME_D->replace_ctime(data["jointime"])[0..9] );
		}

		msg += membermsg;
		msg += "\n"+("/cmds/std/ppl/estate"->show_estate("ENTERPRISE/"+arg, estates, ENTERPRISE_D->query_enterprise_color_id(arg), 1));

		msg += WHT"─────────────────────────────────────\n"NOR;
		return tell(me, msg);
	}
	
	if( sscanf(arg, "register %s", enterprise_color_id) )
	{
		strlen = noansi_strlen(enterprise_color_id);
		if( strlen < 4 || strlen > 14 )
			return tell(me, "企業名稱字數必須介於 4 - 14 個字元。\n");

		enterprise_id = replace_string(remove_ansi(enterprise_color_id), "\n", "");
		enterprise_color_id = kill_repeat_ansi(replace_string(enterprise_color_id, "\n", "")+NOR);
		
		if( ENTERPRISE_D->enterprise_exists(enterprise_id) )
			return tell(me, ENTERPRISE_D->query_enterprise_color_id(enterprise_id)+"已經存在，不能使用相同的企業名稱。\n");
		
		if( ENTERPRISE_D->in_enterprise(myid) )
			return tell(me, pnoun(2, me)+"已經加入"+ENTERPRISE_D->query_enterprise_color_id(ENTERPRISE_D->in_enterprise(myid))+"，無法再另外申請成立企業集團組織。\n");
	
		if( !me->spend_money(MONEY_D->query_default_money_unit(), REGISTER_ENTERPRISE_MONEY) )
			return tell(me, "登記註冊需支付 $"+MONEY_D->query_default_money_unit()+" "+NUMBER_D->number_symbol(REGISTER_ENTERPRISE_MONEY)+"。\n");
	
		ENTERPRISE_D->register_enterprise(enterprise_id, enterprise_color_id, myid);
	
		CHANNEL_D->channel_broadcast("news", me->query_idname()+"登記成立了國際企業集團「"+ENTERPRISE_D->query_enterprise_color_id(enterprise_id)+"」。");
		
		set("enterprise", enterprise_id, me);
		
		me->save();
	
		return tell(me, pnoun(2, me)+"花費 $"+MONEY_D->query_default_money_unit()+" "+NUMBER_D->number_symbol(REGISTER_ENTERPRISE_MONEY)+" 登記成立了國際性企業集團「"+ENTERPRISE_D->query_enterprise_color_id(enterprise_id)+"」。\n");
	}
	else if( sscanf(arg, "unregister %s", enterprise_color_id) )
	{
		enterprise_id = replace_string(remove_ansi(enterprise_color_id), "\n", "");
		
		if( !ENTERPRISE_D->enterprise_exists(enterprise_id) )
			return tell(me, "這個企業並不存在。\n");
			
		if( !wizardp(me) && !ENTERPRISE_D->is_president(enterprise_id, myid) )
			return tell(me, pnoun(2, me)+"並不是"+ENTERPRISE_D->query_enterprise_color_id(enterprise_id)+"的總裁。\n");
		
		CHANNEL_D->channel_broadcast("news", me->query_idname()+"宣布解散國際企業集團「"+ENTERPRISE_D->query_enterprise_color_id(enterprise_id)+"」。");
		tell(me, pnoun(2, me)+"宣布解散國際性企業集團「"+ENTERPRISE_D->query_enterprise_color_id(enterprise_id)+"」。\n");
		
		ENTERPRISE_D->unregister_enterprise(enterprise_id);
		return;
	}
	else if( sscanf(arg, "invite %s", inviteid) )
	{
		object target;
		
		if( !objectp(target = find_player(inviteid)) )
			return tell(me, "目前線上並沒有 "+inviteid+" 這位玩家。\n");
		
		if( !myenterprise )
			return tell(me, pnoun(2, me)+"並未組成任何企業。\n");

		if( !ENTERPRISE_D->is_president(myenterprise, myid) )
			return tell(me, pnoun(2, me)+"並不是"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"的總裁，無法邀請成員。\n");

		if( ENTERPRISE_D->in_enterprise(inviteid) )
			return tell(me, target->query_idname()+"已經是"+ENTERPRISE_D->query_enterprise_color_id(ENTERPRISE_D->in_enterprise(inviteid))+"的成員。\n");
		
		if( !same_environment(me, target) )
			return tell(me, target->query_idname()+"必須來到此地才能邀請。\n");

		CHANNEL_D->channel_broadcast("ent", me->query_idname()+"邀請"+target->query_idname()+"成為企業集團「"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"」的成員。", me);
		
		msg("\n$ME邀請$YOU成為國際企業集團「"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"」的一員。\n", me, target, 1);
		set_temp("enterprise_invite", myenterprise, target);
		tell(me, "正在等候"+target->query_idname()+"的回應。\n");
		tell(target, "若願意加入此企業請輸入 enterprise join。\n");
		return;
	}
	else if( sscanf(arg, "kickout %s", kickoutid) )
	{
		object target = load_user(kickoutid);

		if( !myenterprise )
			return tell(me, pnoun(2, me)+"並未組成任何企業。\n");

		if( !ENTERPRISE_D->is_president(myenterprise, myid) )
			return tell(me, pnoun(2, me)+"並不是"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"的總裁，無法邀請成員。\n");

		if( !ENTERPRISE_D->in_enterprise(kickoutid) )
			return tell(me, target->query_idname()+"原本就不是"+ENTERPRISE_D->query_enterprise_color_id(ENTERPRISE_D->in_enterprise(inviteid))+"的成員。\n");
		
		CHANNEL_D->channel_broadcast("ent", me->query_idname()+"將"+target->query_idname()+"踢出企業集團。", me);
		
		tell(target, pnoun(2, target)+"被企業總裁"+me->query_idname()+"開除了成員資格。\n");
		ENTERPRISE_D->unregister_member(kickoutid);

		delete("enterprise", target);
		target->save();
		
		if( !userp(target) )
			destruct(target);

		return;
	}
	else if( arg == "leave" )
	{
		if( !myenterprise )
			return tell(me, pnoun(2, me)+"並未加入任何企業。\n");
		
		if( ENTERPRISE_D->is_president(myenterprise, myid) )
			return tell(me, pnoun(2, me)+"是"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"的總裁，無法擅自離開企業。\n");

		CHANNEL_D->channel_broadcast("ent", me->query_idname()+"註銷了企業集團「"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"」的成員資格。", me);
		tell(me, pnoun(2, me)+"註銷了企業集團「"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"」的成員資格。\n");
		delete("enterprise", me);
		me->save();
		ENTERPRISE_D->unregister_member(myid);
		return;
	}
	else if( arg == "join" )
	{
		myenterprise = query_temp("enterprise_invite", me);
		
		if( !myenterprise )
			return tell(me, pnoun(2, me)+"尚未收到任何企業的邀請。\n");
		
		delete_temp("enterprise_invite", me);
			
		if( ENTERPRISE_D->in_enterprise(myid) )
			return tell(me, pnoun(2, me)+"已經是"+ENTERPRISE_D->query_enterprise_color_id(ENTERPRISE_D->in_enterprise(myid))+"的成員。\n");

		if( !ENTERPRISE_D->enterprise_exists(myenterprise) )
			return tell(me, pnoun(2, me)+"欲加入的企業已經解散。\n");
		
		set("enterprise", myenterprise, me);
		me->save();

		ENTERPRISE_D->register_member(myenterprise, myid);
		CHANNEL_D->channel_broadcast("ent", me->query_idname()+"接受邀請成為企業集團「"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"」的成員。", me);
		tell(me, pnoun(2, me)+"接受邀請成為企業集團「"+ENTERPRISE_D->query_enterprise_color_id(myenterprise)+"」的成員。\n");

		return;
	}

	return tell(me, "請輸入正確的企業管理指令格式，請參考 help enterprise。\n");
}