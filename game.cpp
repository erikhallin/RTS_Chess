#include "game.h"

game::game()
{
    m_game_state=gs_init;
}

bool game::init(int* window_size,bool* pKeys_real,bool* pKeys_translated,
                int* pMouse_pos,bool* pMouse_but,bool reinit)
{
    cout<<"Game: Initialization\n";

    m_vec_units.clear();
    m_vec_events.clear();
    m_vec_pings.clear();
    m_vec_rollbacks_to_ignore.clear();
    m_vec_history_move.clear();

    m_rollback_unconfirmed=false;
    m_window_size[0]=window_size[0];
    m_window_size[1]=window_size[1];
    m_pKeys_real=pKeys_real;
    m_pKeys_translated=pKeys_translated;
    m_pMouse_pos=pMouse_pos;
    m_pMouse_but=pMouse_but;
    m_LMB_trig=false;
    m_tick_counter=0;
    m_game_started=m_game_over=m_pause_game=false;
    m_game_start_countdown_timer=0.0;
    m_game_start_countdown=(int)_game_start_time;
    //default val
    m_static_event_delay=100;
    m_event_resend_delay=30;
    m_automove_timer=m_automove_delay=0.1;
    m_automove_on=false;

    m_pix_per_unit=(float)m_window_size[0]/8.0;
    m_player_id=0;
    m_print_log=false;

    //load textures and sound
    if(!reinit)
    {
        //texture
        if(!load_textures())
        {
            return false;
        }

        //sound
        if(!load_sounds())
        {
            return false;
        }
    }

    /*//init gamepads and players
    for(int i=0;i<4;i++)
    {
        m_gamepad[i]=gamepad(i);
        if( m_gamepad[i].IsConnected() )
         m_gamepad_connected[i]=true;
        else
         m_gamepad_connected[i]=false;
    }*/
    //m_key_rerouter.init(m_pKeys_real,m_pKeys_translated);

    //resent board movement
    for(int x=0;x<8;x++)
    for(int y=0;y<8;y++)
    {
        m_arr_board_movements[x][y]=0;
    }

    //place pieces
    {
    m_vec_units.push_back(new unit(0,0,ut_tower,2,m_tex_figures));
    m_vec_units.push_back(new unit(1,0,ut_horse,2,m_tex_figures));
    m_vec_units.push_back(new unit(2,0,ut_runner,2,m_tex_figures));
    m_vec_units.push_back(new unit(3,0,ut_king,2,m_tex_figures));
    m_vec_units.push_back(new unit(4,0,ut_queen,2,m_tex_figures));
    m_vec_units.push_back(new unit(5,0,ut_runner,2,m_tex_figures));
    m_vec_units.push_back(new unit(6,0,ut_horse,2,m_tex_figures));
    m_vec_units.push_back(new unit(7,0,ut_tower,2,m_tex_figures));
    m_vec_units.push_back(new unit(0,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(1,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(2,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(3,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(4,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(5,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(6,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(7,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(0,7,ut_tower,1,m_tex_figures));
    m_vec_units.push_back(new unit(1,7,ut_horse,1,m_tex_figures));
    m_vec_units.push_back(new unit(2,7,ut_runner,1,m_tex_figures));
    m_vec_units.push_back(new unit(3,7,ut_queen,1,m_tex_figures));
    m_vec_units.push_back(new unit(4,7,ut_king,1,m_tex_figures));
    m_vec_units.push_back(new unit(5,7,ut_runner,1,m_tex_figures));
    m_vec_units.push_back(new unit(6,7,ut_horse,1,m_tex_figures));
    m_vec_units.push_back(new unit(7,7,ut_tower,1,m_tex_figures));
    m_vec_units.push_back(new unit(0,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(1,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(2,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(3,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(4,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(5,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(6,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(7,6,ut_pawn,1,m_tex_figures));
    }


    m_game_state=gs_menu;

    cout<<"Game: Initialization complete\n";

    //online setup
    m_mp_status=0;//1-Host, 2-Join

    //text file read
    ifstream settings_file("Multiplayer_Settings.txt");
    if(settings_file==0)
    {
        settings_file.close();

        //create new file
        ofstream print_file("Multiplayer_Settings.txt");
        if(print_file==0)
        {
            cout<<"ERROR: Could not create file Multiplayer_Settings.txt\n";
            return false;
        }
        print_file<<"Rapid Chess Multiplayer Settings\n";
        print_file<<"Enter Host/Join status below:\n";
        print_file<<"HOST\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"How to HOST a game\n";
        print_file<<"********************************\n";
        print_file<<"Rapid Chess Multiplayer Settings\n";
        print_file<<"Enter Host/Join status below:\n";
        print_file<<"HOST\n";
        print_file<<"********************************\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"\n";
        print_file<<"How to JOIN a game\n";
        print_file<<"********************************\n";
        print_file<<"Rapid Chess Multiplayer Settings\n";
        print_file<<"Enter Host/Join status below:\n";
        print_file<<"JOIN\n";
        print_file<<"192.168.1.1\n";
        print_file<<"********************************\n";
        print_file<<"\n";
        print_file<<"\n";

        print_file.close();
    }
    else//read file
    {
        string line;
        getline(settings_file,line);
        getline(settings_file,line);
        getline(settings_file,line);
        //enable automove
        if(line=="AUTOMOVE ON")
        {
            cout<<"AUTOMOVE is ON\n";
            m_automove_on=true;
        }
        //host/join
        getline(settings_file,line);
        if(line=="HOST")
        {
            m_mp_status=ps_hoster;
        }
        else if(line=="JOIN")
        {
            m_mp_status=ps_joiner;
            //get IP
            //default port is 5001
            getline(settings_file,line);
            m_host_ip=line;
        }
        else if(line=="NONE")
        {
            m_mp_status=ps_none;
        }
        else
        {
            cout<<"ERROR: Bad Multiplayer_Settings.txt file\n";
            return false;
        }

        settings_file.close();
    }

    /*//console read
    cout<<"\n[H]ost or [J]oin game: ";
    string line;
    getline(cin,line);
    while(getline(cin,line))
    {
        if(line=="H" || line=="h" || line=="Host" || line=="host")
         mp_status=1;
        if(line=="J" || line=="j" || line=="Join" || line=="join")
         mp_status=2;

        if(mp_status==0)
        {
            cout<<"\nInvalid option, please type Host or Join: ";
        }
        else break;
    }
    switch(mp_status)
    {
        case 0: cout<<"ERROR: Bad selection\n"; return false;
        case 1: cout<<"\nYou will HOST a game\n"; break;
        case 2: cout<<"You will JOIN a game\n"; break;
    }*/

    m_game_state=gs_in_game;

    //create log file
    if(true)
    {
        m_print_log=true;

        time_t t = time(0);// get time now
        struct tm * now = localtime( & t );
        string time_now("LOG_");
        stringstream ss;
        if((now->tm_year-100)<10) ss<<"0";
        ss<<(now->tm_year-100);
        if((now->tm_mon+1)<10) ss<<"0";
        ss<<(now->tm_mon+1);
        if((now->tm_mday)<10) ss<<"0";
        ss<<now->tm_mday;
        ss<<"_";
        if((now->tm_hour)<10) ss<<"0";
        ss<<now->tm_hour;
        if((now->tm_min)<10) ss<<"0";
        ss<<now->tm_min;
        if((now->tm_sec)<10) ss<<"0";
        ss<<now->tm_sec;
        time_now.append(ss.str());
        time_now.append(".txt");

        cout<<"Log data will be printed in: "<<time_now<<endl;;
        m_log.open(time_now.c_str());
        if(m_log==0)
        {
            cout<<"ERROR: Could not create logfile\n";
            m_print_log=false;
        }
    }

    return true;
}

bool game::update(bool& quit_flag)
{
    /*//get gamepad data
    st_gamepad_data gamepad_data[4];
    for(int gamepad_i=0;gamepad_i<4;gamepad_i++)
    {
        if( m_gamepad[gamepad_i].IsConnected() )
        {
            //test if new connection
            if(!m_gamepad_connected[gamepad_i])
            {
                cout<<"Gamepad: New controller conencted: "<<gamepad_i+1<<endl;
            }

            m_gamepad_connected[gamepad_i]=true;
        }
        else//lost controller
        {
            if( m_gamepad_connected[gamepad_i] )//had connection and lost it
            {
                m_gamepad_connected[gamepad_i]=false;
                cout<<"Gamepad: Lost connection to controller: "<<gamepad_i+1<<endl;
            }
        }
    }

    //update key rerouter
    m_key_rerouter.update();*/

    switch(m_game_state)
    {
        case gs_init:
        {
            //nothing
        }break;

        case gs_menu:
        {
            //nothing
        }break;

        case gs_in_game:
        {
            if(m_pause_game) break;

            //update tick
            m_tick_counter++;

            //start timer
            if(!m_game_started && m_game_start_countdown_timer>0.0)
            {
                m_game_start_countdown_timer-=_time_step;
                //start
                if(m_game_start_countdown_timer<=0)
                {
                    m_game_started=true;
                    m_game_start_countdown_timer=0;
                    cout<<"Match has started!\n";

                    //calc ping
                    if(m_player_id==ps_hoster)
                    {
                        float ping_avg=0;
                        float ping_max=0;
                        for(int i=0;i<(int)m_vec_pings.size();i++)
                        {
                            ping_avg+=m_vec_pings[i];
                            if(m_vec_pings[i]>ping_max) ping_max=m_vec_pings[i];

                            cout<<m_vec_pings[i]<<endl;
                        }
                        ping_avg/=(float)m_vec_pings.size();
                        m_ping_avg=ping_avg;

                        if(m_vec_pings.empty() || ping_max>1000.0)
                        {
                            cout<<"WARNING: Bad connection, abort game\n";
                            return false;
                        }

                        cout<<"Average ping: "<<m_ping_avg<<"\tMax ping: "<<ping_max<<endl;

                        //set send settings
                        m_static_event_delay=(ping_max+1)*2;
                        if(m_static_event_delay<2) m_static_event_delay=2;
                        m_event_resend_delay=int((float)m_static_event_delay/4.0);
                        if(m_event_resend_delay<1) m_event_resend_delay=1;
                        //set min values
                        if(m_static_event_delay<_move_delay_min) m_static_event_delay=_move_delay_min;
                        if(m_event_resend_delay<_resend_delay_min) m_event_resend_delay=_resend_delay_min;

                        //m_event_resend_delay=m_static_event_delay;//TEMP

                        if(m_print_log)
                        {
                            m_log<<"Event delay: "<<m_static_event_delay<<"\tResend delay: "<<m_event_resend_delay<<endl;
                        }

                        cout<<"Event delay: "<<m_static_event_delay<<"\tResend delay: "<<m_event_resend_delay<<endl;
                        //send to client
                        m_vec_events.push_back(st_event(m_static_event_delay,m_event_resend_delay));
                    }
                }
                else
                {
                    //print progress
                    if((int)m_game_start_countdown_timer<m_game_start_countdown)
                    {
                        for(int i=0;i<m_game_start_countdown-1;i++) cout<<"-";
                        cout<<m_game_start_countdown<<endl;
                        m_game_start_countdown--;

                        //ping
                        if(m_player_id==ps_hoster)
                         m_vec_events.push_back( st_event(m_tick_counter) );
                    }
                }
            }

            //check for active events
            for(int event_i=0;event_i<(int)m_vec_events.size();event_i++)
            {
                //ping
                if(m_vec_events[event_i].type==et_ping)
                {
                    send_data(m_vec_events[event_i]);

                    m_vec_events.erase(m_vec_events.begin()+event_i);
                    event_i--;
                    continue;
                }

                //settings
                if(m_vec_events[event_i].type==et_settings)
                {
                    send_data(m_vec_events[event_i]);

                    m_vec_events.erase(m_vec_events.begin()+event_i);
                    event_i--;
                    continue;
                }

                //rollback request/resend
                if(m_vec_events[event_i].type==et_rollback)
                {
                    //if player is owner, resend
                    if(m_vec_events[event_i].owner==m_player_id)
                    {
                        m_vec_events[event_i].resend_counter--;
                        if(m_vec_events[event_i].resend_counter<=0)
                        {
                            m_vec_events[event_i].resend_counter=m_event_resend_delay;
                            //resend
                            if(!send_data(m_vec_events[event_i]))
                            {
                                cout<<"ERROR: Could not resend data\n";
                            }
                        }
                    }
                    else//if player is not owner, do rollback
                    {
                        rollback(m_vec_events[event_i].tick_target);

                        //reset tick
                        m_tick_counter=m_rollback_tick_target;
                    }

                    continue;
                }

                //rollback confirmation
                if(m_vec_events[event_i].type==et_rollback_confirmation)
                {
                    //should rollback be ignored, check list
                    bool ignore_rollback=false;
                    for(int i=0;i<(int)m_vec_rollbacks_to_ignore.size();i++)
                    {
                        if(m_vec_rollbacks_to_ignore[i]==m_vec_events[event_i].tick_target)
                        {
                            ignore_rollback=true;
                            break;
                        }
                    }
                    //m_vec_rollbacks_to_ignore

                    if(ignore_rollback)
                    {
                        //delete this event
                        m_vec_events.erase(m_vec_events.begin()+event_i);
                    }
                    else//do not ignore
                    {
                        //store rollback in ignore list
                        m_vec_rollbacks_to_ignore.push_back(m_vec_events[event_i].tick_target);

                        //allow game input, game resumed
                        m_rollback_unconfirmed=false;

                        //reset tick
                        m_tick_counter=m_rollback_tick_target;

                        //remove other events (to remove resend event)
                        m_vec_events.clear();

                        return true;
                    }
                }

                //movement

                //check if event is ready and confirmed
                if((m_vec_events[event_i].tick_target==m_tick_counter &&
                    m_vec_events[event_i].confirmed) || m_player_id==ps_none)//SP, run directly
                {
                    //TEMP check for duplicates
                    for(int event_i2=0;event_i2<(int)m_vec_events.size();event_i2++)
                    {
                        if(event_i==event_i2) continue;

                        if(m_vec_events[event_i]==m_vec_events[event_i2])
                        {
                            cout<<"ERROR: Event duplication found\n";
                            if(m_print_log) m_log<<"ERROR: Event duplication found\n";
                        }
                    }


                    //add event
                    switch(m_vec_events[event_i].type)
                    {
                        case et_unit_move:
                        {
                            //check if other unit taken
                            for(int unit_i=0;unit_i<(int)m_vec_units.size();unit_i++)
                            {
                                if(m_vec_units[unit_i]->m_pos[0]==m_vec_events[event_i].x_to &&
                                   m_vec_units[unit_i]->m_pos[1]==m_vec_events[event_i].y_to)
                                {
                                    m_vec_units[unit_i]->m_remove=true;

                                    //king test
                                    if(!m_game_over)
                                    {
                                        if(m_vec_units[unit_i]->m_type==ut_king)
                                        {
                                            m_game_over=true;

                                            cout<<"***************\n** GAME OVER **\n***************\n";
                                            if(m_vec_units[unit_i]->m_owner==ps_joiner) cout<<"-- HOST WON --\n";
                                            if(m_vec_units[unit_i]->m_owner==ps_hoster) cout<<"--CLIENT WON--\n";
                                        }
                                    }
                                }
                            }

                            //find unit
                            bool unit_found=false;
                            for(int unit_i=0;unit_i<(int)m_vec_units.size();unit_i++)
                            {
                                if(m_vec_units[unit_i]->m_pos[0]==m_vec_events[event_i].x_from &&
                                   m_vec_units[unit_i]->m_pos[1]==m_vec_events[event_i].y_from)
                                {
                                    unit_found=true;

                                    //move unit
                                    m_vec_units[unit_i]->m_pos[0]=m_vec_events[event_i].x_to;
                                    m_vec_units[unit_i]->m_pos[1]=m_vec_events[event_i].y_to;
                                    m_vec_units[unit_i]->m_has_moved=true;
                                    m_vec_units[unit_i]->m_move_timer=1.0;

                                    break;
                                }
                            }

                            if(!unit_found)
                            {
                                cout<<"ERROR: Event unit move could not find unit\n";


                                //XXXX this happens!
                                cout<<"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
                                if(m_print_log)
                                {
                                    m_log<<"ERROR: Invalid move from/to: "<<m_vec_events[event_i].x_from<<", "<<m_vec_events[event_i].y_from<<"\t";
                                    m_log<<m_vec_events[event_i].x_to<<", "<<m_vec_events[event_i].y_to<<"\n";
                                }
                                //XXXX
                            }

                        }break;
                    }

                    m_vec_events.erase(m_vec_events.begin()+event_i);
                    event_i--;
                    continue;
                }

                //too old?
                if(m_vec_events[event_i].tick_target<m_tick_counter)
                {
                    //out of sync
                    cout<<"ERROR: Event out of sync\n";

                    m_rollback_unconfirmed=true;
                    //send rollback status to other player
                    st_event event(m_vec_events[event_i].tick_target);
                    event.type=et_rollback;
                    event.owner=m_player_id;
                    send_data(event);//quick send

                    //rollback
                    rollback(m_vec_events[event_i].tick_target);

                    //store for resending
                    event.resend_counter=m_event_resend_delay;
                    m_vec_events.push_back(event);

                    return true;
                }

                //check if unconfirmed package should be sent (again)
                if(!m_vec_events[event_i].confirmed)
                {
                    //init first pass
                    if(m_vec_events[event_i].resend_counter<=-100) m_vec_events[event_i].resend_counter=m_event_resend_delay;

                    m_vec_events[event_i].resend_counter--;
                    if(m_vec_events[event_i].resend_counter<=0)
                    {
                        m_vec_events[event_i].resend_counter=m_event_resend_delay;
                        //resend
                        if(!send_data(m_vec_events[event_i]))
                        {
                            cout<<"ERROR: Could not send data\n";
                        }
                    }
                }
            }

            //no input if a rollback is not confirmed
            if(m_rollback_unconfirmed) break;


            for(int i=0;i<(int)m_vec_units.size();i++)
            {
                m_vec_units[i]->update();
            }

            //unit marked test
            bool unit_selected=false;
            for(int i=0;i<(int)m_vec_units.size();i++)
            {
                if(m_pSelected_unit==m_vec_units[i])
                {
                    unit_selected=true;
                    m_pSelected_unit->m_marked=true;
                    break;
                }
            }

            //clear movement board
            for(int x=0;x<8;x++)
            for(int y=0;y<8;y++)
            {
                m_arr_board_movements[x][y]=0;
            }
            //mark possible movements
            if(unit_selected)
            {
                for(int x=0;x<8;x++)
                for(int y=0;y<8;y++)
                {
                    if(move_test(m_pSelected_unit,x,y))
                    {
                        //end pos block test for allied units
                        bool unit_block=false;
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==x &&
                               m_vec_units[i]->m_pos[1]==y &&
                               m_vec_units[i]->m_owner==m_pSelected_unit->m_owner)
                            {
                                unit_block=true;
                                break;
                            }
                        }

                        if(!unit_block)
                         m_arr_board_movements[x][y]=1;
                    }
                }
            }

            //click test (inactive if game not started)
            bool LMB_click=false;
            if(m_pMouse_but[0] && m_game_started)
            {
                if(!m_LMB_trig)
                {
                    m_LMB_trig=true;
                    LMB_click=true;
                }
            }
            else
            {
                m_LMB_trig=false;
            }

            //selection and movement test
            if(LMB_click)
            {
                if(unit_selected)
                {
                    //test possible movement
                    int mouse_pos_tile_x=int((float)m_pMouse_pos[0]/m_pix_per_unit);
                    int mouse_pos_tile_y=int((float)m_pMouse_pos[1]/m_pix_per_unit);

                    bool move_ok=false;
                    if(move_test(m_pSelected_unit,mouse_pos_tile_x,mouse_pos_tile_y))
                    {
                        //test if other unit at target pos
                        bool allied_unit_col=false;
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==mouse_pos_tile_x &&
                               m_vec_units[i]->m_pos[1]==mouse_pos_tile_y)
                            {
                                if(m_vec_units[i]->m_owner!=m_pSelected_unit->m_owner)
                                {
                                    //take unit (occurs at package interpretation)
                                    //m_vec_units[i]->m_remove=true;

                                }
                                else allied_unit_col=true;

                                break;
                            }


                        }

                        if(!allied_unit_col)
                        {
                            move_ok=true;

                            /*m_pSelected_unit->m_pos[0]=mouse_pos_tile_x;
                            m_pSelected_unit->m_pos[1]=mouse_pos_tile_y;
                            m_pSelected_unit->m_has_moved=true;
                            m_pSelected_unit->m_move_timer=1.0;

                            //pawn to queen test
                            if(m_pSelected_unit->m_type==ut_pawn && m_pSelected_unit->m_pos[1]==0)
                            {
                                m_pSelected_unit->m_type=ut_queen;
                            }*/

                            //test if unit already requested to move
                            bool abort_send=false;
                            for(int i=0;i<(int)m_vec_events.size();i++)
                            {
                                if(m_vec_events[i].x_from==mouse_pos_tile_x && m_vec_events[i].y_from==mouse_pos_tile_y)
                                {
                                    abort_send=true;
                                    break;
                                }
                            }

                            if(!abort_send)
                            {
                                //send event
                                m_vec_events.push_back(st_event(m_tick_counter+m_static_event_delay,m_player_id,
                                                                m_pSelected_unit->m_pos[0],mouse_pos_tile_x,
                                                                m_pSelected_unit->m_pos[1],mouse_pos_tile_y));
                                m_vec_events.back().resend_counter=0;//sent directly
                            }
                        }

                        //unselect
                        m_pSelected_unit=NULL;
                    }

                    if(!move_ok)
                    {
                        unit_selected=false;
                        m_pSelected_unit=NULL;
                    }
                }

                if(!unit_selected)
                {
                    //select unit
                    int mouse_pos_tile_x=int((float)m_pMouse_pos[0]/m_pix_per_unit);
                    int mouse_pos_tile_y=int((float)m_pMouse_pos[1]/m_pix_per_unit);
                    //cout<<"mouse pos: "<<mouse_pos_tile_x<<", "<<mouse_pos_tile_y<<endl;

                    //find unit
                    for(int i=0;i<(int)m_vec_units.size();i++)
                    {
                        //test pos
                        if(m_vec_units[i]->m_pos[0]==mouse_pos_tile_x &&
                           m_vec_units[i]->m_pos[1]==mouse_pos_tile_y )
                        {
                            //test owner
                            if(m_vec_units[i]->m_owner==m_player_id || m_player_id==ps_none)
                            {
                                //test if unit already has a move order
                                bool abort_select=false;
                                for(int i2=0;i2<(int)m_vec_events.size();i2++)
                                {
                                    if(m_vec_events[i2].x_from==mouse_pos_tile_x && m_vec_events[i2].y_from==mouse_pos_tile_y)
                                    {
                                        abort_select=true;
                                        break;
                                    }
                                }

                                if(!abort_select)
                                {
                                    m_pSelected_unit=m_vec_units[i];
                                    m_pSelected_unit->m_marked=true;
                                    //cout<<"marked unit\n";
                                }
                            }

                            break;
                        }
                    }
                }
            }

            //automove
            if(m_automove_on && m_game_started && !m_vec_units.empty())
            {
                m_automove_timer-=_time_step;
                if(m_automove_timer<=0)
                {
                    m_automove_timer=m_automove_delay;

                    //move random unit
                    unit* unit_to_move;
                    int try_counter=0;
                    int max_tries=1000;
                    bool move_ok=false;
                    int unit_ind=rand()%(int)m_vec_units.size();
                    while(m_vec_units[unit_ind]->m_owner!=m_player_id && try_counter<max_tries)
                    {
                        try_counter++;
                        unit_ind=rand()%(int)m_vec_units.size();
                    }
                    unit_to_move=m_vec_units[unit_ind];
                    //fint acceptable target pos
                    int target_pos_x=rand()%8;
                    int target_pos_y=rand()%8;
                    while(!move_test(unit_to_move,target_pos_x,target_pos_y) && try_counter<max_tries)
                    {
                        try_counter++;
                        target_pos_x=rand()%8;
                        target_pos_y=rand()%8;
                    }
                    if(move_test(unit_to_move,target_pos_x,target_pos_y))
                    {
                        //test if other unit at target pos
                        bool allied_unit_col=false;
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==target_pos_x &&
                               m_vec_units[i]->m_pos[1]==target_pos_y)
                            {
                                if(m_vec_units[i]->m_owner!=unit_to_move->m_owner)
                                {
                                    //take unit (occurs at package interpretation)
                                    //m_vec_units[i]->m_remove=true;

                                }
                                else allied_unit_col=true;

                                break;
                            }


                        }

                        if(!allied_unit_col)
                        {
                            move_ok=true;

                            //test if unit already requested to move
                            bool abort_send=false;
                            for(int i=0;i<(int)m_vec_events.size();i++)
                            {
                                if(m_vec_events[i].x_from==target_pos_x && m_vec_events[i].y_from==target_pos_y)
                                {
                                    abort_send=true;
                                    break;
                                }
                            }

                            if(!abort_send)
                            {
                                //send event
                                m_vec_events.push_back(st_event(m_tick_counter+m_static_event_delay,m_player_id,
                                                                unit_to_move->m_pos[0],target_pos_x,
                                                                unit_to_move->m_pos[1],target_pos_y));
                                m_vec_events.back().resend_counter=0;//sent directly
                            }
                        }
                    }

                    if(move_ok) cout<<"Automove Successful\n";
                    else cout<<"Automove failed\n";
                }
            }

            //remove units
            for(int i=0;i<(int)m_vec_units.size();i++)
            {
                if(m_vec_units[i]->m_remove)
                {
                    if(m_vec_units[i]==m_pSelected_unit)
                    {
                        //unselect
                        m_pSelected_unit=NULL;
                    }

                    delete m_vec_units[i];
                    m_vec_units.erase(m_vec_units.begin()+i);
                    i--;
                }
            }


        }break;
    }

    return true;
}

bool game::draw(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    //glLoadIdentity();

    switch(m_game_state)
    {
        case gs_init:
        {
            //nothing
        }break;

        case gs_menu:
        {
            //nothing
        }break;

        case gs_in_game:
        {


            //rotate
            glPushMatrix();
            /*glTranslatef(m_window_size[0],m_window_size[1],0);
            glRotatef(180,0,0,1);*/

            //board
            glColor3f(1,1,1);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBindTexture(GL_TEXTURE_2D,m_tex_board);
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);
            glVertex2f(0,0);
            glTexCoord2f(0,1);
            glVertex2f(0,m_window_size[1]);
            glTexCoord2f(1,1);
            glVertex2f(m_window_size[0],m_window_size[1]);
            glTexCoord2f(1,0);
            glVertex2f(m_window_size[0],0);
            glEnd();
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);

            //allowed movement tiles
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.2,0.8,0.2,0.5);
            glBegin(GL_QUADS);
            for(int x=0;x<8;x++)
            for(int y=0;y<8;y++)
            {
                if(m_arr_board_movements[x][y]==1)
                {
                    glVertex2f(x*m_pix_per_unit,y*m_pix_per_unit);
                    glVertex2f(x*m_pix_per_unit,y*m_pix_per_unit+m_pix_per_unit);
                    glVertex2f(x*m_pix_per_unit+m_pix_per_unit,y*m_pix_per_unit+m_pix_per_unit);
                    glVertex2f(x*m_pix_per_unit+m_pix_per_unit,y*m_pix_per_unit);
                }
            }
            glEnd();
            glDisable(GL_BLEND);

            /*//lines
            glBegin(GL_LINES);
            for(int x=0;x<=m_window_size[0];x+=int((float)m_window_size[0]/8.0))
            {
                glVertex2f(x,0);
                glVertex2f(x,m_window_size[1]);
            }
            for(int y=0;y<=m_window_size[1];y+=int((float)m_window_size[1]/8.0))
            {
                glVertex2f(0,y);
                glVertex2f(m_window_size[0],y);
            }
            glEnd();*/

            //units
            for(int unit_i=0;unit_i<(int)m_vec_units.size();unit_i++)
            {
                m_vec_units[unit_i]->draw(m_pix_per_unit);
            }

            glPopMatrix();

        }break;
    }


    return true;
}

int game::get_MP_settings(string& ip)
{
    if(m_mp_status==2) ip=m_host_ip;

    return m_mp_status;
}

bool game::start_game(void)
{
    m_tick_counter=0;
    m_game_start_countdown_timer=_game_start_time;
    cout<<"Match will start in..."<<endl;

    return true;
}


//Private

bool game::load_textures(void)
{
    cout<<"Game: Loading texture\n";

    m_tex_figures=SOIL_load_OGL_texture
    (
        "data\\texture\\figures.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        /*SOIL_FLAG_INVERT_Y | */SOIL_FLAG_COMPRESS_TO_DXT
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    m_tex_board=SOIL_load_OGL_texture
    (
        "data\\texture\\board.png",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        /*SOIL_FLAG_INVERT_Y | */SOIL_FLAG_COMPRESS_TO_DXT
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(m_tex_figures==0 || m_tex_board==0)
    {
        cout<<"ERROR: Could not load texture\n";
        return false;
    }


    return true;
}

bool game::load_sounds(void)
{
    cout<<"Game: Loading sound\n";

    m_pSound=new sound();

    bool error_flag=false;

    if( !m_pSound->load_WAVE_from_file( wav_beep1,"data\\sound\\beep1.wav" ) ) error_flag=true;

    if(error_flag)
    {
        cout<<"ERROR: Could not load sound\n";
        return false;
    }

    return true;
}

bool game::move_test(unit* pUnit,int target_x,int target_y)
{
    int unit_pos_x=pUnit->m_pos[0];
    int unit_pos_y=pUnit->m_pos[1];

    //no movement during move anim
    if(pUnit->m_move_timer>0)
     return false;

    bool allow_move=false;
    switch(pUnit->m_type)
    {
        case ut_pawn:
        {
            int direction=0;
            switch(pUnit->m_owner)
            {
                case 0: direction=0; break;
                case 1: direction=-1; break;
                case 2: direction=1; break;
            }

            //normal
            if(unit_pos_x==target_x && target_y==unit_pos_y+direction) allow_move=true;

            //first step
            if(unit_pos_x==target_x && target_y==unit_pos_y+direction*2 && !pUnit->m_has_moved) allow_move=true;

            //diagonal attack
            if(!allow_move)
            {
                for(int i=0;i<(int)m_vec_units.size();i++)
                {
                    if(  m_vec_units[i]->m_pos[1]==unit_pos_y+direction &&
                      ( (m_vec_units[i]->m_pos[0]==unit_pos_x+1) ||
                        (m_vec_units[i]->m_pos[0]==unit_pos_x-1) ) )
                    {
                        //target and unit pos should be same
                        if(m_vec_units[i]->m_pos[0]!=target_x ||
                           m_vec_units[i]->m_pos[1]!=target_y)
                         continue;

                        if(m_vec_units[i]->m_owner!=m_player_id)
                         allow_move=true;

                        break;
                    }
                }
            }

        }break;

        case ut_tower:
        {
            if(unit_pos_x==target_x || unit_pos_y==target_y)
            {
                //test path, end pos not tested
                bool x_move=false;//move along x-axis
                bool pos_move=false;
                if(unit_pos_x==target_x) x_move=true;
                if(target_x>unit_pos_x || target_y>unit_pos_y) pos_move=true;

                bool col_made=false;
                if(x_move)
                {
                    if(pos_move)
                    {
                        for(int y=unit_pos_y+1;y<target_y;y++)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==unit_pos_x &&
                                   m_vec_units[i]->m_pos[1]==y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                    else
                    {
                        for(int y=unit_pos_y-1;y>target_y;y--)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==unit_pos_x &&
                                   m_vec_units[i]->m_pos[1]==y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                }
                else//y move
                {
                    if(pos_move)
                    {
                        for(int x=unit_pos_x+1;x<target_x;x++)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==x &&
                                   m_vec_units[i]->m_pos[1]==unit_pos_y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                    else
                    {
                        for(int x=unit_pos_x-1;x>target_x;x--)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==x &&
                                   m_vec_units[i]->m_pos[1]==unit_pos_y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                }

                if(!col_made) allow_move=true;
            }

        }break;

        case ut_runner:
        {
            int x_dif=target_x-unit_pos_x;
            int y_dif=target_y-unit_pos_y;
            bool x_pos=false;
            bool y_pos=false;
            if(x_dif>0) x_pos=true;
            if(y_dif>0) y_pos=true;
            int steps=fabs(x_dif);

            if(fabs(x_dif)==fabs(y_dif) && x_dif!=0)
            {
                //test path, end pos not tested
                bool col_made=false;

                if(x_pos && y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x+dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y+dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }
                if(x_pos && !y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x+dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y-dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }
                if(!x_pos && y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x-dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y+dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }
                if(!x_pos && !y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x-dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y-dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }

                if(!col_made) allow_move=true;
            }

        }break;

        case ut_queen:
        {
            //runner
            int x_dif=target_x-unit_pos_x;
            int y_dif=target_y-unit_pos_y;
            bool x_pos=false;
            bool y_pos=false;
            if(x_dif>0) x_pos=true;
            if(y_dif>0) y_pos=true;
            int steps=fabs(x_dif);

            if(fabs(x_dif)==fabs(y_dif) && x_dif!=0)
            {
                //test path, end pos not tested
                bool col_made=false;

                if(x_pos && y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x+dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y+dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }
                if(x_pos && !y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x+dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y-dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }
                if(!x_pos && y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x-dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y+dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }
                if(!x_pos && !y_pos)
                {
                    for(int dif=1;dif<steps;dif++)
                    {
                        for(int i=0;i<(int)m_vec_units.size();i++)
                        {
                            if(m_vec_units[i]->m_pos[0]==unit_pos_x-dif &&
                               m_vec_units[i]->m_pos[1]==unit_pos_y-dif)
                            {
                                col_made=true;
                                break;
                            }
                        }
                        if(col_made) break;
                    }
                }

                if(!col_made) allow_move=true;
            }

            //tower
            if(unit_pos_x==target_x || unit_pos_y==target_y)
            {
                //test path, end pos not tested
                bool x_move=false;//move along x-axis
                bool pos_move=false;
                if(unit_pos_x==target_x) x_move=true;
                if(target_x>unit_pos_x || target_y>unit_pos_y) pos_move=true;

                bool col_made=false;
                if(x_move)
                {
                    if(pos_move)
                    {
                        for(int y=unit_pos_y+1;y<target_y;y++)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==unit_pos_x &&
                                   m_vec_units[i]->m_pos[1]==y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                    else
                    {
                        for(int y=unit_pos_y-1;y>target_y;y--)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==unit_pos_x &&
                                   m_vec_units[i]->m_pos[1]==y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                }
                else//y move
                {
                    if(pos_move)
                    {
                        for(int x=unit_pos_x+1;x<target_x;x++)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==x &&
                                   m_vec_units[i]->m_pos[1]==unit_pos_y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                    else
                    {
                        for(int x=unit_pos_x-1;x>target_x;x--)
                        {
                            for(int i=0;i<(int)m_vec_units.size();i++)
                            {
                                if(m_vec_units[i]->m_pos[0]==x &&
                                   m_vec_units[i]->m_pos[1]==unit_pos_y)
                                {
                                    col_made=true;
                                    break;
                                }
                            }
                            if(col_made) break;
                        }
                    }
                }

                if(!col_made) allow_move=true;
            }

        }break;

        case ut_king:
        {
            if( fabs(target_x-unit_pos_x)<=1 && fabs(target_y-unit_pos_y)<=1 )
            {
                allow_move=true;
            }

        }break;

        case ut_horse:
        {
            int x_dif=target_x-unit_pos_x;
            int y_dif=target_y-unit_pos_y;

            if( (x_dif==-1 && y_dif==-2) ||
                (x_dif==-1 && y_dif==2) ||
                (x_dif==1 && y_dif==-2) ||
                (x_dif==1 && y_dif==2) ||
                (x_dif==-2 && y_dif==-1) ||
                (x_dif==-2 && y_dif==1) ||
                (x_dif==2 && y_dif==-1) ||
                (x_dif==2 && y_dif==1) )
            {
                allow_move=true;
            }

        }break;
    }

    //collision test
    if(allow_move)
    {
        //cout<<"Move OK\n";
        return true;
    }

    return false;
}

bool game::send_data(st_event event)
{
    //convert to int array
    int data[8]={8*sizeof(int),
                 event.type,
                 event.tick_target,
                 event.owner,
                 event.x_from,
                 event.x_to,
                 event.y_from,
                 event.y_to};


    return m_pNetCom->send_data(data);
}

bool game::send_data(int* data_array)
{
    return m_pNetCom->send_data(data_array);
}

bool game::recv_data(SOCKET soc_sender)
{
    //int data_array[8]={0,0,0,0,0,0,0,0};
    for(int i=0;i<256;i++) m_data_array[i]=0;//requires 256 places to not interfere with others

    bool retval=m_pNetCom->recv_data(m_data_array);
    if(!retval) return false;

    //interpret package
    switch(m_data_array[1])//type
    {
        case et_error:
        {
            cout<<"ERROR: Received bad package\n";
            retval=false;
        }break;

        case et_unit_move:
        {
            //place event in confirmed events vector
            st_event event(m_data_array[2],
                           m_data_array[3],
                           m_data_array[4],
                           m_data_array[5],
                           m_data_array[6],
                           m_data_array[7]);
            event.confirmed=true;

            //test if package already received (reveresed order)
            bool move_order_already_in_history=false;
            for(int i=(int)m_vec_history_move.size()-1;i>=0;i--)
            {
                if(m_vec_history_move[i]==event)
                {
                    move_order_already_in_history=true;
                    cout<<"Received move order already received\n";
                    break;
                }
            }

            if(!move_order_already_in_history)
            {
                m_vec_events.push_back(event);

                //add to history
                m_vec_history_move.push_back(event);
            }

            //send confirmation package back
            m_data_array[1]=et_unit_move_confirmation;
            retval=send_data(m_data_array);

        }break;

        case et_unit_move_confirmation:
        {
            //translate to event
            st_event event(m_data_array[2],
                           m_data_array[3],
                           m_data_array[4],
                           m_data_array[5],
                           m_data_array[6],
                           m_data_array[7]);

            //locate event in not confirmed packages
            bool event_found=false;
            for(int i=0;i<(int)m_vec_events.size();i++)
            {
                if(m_vec_events[i]==event)
                {
                    event_found=true;

                    //check if already confirmed
                    if(m_vec_events[i].confirmed)
                    {
                        cout<<"Received package was already confirmed\n";
                    }
                    else
                    {
                        //confirm event
                        m_vec_events[i].confirmed=true;

                        //check history
                        bool move_order_already_in_history=false;
                        for(int i2=(int)m_vec_history_move.size()-1;i2>=0;i2--)
                        {
                            if(m_vec_history_move[i2]==m_vec_events[i])
                            {
                                move_order_already_in_history=true;
                                cout<<"Received move order confirmation already received\n";
                                break;
                            }
                        }

                        //add to history
                        if(!move_order_already_in_history)
                        {
                            m_vec_history_move.push_back(m_vec_events[i]);
                        }

                    }



                    break;
                }
            }

            if(!event_found)
            {
                cout<<"ERROR: Received event confirmation was not present in vector\n";
                if(m_print_log) m_log<<"ERROR: Received event confirmation was not present in vector\n";
                retval=false;
            }

        }break;

        case et_ping:
        {
            //server calc ping
            if(m_player_id==ps_hoster)
            {
                m_vec_pings.push_back(m_tick_counter-m_data_array[2]);
            }

            //client replies
            if(m_player_id==ps_joiner)
            {
                st_event event(m_data_array[2]);

                m_vec_events.push_back(event);
            }

        }break;

        case et_settings:
        {
            m_static_event_delay=m_data_array[2];
            m_event_resend_delay=m_data_array[3];

            cout<<"Received new send settings: "<<m_static_event_delay<<", "<<m_event_resend_delay<<endl;

            if(m_print_log)
            {
                m_log<<"Event delay: "<<m_static_event_delay<<"\tResend delay: "<<m_event_resend_delay<<endl;
            }

        }break;

        case et_rollback:
        {
            //rollback requested by the other player, place in event vector
            st_event event(m_data_array[2],
                           m_data_array[3],
                           m_data_array[4],
                           m_data_array[5],
                           m_data_array[6],
                           m_data_array[7]);
            //event.confirmed=true;
            m_vec_events.push_back(event);

            //send confirmation now
            event.type=et_rollback_confirmation;
            send_data(event);

        }break;

        case et_rollback_confirmation:
        {
            //the requested rollback is now confirmed by the other player, place in event vector
            st_event event(m_data_array[2],
                           m_data_array[3],
                           m_data_array[4],
                           m_data_array[5],
                           m_data_array[6],
                           m_data_array[7]);
            //event.confirmed=true;
            m_vec_events.push_back(event);

        }break;
    }
    //if print data to console
    if(true)
    {
        for(int i=0;i<8;i++)
        {
            cout<<m_data_array[i]<<"\t";
        }
        cout<<endl;
    }
    //print data to file
    if(m_print_log)
    {
        time_t t = time(0);// get time now
        struct tm * now = localtime( & t );
        stringstream ss;
        if((now->tm_hour)<10) ss<<"0";
        ss<<now->tm_hour;
        if((now->tm_min)<10) ss<<"0";
        ss<<now->tm_min;
        if((now->tm_sec)<10) ss<<"0";
        ss<<now->tm_sec;
        string time_now=ss.str();
        m_log<<time_now<<"_";
        for(int i=0;i<8;i++)
        {
            m_log<<m_data_array[i]<<"\t";
        }
        m_log<<endl;
    }

    return retval;
}

bool game::rollback(int end_tick)
{
    cout<<"Game rollback in progress\n";
    m_rollback_tick_target=end_tick;

    //reset units
    m_pSelected_unit=NULL;
    for(int i=0;i<(int)m_vec_units.size();i++)
    {
        delete m_vec_units[i];
    }
    m_vec_units.clear();
    m_vec_events.clear();


    //place new units
    {
    m_vec_units.push_back(new unit(0,0,ut_tower,2,m_tex_figures));
    m_vec_units.push_back(new unit(1,0,ut_horse,2,m_tex_figures));
    m_vec_units.push_back(new unit(2,0,ut_runner,2,m_tex_figures));
    m_vec_units.push_back(new unit(3,0,ut_king,2,m_tex_figures));
    m_vec_units.push_back(new unit(4,0,ut_queen,2,m_tex_figures));
    m_vec_units.push_back(new unit(5,0,ut_runner,2,m_tex_figures));
    m_vec_units.push_back(new unit(6,0,ut_horse,2,m_tex_figures));
    m_vec_units.push_back(new unit(7,0,ut_tower,2,m_tex_figures));
    m_vec_units.push_back(new unit(0,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(1,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(2,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(3,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(4,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(5,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(6,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(7,1,ut_pawn,2,m_tex_figures));
    m_vec_units.push_back(new unit(0,7,ut_tower,1,m_tex_figures));
    m_vec_units.push_back(new unit(1,7,ut_horse,1,m_tex_figures));
    m_vec_units.push_back(new unit(2,7,ut_runner,1,m_tex_figures));
    m_vec_units.push_back(new unit(3,7,ut_queen,1,m_tex_figures));
    m_vec_units.push_back(new unit(4,7,ut_king,1,m_tex_figures));
    m_vec_units.push_back(new unit(5,7,ut_runner,1,m_tex_figures));
    m_vec_units.push_back(new unit(6,7,ut_horse,1,m_tex_figures));
    m_vec_units.push_back(new unit(7,7,ut_tower,1,m_tex_figures));
    m_vec_units.push_back(new unit(0,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(1,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(2,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(3,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(4,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(5,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(6,6,ut_pawn,1,m_tex_figures));
    m_vec_units.push_back(new unit(7,6,ut_pawn,1,m_tex_figures));
    }

    //sort history
    while(true)
    {
        bool updated=false;
        for(int i=0;i<(int)m_vec_history_move.size()-1;i++)
        {
            if(m_vec_history_move[i].tick_target >
               m_vec_history_move[i+1].tick_target)
            {
                updated=true;
                //swap
                st_event temp(m_vec_history_move[i]);
                m_vec_history_move[i]=m_vec_history_move[i+1];
                m_vec_history_move[i+1]=temp;
            }
        }

        if(!updated) break;
    }

    //remove history at target tick or later
    for(int i=0;i<(int)m_vec_history_move.size();i++)
    {
        if(m_vec_history_move[i].tick_target>=end_tick)
        {
            m_vec_history_move.erase(m_vec_history_move.begin()+i,m_vec_history_move.end()-1);
            break;
        }
    }

    //repeat history
    for(int i=0;i<(int)m_vec_history_move.size();i++)
    {
        switch(m_vec_history_move[i].type)
        {
            case et_unit_move:
            {
                //check if other unit taken
                for(int unit_i=0;unit_i<(int)m_vec_units.size();unit_i++)
                {
                    if(m_vec_units[unit_i]->m_pos[0]==m_vec_history_move[i].x_to &&
                       m_vec_units[unit_i]->m_pos[1]==m_vec_history_move[i].y_to)
                    {
                        m_vec_units[unit_i]->m_remove=true;

                        //king test
                        if(!m_game_over)
                        {
                            if(m_vec_units[unit_i]->m_type==ut_king)
                            {
                                m_game_over=true;

                                cout<<"***************\n** GAME OVER **\n***************\n";
                                if(m_vec_units[unit_i]->m_owner==ps_joiner) cout<<"-- HOST WON --\n";
                                if(m_vec_units[unit_i]->m_owner==ps_hoster) cout<<"--CLIENT WON--\n";
                            }
                        }
                    }
                }

                //find unit
                bool unit_found=false;
                for(int unit_i=0;unit_i<(int)m_vec_units.size();unit_i++)
                {
                    if(m_vec_units[unit_i]->m_pos[0]==m_vec_history_move[i].x_from &&
                       m_vec_units[unit_i]->m_pos[1]==m_vec_history_move[i].y_from)
                    {
                        unit_found=true;

                        //move unit directly
                        m_vec_units[unit_i]->m_pos_prev[0]=m_vec_units[unit_i]->m_pos[0]=m_vec_history_move[i].x_to;
                        m_vec_units[unit_i]->m_pos_prev[1]=m_vec_units[unit_i]->m_pos[1]=m_vec_history_move[i].y_to;

                        //m_vec_units[unit_i]->m_has_moved=true;
                        //m_vec_units[unit_i]->m_move_timer=1.0;

                        break;
                    }
                }

                if(!unit_found)
                {
                    cout<<"ERROR: Rollback: Event unit move could not find unit\n";
                }

            }break;
        }

        //remove inits
        for(int i=0;i<(int)m_vec_units.size();i++)
        {
            if(m_vec_units[i]->m_remove)
            {
                delete m_vec_units[i];
                m_vec_units.erase(m_vec_units.begin()+i);
                i--;
            }
        }
    }

    return true;
}
