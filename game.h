#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <fstream>
#include <SOIL/SOIL.h>
#include <gl/gl.h>
#include <string>
#include <vector>
#include <math.h>
#include <ctime>


#include "sound.h"
#include "gamepad.h"
#include "key_reroute.h"
#include "unit.h"
#include "networkCom.h"

enum game_states
{
    gs_init=0,
    gs_menu,
    gs_in_game
};

enum player_status_types
{
    ps_none=0,
    ps_hoster,
    ps_joiner
};

enum event_types
{
    et_error=0,
    et_unit_move,
    et_unit_move_confirmation,
    et_game_over,
    et_ping,
    et_settings,
    et_rollback,//request
    et_rollback_confirmation//sent back to rollback requester when confirmed
};

struct st_event
{
    st_event(const st_event& _event)
    {
        tick_target=_event.tick_target;
        owner=_event.owner;
        x_from=_event.x_from;
        x_to=_event.x_to;
        y_from=_event.y_from;
        y_to=_event.y_to;
        type=_event.type;
        confirmed=_event.confirmed;
        resend_counter=_event.resend_counter;
    }

    st_event(int tick)
    {
        type=et_ping;
        tick_target=tick;
        confirmed=false;
        x_from=x_to=y_from=y_to=0;
        resend_counter=0;
        owner=0;
    }

    st_event(int static_event_delay,int event_resend_delay)
    {
        type=et_settings;
        tick_target=static_event_delay;
        owner=event_resend_delay;
        confirmed=false;
        x_from=x_to=y_from=y_to=0;
        resend_counter=0;
    }

    st_event(int tick,int _owner,int xfrom,int xto,int yfrom,int yto)
    {
        tick_target=tick;
        owner=_owner;
        x_from=xfrom;
        x_to=xto;
        y_from=yfrom;
        y_to=yto;
        type=et_unit_move;
        confirmed=false;
        resend_counter=-100;
    }

    bool operator==(st_event& _event)
    {
        return (_event.tick_target==tick_target &&
                _event.owner==owner &&
                _event.x_from==x_from &&
                _event.x_to==x_to &&
                _event.y_from==y_from &&
                _event.y_to==y_to );
    }

    st_event operator=(const st_event& _event)
    {
        tick_target=_event.tick_target;
        owner=_event.owner;
        x_from=_event.x_from;
        x_to=_event.x_to;
        y_from=_event.y_from;
        y_to=_event.y_to;
        type=_event.type;
        confirmed=_event.confirmed;
        resend_counter=_event.resend_counter;

        return *this;
    }



    int tick_target;
    int owner;
    int x_from,x_to,y_from,y_to;
    int type;
    bool confirmed;
    int resend_counter;
};

class game
{
    public:

        game();

        networkCom* m_pNetCom;
        int   m_tick_counter;
        int   m_player_id;
        bool  m_game_started,m_game_over,m_pause_game;

        bool init(int* pWindow_size,bool* pKeys_real,bool* pKeys_translated,
                  int* pMouse_pos,bool* pMouse_but,bool reinit=false);
        bool update(bool& quit_flag);
        bool draw(void);
        int  get_MP_settings(string& ip_and_port);
        bool recv_data(SOCKET soc_sender);
        bool start_game(void);

    private:

        float m_pix_per_unit;
        float m_ping_avg;
        int   m_mp_status;
        string m_host_ip;
        int   m_static_event_delay=100;
        int   m_event_resend_delay=30;

        int   m_game_state;
        int   m_window_size[2];
        bool* m_pKeys_real;
        bool* m_pKeys_translated;
        int*  m_pMouse_pos;
        bool* m_pMouse_but;
        bool  m_gamepad_connected[4];
        bool  m_LMB_trig;
        int   m_game_start_countdown;
        float m_game_start_countdown_timer;
        float m_automove_timer,m_automove_delay;
        bool  m_automove_on;

        vector<unit*>    m_vec_units;
        vector<st_event> m_vec_events;
        vector<int>      m_vec_pings;
        vector<st_event> m_vec_history_move;
        vector<int>      m_vec_rollbacks_to_ignore;

        bool  m_rollback_unconfirmed;
        int   m_rollback_tick_target;
        unit* m_pSelected_unit;
        int   m_arr_board_movements[8][8];
        int   m_data_array[256];

        //texture
        int   m_tex_figures,m_tex_board;

        //object
        key_reroute m_key_rerouter;
        sound*      m_pSound;
        gamepad     m_gamepad[4];

        ofstream m_log;
        bool     m_print_log;

        bool  load_textures(void);
        bool  load_sounds(void);
        bool  move_test(unit* pUnit,int target_x,int target_y);
        bool  send_data(st_event event);
        bool  send_data(int* data_array);
        bool  rollback(int end_tick);

};

#endif // GAME_H
