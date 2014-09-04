#include <cmath>

#include "Config.h"
#include "Grid.h"
#include "SoundMgr.h"
#include "Plane.h"

namespace fc {

    std::map<int, std::shared_ptr<Grid> > GridPool::m;

    GridPool::grid_ptr GridPool::Find(int id) {
        auto it = m.find(id);

        if (it == m.end())
            return grid_ptr();

        return it->second;
    }

    Grid::Grid(int x_, int y_, int id_, EnPlayerColor color_) {
        x = x_;
        y = y_;
        id = id_;
        color = color_;
        place_plane = NULL;
    }

    void Grid::Init(int pre_, int next_, int jump_) {
        pre = pre_;
        next = next_;
        jump = jump_;
    }

    float Grid::GetPositionX() {
        return Config::GetInstance().GridCfg.MarginLeft + (x - 0.5f) * Config::GetInstance().GridCfg.Width;
    }

    float Grid::GetPositionY() {
        return Config::GetInstance().GridCfg.MarginTop + (16 - y - 0.5f) * Config::GetInstance().GridCfg.Height;
    }


    void Grid::Reset() {
        place_plane = NULL;
    }

    int Grid::GetScore(EnPlayerColor color, int point, int left_jump, int left_fly) {
        if (0 != point) {
            auto next = GetNextGrid();
            return next->GetScore(color, point - point / abs(point), left_jump, left_fly) + Config::GetInstance().AICfg.Move;
        }

        int ret = 0;
        if (NULL != place_plane){
            if (place_plane->Color() == color)
                ret += Config::GetInstance().AICfg.KillFriend;
            else
                ret += Config::GetInstance().AICfg.KillEnermy;
        }

        if (color == Color() && left_jump > 0) {
            ret += Config::GetInstance().AICfg.Jump;
            auto jump = GetJumpGrid();
            if (jump)
                ret += jump->GetScore(color, point, left_jump - 1, left_fly);
        }

        return ret;
    }

    void Grid::OnLeave(Plane& plane, int reason) {
        if (reason & LGR_MOVE_START) {
            // ����������Ч
            SoundMgr::GetInstance().PlayMove();
        }

        if (reason & LGR_KICKOFF) {
            // ���ű�������Ч
            SoundMgr::GetInstance().PlayGoHome();
        }

        if (reason & LGR_WIN) {
            // ���ŵ����յ���Ч
            SoundMgr::GetInstance().PlayJump();
        }

        if (reason & LGR_JUMP) {
            // ������Ծ��Ч
            SoundMgr::GetInstance().PlayJump();
        }

        place_plane = NULL;
    }

    Grid::grid_ptr Grid::OnPass(Plane& plane, int& left_point) {
        grid_ptr move_to;
        if (left_point > 0) {
            --left_point;
            move_to = GetNextGrid();
        }

        if (left_point < 0) {
            ++left_point;
            move_to = GetPreviousGrid();
        }

        // ��ӷɻ��˶�����
        MoveHere(plane, plane.GetSpeed(), 0.0f);

        if (!move_to)
            return GridPool::Find(ID());

        return move_to;
    }

    void Grid::OnArrive(Plane& plane) {
        Plane* old_place_plane = place_plane;
        // ��ӷɻ��˶�����
        MoveHere(plane, plane.GetSpeed(), 0.0f);

        // �������е�Plane
        if (NULL != old_place_plane) {
            // �����ٷɻ��ؼ�
            old_place_plane->GoHome(LGR_KICKOFF);
        }
        place_plane = &plane;


        arrive_event(plane);

        // ������Ծ
        if (Color() == plane.Color() && plane.CanJump() && jump != ID()) {
            // ��Ծ����
            plane.JumpTo(jump);
        }

        // ʤ������
    }

    void Grid::OnCross(Plane& plane) {
        // �������е�Plane
        if (NULL != place_plane) {
            // �����ٷɻ��ؼ�
            plane.GoHome(LGR_KICKOFF);
        }

    }


    Grid::grid_ptr Grid::GetNextGrid() {
        return GridPool::Find(next);
    }

    Grid::grid_ptr Grid::GetPreviousGrid() {
        return GridPool::Find(pre);
    }

    Grid::grid_ptr Grid::GetJumpGrid() {
        return GridPool::Find(jump);
    }

    void Grid::MoveHere(Plane& plane, float speed, float delay_time) {
        if (ID() == plane.GetLocateGridID()) {
            plane.AddAnimationAction(*this, 0.0f, delay_time);
            return;
        }

        // �ɻ��ƶ�����
        grid_ptr locate = GridPool::Find(plane.GetLocateGridID());
        assert(locate);

        cocos2d::Vec2 from, to;
        from.set(locate->GetPositionX(), locate->GetPositionY());
        to.set(GetPositionX(), GetPositionY());

        plane.AddAnimationAction(*this, to.distance(from) / speed, delay_time);
    }

    void Grid::arrive_event(Plane& plane) {
    }
}