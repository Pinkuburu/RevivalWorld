/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : rose_seed.c
 * Author : Clode@RevivalWorld
 * Date   : 2005-09-09
 * Note   : 玫瑰種子
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
        set_idname("rose seed","玫瑰種子");
        
        if( this_object()->set_shadow_ob() ) return;
        
        set("long","玫瑰花語：相愛•高貴•愛情。\n");
        set("unit", "粒");
        set("mass", 2);
        set("value", 100);
        set("badsell", 1);

	// 種子資訊
	set("growth",
	([
		//種類
		"type":		FARM,

		//名稱
		"idname":	this_object()->query_idname(),
		
		//插秧播種時的訊息
		"start_msg":"玫瑰種子整齊地佈滿整片田地，一片欣欣向榮的感覺。\n" ,
			
	       	//每單位種子或秧苗需要的原料數量
	       	"material":
	       	([
	       		"/obj/materials/water":12,
	       	]),
			
	       	//收割產品的數量設定與產品路徑
	       	"harvest":
	       	([
	       		"/obj/farming/product/rose":50,
	       	]),

		//成熟收割時的訊息
		"harvest_msg":"$ME歡喜地收割這片成熟的玫瑰田。\n",

		//種植此類產品所需要的技能與其等級
		"skill":
		([
			"farming":70,
		]),

		//成長時的訊息
		"growing":
		([
			5:	"玫瑰種子萌出一根根嫩芽漸漸生長。\n",
			50:	"玫瑰幼苖慢慢生長出有刺藤蔓，上面的綠葉一一舒展開來。\n",
			150:	"玫瑰田冒出了許多玫瑰花苞，再過不久就會開花了。\n",
			300:	"只見一朵又一朵綻開的鮮紅色玫瑰在綠葉的映襯之下明艷動人，再等一下子就可以採收了。\n",
			500:	"玫瑰田上的玫瑰花已全部綻放，已經可以採收了。\n",
		]),
	
		// 天氣影響敘述
		"weather":
		([
			SNOWY:	"大雪連連嚴重使得玫瑰陸續地枯萎了。\n",
			SHOWER:	"連日來的雨量使得一些玫瑰泡水腐爛了。\n",
			TYPHOON:"颱風來襲使得部份玫瑰被強風攔腰折斷。\n",
			FOEHN:	"焚風的吹襲讓部份的玫瑰快速的枯黃。\n",
		]),
	]));
}
