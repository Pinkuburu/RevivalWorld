/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : _skill_book.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-02-08
 * Note   : 技能書
 * Update :
 *  o 2003-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>
#include <skill.h>

inherit STANDARD_OBJECT;

#define BOOK_EXP	200
#define COST_SOCIAL_EXP	100000

mapping actions;
	
void do_readbook(object me, string arg)
{
	int exp;
	string skill = query("skill");
	mapping baseskill = query("baseskill");

	foreach(string sk, int level in baseskill)
	{
		if( me->query_skill_level(sk) < level )
			return tell(me, pnoun(2, me)+"的「"+(SKILL(sk))->query_idname()+"」等級還不足以用來瞭解「"+(SKILL(skill))->query_idname()+"」。\n");
	}

	if( !me->cost_social_exp(COST_SOCIAL_EXP) )
		return tell(me, pnoun(2, me)+"的社會經驗值不足 "+COST_SOCIAL_EXP+"，無法學習此技能。\n");

	exp = BOOK_EXP * me->query_int();

 	msg("$ME開始認真讀起$YOU，只見$ME豁然開朗似的不斷的點頭，似乎從裡頭得到了很多的東西。\n", me, this_object(), 1);

	msg("$ME耗費 "+COST_SOCIAL_EXP+" 點的社會經驗值來進行學習，共得到 "+exp+" 點的「"+(SKILL(skill))->query_idname()+"」經驗值。\n", me, 0, 1);
	me->add_skill_exp(skill, exp);
	me->save();
	msg("$YOU在$ME閱讀完後便化為碎片。\n", me, this_object(), 1);

	destruct(this_object(), 1);
}

void create()
{
	string skill = query("skill");
	
	if( !skill ) return;

	set_idname(HIG+(SKILL(skill))->query_id()+NOR GRN" book"+NOR, HIG+(SKILL(skill))->query_name()+NOR GRN"技術書"NOR);
	set_temp("status", HIC"稀"NOR CYN"有");

	actions = ([ "readbook" : (: do_readbook :) ]);

	if( this_object()->set_shadow_database() ) return;
	
	set("long", "一本極為稀有的技術書，或許您能從中學習到一些特殊技能。\n\n  "+query("note", find_object(SKILL(skill))));
	set("unit", "本");
	set("mass", 300);
	set("value", 10000);
	set("badsell", 1);
}
