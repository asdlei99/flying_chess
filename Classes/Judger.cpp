
#include "Config.h"
#include "Plane.h"
#include "Player.h"
#include "RollPoints.h"
#include "Judger.h"

#include "cocos2d.h"
#include "GameScene.h"

namespace fc {


    Judger::Judger(){
        
    }

    void Judger::Reset(bool clear) {
        cache_action.status = JS_NONE;
        cache_action.player = EPC_PINK;


        // reset all player
        PlayerSelecter ps[EPC_MAX] = {fc::PS_DISABLE};
        for (int i = 0; i < EPC_MAX && !clear; ++i) {
            ps[i] = Player::Pool[i].GetStatus();
        }

        if (clear) {
            plane_number = EP_MAX;
            RollPoints::GetInstance().Reset();
        }

        Player::ResetAll(ps, plane_number);
        events.clear();
    }

    bool Judger::OnRollPointBegin() {
        if (events.empty() || events.front().status != JS_ROLL_POINTS) {
            return false;
        }

        cache_action = events.front();
        events.front().status = JS_NONE; // ����Ӧ

        RollPoints::GetInstance().RollPoint();
        return true;
    }

    bool Judger::OnRollPointEnd(int point) {
        if (events.empty() || events.front().status != JS_NONE) {
            cocos2d::log("[ERROR]: Roll points end but action error.");
            return false;
        }
        cache_points = point;

        // �ӳ����Ŀ��Զ���һ��
        if (6 == point) {
            auto it = events.begin();
            ++it;
            events.insert(it, action_event(JS_ROLL_POINTS, cache_action.player));
        }

        auto iter = events.begin();
        ++iter;
        events.insert(iter, action_event(JS_SELECT_PLANE, cache_action.player));

        DoNext();
        return true;
    }

    bool Judger::OnSelectPlane(Plane& plane) {
        if (events.empty() || events.front().status != JS_SELECT_PLANE) {
            return false;
        }

        // ��ɫ��ƥ��
        if (plane.Color() != cache_action.player) {
            return false;
        }

        // ���Ӳ����û���ʤ��
        if (!plane.IsAvailable() || plane.IsWin()) {
            return false;
        }

        // ����δ����
        if (6 != cache_points && !plane.IsStarted()) {
            return false;
        }

        cache_action = events.front();
        events.front().status = JS_NONE; // ����Ӧ

        plane.Move(cache_points);
        return true;
    }

    void Judger::DoNext() {
        // ����actionִ����ϣ�����
        if (!events.empty() && JS_NONE == events.front().status)
            events.pop_front();

        // ������һ��action
        next_action();
    }

    void Judger::SetPlaneNumber(int n) {
        plane_number = n % (EP_MAX + 1);
    }

    EnPlayerColor Judger::GetTopActionPlayer() const {
        if (events.empty()) {
            cocos2d::log("[WARNING]: Get top action but judger has no actions.");
            return EPC_GREEN;
        }

        return events.front().player;
    }

    void Judger::next_action() {
        if (events.empty())
            next_player();

        // game over
        if (events.empty())
            return;

        if (JS_ROLL_POINTS == events.front().status)
            RollPoints::GetInstance().OnUpdate();

        // AI
        action_event evt = events.front();
        if (PS_AI != Player::Pool[evt.player].GetStatus()) {
            return;
        }

        // roll points
        if (JS_ROLL_POINTS == evt.status) {
            // ģ��������
            OnRollPointBegin();

        } else if (JS_SELECT_PLANE == evt.status) {
            // TODO ѡ��һ���ɻ�
        }
    }

    void Judger::next_player() {
        int player_index = (cache_action.player + 1) % EPC_MAX;
        int available_num = 0, select_index = EPC_MAX;
        for (int i = 0; i < EPC_MAX; ++i, player_index = (player_index + 1) % EPC_MAX) {
            if (!Player::Pool[player_index].IsAvailable())
                continue;

            if (Player::Pool[player_index].IsGameOver())
                continue;

            if (0 == available_num)
                select_index = player_index;
            ++available_num;
        }

        if (select_index >= EPC_MAX || available_num < 1) {
            gameover();
            return;
        }

        events.push_back(action_event(JS_ROLL_POINTS, (EnPlayerColor) select_index));
    }

    void Judger::gameover() {
        GameScene* game_scene = dynamic_cast<GameScene*>(cocos2d::Director::getInstance()->getRunningScene());
        if (NULL != game_scene)
            game_scene->onGameOver();
    }
}