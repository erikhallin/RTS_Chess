#ifndef UNIT_H
#define UNIT_H

#include <gl/gl.h>

#include "definitions.h"

enum unit_types
{
    ut_pawn=0,
    ut_horse,
    ut_runner,
    ut_tower,
    ut_queen,
    ut_king
};

class unit
{
    public:
        unit(int x_pos,int y_pos,int type,int owner,int tex);

        int m_type,m_owner;
        int m_pos[2];
        int m_pos_prev[2];
        int m_texture;
        bool m_marked,m_remove,m_has_moved;
        float m_move_timer;

        bool init(void);
        bool update(void);
        bool draw(float pos2pix);

    private:
};

#endif // UNIT_H
