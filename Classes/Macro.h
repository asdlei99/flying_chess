#ifndef _FLY_CHESS_MACRO_H_
#define _FLY_CHESS_MACRO_H_

namespace fc {
    enum EnPlayerColor {
        EPC_GREEN = 0,
        EPC_BLUE,
        EPC_YELLOW,
        EPC_PINK,

        EPC_MAX
    };

    enum EnPlane {
        EP_FIRST = 0,
        EP_SECOND,
        EP_THIRD,
        EP_FOURTH,

        EP_MAX
    };

    enum LeaveGridReason {
        LGR_MOVE_START = 0x0001, // �����ƶ�����
        LGR_KICKOFF = 0x0002,    // ���ٲ����߻ؼ�
        LGR_WIN = 0x0004,        // �����յ�
        LGR_PASS = 0x0008,       // ·��
        LGR_JUMP = 0x0010,       // ��Ծ
    };

    enum PlayerSelecter {
        PS_DISABLE = 0,
        PS_HUMEN,
        PS_AI,
        PS_MAX
    };

    enum JudgeStatus {
        JS_NONE = 0,
        JS_ROLL_POINTS,
        JS_SELECT_PLANE
    };

}
#endif /* defined(_FLY_CHESS_MACRO_H_) */
