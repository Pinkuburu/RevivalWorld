/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : channel_d.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-2-20
 * Note   : 頻道精靈
 * Update :
 *  o 2001-07-08 Cookys 修正 other channel 的錯誤一併加上不隨意送出
 *  o 2001-07-25 Cookys other 送出無法送出的頻道時會以[站內]顯示之。
 *
 -----------------------------------------
 */

#define CHAT 		(1<<0)	//閒聊
#define RUMOR		(1<<1)	//謠言
#define MUSIC		(1<<2)	//音樂
#define WIZ		(1<<3)	//巫師
#define SYS		(1<<4)	//系統
#define NCH		(1<<5)	//監測
#define NEWS		(1<<6)	//新聞
#define TWIZ		(1<<7)	//臺灣巫師
#define GWIZ		(1<<8)	//國際巫師
#define OTHER		(1<<9)	//其他
#define AD		(1<<10)	//廣告
#define CITY		(1<<11)	//城市
#define GOD		(1<<12)	//天神
#define REPORT		(1<<13)	//報告
#define ENT		(1<<14)	//企業
#define STOCK		(1<<15) //股票
#define AUC		(1<<16) //拍賣

#include <ansi.h>
#include <secure.h>
#include <daemon.h>
#include <message.h>
#include <gender.h>

#define DEFAULT_REFUSE  ( NCH | MUSIC | OTHER | AD )

class channel_class
{
	int level, number;
	string msg;
	mapping extra;
}

mapping channels;
mapping users;
nosave int water_number;

string help = @HELP
此為頻道指令，查詢頻道的使用方法請 help channels。
HELP;

private array info = 
({
    ({
	REPORT,	
	"report",
	GUEST, 
	RED "【"NOR HIR"回報"NOR RED"】"NOR, 	
	([
	    "time"		:	1, 
	    "msgcolor"	:	NOR HIR,
	]),
    }),
    ({ 
	GOD,
	"god",	
	ADMIN,  
	BLU "【"HIB"神話"NOR BLU"】"NOR, 
	([

	]),
    }),
    ({ 
	CHAT, 	
	"chat",
	PLAYER,
	CYN "【"HIC"閒聊"NOR CYN"】"NOR,
	([

	]),
    }),
    ({
	RUMOR,
	"rumor",
	PLAYER,
	MAG "【"HIM"謠言"NOR MAG"】"NOR,
	([
	    "wizlevel"	:	1,
	]) 
    }),
    ({ 
	MUSIC,
	"music",
	PLAYER,
	GRN "【"HIG"音樂"NOR GRN"】"NOR,
	([

	])
    }),
    ({ 
	WIZ, 
	"wiz",
	GUEST,
	YEL "【"HIY"巫師"NOR YEL"】"NOR,
	([

	]), 
    }),
    ({ 
	SYS,
	"sys",
	GUEST,
	WHT "【"HIW"系統"NOR WHT"】"NOR,
	([
	    "time"		:	1,
	    "msgcolor"	:	NOR WHT,
	]),
    }),
    ({ 
	NCH,
	"nch",
	GUEST,
	WHT "【"NOR YEL"監測"NOR WHT"】"NOR,
	([
	    "time"		:	1,
	    "msgcolor"	:	NOR YEL,
	]),
    }),
    ({ 
	NEWS,
	"news",
	PLAYER,
	GRN "【"HIG"新"NOR GRN"聞"HIG"】"NOR,
	([
	    "time"		:	1,
	    "notalk"	:	1,
	]),
    }),
    ({
	TWIZ,
	"twiz",
	WIZARD,
	WHT "【"NOR YEL"臺灣" NOR WHT"】"NOR,
	([
	    "intermud"	:	INTERMUD2_D,
	    "channel"	:	"twiz",
	]),
    }),
    ({ 
	GWIZ,
	"gwiz",
	WIZARD,
	WHT "【"HIW"網際%T"NOR WHT"】"NOR,
	([
	    "intermud"	:	INTERMUD2_D,
	    "channel"	:	"CREATOR",
	    "subchannel"	:	1,
	    "newtitle"	:	(: stringp($1) ? "-"+$1 : "" :),
	]),
    }),
    ({ 
	OTHER,
	"other",
	PLAYER,
	CYN "【"NOR WHT"未知%T"NOR CYN"】"NOR,
	([
	    "intermud"	:	INTERMUD2_D,
	    "channel"	:	"",
	    "subchannel"	:	1,
	    "newtitle"	:	(: stringp($1) ? ("-"+HIG+$1+NOR) : "" :),
	]),
    }),
    ({
	AD,
	"ad",
	PLAYER,
	WHT "【"NOR GRN"廣告" NOR WHT"】"NOR,
	([

	]),
    }),
    ({ 
	CITY,
	"city",
	PLAYER,
	HIW"【"NOR"%T"NOR HIW"】"NOR,
	([
	    "subchannel"	:	1,
	    "filter"	:	(: query("city",$1) == (stringp($2) ? $2 : query("city", $2)) || wizardp($1) :),
	    "newtitle"	:	(: CITY_D->query_city_name(stringp($1) ? $1 : query("city", $1))||"流浪遊民":),
	]),
    }),
    ({ 
	ENT,
	"ent",
	PLAYER,
	HIW"【"NOR"%T"NOR HIW"】"NOR,
	([
	    "subchannel"	:	1,
	    "filter"	:	(: query("enterprise",$1) == (stringp($2) ? $2 : query("enterprise", $2)) || wizardp($1) :),
	    "newtitle"	:	(: ENTERPRISE_D->query_enterprise_color_id(stringp($1) ? $1 : query("enterprise", $1))||"個體戶":),
	]),
    }),
    ({
	STOCK,
	"stock",
	PLAYER,
	YEL "【"NOR HIY"股"NOR YEL"市" NOR HIY"】"NOR,
	([
	    "time"		:	1,
	]),
    }),
    ({
	AUC,
	"auc",
	PLAYER,
	BLU "【"NOR HIB"拍賣" NOR BLU"】"NOR,
	([
	    "time"	:	1,
	    "notalk"	:	1,
	]),
    }),
});

nosave mapping replace_word = ([
    "幹你娘":	"******",
]);

int valid_channel(int level, string channel)
{
	foreach(array data in info)
	{
		if( data[1] == channel && level >= data[2] )
			return 1;
	}

	return 0;
}

varargs private void do_channel(object me, string channel, string msg, mixed raw)
{
	int emote,internal_flag;
	object *fit_users;
	class channel_class ch;
	string channel_title, idname, send_msg, time, my_id, my_idname, color, subchannel;

	if( !channel || channel == "" || !me ) return;

        if( query("ignore", me) )
		return tell( me, sprintf("您現在無法使用公用頻道, 請通知巫師。\n"));

	// 檢查是否為表情指令
	if( channel[<1] == '*' ) { emote = 1; channel = channel[0..<2]; }

	if( sscanf(channel, "gwiz-%s", subchannel) ) channel = "gwiz";

	if( !classp(ch = channels[channel]) ) return;

	if( channel == "city" && interactive(me) && !wizardp(me) && !query("city", me) )
		return tell(me, pnoun(2, me)+"並未加入任何城市。\n");
	else if( channel == "ent" && interactive(me) && !wizardp(me) && !query("enterprise", me) )
		return tell(me, pnoun(2, me)+"並未加入任何企業。\n");

	my_id = me->query_id(1);
	my_idname = me->query_idname();

	// 若有子頻道標記, 則分析子頻道名稱
	if( ch->extra["subchannel"] ) 
		if( msg && sscanf(msg, "[%s] %s", subchannel, msg) ) internal_flag=1;
		// 檢查本頻道之特殊替換頻道名
	if( ch->extra["newtitle"] )
	{

		channel_title = replace_string(ch->msg, "%T", evaluate(ch->extra["newtitle"], stringp(raw) ? raw : (wizardp(me)||previous_object(1)==find_object(INTERMUD2_D)) ? subchannel || me : me));
	}
	else
		channel_title = ch->msg;

	// 假如是使用者, 則檢查頻道註冊
	if( interactive(me) && !(ch->number & users[me]) )
	{
		if( SECURE_D->level_num(my_id) < ch->level ) return;

		users[me] |= ch->number;
		set("channels", query("channels",me) | ch->number, me);
		tell(me, "開啟 "+channel_title+" 頻道。\n");

		if( !msg ) return;
	}

	// 若沒有輸入訊息
	msg = msg || (emote ? "看起來表情豐富。" : "..." );

	// 檢查ID, NAME 
	idname = my_idname || "NO_IDNAME";

	// 整理出正在使用本頻道的頻道使用者 
	fit_users = keys(filter(users, (: $(ch->number) & $2 :)));

	// 檢查本頻道的使用之過濾條件
	if( ch->extra["filter"] )
	{
		if( sizeof(fit_users) )
			fit_users = filter(fit_users, ch->extra["filter"], stringp(raw) ? raw : (wizardp(me) ? subchannel || me : me));
	}


	// 若此頻道有顯示時間標記
	if( ch->extra["time"] == 1 )
		time = HIW+ctime(time())[11..15]+NOR" ";
	else if( ch->extra["time"] == 2 )
		time = HIW+ctime(TIME_D->query_game_time())[11..15]+NOR" ";
	else	
		time = "";

	// 若使用表情模式
	if( emote ) 
	{
		string verb, arg;
		verb = msg;
		sscanf(msg, "%s %s", verb, arg);

		// Added by Ekac
		if( EMOTE_D->is_emote(verb) )
			msg = evaluate(COMMAND_D->find_ppl_command(verb),me,arg,1) || (idname+msg);
		else
			msg = idname + msg;
	}

	// 若此頻道有設定特定訊息顏色
	if( ch->extra["msgcolor"] )
	{
		color = ch->extra["msgcolor"];
		msg = remove_ansi(msg);
		idname = remove_ansi(idname);
	}
	else
		color = "";

	if( query("no_channel", me) )
		return tell(me, pnoun(2, me)+"目前不被允許使用公開頻道發言。\n");

	// 限制洗螢幕的行為
	if( undefinedp(raw) && previous_object() != this_object() && (interactive(me) || me->is_npc()) )
	{
		int nowtime = time();

		if( nowtime - query_temp("lastchat/time", me) < 1 )
		{
			if( addn_temp("lastchat/warning", 1, me) > 100 )
			{
				tell(me, "由於"+pnoun(2, me)+"大量重覆嘗試不合法的頻道發言動作，從現在起將自動封鎖"+pnoun(2, me)+"的指令功能。\n");
				set("cmdlimit", 30, me);
				CHANNEL_D->channel_broadcast("sys", me->query_idname()+"大量重覆嘗試不合法的頻道發言動作，從現在起將自動限制指令速度。\n");
			}
			return tell(me, "公開頻道不能連續發送訊息。\n");
		}
		if( query_temp("lastchat/msg", me) == msg && nowtime - query_temp("lastchat/time", me) < 300 )
		{
			if( addn_temp("lastchat/repeattimes", 1, me) > 5 )
			{
				tell(me, "公開頻道不能連續發送相同訊息。\n");

				if( addn_temp("lastchat/warning", 1, me) > 100 )
				{
					tell(me, "由於"+pnoun(2, me)+"大量重覆嘗試不合法的頻道發言動作，從現在起將自動封鎖"+pnoun(2, me)+"的指令功能。\n");
					set("cmdlimit", 30, me);
					CHANNEL_D->channel_broadcast("sys", me->query_idname()+"大量重覆嘗試不合法的頻道發言動作，從現在起將自動限制指令速度。\n");
				}
				return;
			}
		}
		else
		{
			set_temp("lastchat/msg", msg, me);
			delete_temp("lastchat/repeattimes", me);
		}

		set_temp("lastchat/time", nowtime, me);
	}
	// 玩家無法用 News 頻道發言
	if( ch->extra["notalk"] )
	{	
		if( !wizardp(me) && interactive(me) )
		{
			return tell(me, "玩家無法使用這個頻道發言。\n");
		}
	}
	// 若是網際頻道
	if( ch->extra["intermud"] && interactive(me) )
	{
		string intermud_msg = msg;
		if( emote ) intermud_msg = replace_string(msg, my_idname, my_idname[0..<2] + "@RW)");

		if( channel == "gwiz" )
			ch->extra["intermud"]->send_gwiz_msg(subchannel, ansi_capitalize(me->query_id()), me->query_name(), intermud_msg, emote);
		else
		if((!internal_flag && subchannel!="") || ( internal_flag && member_array(subchannel,keys(channels))==-1 )) ch->extra["intermud"]->send_gchannel_msg(subchannel, ansi_capitalize(me->query_id()), me->query_name(), intermud_msg, emote);
		else 	subchannel="站內";
	}

	// 若有純訊息參數或表情參數, send_msg = 頻道名稱 + 訊息
	if( !undefinedp(raw) || emote )
		send_msg = channel_title+time+color;
	else
		send_msg = channel_title+time+color+idname+"："+color;


	// 處理精簡字串
	send_msg = send_msg + msg + NOR + (noansi_strlen(msg) > 0 && remove_ansi(msg)[<1] == '\n' ? "" : "\n");


	// replace string
	foreach(string a , string b in replace_word)
	send_msg=replace_string(send_msg,a,b);

	// 使用者權限處理, 等級高等於說話者直接顯示名稱
	if( ch->extra["wizlevel"] )
	{
		int my_level = SECURE_D->level_num(my_id);
		string level_send_msg;
		object *level_fit_users = filter(fit_users, (: wizardp($1) && $(my_level) <= SECURE_D->level_num($1->query_id(1)) :));

		if( random(50) || wizardp(me) )
			switch(query("env/rumor", me))
		{
		case 1:
			{
				string random_idname;
				object* fit_users_without_wiz = filter_array(fit_users, (: !wizardp($1) :));

				if( !sizeof(fit_users_without_wiz) )
					random_idname = fit_users[random(sizeof(fit_users))]->query_idname();
				else
					random_idname = fit_users_without_wiz[random(sizeof(fit_users_without_wiz))]->query_idname();

				level_send_msg = replace_string(send_msg, my_idname, "["+my_idname+"]-假冒的" + random_idname );
				send_msg = replace_string(send_msg, my_idname, "假冒的" + random_idname );
				break;
			}
		case 2:
			{
				if( undefinedp(query_temp("rumor_serial",me)) )
				{
					water_number += 1 + random(50);
					set_temp("rumor_serial", water_number,me);
				}
				level_send_msg = replace_string(send_msg, my_idname, "["+my_idname+"]-"+(query("gender", me) == MALE_GENDER?"Mr.":"Miss")+  query_temp("rumor_serial",me) );
				send_msg = replace_string(send_msg, my_idname, (query("gender", me) == MALE_GENDER?"Mr.":"Miss")+  query_temp("rumor_serial",me));

				break;
			}
		default:
			level_send_msg = replace_string(send_msg, my_idname, "["+my_idname+"]-某人(Someone.)");
			send_msg = replace_string(send_msg, my_idname, "某人(Someone.)");
			break;
		}
		else
		{
			level_send_msg = replace_string(send_msg, my_idname, "["+my_idname+"]-穿幫的"+my_idname);
			send_msg = replace_string(send_msg, my_idname, "穿幫的"+my_idname);
		}

		fit_users -= level_fit_users;

		// 權限足夠之使用者
		tell(level_fit_users, level_send_msg, CHNMSG);

		level_fit_users->add_msg_log(channel, level_send_msg);
	}

	tell(fit_users, send_msg, CHNMSG);

	// 城市傳言
	if( channel == "city" && previous_object() != this_object() && query("city", me) )
	{
		string city, mycity = query("city", me);
		string no_ansi_send_msg = remove_ansi(send_msg);
		object *citizens;

		foreach(city in CITY_D->query_cities())
		{
			if( city == mycity ) continue;
			if( strsrch(no_ansi_send_msg, remove_ansi(CITY_D->query_city_name(city)||"")) != -1	&& strsrch(lower_case(no_ansi_send_msg), city) != -1 )
			{
				citizens = filter_array(users(), (: query("city", $1) == $(city) :));

				if( sizeof(citizens) )
				{
					this_object()->channel_broadcast("city", "訊息送往"+CITY_D->query_city_idname(city)+"...", me);
					this_object()->channel_broadcast("city", "「"+send_msg[0..<2]+"」", citizens[0]);
					//this_object()->channel_broadcast("city", "坊間傳來消息，"+CITY_D->query_city_idname(mycity)+"的市民似乎正在討論著有關"+CITY_D->query_city_idname(city)+"的事情...", citizens[0]);
				}
			}
		}
	}

	fit_users->add_msg_log(channel, send_msg);

	switch(channel)
	{
	case "god":
		break;
	case "chat":
	case "city":
	case "ent":
	case "rumor":
	case "twiz":
	case "gwiz":
	case "music":
	case "wiz":
		log_file("channel/"+channel, send_msg, -1);
		break;
	}
}

/* Called from COMMAND_D */
nomask mapping query_channels_fpointer()
{
	mapping m = allocate_mapping(sizeof(info)*2);

	if( previous_object() != load_object(COMMAND_D) )
		return 0;

	foreach( mixed *c in info )
	{
		string ch = (string) c[1];
		m[ch] = ({ c[2], (: do_channel($1, $(ch), $2, $3) :) });
		m[ch+"*"] = ({ c[2], (: do_channel($1, $(ch+"*"), $2, $3) :) });
	}

	return m;
}

/* 註冊頻道 */
nomask void register_channel(object me, int channel)
{
	if( !channel ) return;
	if( undefinedp(users[me]) ) users[me] = channel;
	else users[me] |= channel;
}

/* 移除註冊 */
nomask void remove_register(object me, int channel)
{
	if( !channel ) return;
	users[me] ^= channel;
	if( users[me] == 0 ) map_delete(users, me);
}

/* 直接移除一位使用者 */
nomask void remove_user(object me)
{
	map_delete(users, me);
}

/* 所有玩家重新註冊頻道 */
nomask void reset_all_register()
{
	users = allocate_mapping(0);
	filter_array( users(), (: register_channel($1, query("channels",$1)) :) );
}

nomask varargs array query_channel_users(string channel, int flag)
{
	class channel_class ch;
	array res = allocate(0);

	if( undefinedp(ch = channels[channel]) ) return res;
	foreach(object ob, int i in users)
	{
		if( !flag && (i & ch->number) ) res += ({ ob });
		if( flag && !(i & ch->number) ) res += ({ ob });
	}
	return res;
}

nomask int query_default_channel(string id)
{
	int default_channel, level = SECURE_D->level_num(id);
	foreach( string name, class channel_class ch in channels )
	{
		if( level < ch->level ) continue;
		default_channel |= ch->number;
	}
	return default_channel &~ DEFAULT_REFUSE;
}

nomask string query_using_channel(object user)
{
	int channel = users[user];
	string using_channel = "";

	if( channel )
		foreach( string name, class channel_class ch in channels )
		if( channel & ch->number ) using_channel += " "+name;

	return using_channel == "" ? "無" : using_channel;
}

varargs void channel_broadcast(string channel, string msg, mixed arg)
{
	switch(channel)
	{
	case "city":
		msg = NOR GRN"地方"+HIG+"新聞 "NOR+msg; break;
	case "ent":
		msg = NOR CYN"企業"+HIC+"新聞 "NOR+msg; break;
	default: break;
	}

	if( undefinedp(arg) )
		do_channel(this_object(), channel, msg, 1);
	else if( objectp(arg) )
		do_channel(arg, channel, msg, 1);
	else
		do_channel(this_object(),channel, msg, arg); 

}

varargs void announce(object me, string msg)
{
	shout("\n\a"HIR + (objectp(me) ? me->query_idname() + HIR"發佈":"") + "系統公告 ("HIW + ctime(time()) + NOR + HIR")：\n\n"NOR + msg + NOR"\n\n");
}


private void create()
{
	class channel_class ch;

	if( clonep() ) destruct(this_object());

	channels = allocate_mapping(0);
	users = allocate_mapping(0);
	water_number = 10000;
	foreach( mixed *c in info )
	{
		ch = new(class channel_class);
		ch->number	= c[0];		
		ch->level	= c[2];
		ch->msg		= c[3];
		ch->extra	= c[4];
		channels[c[1]] = ch;
	}
	reset_all_register();

	COMMAND_D->do_reset();
}


string query_name()
{
	return "頻道系統(CHANNEL_D)";
}
