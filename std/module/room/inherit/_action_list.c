/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _list_action.c
 * Author : Clode@RevivalWorld
 * Date   : 2004-06-23
 * Note   : 
 * Update :
 *  o 2002-00-00
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <daemon.h>
#include <message.h>

void output_object(object env, string database, string basename, string amount);

// 列出商品列表
void do_list(object me, string arg, string database, int capacity)
{
	int i, j, size, average_desire;
	object env, ob;
	string msg, money_unit, popproduct, desirehint;
	string setprice, pricepercent, sell;
	mixed value;
	mapping obs;
	string *shelfsort, shelf, totalamount, replacename;
	array data;
	
	env = environment(me)->query_master();
	
	money_unit = MONEY_D->city_to_money_unit(env->query_city());
	
	// 若是物流中心
	if( env->query_building_type() == "trading_post" )
		obs = fetch_variable("allowable_trade");
	else
		obs = query(database, env);
	
	if( !sizeof(obs) )
		return tell(me, query("short", env)+"目前沒有任何物品。\n");
	
	popproduct = SHOPPING_D->query_pop_product();

	msg =  NOR WHT"───────────────────────────────────────\n"NOR;
	msg += HIW    "編號 商品"+(query("mode", env)?HIR"(管理模式)"NOR:HIC"(營業模式)"NOR)+"                    人氣 數量     單價($"+money_unit+")   價格比 收購\n"NOR;
	msg += NOR WHT"───────────────────────────────────────\n"NOR;
	
	shelfsort = keys(obs);
	
	average_desire = query("shopping/average_desire", env);

	foreach( shelf in shelfsort )
	{
		switch(shelf)
		{
			case 0:		msg += HIC"展示"NOR"\n"; break;
			case "促銷":	msg += HIY"促銷"NOR"\n"; break;
			default:	msg += HIG+shelf+NOR"\n"; break;
		}

		data = copy(obs[shelf]);

		size = sizeof(data);
		
		for(j=0;j<size;j+=2)
		{
			if( catch(ob = load_object(data[j]) ) )
				continue;
		
			// 檔案已經消失, 刪除資料
			if( !objectp(ob) && !file_exists(data[j]) )
			{
				output_object(env, database, data[j], "all");
				continue;
			}
			
			replacename = replace_string(data[j], "/", "#");
			
			setprice = query("setup/price/"+replacename, env);
			value = query("value", ob);
			totalamount = count(totalamount, "+", data[j+1]);

			if( !query("setup/class/"+replacename, env) )
				desirehint = "";
			else
			switch(average_desire + query("shopping/productdesire/"+replacename, env))
			{
				case -1999999999..0:
					desirehint = ""; break;
				case 1..30:
					desirehint = NOR WHT"*"; break;
				case 31..60:
					desirehint = NOR WHT"**"; break;
				case 61..90:
					desirehint = NOR WHT"***"; break;
				case 91..120:
					desirehint = HIW"*"NOR WHT"***"; break;
				case 121..150:
					desirehint = HIW"**"NOR WHT"***"; break;
				case 151..180:
					desirehint = HIW"***"NOR WHT"***"; break;
				case 181..1000000000:
					desirehint = HIW BLINK"*"NOR HIW"**"NOR WHT"***"; break;
				default:
					
					desirehint = HIR"Bug"NOR; break;
			}
			
			if( setprice )
				pricepercent = count(count(setprice, "*", 100), "/", value);
			else
				pricepercent = "100";
			
			if( query("setup/sell/"+replacename+"/nosell", env) )
				sell = "";
			else if( !undefinedp(query("setup/sell/"+replacename+"/percent", env)) )
				sell = query("setup/sell/"+replacename+"/percent", env) +"%";
			else if( !undefinedp(query("setup/sell/all/percent", env)) )
				sell = query("setup/sell/all/percent", env) +"%";
			else
				sell = "";

			msg += sprintf(HIW"%-3d-"NOR" %-32s%6s "HIC"%-9s"HIY"%-13s"HIG"%5s"HIR"%5s"NOR"%s"NOR"\n", 
				++i, 
				ob->query_idname(),
				desirehint,
				(data[j+1]=="-1"?HIC"無限"NOR:data[j+1])+"", 
				NUMBER_D->number_symbol(setprice||value),
				(count(pricepercent, ">", 999)?">999":pricepercent)+"%",
				sell,
				(data[j]==popproduct?HIR"Hot!"NOR:"")+(wizardp(me) ? " "+(average_desire+query("shopping/productdesire/"+replacename, env))+"":""),
				);
		}
	}
	
	msg += NOR WHT"───────────────────────────────────────\n"NOR;
	msg += sprintf("%-54s%24s\n", NOR WHT"共有 "HIW+i+NOR WHT" 種商品正在販賣中，容量 "+(capacity == -1 ? HIW"無盡"NOR:HIW+NUMBER_D->number_symbol(totalamount)+NOR WHT" / "HIW+NUMBER_D->number_symbol(count(capacity, "*", (sizeof(query("slave", env))+1))))+NOR, query("short", env));
	me->more(msg);
}

void do_warehouse_list(object me, string arg, string database, int capacity)
{
	int i, j, size;
	object env, ob;
	string msg, money_unit;
	string setprice, value;
	mapping obs;
	string *shelfsort, shelf, totalamount;
	array data;
	
	env = environment(me)->query_master();
	
	money_unit = MONEY_D->city_to_money_unit(env->query_city());
	
	// 若是物流中心
	if( env->query_building_type() == "trading_post" )
		obs = fetch_variable("allowable_trade");
	else
		obs = query(database, env);
	
	if( !sizeof(obs) )
		return tell(me, query("short", env)+"目前沒有任何物品。\n");
	
	msg =  NOR WHT"────────────────────────────────────\n"NOR;
	msg += HIW    "編號  物品                                                          數量\n"NOR;
	msg += NOR WHT"────────────────────────────────────\n"NOR;
	
	shelfsort = keys(obs);
	
	foreach( shelf in shelfsort )
	{
		if( !shelf )
			msg += HIC"展示\n"NOR;
		else
			msg += shelf + "\n";

		data = copy(obs[shelf]);

		size = sizeof(data);
		
		for(j=0;j<size;j+=2)
		{
			if( catch(ob = load_object(data[j]) ) )
				continue;
		
			// 檔案已經消失, 刪除資料
			if( !objectp(ob) && !file_exists(data[j]) )
			{
				output_object(env, database, data[j], "all");
				continue;
			}
			
			setprice = query("setup/price/"+replace_string(data[j], "/", "#"), env);
			value = query("value", ob);
			totalamount = count(totalamount, "+", data[j+1]);

			msg += sprintf(HIW"%-4d-"NOR" %-50s%16s\n", 
				++i, 
				ob->query_idname(), 
				data[j+1]=="-1"?"無限":data[j+1], 
				);
		}
	}
	
	msg += NOR WHT"────────────────────────────────────\n"NOR;
	msg += sprintf("%-54s%18s\n", NOR WHT"共有 "HIW+i+NOR WHT" 種物品，容量 "+(capacity == -1 ? HIW"無盡"NOR:HIW+NUMBER_D->number_symbol(totalamount)+NOR WHT" / "HIW+NUMBER_D->number_symbol(count(capacity, "*", (sizeof(query("slave", env))+1))))+NOR, query("short", env));
	me->more(msg);
}