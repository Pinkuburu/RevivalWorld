/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : room_wizhall_7.c
 * Author : Clode@RevivalWorld
 * Date   : 2003-08-23
 * Note   : 標準房間
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <daemon.h>

inherit STANDARD_ROOM;

string help = @HELP
在這個平凡無奇的房間內並沒有任何特殊的資訊。
HELP;

/*
varargs string percent_bar(int len, string str, int cur, int max, int flag)
{
	int percent_len;
	float percent = 0.;

	if( !len ) return "Something Wrong With Bar.";
	if( max ) percent = to_float(cur)/to_float(max);
	percent_len = to_int(len * percent);
	str = sprintf("%|:" + len + "s", str);
	return sprintf("%s%s"NOR"%s%s"NOR" %s",
	    U + BBLU, (percent_len ? str[0..percent_len-1] : ""),
	    BRED, str[percent_len..],
	    (flag ? sprintf("%s%.1f%%", (percent >= 0.1 ? "":" "), percent*100):"")
	);
}

void refresh_clode_time()
{
	int days, hours, minutes;
	int time = 1129284000 - time();
	string bar, percent;
	if( time < 0 )
	{
		set("status", HIW"漫"NOR WHT"雲端"HIY"結訓了！！\n"NOR);
		return;
	}
	
	days = time / 60 / 60 / 24;
	hours = time / 60 / 60 % 24;
	minutes = time / 60 % 60;
	percent = sprintf("%.2f", (7257600-time)/72576.);
	bar = percent_bar(72, percent+"%", 7257600-time, 7257600);
	set("status", CYN"距離"HIW"漫"NOR WHT"雲端"NOR"("WHT"Cl"NOR"o"HIW"de"NOR")"CYN"結訓的日子還有 "HIC+days+NOR CYN" 天 "HIC+hours+NOR CYN" 時 "HIC+minutes+NOR CYN" 分(已經過了 "HIC+percent+"%"NOR CYN")。\n\n"+bar+"\n\n"NOR);

	call_out( (: refresh_clode_time :), 60 );
}
*/

void create()
{
	//int num;
	//array wizhallloc;

	set("short", HIW"巫師"NOR WHT"大廳"NOR);
	set("long",@TEXT
　　巫師大廳的上方懸掛著中古時代西方風格的水晶弔燈，屋外的陽光藉由五彩繽
紛的玻璃映射進來，你感受到四周撲來陰森氣息，似乎馬上要被吞蝕掉，這裡是平
常巫師們交換心得的地方，在你的前方擺放著一塊留言板上面貼滿了密密麻麻的字
條，上面寫著一些難解的魔法文字。
TEXT);

	set("exits", ([
		"down" : "/wiz/wizhall/room_wizhall_4",
		"up" : "/wiz/wizhall/room_wizhall_17",
	]));

	set("objects", ([
		"/obj/board/wizhall" : 1,
	]));

	set("exits/south", CITY_D->query_section_info("wizard", 0, "wizhall"));

	/*
	foreach( string city in CITY_D->query_cities() )
	{
		num = 0;
		
		while(CITY_D->city_exist(city, num++))
		{
			wizhallloc = CITY_D->query_section_info(city, num-1, "wizhall");
			
			if( arrayp(wizhallloc) )
				set("exits/"+city+num, wizhallloc);
			else
				set("exits/"+city+num, ({ 49,49,0,city,num-1,0,"49/49/0/"+city+"/"+(num-1)+"/0" }));
		}
	}
	
	foreach( string area in AREA_D->query_areas() )
	{
		num = 0;
		
		while(AREA_D->area_exist(area, num++))
			set("exits/"+area+num, ({ 49,49,0,area,num-1,0,"49/49/0/"+area+"/"+(num-1)+"/0" }));
	}
	*/

	::reset_objects();
	
	//refresh_clode_time();
}

varargs void catch_tell(string msg)
{
	log_file("wizhall/chatlog", msg, -1);
}
