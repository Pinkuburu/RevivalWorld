/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : physique-adv.c
 * Author : Clode@RevivalWorld
 * Date   : 2006-02-09
 * Note   : 體格無涯技能書
 * Update :
 *  o 2003-00-00  

 -----------------------------------------
 */

#include <ansi.h>
#include <skill.h>

#define BOOK_SKILL 	"physique-adv"
#define BASE_SKILL	([ "physique" : 100 ])

inherit __DIR__"_skill_book.c";

void create()
{
	set("skill", BOOK_SKILL);
	set("baseskill", BASE_SKILL);
	
	::create();
}
