/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : young_shrimp.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-30
 * Note   : 小蝦
 * Update :
 *  o 2000-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <nature.h>
#include <map.h>

inherit STANDARD_OBJECT;

void create()
{
        set_idname("young shrimp","小蝦");
        
        if( this_object()->set_shadow_ob() ) return;
        
        set("long", "小蝦。\n");
        set("unit", "隻");
        set("mass", 100);
        set("value", 1000);
        set("badsell", 1);

	// 種子資訊
	set("growth",
	([
		//種類
		"type":		FISHFARM,

		//名稱
		"idname":	this_object()->query_idname(),
		
		//開始生產時的訊息
		"start_msg":"小蝦四處游走著，水流輕輕地流過。\n" ,
			
	       	//每單位需要的原料種類與數量
	       	"material":
	       	([
	       		"/obj/materials/water":10,
	       		"/obj/materials/wood":10,
	       	]),
			
	       	//產品的數量設定與產品路徑
	       	"harvest":
	       	([
	       		"/obj/fishfarm/product/big_shrimp":12,
	       	]),

		//生成時的訊息
		"harvest_msg":"$ME歡喜地取出草蝦。\n",

		//種植此類產品所需要的技能與其等級
		"skill":
		([
			"fishfarm":40,
		]),

		//成長時的訊息
		"growing":
		([
			5:	"小蝦在水底四處游來游去。\n",
			100:	"小蝦半透明的身體在水草中快速的穿梭。\n",
			240:	"小蝦愈來愈大，身體表面也漸漸跑出顏色了。\n",
			360:	"草蝦的兩排腳快速的擺動著，在水底快速游動。\n",
			540:	"草蝦吃得飽飽的待在水底靜靜地休息。\n",
			720:	"肥滿的草蝦已經成長到可以收成的程度了。\n",
		]),
	
		// 天氣影響敘述
		"weather":
		([
			SNOWY:	"大雪連連嚴重使得草蝦們陸續的凍死。\n",
			SHOWER:	"連日來的雨量使得部份的草蝦們被大水沖走。\n",
			TYPHOON:"颱風來襲使得部份的草蝦們被大水沖走。\n",
			FOEHN:	"焚風的吹襲讓部份的草蝦們因水溫過高而死。\n",
		]),
	]));
}
