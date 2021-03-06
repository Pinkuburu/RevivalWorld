/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : object.c
 * Author : Clode@RevivalWorld
 * Date   : 2001-07-05
 * Note   : included by Master Object
 * Update :
 *  o 2002-00-00

 -----------------------------------------
 */

// When an object is destructed, this function is called with every
// item in that room.  We get the chance to save users from being destructed.
/* 環境遭到毀滅時, 所有內在物件都會呼叫此函式 */
void destruct_env_of(object ob)
{
	if( interactive(ob) )
	{
		tell(ob, "你所在的空間消失了，你現在身處在空虛縹緲之中。\n", "ENVMSG");
		ob->move(VOID_OB);
	}
}