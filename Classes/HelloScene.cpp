#include <cstdio>

#include "HelloScene.h"
#include "GameScene.h"
#include "VisibleRect.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "cocostudio/CCSSceneReader.h"
#include "cocostudio/CCSGUIReader.h"
#include "cocostudio/CCActionManagerEx.h"
#include "cocostudio/CCArmatureDataManager.h"

#include "Player.h"
#include "Judger.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

Scene* HelloScene::scene()
{
    fc::Judger::GetInstance().Reset();

    // 'scene' is an autorelease object
    Scene *scene = Scene::create();

    // 'layer' is an autorelease object
    HelloScene *layer = HelloScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return TransitionFade::create(1.2f, scene);
}

// on "init" you need to initialize your instance
bool HelloScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    // ui animation root from json
    m_pHelloLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("publish/hello_panel.ExportJson"));
    {
        m_pHelloLayout->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type){
            if (Widget::TouchEventType::BEGAN == type) {
                auto me = static_cast<Widget*>(pSender);
                this->m_stTouchMoveStartPosition = me->getPosition();
            } else if (Widget::TouchEventType::MOVED == type) {
                auto me = static_cast<Widget*>(pSender);
                auto move_end = me->getTouchMovePosition();
                auto move_begin = me->getTouchBeganPosition();
                auto my_pos = this->m_stTouchMoveStartPosition + move_end - move_begin;
                me->setPosition(my_pos);
            }
        });

        auto my_size = getContentSize();
        m_pHelloLayout->setAnchorPoint(Vec2(0.5, 0.5));
        m_pHelloLayout->setPosition(Vec2(my_size.width / 2, my_size.height / 2));
    }
    addChild(m_pHelloLayout);

    // ��ʼ��ť
    {
        Button* start_button = static_cast<Button*>(Helper::seekWidgetByName(m_pHelloLayout, "Button_StartGame"));
        start_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type){
            if (type == Widget::TouchEventType::ENDED) {
                // ��ʼ��Ϸ���л�scene
                Director::getInstance()->replaceScene(GameScene::scene());
            }
        });
    }

    // ���״̬�л���ť
    {
        auto player_status_btn = [](cocos2d::ui::Layout* m_pHelloLayout, fc::EnPlayerColor color) {
            char btn_name[32] = { 0 };
            sprintf(btn_name, "Button_Player_Color_%d", color);

            Button* player_button = static_cast<Button*>(Helper::seekWidgetByName(m_pHelloLayout, btn_name));
            //std::function<void(Ref*, Widget::TouchEventType)>
            auto evt_func = [player_button, color](Ref* pSender, Widget::TouchEventType type){
                if (type == Widget::TouchEventType::ENDED) {
                    fc::PlayerSelecter ps = fc::Player::Pool[color].GetStatus();
                    ps = static_cast<fc::PlayerSelecter>((ps + 1) % fc::PS_MAX);
                    fc::Player::Pool[color].SetStatus(ps);

                    std::string name = fc::Player::Pool[color].GetPlayerName();
                    if (fc::PS_DISABLE == ps) {
                        name += ": None";
                    }
                    else if (fc::PS_HUMEN == ps) {
                        name += ": Player";
                    }
                    else if (fc::PS_AI == ps) {
                        name += ": AI";
                    }
                    else {
                        name += ": Invalid";
                    }

                    player_button->setTitleText(name);
                }
            };

            player_button->addTouchEventListener(evt_func);
            evt_func(NULL, Widget::TouchEventType::ENDED);
        };


        for (int i = 0; i < fc::EPC_MAX; ++i) {
            player_status_btn(m_pHelloLayout, static_cast<fc::EnPlayerColor>(i));
        }
    }

    // �ɻ�����ѡ���
    {

    }

    // ���а�
    {
        fc::EnPlayerColor player_ranking[fc::EPC_MAX];
        fc::Player::SortByRanking(player_ranking);

        auto player_rk = static_cast<Text*>(Helper::seekWidgetByName(m_pHelloLayout, "Label_Ranking_1"));
        auto color = fc::Player::Pool[player_ranking[0]].GetPlayerColor();
        player_rk->setColor(Color3B(std::get<0>(color), std::get<1>(color), std::get<2>(color)));
        player_rk->setString(std::string(" 1st: ") + fc::Player::Pool[player_ranking[0]].GetPlayerName());

        player_rk = static_cast<Text*>(Helper::seekWidgetByName(m_pHelloLayout, "Label_Ranking_2"));
        color = fc::Player::Pool[player_ranking[1]].GetPlayerColor();
        player_rk->setColor(Color3B(std::get<0>(color), std::get<1>(color), std::get<2>(color)));
        player_rk->setString(std::string(" 2nd: ") + fc::Player::Pool[player_ranking[1]].GetPlayerName());

        player_rk = static_cast<Text*>(Helper::seekWidgetByName(m_pHelloLayout, "Label_Ranking_3"));
        color = fc::Player::Pool[player_ranking[2]].GetPlayerColor();
        player_rk->setColor(Color3B(std::get<0>(color), std::get<1>(color), std::get<2>(color)));
        player_rk->setString(std::string(" 3rd: ") + fc::Player::Pool[player_ranking[2]].GetPlayerName());
    }

    scheduleUpdate();

    return true;
}

void HelloScene::menuCloseCallback(Ref* pSender)
{
    //cocostudio::ArmatureDataManager::destroyInstance();
    //cocostudio::SceneReader::destroyInstance();
    cocostudio::GUIReader::destroyInstance();
    //cocostudio::ActionManagerEx::destroyInstance();

    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloScene::animationEvent(Armature *pArmature,
    MovementEventType movementType, const char *movementID)
{
    //ComRender *render = (ComRender*) (m_pGameScene->getChildByTag(10006)->getComponent("CCArmature"));
    //Armature *pEnemy = (Armature*) (render->getNode());

    //if (movementType == COMPLETE)
    //{
    //    pEnemy->getAnimation()->play("death");
    //    pArmature->getAnimation()->setMovementEventCallFunc(NULL, NULL);
    //}

}

void HelloScene::update(float delta)
{
    //if (m_bStart)
    //{
    //    ComRender *pHeroRender = (ComRender*) (m_pGameScene->getChildByTag(10005)->getComponent("CCArmature"));
    //    Armature *pHero = (Armature*) (pHeroRender->getNode());
    //    pHero->getParent()->setPositionX(pHero->getParent()->getPositionX() + m_fSpeed);

    //    ComRender *pEnemyRender = (ComRender*) (m_pGameScene->getChildByTag(10006)->getComponent("CCArmature"));
    //    Armature *pEnemy = (Armature*) (pEnemyRender->getNode());

    //    /* for 3.0 */
    //    float distance = Point(pHero->getParent()->getPositionX(), 0).getDistance(Point(pEnemy->getParent()->getPositionX(), 0));
    //    if (distance < m_fAttackDis)
    //    {
    //        pHero->getAnimation()->play("attack");
    //        pHero->getAnimation()->setMovementEventCallFunc(this,
    //            movementEvent_selector(HelloScene::animationEvent));
    //        m_bStart = false;
    //    }
    //    // before
    //    /*
    //    if(ccpDistance(ccp(pHero->getParent()->getPositionX(), 0), ccp(pEnemy->getParent()->getPositionX(), 0)) < m_fAttackDis)
    //    {
    //    pHero->getAnimation()->play("attack");
    //    pHero->getAnimation()->setMovementEventCallFunc(this,
    //    movementEvent_selector(HelloScene::animationEvent));
    //    m_bStart = false;
    //    }
    //    */
    //    /**/
    //}

    //if (m_bDead)
    //{
    //    ComRender *pUIRender = static_cast<ComRender*>(m_pGameScene->getChildByTag(10007)->getComponent("GUIComponent"));


    //    /* for 3.0 */
    //    Node* pUILayer = static_cast<Node*>(pUIRender->getNode());
    //    Layout* root = static_cast<Layout*>(pUILayer->getChildren().at(2));
    //    LoadingBar *pHPLoadingBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(root, "hp02_LoadingBar"));
    //    LoadingBar *pMPLoadingBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(root, "mp02_LoadingBar"));
    //    // before
    //    /*
    //    cocos2d::ui::TouchGroup *pUILayer = static_cast<cocos2d::ui::TouchGroup*>(pUIRender->getNode());
    //    cocos2d::ui::LoadingBar *pHPLoadingBar = static_cast<cocos2d::ui::LoadingBar*>(pUILayer->getWidgetByName("hp02_LoadingBar"));
    //    cocos2d::ui::LoadingBar *pMPLoadingBar = static_cast<cocos2d::ui::LoadingBar*>(pUILayer->getWidgetByName("mp02_LoadingBar"));
    //    */
    //    /**/

    //    pHPLoadingBar->setPercent(m_fPercentage);
    //    pMPLoadingBar->setPercent(m_fPercentage);

    //    m_fPercentage -= 2.0f;
    //    if (m_fPercentage < 0.0f) {
    //        unscheduleUpdate();
    //    }
    //}

}

void HelloScene::updateData() {
}

void HelloScene::startGame(Ref* pSender, TouchEventType type)
{
    if (type == TOUCH_EVENT_ENDED)
    {
        
    }
}