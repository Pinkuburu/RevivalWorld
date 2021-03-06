/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _sell_action.c
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
#include <npc.h>
#include <money.h>

int over_capacity(object env, string database, string amount, mixed capacity);
void input_object(object env, string database, string basename, string amount);
void output_object(object env, string database, string basename, string amount);

// 私自販賣商品動作
void do_sell(object me, string arg, string database, mixed capacity)
{
	int percent;
	mixed value;
	mapping sell;
	mapping sell_setup;
	string amount, basename, unit;
	object ob, env, boss, master;
	
	env = environment(me);
	master = env->query_master();

	if( query("mode", master) )
		return tell(me, env->query_room_name()+"目前狀態為「管理模式」，停止收購物品。\n");

	sell = query("setup/sell", master)||allocate_mapping(0);

	if( !arg )
	{
		int number;
		string msg, *worldproductlist;
		

		worldproductlist = PRODUCT_D->query_world_product();
			
		msg  = "允許收購商品列表\n";
		msg += "─────────────────────────────────────\n";
		msg += "編號 物品                            欲收購數量 收購價              收購比\n";
		msg += "─────────────────────────────────────\n";
		foreach(basename in worldproductlist)
		{
			if( !objectp(ob = load_object(basename)) ) continue;
					
			sell_setup = sell[replace_string(basename, "/", "#")]||sell["all"];

			if( !mapp(sell_setup) || sell_setup["nosell"] )
				continue;

			msg += sprintf("%s%-5s"NOR"%-34s%s%8s %-20s%6s\n"NOR, 
				!(number % 2) ? "" : WHT,
				++number+".", 
				ob->query_idname(),
				number % 2 ? "" : WHT,
				(sell_setup["amount"]||"無限")+"",
				NUMBER_D->number_symbol(count(count(query("value", ob), "*", sell_setup["percent"]), "/", 100)),
				sell_setup["percent"]+"%",
				);
		}
				
		msg += "─────────────────────────────────────\n";
		return me->more(msg+"\n");
	}

	if( query("owner", env) == me->query_id(1) )
		return tell(me, "直接上架商品不是比較快嗎？\n");

	unit = env->query_money_unit();


	if( lower_case(arg) == "all" )
	{
		object *all_ob = all_inventory(me);
		
		if( sizeof(all_ob) )
		{
			if( !objectp(boss = load_user(query("owner", env)) ) )
				return tell(me, "這間商店老闆的連鎖功能已經損壞，無法向"+pnoun(2, me)+"收購商品。\n");

			foreach(ob in all_ob)
			{
				amount = query_temp("amount",ob)||"1";
				
				if( ob->is_keeping() || query("flag/no_import", ob) || count(query("value", ob),"<",1) || query_temp("endurance", ob) < 0 ) continue;
				
				basename = replace_string(base_name(ob), "/", "#");
				
				sell_setup = sell[basename]||sell["all"];
				
				if( !mapp(sell_setup) || sell_setup["nosell"] )
				{
					tell(me, "這裡並不收購"+ob->query_idname()+"這種物品。\n");
					continue;
				}
					
				percent = sell_setup["percent"];
				
				if( sell_setup["amount"] && count(sell_setup["amount"], "<", amount) )
				{
					tell(me, pnoun(2, me)+"欲販賣的"+ob->query_idname()+"數量已超過允許的收購數。\n");
					continue;
				}
				
				if( over_capacity(master, database, amount, capacity) )
				{
					if( !userp(boss) )
					{
						boss->save();
						destruct(boss);
					}
					return tell(me, env->query_room_name()+"無法再容納這麼多的物品了("+capacity+")。\n");
				}

				value = count(count(count(copy(query("value", ob)),"*",amount),"*",percent),"/",100);
				
				if( value != "0" && !boss->spend_money(unit, value, BANK_PAYMENT) )
				{
					if( !userp(boss) )
					{
						boss->save();
						destruct(boss);
					}
					return tell(me, "商店老闆已經沒有那麼多的金錢可以收購"+pnoun(2, me)+"的商品了。\n");
				}
				else
					tell(boss, env->query_room_name()+"花了"HIY+QUANTITY_D->money_info(unit, value)+NOR" 元向"+me->query_idname()+"收購了"+ob->query_idname()+"。\n");
				
				
				me->earn_money(unit, value);
				
				input_object(master, database, replace_string(basename, "#", "/") , amount);

				if( sell_setup["amount"] )
				{
					if( sell[basename] )
						set("setup/sell/"+basename+"/amount", count(sell_setup["amount"], "-", amount), master);
					else
						set("setup/sell/all/amount", count(sell_setup["amount"], "-", amount), master);
				}

				msg("$ME以 "+percent+"% 的價格將"+QUANTITY_D->obj(ob, amount)+"賣出，得到了"HIY+QUANTITY_D->money_info(unit, value)+NOR"。\n",me,0,1);
				
				if( count(value, ">", "10000000") )
					log_file("command/sell", me->query_idname()+"以 "+percent+"% 的價格將"+QUANTITY_D->obj(ob, amount)+"賣給 "+query("owner", env)+"，得到了"HIY+QUANTITY_D->money_info(unit, value)+NOR"。");
				
				destruct(ob, amount);
			}
			
			if( !userp(boss) )
			{
				boss->save();
				destruct(boss);
			}

			return msg("$ME把身上的能賣的東西全賣掉了。\n"NOR,me,0,1);
		}
		return tell(me, pnoun(2, me)+"身上沒有東西可以賣！\n");
	}

	if( sscanf(arg, "%s %s", amount, arg) == 2 && (!arg || (amount != "all" && !big_number_check(amount))) )
	{
		arg = amount+" "+arg;
		amount = "1";
	}

	if( !objectp(ob = present(arg, me) || present(arg, environment(me))) )
		return tell(me, "這附近並沒有 "+arg+" 這樣東西。\n");

	if( ob->is_keeping() )
		return tell(me, pnoun(2, me)+"必須先解除"+ob->query_idname()+"的保留(keep)狀態。\n");

	if( query("flag/no_import", ob) )
		return tell(me, ob->query_idname()+"不允許販賣。\n");

	if( amount == "all" )
		amount = (query_temp("amount", ob) || 1)+"";
	else if( !(amount = big_number_check(amount)) || count(amount, "<", 1) )
		amount = "1";

	if( count(amount, ">", query_temp("amount", ob)||1) )
		return tell(me, "這附近並沒有那麼多"+(query("unit", ob)||"個")+ob->query_idname()+"。\n");
	
	if( query_temp("endurance", ob) < 0 )
		return tell(me, ob->query_idname()+"已經損壞了，無法向"+pnoun(2, me)+"收購。\n");

	value = query("value", ob);
	
	if( count(value, "<", 1) )
		return tell(me, "抱歉我們不瞭解這個商品價值多少，無法向"+pnoun(2, me)+"收購。\n");
	
	
	basename = replace_string(base_name(ob), "/", "#");

	sell_setup = sell[basename]||sell["all"];
	
	if( !mapp(sell_setup) || sell_setup["nosell"] )
		return tell(me, "這裡並不打算收購這種商品。\n");
	
	if( sell_setup["amount"] && count(sell_setup["amount"], "<", amount) )
		return tell(me, pnoun(2, me)+"欲販賣的"+ob->query_idname()+"數量已超過允許的收購數。\n");

	percent = sell_setup["percent"];
	
	value = count(count(count(value,"*",amount),"*",percent),"/",100);
	
	if( over_capacity(master, database, amount, capacity) )
		return tell(me, env->query_room_name()+"無法再容納這麼多的物品了。\n");

	boss = load_user(query("owner", env));

	if( value != "0" && !boss->spend_money(unit, value, BANK_PAYMENT) )
	{
		if( !userp(boss) )
		{
			boss->save();
			destruct(boss);
		}
		return tell(me, "商店老闆已經沒有那麼多的金錢可以收購"+pnoun(2, me)+"的商品了。\n");
	}
	else if( !userp(boss) )
	{
		boss->save();
		destruct(boss);
	}
	else
		tell(boss, env->query_room_name()+"花了"HIY+QUANTITY_D->money_info(unit, value)+NOR" 元向"+me->query_idname()+"收購了"+ob->query_idname()+"。\n");

	input_object(master, database, replace_string(basename, "#", "/") , amount);

	me->earn_money(unit, value);
	
	if( sell_setup["amount"] )
	{
		if( sell[basename] )
			set("setup/sell/"+basename+"/amount", count(sell_setup["amount"], "-", amount), master);
		else
			set("setup/sell/all/amount", count(sell_setup["amount"], "-", amount), master);
	}
	msg("$ME以 "+percent+"% 的價格將"+QUANTITY_D->obj(ob, amount)+"賣出，得到了"HIY+QUANTITY_D->money_info(unit, value)+NOR"。\n",me,0,1);
	
	if( count(value, ">", "10000000") )
		log_file("command/sell", me->query_idname()+"以 "+percent+"% 的價格將"+QUANTITY_D->obj(ob, amount)+"賣給 "+query("owner", env)+"，得到了"HIY+QUANTITY_D->money_info(unit, value)+NOR"。");
	
	destruct(ob, amount);
}

// 賣出商品
void do_trading_post_sell(object me, string arg, int percent)
{
	mixed value;
	string amount, env_city;
	object ob, env;
	
	env = environment(me)->query_master();

	if( !arg )
		return tell(me, pnoun(2, me)+"想要賣什麼東西？\n");

	env_city = env->query_city();

	if( lower_case(arg) == "all" )
	{
		object *all_ob = all_inventory(me);
		
		if( sizeof(all_ob) )
		{
			foreach(ob in all_ob)
			{
				amount = query_temp("amount",ob)||"1";
				
				if( ob->is_keeping() || count(query("value", ob),"<",1) || query_temp("endurance", ob) < 0 ) continue;
				
				if( query("badsell", ob) )
					percent = 50;
				else
					percent = 50+me->query_skill_level("price")/5;

				value = count(count(count(copy(query("value", ob)),"*",amount),"*",percent),"/",100);
				
				me->earn_money(MONEY_D->city_to_money_unit(env_city), value);
				msg("$ME以 "+percent+"% 的價格將"+QUANTITY_D->obj(ob, amount)+"賣出，得到了"HIY+QUANTITY_D->money_info(env_city, value)+NOR"。\n",me,0,1);

				destruct(ob, amount);
			}
			return msg("$ME把身上的能賣的東西全賣掉了。\n"NOR, me, 0, 1);
		}
		return tell(me, pnoun(2, me)+"身上沒有東西可以賣！\n");
	}

	if( sscanf(arg, "%s %s", amount, arg) == 2 && (!arg || (amount != "all" && !big_number_check(amount))) )
	{
		arg = amount+" "+arg;
		amount = "1";
	}

	if( !objectp(ob = present(arg, me) || present(arg, environment(me))) )
		return tell(me, "這附近並沒有 "+arg+" 這樣東西。\n");
	
	if( ob->is_keeping() )
		return tell(me, pnoun(2, me)+"必須先解除"+ob->query_idname()+"的保留(keep)狀態。\n");
			
	if( amount == "all" )
		amount = (query_temp("amount", ob) || 1)+"";
	else if( !(amount = big_number_check(amount)) || count(amount, "<", 1) )
		amount = "1";

	if( count(amount, ">", query_temp("amount", ob)||1) )
		return tell(me, "這附近並沒有那麼多"+(query("unit", ob)||"個")+ob->query_idname()+"。\n");
	
	if( query_temp("endurance", ob) < 0 )
		return tell(me, ob->query_idname()+"已經損壞了，無法向"+pnoun(2, me)+"收購。\n");
	
	value = query("value", ob);
	
	if( count(value, "<", 1) )
		return tell(me, "抱歉我們不瞭解這個商品價值多少，無法向"+pnoun(2, me)+"收購。\n");

	if( query("badsell", ob) )
		percent = 50;
	else
		percent = 50+me->query_skill_level("price")/5;
					
	value = count(count(count(value,"*",amount),"*",percent),"/",100);
	
	//CITY_D->set_city_info(env_city, "trading_post_gain", count(CITY_D->query_city_info(env_city, "trading_post_gain"), "-", count(value, "/", 5)));
	me->earn_money(MONEY_D->city_to_money_unit(env_city), value);
	
	msg("$ME以 "+percent+"% 的價格將"+QUANTITY_D->obj(ob, amount)+"賣出，得到了"HIY+QUANTITY_D->money_info(env_city, value)+NOR"。\n",me,0,1);
	//log_file("command/sell", me->query_idname()+"以 "+percent+"% 的價格將"+QUANTITY_D->obj(ob, amount)+"賣出，得到了"HIY+QUANTITY_D->money_info(env_city, value)+NOR"。");
	destruct(ob, amount);
}