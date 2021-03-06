/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : young_crab.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-30
 * Note   : 小螃蟹
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
        set_idname("young crab","小螃蟹");
        
        if( this_object()->set_shadow_ob() ) return;
        
        set("long", "小螃蟹。\n");
        set("unit", "隻");
        set("mass", 150);
        set("value", 4000);
        set("badsell", 1);

	// 種子資訊
	set("growth",
	([
		//種類
		"type":		FISHFARM,

		//名稱
		"idname":	this_object()->query_idname(),
		
		//開始生產時的訊息
		"start_msg":"小螃蟹在水底交叉移動著，水流輕輕地流過。\n" ,
			
	       	//每單位需要的原料種類與數量
	       	"material":
	       	([
	       		"/obj/materials/water":10,
	       		"/obj/materials/wood":20,
	       		"/obj/materials/stone":10,
	       	]),
			
	       	//產品的數量設定與產品路徑
	       	"harvest":
	       	([
	       		"/obj/fishfarm/product/big_crab":8,
	       	]),

		//生成時的訊息
		"harvest_msg":"$ME歡喜地取出大閘蟹。\n",

		//種植此類產品所需要的技能與其等級
		"skill":
		([
			"fishfarm":70,
		]),

		//成長時的訊息
		"growing":
		([
			5:	"小螃蟹在水底四處跑來跑去。\n",
			100:	"小螃蟹的兩隻小螯一直在四周夾取水草。\n",
			300:	"小螃蟹愈來愈大，肚子漸漸地胖了起來。\n",
			500:	"螃蟹的兩隻螯愈來愈大也愈來愈有力。\n",
			800:	"螃蟹伸長著兩隻黑黑的大眼珠，靜靜地由水底往這頭看。\n",
			1200:	"肥滿的大閘蟹已經成長到可以收成的程度了。\n",
		]),
	
		// 天氣影響敘述
		"weather":
		([
			SNOWY:	"大雪連連嚴重使得大閘蟹們陸續的凍死。\n",
			SHOWER:	"連日來的雨量使得部份的大閘蟹們被大水沖走。\n",
			TYPHOON:"颱風來襲使得部份的大閘蟹們被大水沖走。\n",
			FOEHN:	"焚風的吹襲讓部份的大閘蟹們因水溫過高而死。\n",
		]),
	]));
}
