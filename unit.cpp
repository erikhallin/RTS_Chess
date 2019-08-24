#include "unit.h"

unit::unit(int x_pos,int y_pos,int type,int owner,int tex)
{
    m_type=type;
    m_owner=owner;
    m_texture=tex;
    m_pos_prev[0]=m_pos[0]=x_pos;
    m_pos_prev[1]=m_pos[1]=y_pos;
    m_marked=false;
    m_remove=false;
    m_has_moved=false;
    m_move_timer=0;
}

bool unit::init(void)
{


    return true;
}

bool unit::update(void)
{
    m_marked=false;

    if(m_move_timer>0)
    {
        m_move_timer-=_time_step*_unit_move_anim_speed;
        if(m_move_timer<=0.0)
        {
            //done
            m_move_timer=0;

            m_pos_prev[0]=m_pos[0];
            m_pos_prev[1]=m_pos[1];
        }
    }

    return true;
}

bool unit::draw(float pos2pix)
{
    float tex_off_x=(float)m_type/6.0;
    float tex_off_y=(float)(m_owner-1)/2.0;

    float draw_pos_x=m_pos[0]*pos2pix;
    float draw_pos_y=m_pos[1]*pos2pix;

    //move anim
    if(m_move_timer>0)
    {
        draw_pos_x+=(m_pos_prev[0]*pos2pix-m_pos[0]*pos2pix)*m_move_timer;
        draw_pos_y+=(m_pos_prev[1]*pos2pix-m_pos[1]*pos2pix)*m_move_timer;
    }

    //rotate view
    glPushMatrix();
    //glTranslatef(pos2pix,pos2pix,0);
    //glRotatef(180,0,0,1);
    //glTranslatef(pos2pix,pos2pix,0);

    glColor3f(1,1,1);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D,m_texture);
    glBegin(GL_QUADS);
    glTexCoord2f(tex_off_x,tex_off_y);
    glVertex2f(draw_pos_x,draw_pos_y);
    glTexCoord2f(tex_off_x,tex_off_y+0.5);
    glVertex2f(draw_pos_x,draw_pos_y+pos2pix);
    glTexCoord2f(tex_off_x+1.0/6.0,tex_off_y+0.5);
    glVertex2f(draw_pos_x+pos2pix,draw_pos_y+pos2pix);
    glTexCoord2f(tex_off_x+1.0/6.0,tex_off_y);
    glVertex2f(draw_pos_x+pos2pix,draw_pos_y);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    //marked
    if(m_marked)
    {
        glColor3f(1,0,0);
        glLineWidth(3);
        glBegin(GL_LINE_STRIP);
        glVertex2f(m_pos[0]*pos2pix,m_pos[1]*pos2pix);
        glVertex2f(m_pos[0]*pos2pix,m_pos[1]*pos2pix+pos2pix);
        glVertex2f(m_pos[0]*pos2pix+pos2pix,m_pos[1]*pos2pix+pos2pix);
        glVertex2f(m_pos[0]*pos2pix+pos2pix,m_pos[1]*pos2pix);
        glVertex2f(m_pos[0]*pos2pix,m_pos[1]*pos2pix);
        glEnd();
        glLineWidth(1);
    }

    glPopMatrix();

    return true;
}

