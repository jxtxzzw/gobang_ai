#include <bits/stdc++.h>

using namespace std;

// 宏定义，如果定义了DEBUG_USE_ONLY，则在运行到关键步骤的时候会在Console输出相关信息，便于调试，实际运行时注释掉这行，否则会比较慢
//#define DEBUG_USE_ONLY

#define GRID_NUM 15

#define BLACK 1
#define WHITE 0
#define NONE (-1)

const int POSITIVE_INFINITY = 0x3F3F3F3F;
const int NEGATIVE_INFINITY = -POSITIVE_INFINITY;

const int DIRECTION[4][4] = {{0,  -1, 0, 1},
                             {-1, 0,  1, 0},
                             {-1, -1, 1, 1},
                             {-1, 1,  1, -1}};


const int PosValue[GRID_NUM][GRID_NUM] =
        {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0},
                {0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0},
                {0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0},
                {0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0},
                {0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0},
                {0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1, 0},
                {0, 1, 2, 3, 4, 5, 6, 6, 6, 5, 4, 3, 2, 1, 0},
                {0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 4, 3, 2, 1, 0},
                {0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 3, 2, 1, 0},
                {0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0},
                {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0},
                {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        };

class Point {

public:
    int x;
    int y;

    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Point() = default;


    Point &operator=(const Point &other) = default;

    bool operator==(const Point &other) const {
        return (x == other.x) && (y == other.y);
    }

    bool operator<(const Point &other) const {
        if (x != other.x)
            return x < other.x;
        return y < other.y;
    }
};

class AI {

private:
    Point next;

    int depth = 1;

    int myColor = BLACK;

    bool isValid(int, int);

    void setPos(Point, int);

    int MINMAX_Search_With_AlphaBetaCutOff(int, int);

    int counter(int, int, int, int);

    int heuristic(int);

    bool nearby(int, int);

    int f(Point, int);

public:
    AI();

    ~AI();

    void setDep(int);

    int chessBoard[GRID_NUM][GRID_NUM];

    int getPos(Point);

    Point GetFirstStep();

    Point getNextStep(int);

    void setPos(int, int, int);

    bool isGameOver(int, int);
};

/**
 * 默认构造函数，清空棋盘，全部设置为NONE
 */
AI::AI() {
    for (int i = 0; i < GRID_NUM; ++i)
        for (int j = 0; j < GRID_NUM; ++j)
            chessBoard[i][j] = NONE;
}

AI::~AI() = default;

/**
 * 在给定的位置下棋
 * @param x 行号
 * @param y 列号
 * @param color 棋子颜色
 */
void AI::setPos(int x, int y, int color) {
    chessBoard[x][y] = color;
#ifdef DEBUG_USE_ONLY
    printf("Set (%d, %d) = %s\n", x, y, color == BLACK ? "BLACK" : color == WHITE ? "WHITE" : "NONE");
#endif
}

/**
 * 在给定的位置下棋
 * @param point 行列点对
 * @param color 棋子颜色
 */
void AI::setPos(Point point, int color) {
    setPos(point.x, point.y, color);
}

/**
 * 设置难度，即搜索的深度
 * @param depth 深度
 */
void AI::setDep(int depth) {
    this->depth = depth;
#ifdef DEBUG_USE_ONLY
    cout << "Set depth " << this->depth << endl;
#endif

}

/**
 * AI先手，产生第一步下棋的位置
 * @return 第一步下棋的位置
 */
Point AI::GetFirstStep() {
    // 第一步的最佳位置一定是“天元”
    Point TIAN_YUAN = Point(7, 7);
    setPos(TIAN_YUAN, myColor);
    return TIAN_YUAN;
}

/**
 * 计算给定的方向有多少个连着一样的颜色的棋子（不含起始点）
 * @param x 开始的行
 * @param y 开始的列
 * @param xDir 行坐标的变化量
 * @param yDir 列坐标的变化量
 * @return 有多少个一样颜色的棋子（不含起始点）
 */
int AI::counter(int x, int y, int xDir, int yDir) {
    int cnt = 0;
    int color = chessBoard[x][y]; // 获取起始点的颜色
    int r = x + xDir;
    int c = y + yDir;
    // 只要没有超过边界且颜色一样，就给行和列下标增加或减少对应的变化量
    while (isValid(r, c) && chessBoard[r][c] == color) {
        r += xDir;
        c += yDir;
        cnt++; // 一样的颜色多了一个
    }
    return cnt;
}

/**
 * 判断游戏是不是结束了
 * @param x 最后一步下的行号
 * @param y 最后一步下的列号
 * @return 如果游戏结束，返回true，否则，false
 */
bool AI::isGameOver(int x, int y) {
    // 分别对横向、纵向、对角线、反对角线计算连着的同色的棋子数量
    for (int dir = 0; dir < 4; dir++) {
        int cnt = 1; // 下下去的这个点肯定算一个数量
        cnt += counter(x, y, DIRECTION[dir][0], DIRECTION[dir][1]); // 分别加上两边的数量
        cnt += counter(x, y, DIRECTION[dir][2], DIRECTION[dir][3]);
        if (cnt >= 5)
            return true; // 如果某条线上同色的棋子数大于等于5，则有一方赢了
    }
    return false;
}

/**
 * 判断给定的行列有没有超过边界
 * @param x 行
 * @param y 列
 * @return 如果是合法的，返回true，否则，false
 */
bool AI::isValid(int x, int y) {
    return (x >= 0 && x < GRID_NUM && y >= 0 && y < GRID_NUM);
}

/**
 * 产生下一步
 * @param color 颜色
 * @return 下一步落子的行列点对
 */
Point AI::getNextStep(int color) {
    /*
     * 极大极小值搜索
     * 因为这个函数本身需要返回一个评估值以完成递归，所以落子点不能作为它的返回值，只能是由它去维护那个数据域成员
     */
    MINMAX_Search_With_AlphaBetaCutOff(0, color);
    // 落子，并将next返回
    setPos(next, color);
    return next;
}

/**
 * 给定的行列点对位置上是什么颜色的棋
 * @param p 行列点对
 * @return 棋子的颜色，黑棋为1、白棋为0、空为-1
 */
int AI::getPos(Point p) {
    return chessBoard[p.x][p.y];
}


/**
 * 尝试落子的位置是不是周围有棋子
 * @param x 行
 * @param y 列
 * @return 如果周围有棋子，返回true，否则，false
 */
/*
 * 一般情况下，我们落子要么落在自己已经有的棋子边上完成进攻，要么落在对方的棋子边上完成防守
 * 总之，落子点一定是棋盘上已经有的棋子的周围一定近距离，不会落在很远的周围都是空的位置
 */
bool AI::nearby(int x, int y) {
    // 检索周围的8个点，是不是有棋子
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            if (isValid(x + i, y + j) && chessBoard[x + i][y + j] != NONE)
                return true;
        }
    }
    // 注：必要的时候，邻居的定义可以放宽到2步以内
    return false;
}

/**
 * 极大极小值搜索
 * @param depth 当前的搜索深度
 * @param player 下棋者的棋子颜色
 * @return 评估值
 */
/*
 * 可以分别用两部分代码处理极大节点和极小节点两种情况
 * 其实，可以只用一部分代码，既处理极大节点也处理极小节点
 * 这里的评估函数是根据当前搜索节点来给出分数的
 * 每个人都会选取最大的分数，然后，返回到上一层节点时，会给出分数的相反数
 */
int AI::MINMAX_Search_With_AlphaBetaCutOff(int depth, int player) {

    int best = NEGATIVE_INFINITY; // 初值为负无穷大
    if (depth == this->depth) {
        return heuristic(player); // 搜到指定深度的地方，返回启发式函数的评估值
    }
    list<Point> children; // 链表记录下和当前棋盘上的点互为邻居的所有的点，这些点是我可能下棋的地方
    // 遍历，如果在邻居范围内，且当前为空，加入链表
    for (int i = 0; i < GRID_NUM; ++i)
        for (int j = 0; j < GRID_NUM; ++j) {
            if (chessBoard[i][j] == NONE && nearby(i, j)) {
                children.emplace_back(Point(i, j));
            }
        }
    // 遍历链表的每一个点
    for (list<Point>::iterator it = children.begin(); it != children.end(); it++) {
        // 首先尝试在这里预摆棋子，摆上给定棋子颜色的棋
        setPos(*it, player);
        // 然后递归搜索，深度加1，游戏者交换
        int val = -MINMAX_Search_With_AlphaBetaCutOff(depth + 1, 1 - player); // 注意这里有个负号
        // 回溯到不摆棋子的状态
        setPos(*it, NONE);
        // 如果val比bast大，更新best，并将当前点作为下一个要落子的点
        if (val > best) {
            best = val;
            next = *it;
        }
    }
    // 返回前确保对下一个落子点维护正确
    return best;
}

/**
 * 启发式函数
 * @param player 什么颜色的棋
 * @return 评估值
 */
int AI::heuristic(int player) {
//    return 0; // 启发式函数恒为0，则每次落子都一定是与棋盘上棋子互为邻居的那么多棋子中最左上角的那个
//    return rand()%10000; // 随机返回启发式函数的值，则每次落子将完全随机
    int ally = 0; // 表示自己的棋子的评估值
    int enemy = 0; // 表示对手的棋子的评估值
    // 遍历棋盘，分别对自己和对手的棋子的每一个位置计算f(x)，加到评估值中，空位置不管
    for (int i = 0; i < GRID_NUM; i++) {
        for (int j = 0; j < GRID_NUM; j++) {
            if (chessBoard[i][j] == player)
                ally += f(Point(i, j), player);
            else if (chessBoard[i][j] == 1 - player)
                enemy += f(Point(i, j), 1 - player);
        }
    }
    // 棋盘遍历完毕，至此，已经分别求出每个黑子和白子的评估值，并对应的加入到对应玩家的评估值中
    /*
     * 启发式函数的评估值为自己的评估值减去对手的评估值
     * 这里我加大了自己的权重，更加注重进攻性，也就是对自己有利的部分所占比重更高
     * 修改这里的权重不影响程序正确性，也不影响AI的智能程度，只对采取策略的理解所有偏重
     * （注重进攻还是注重防守，只是策略的不同，没有优劣之分）
     */
    int heuristic = 10 * ally - enemy;
#ifdef DEBUG_USE_ONLY
    cout << "Heuristic = " << heuristic << endl;
#endif
    return heuristic;
}

/**
 * 计算每个点的f(x)
 * @param p 点
 * @param color 什么颜色的棋
 * @return f(x)
 */
int AI::f(Point p, int color) {
    // 常量的定义，表示不同的局面的字符串表示和对应的分数
    const int CHENG_5_SCORE = 5000000;
    const int HUO_4_SCORE = 100000;
    const int CHONG_4_SCORE = 10000;
    const int DAN_HUO_3_SCORE = 8000;
    const int TIAO_HUO_3_SCORE = 7000;
    const int MIAN_3_SCORE = 500;
    const int HUO_2_SCORE = 50;
    const int MIAN_2_SCORE = 10;
    string CHENG_5_STRING = "11111";
    string HUO_4_STRING = "011110";
    string CHONG_4_STRING_1_1 = "01111-";
    string CHONG_4_STRING_1_2 = "-11110";
    string CHONG_4_STRING_2_1 = "10111";
    string CHONG_4_STRING_2_2 = "11101";
    string CHONG_4_STRING_3 = "11011";
    string DAN_HUO_3_STRING = "01110";
    string TIAO_HUO_3_STRING_1_1 = "1011";
    string TIAO_HUO_3_STRING_1_2 = "1101";
    string MIAN_3_1_1 = "00111-";
    string MIAN_3_1_2 = "-11100";
    string MIAN_3_2_1 = "01011-";
    string MIAN_3_2_2 = "-11010";
    string MIAN_3_3_1 = "01101-";
    string MIAN_3_3_2 = "-10110";
    string MIAN_3_4_1 = "10011";
    string MIAN_3_4_2 = "11001";
    string MIAN_3_5 = "10101";
    string MIAN_3_6 = "-01110-";
    string HUO_2_STRING_1 = "001100";
    string HUO_2_STRING_2 = "01010";
    string HUO_2_STRING_3 = "1001";
    string MIAN_2_1_1 = "00011-";
    string MIAN_2_1_2 = "-11000";
    string MIAN_2_2_1 = "00101-";
    string MIAN_2_2_2 = "-10100";
    string MIAN_2_3_1 = "01001-";
    string MIAN_2_3_2 = "-10010";
    string MIAN_2_4 = "10001";
#ifdef DEBUG_USE_ONLY
    printf("The chess board is:\n");
    for (int i = 0; i < GRID_NUM; ++i)
        for (int j = 0; j < GRID_NUM; ++j)
            printf("%3d%c", chessBoard[i][j], j == GRID_NUM - 1 ? '\n' : ' ');
    printf("\n");
    printf("Now, calculate f(p(%d,%d)).\n", p.x, p.y);
#endif
    int x = p.x;
    int y = p.y;
    int score = 0; // 分数
    int weight = PosValue[p.x][p.y]; // 权重，距离中间越近，权重越高，表示越是好的地段
    // 分别构造4个方向的局面的字符串表示
    for (int dir = 0; dir < 4; dir++) {
        string s = "";
        // 计算该方向上的起始点坐标
        int rBegin = x + DIRECTION[dir][0] * 4;
        int cBegin = y + DIRECTION[dir][1] * 4;
        // 坐标递增的方向
        int rDir = DIRECTION[dir][2];
        int cDir = DIRECTION[dir][3];
        // 计算该方向上的终止点坐标
        int rEnd = x + rDir * 4;
        int cEnd = y + cDir * 4;
        // 当行列没到终点的时候（表示没有收集齐9个点），循环
        int r = rBegin;
        int c = cBegin;
        while (r != rEnd || c != cEnd) {
            // 如果这个点没有超过棋盘范围，是自己颜色就记为1，是空记为0，是对手记为-，超过棋盘的点记为#
            if (isValid(r, c))
                if (chessBoard[r][c] == color) s += "1";
                else if (chessBoard[r][c] == NONE) s += "0";
                else s += "-";
            else
                s += "#";
            r += rDir;
            c += cDir;
        }
#ifdef DEBUG_USE_ONLY
        cout << "String[" << dir << "]:" << s << endl;
#endif
        // 如果构建出来的字符串中包含“成五”的子串，加上其分数
        if (s.find(CHENG_5_STRING) != string::npos) {
            score += CHENG_5_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "CHENG_5", score);
#endif
        }

        // 如果包含“活四”的子串，加上其分数
        if (s.find(HUO_4_STRING) != string::npos) {
            score += HUO_4_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "HUO_4", score);
#endif
        }

        // “冲四”不止一种情况，如果包含任意一个子串，加上其分数，下面的情况同理
        if (s.find(CHONG_4_STRING_1_1) != string::npos
            || s.find(CHONG_4_STRING_1_2) != string::npos
            || s.find(CHONG_4_STRING_2_1) != string::npos
            || s.find(CHONG_4_STRING_2_2) != string::npos
            || s.find(CHONG_4_STRING_3) != string::npos) {
            score += CHONG_4_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "CHONG_4", score);
#endif
        }

        if (s.find(DAN_HUO_3_STRING) != string::npos) {
            score += DAN_HUO_3_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "DAN_HUO_3", score);
#endif
        }

        if (s.find(TIAO_HUO_3_STRING_1_1) != string::npos
            || s.find(TIAO_HUO_3_STRING_1_2) != string::npos) {
            score += TIAO_HUO_3_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "TIAO_HUO_3", score);
#endif
        }

        if (s.find(MIAN_3_1_1) != string::npos
            || s.find(MIAN_3_1_2) != string::npos
            || s.find(MIAN_3_2_1) != string::npos
            || s.find(MIAN_3_2_2) != string::npos
            || s.find(MIAN_3_3_1) != string::npos
            || s.find(MIAN_3_3_2) != string::npos
            || s.find(MIAN_3_4_1) != string::npos
            || s.find(MIAN_3_4_2) != string::npos
            || s.find(MIAN_3_5) != string::npos
            || s.find(MIAN_3_6) != string::npos) {
            score += MIAN_3_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "MIAN_3", score);
#endif
        }

        if (s.find(HUO_2_STRING_1) != string::npos
            || s.find(HUO_2_STRING_2) != string::npos
            || s.find(HUO_2_STRING_3) != string::npos) {
            score += HUO_2_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "HUO_2", score);
#endif
        }

        if (s.find(MIAN_2_1_1) != string::npos
            || s.find(MIAN_2_1_2) != string::npos
            || s.find(MIAN_2_2_1) != string::npos
            || s.find(MIAN_2_2_2) != string::npos
            || s.find(MIAN_2_3_1) != string::npos
            || s.find(MIAN_2_3_2) != string::npos
            || s.find(MIAN_2_4) != string::npos) {
            score += MIAN_2_SCORE;
#ifdef DEBUG_USE_ONLY
            printf("Found: %s : %d \n", "MIAN_2", score);
#endif
        }

    }

    // 四个方向的分数都加起来，乘上权重
    return score * weight;
}
