/* This program is a part of RW mudlib
 * See /doc/help/copyright for more info
 -----------------------------------------
 * File   : seed_of_lily.c
 * Author : Tzj@RevivalWorld
 * Date   : 2003-05-30
 * Note   : 百合種子
 * Update :
 *  o 2000-05-30
 *
 -----------------------------------------
 */

#include <ansi.h>
#include <inherit.h>

inherit STANDARD_OBJECT;

void create()
{
        set_idname("seed of lily",HIW"百合"NOR"種子");
        
        if( this_object()->set_shadow_ob() ) return;
        
        set("unit", "粒");
        set("mass", 2);
        set("value", 200);
        set("long","需要小心呵護才能長成[0m[37;1m百合[0m花朵。\n");
        set("badsell", 1);

        // 種子資訊
        set("seed_data",
                ([
                        //插秧播種, 灌溉, 收割所耗體力設定
                        "str_cost": ([
                                "sow"           :5, //插秧或播種時需要的體力
                                "irrigate"      :9, //灌溉每升水需要的體力 
                                "reap"          :8 //收割時需要的體力
                                ]),
                        
                        //種子種類
                        "seed":"百合種子",
                        
                        //設定插秧或是播種
                        "sow_type":"播種",
                        
                        //收割產品的路徑與數量設定
                        "reap":({ 3, "/obj/farming/lily.c"}),
                        
                        //各個步驟的農作訊息
                        "msg":
                        ([
                                // 插秧播種時的訊息
                                "sow":
                                        "百合種子散播在田裡，還看不出幼苖。\n" ,
                                
                                // 灌溉時的訊息
                                "irrigate":
                                ({
                                        2,"百合種子萌出一根根嫩芽漸漸生長。\n",
                                        3,"百合幼苖慢慢生長，上面的綠葉一一舒展開來。\n",
                                        6,"百合花田冒出了許多百合花苞，再過不久就會開花了。\n",
                                        7,"綻開的白色百合花在綠葉的映襯之下聖潔無比，再等一下子就可以採收了。\n",
                                }),
                                
                                // 成熟收割時的訊息
                                "reap":
                                ({
                                        10,
                                        "百合花田上的百合已全部綻放，已經可以採收了。\n",
                                        "這片百合花田在細心呵護之下，綻放了許多百合。\n共採收了$NUM朵的百合花。\n"
                                }),
                                
                                // 荒廢時的訊息
                                "neglect":
                                ({      
                                        25,
                                        "這片百合花田因為長久沒有澆水，綠葉一片片枯黃凋零。\n",
                                        "這片百合花田因久未善加呵護，已經全部荒廢了。\n",
                                }),
                                
                                "weather":
                                ([
                                        "RAINY":"大雨使得許多百合的根泡水腐爛了。\n",
                                        "SNOWY":"大雪使百合花田遭受到嚴重的冰害。\n",
                                        "WINDY":"狂風使一部分的百合幼苖被攔腰吹斷。\n",
                                ]),
                        ]),
                ])
        );
}
