/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : map_d.c
 * Author : Cookys@RevivalWorld
 * Date   : 2001-06-13
 * Note   : 地圖物件
 *	    要利用 MAP_D 地圖系統之物件, 必須定義以下函式
 *          o string show_map(array loc, int cover_range)      // 顯示地圖
 *          o object query_maproom(array loc)                  // 回傳地圖物件
 *	    o object *query_all_maproom()                      // 回傳所有地圖物件
 *          o int move(object ob, string dir)                  // 使用者移動
 *          o varargs array query_map(array loc, int realtime) // 回傳地圖陣列
 *	    o void remove_player_icon(object me, array loc)    // 若想有人形圖像請設計此函式
 *          o void set_player_icon(object me, array loc)       // 若想有人形圖像請設計此函式
 * mapping query_coor_range(array loc, string prop, int radius)// look.c
 *	      int valid_move(object me,string direction)	// combat_d 逃跑用 in go.c
 * string coor_short_name(array loc)
 * varargs mixed query_coor_data(array loc, mixed prop)
 *	    並且必須在 map_system_table 中設定座標隸屬判斷函式
 * Update :
 *  o 2001-06-20 Clode 修改地圖檔案儲存路徑 ex: /city/xx/realtime_map/1_map
 *  o 2001-08-07 Clode 大幅修改增加完整性
 *  o 2002-09-17 Clode 修改部份 code 提高容錯能力
 *  o 2002-09-24 Clode 重新設定 MAP_D 之定位, 使地圖系統功能能支援多種特殊地圖
 *
 -----------------------------------------
 */
//#pragma save_binary
#include <daemon.h>
#include <ansi.h>
#include <location.h>

#define DATA_PATH	"/data/daemon/mapload.o"

private mapping mapload;
private mapping worldmap;
private nosave mapping coordinates = allocate_mapping(0);

// 由此設定可利用 MAP_D 地圖系統之所有系統列表
nosave mapping map_system_table =
([
	// 系統列表	// 座標系統隸屬判斷函式
	CITY_D	:	(: CITY_D->is_city_location($1) :),
	AREA_D	:	(: AREA_D->is_area_location($1) :),
]);	

// 傳回地圖系統種類
string query_map_system(mixed map)
{		
	foreach( string system, function fp in map_system_table )
		if( find_object(system) && evaluate(fp, map) ) return system;
		
	return 0;
}

//int showmap_count=0;	
// 由指令 look 呼叫, 傳回至 title screen 顯示
string show_map(array loc)
{
	string map_system;
	
	if( !arrayp(loc) ) return 0;
	
	map_system = query_map_system(loc);
	//if( (showmap_count++ %10) == 0 )
	//	CHANNEL_D->channel_broadcast("nch", "map_d show_map count："+showmap_count);
	if( map_system )
		return map_system->show_map(loc);
	
	return 0;
}

// 傳回地圖物件
object query_maproom(array loc)
{
	string map_system;
	
	if( !arrayp(loc) ) return 0;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		return map_system->query_maproom(loc);
	
	return 0;
}

// 能移動乎?
varargs int valid_move(object me, string direction, array loc)
{
	string map_system;
	
	if( !arrayp(loc) )
		loc = query_temp("location", me);

	if( !arrayp(loc) ) return 0;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		return map_system->valid_move(me, direction, loc);
	
	return 0;
}

object *query_all_maproom()
{
	object *all_maproom = allocate(0);
	
	foreach( string system, function fp in map_system_table )
		all_maproom += system->query_all_maproom() || allocate(0);
		
	return all_maproom;
}
	
// 物件移動處理
void move(object ob, string dir)
{
	string map_system;
	
	array loc;
	
	if( !objectp(ob) )
		return;

	loc = query_temp("location", ob);

	if( !arrayp(loc) ) return;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		map_system->move(ob, dir);
}

// look.c need
mapping query_coor_range(array loc, string prop, int radius)
{
	string map_system;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		return map_system->query_coor_range(loc, prop,radius);
}

// look.c need
string coor_short_name(array loc)
{
	string map_system;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		return map_system->coor_short_name(loc);
	
	return 0;
}

// look.c need
varargs mixed query_coor_data(array loc, mixed prop)
{
	string map_system;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		return map_system->query_coor_data(loc,prop);
		
	return 0;
}

varargs array query_map(array loc, int realtime)
{
	string map_system;
	
	if( !arrayp(loc) ) return 0;
	
	map_system = query_map_system(loc);
	
	if( map_system )
	{
		if( !realtime )
			return map_system->query_map(loc);
		else
			return map_system->query_map(loc, realtime);
	}
	
	return 0;
}

string query_raw_map(array loc)
{
	string map_system, strmap = "";
	array rawmap;
	
	if( !arrayp(loc) ) return 0;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		rawmap = map_system->query_map(loc);

	if( arrayp(rawmap) )
	{
		foreach( array arr in rawmap )
			strmap += implode(arr, "");
		return remove_ansi(strmap);
	}
	else
	 	return 0;
}

void remove_player_icon(object me, array loc)
{
	string map_system;
	
	if( !arrayp(loc) ) return;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		map_system->remove_player_icon(me, loc);
}

void set_player_icon(object me, array loc)
{
	string map_system;
	
	if( !arrayp(loc) ) return;
	
	map_system = query_map_system(loc);
	
	if( map_system )
		map_system->set_player_icon(me, loc);
}

mapping query_action(array loc)
{
	string map_system;
	
	if( !arrayp(loc) ) return allocate_mapping(0);
	
	map_system = query_map_system(loc);
	
	if( map_system )
		return map_system->query_action(loc) || allocate_mapping(0);
	
	return allocate_mapping(0);
}


// 回傳縮小比例之地圖
string query_map_reduction(array map, int ratio)
{
	int x, y;
	int x_size, y_size;
	string str = "";

	map = copy(map);
	
	if( ratio < 1 || ratio > 5 ) return 0;
	
	y_size = sizeof(map);
	
	for(y=0;y<y_size;y++)
	{
		if( y % ratio ) continue;
		
		x_size = sizeof(map[y]);
		
		for(x=0;x<x_size;x++)
		{
			if( x % ratio )
				map[y][x] = 0;
		}
		
		str += implode(map[y], "")+"\n";
	}

	return str+NOR WHT"[ "HIW"比例 1 : "+ratio+NOR WHT" ]\n"NOR;
}

void enter_coordinate(array loc, object ob)
{
	if( !arrayp(loc) || !objectp(ob) )
		return;

	if( !arrayp(coordinates[loc[CODE]]) )
		coordinates[loc[CODE]] = ({ ob });
	
	else coordinates[loc[CODE]] |= ({ ob });
}

void leave_coordinate(array loc, object ob)
{
	if( !arrayp(loc) || !objectp(ob) )
		return;

	if( !arrayp(coordinates[loc[CODE]]) )
		return;
		
	if( !sizeof(coordinates[loc[CODE]] -= ({ ob, 0 })) )
		map_delete(coordinates, loc[CODE]);
}

object *coordinate_inventory(array loc)
{
	return arrayp(loc) && coordinates[loc[CODE]] ? filter_array(coordinates[loc[CODE]], (: objectp($1) :)) : allocate(0);
}

mapping query_coordinates()
{
	return copy(coordinates);
}

void restore_map_object()
{
	array loc;
	object ob;
	string basename, amount;

	// 重新掃瞄所有物件座標
	foreach( ob in objects((: environment($1) && environment($1)->is_maproom() :)) )
	{
		if( ob->query_database() )
			enter_coordinate(query_temp("location", ob), ob);
	}

	if( mapp(mapload) )
	foreach( string sloc, mapping data in mapload )
	{
		loc = restore_variable(sloc);
		
		if( !arrayp(loc) || sizeof(coordinate_inventory(loc)) ) continue;

		foreach( basename, amount in data )
		{
			reset_eval_cost();
			
			catch( ob = new(basename) || load_object(basename) );
			
			if( objectp(ob) )
			{
				if( !query("flag/no_amount", ob) )
					set_temp("amount", amount, ob);

				ob->move(loc, amount);
			}
			else
				CHANNEL_D->channel_broadcast("sys", "無法載入地圖座標物件 "+sloc+" "+basename);
		}
	}
	
	mapload = allocate_mapping(0);

}

void create()
{
	if( clonep() ) 
		destruct(this_object());
	
	if( restore_object(DATA_PATH) )
	{
		if( !mapp(worldmap) )
			worldmap = allocate_mapping(0);
	
		call_out((: restore_map_object :), 30);
	}
	else
		CHANNEL_D->channel_broadcast("sys", "無法載入地圖座標物件存檔");
}

mapping query_range_inventory(array loc, int radius)
{
	string place;
	int num, x, y;
	mapping range_inv = allocate_mapping(0);

	if( !arrayp(loc) ) return 0;
	
	place = loc[PLACE];
	num = loc[NUM];
	x = loc[X];
	y = loc[Y];
	
	radius *= radius;

	foreach( object ob in all_inventory(query_maproom(loc)) )
	{
		loc = query_temp("location", ob);
		
		if( !arrayp(loc) ) continue;
		
		if( (loc[X]-x)*(loc[X]-x) + (loc[Y]-y)*(loc[Y]-y) <= radius )
			range_inv[ob] = loc;
	}
	
	return range_inv;
}

array set_world_map(int x, int y, string zone, int num)
{
	int ix, iy;
	mapping mdata;
	array adata;
	
	if( !intp(x) || !intp(y) || !stringp(zone) || !intp(num) )
		return 0;

	if( undefinedp(worldmap[x]) )
		worldmap[x] = allocate_mapping(0);
	
	// 若此座標已經被設定了, 必須先 delete, 不能直接覆蓋
	if( !undefinedp(worldmap[x][y]) )
		return 0;

	// 不允許重覆設定
	foreach(ix, mdata in worldmap)
		foreach(iy, adata in mdata)
			if( adata[0] == zone && adata[1] == num )
				return 0;

	CHANNEL_D->channel_broadcast("nch", "將區域 "+zone+" "+num+" 之世界地圖座標設定為 "+x+","+y+"。");

	worldmap[x][y] = ({ zone, num });
	
	HTML_D->create_map_html();

	return worldmap[x][y];
}
int *query_world_map_location(string zone, int num)
{
	int ix, iy;
	mapping mdata;
	array adata;
	
	foreach(ix, mdata in worldmap)
		foreach(iy, adata in mdata)
			if( adata[0] == zone && adata[1] == num )
				return ({ ix, iy });
	
	return 0;
}

varargs mixed query_world_map(int x, int y)
{
	if( undefinedp(x) && undefinedp(y) )
		return worldmap;
	
	if( undefinedp(worldmap[x]) )
		return 0;
		
	return worldmap[x][y];
}

array delete_world_map(int x, int y)
{
	if( undefinedp(worldmap[x]) )
		return 0;
	
	map_delete(worldmap[x], y);
	
	if( !sizeof(worldmap[x]) )
		map_delete(worldmap, x);
}

int save()
{
	string sloc;
	array loc;

	mapload = allocate_mapping(0);

	// 只儲存城市裡的物品
	foreach( object ob in objects((: environment($1) && environment($1)->is_maproom() :)) )
	{
		if( !ob->query_database() || !(loc = query_temp("location", ob)) || userp(ob) ) continue;
		
		if( !CITY_D->valid_coordinate(loc) ) continue;

		reset_eval_cost();

		sloc = save_variable(loc);
		
		if( !mapp(mapload[sloc]) )
			mapload[sloc] = allocate_mapping(0);
		
		mapload[sloc][base_name(ob)] = count(mapload[sloc][base_name(ob)], "+", query_temp("amount", ob) || 1);
	}
	
	return save_object(DATA_PATH);
}


int remove()
{
	return save();
}

string query_name()
{
	return "地圖系統(MAP_D)";
}

