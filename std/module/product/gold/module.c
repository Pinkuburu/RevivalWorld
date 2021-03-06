/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : module.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-04-06
 * Note   : 
 * Update :
 *  o 2000-00-00  
 *
 -----------------------------------------
 */

#include <feature.h>

inherit PRODUCT_ACTION_MOD;

// 放置物品
void do_action(object me, string arg)
{


}

// 設定物件動作
nosave mapping action_info =
([
	"help"	: "尚未建構說明。",
	"action":
		([
			"action"	: (: do_action :),
		]),
]);

// 設定鑲嵌物影響
void setup_inlay(object product, mapping inlay)
{
	set("design/inlay", 1, product);
}

// 設定物件資料
nosave mapping product_info = 
([	
	//中文名稱
	"name":		"金",
	
	//英文名稱
	"id":		"gold",

	//材料需求
	"material":	([ "/obj/materials/metal": 100, "gold":1 ]),

	//技能需求
	"skill":	([ "metalclassify":80, "technology":5 ]),

	//製造耗時(秒鐘)
	"timecost":	1,
	
	//耗費科技值
	"technology":	100,
	
	//耗費研發費用
	"cost":		"1000",

	//產品分類
	"class":	"金屬原物料",
	
	//生產工廠
	"factory":	"metal",
	
	//價值
	"value":	"1000",
	
	//重量(g)
	"mass":		100,
]);
