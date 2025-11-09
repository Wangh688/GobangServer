// ========================================
// 五子棋在线服务器
// 基于你的 Chess.cpp 核心逻辑
// ========================================

#include "httplib.h"
#include "json.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <ctime>

using json = nlohmann::json;
using namespace std;

// ========================================
// 从你的 Chess.h 复制的结构体
// ========================================
struct ChessPos
{
    int row;
    int col;
    ChessPos(int r = 0, int c = 0) : row(r), col(c) {}
};

enum chess_kind
{
    CHESS_WHITE = -1,
    CHESS_BLACK = 1
};

// ========================================
// ChessLogic 类 - 从你的 Chess.cpp 改编
// 移除了所有 EasyX 相关代码，保留核心逻辑
// ========================================
class ChessLogic
{
private:
    int gradeSize;
    int margin_x;
    int margin_y;
    float chessSize;
    vector<vector<int>> chessMap;
    bool playerFlag; // true=黑棋, false=白棋
    ChessPos lastPos;

public:
    // 对应 Chess::Chess()
    ChessLogic(int gradeSize = 13, int marginX = 44, int marginY = 43, float chessSize = 67.3f)
        : gradeSize(gradeSize), margin_x(marginX), margin_y(marginY),
          chessSize(chessSize), playerFlag(true), lastPos(-1, -1)
    {

        for (int i = 0; i < gradeSize; i++)
        {
            vector<int> row;
            for (int j = 0; j < gradeSize; j++)
            {
                row.push_back(0);
            }
            chessMap.push_back(row);
        }
    }

    // 对应 Chess::init()
    void init()
    {
        for (int i = 0; i < gradeSize; i++)
        {
            for (int j = 0; j < gradeSize; j++)
            {
                chessMap[i][j] = 0;
            }
        }
        playerFlag = true;
        lastPos = ChessPos(-1, -1);
    }

    // 对应 Chess::chessDown() - 简化版（无图形）
    bool chessDown(int row, int col, chess_kind kind)
    {
        if (row < 0 || row >= gradeSize || col < 0 || col >= gradeSize)
        {
            return false;
        }
        if (chessMap[row][col] != 0)
        {
            return false;
        }

        ChessPos pos(row, col);
        updateGameMap(&pos);
        return true;
    }

    // 对应 Chess::updateGameMap()
    void updateGameMap(ChessPos *pos)
    {
        chessMap[pos->row][pos->col] = playerFlag ? CHESS_BLACK : CHESS_WHITE;
        playerFlag = !playerFlag;
        lastPos = *pos;
    }

    // 对应 Chess::checkWin() - 100%保留你的算法
    bool checkWin()
    {
        int row = lastPos.row;
        int col = lastPos.col;

        if (row < 0 || col < 0)
            return false;

        int playerChess = chessMap[row][col];

        // 1. 水平方向 (----)
        int count = 1;
        for (int i = col - 1; i >= 0; i--)
        {
            if (chessMap[row][i] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        for (int i = col + 1; i < gradeSize; i++)
        {
            if (chessMap[row][i] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        if (count >= 5)
            return true;

        // 2. 垂直方向 (|)
        count = 1;
        for (int i = row - 1; i >= 0; i--)
        {
            if (chessMap[i][col] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        for (int i = row + 1; i < gradeSize; i++)
        {
            if (chessMap[i][col] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        if (count >= 5)
            return true;

        // 3. 右斜方向 (\)
        count = 1;
        for (int i = row - 1, j = col - 1; i >= 0 && j >= 0; i--, j--)
        {
            if (chessMap[i][j] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        for (int i = row + 1, j = col + 1; i < gradeSize && j < gradeSize; i++, j++)
        {
            if (chessMap[i][j] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        if (count >= 5)
            return true;

        // 4. 左斜方向 (/)
        count = 1;
        for (int i = row - 1, j = col + 1; i >= 0 && j < gradeSize; i--, j++)
        {
            if (chessMap[i][j] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        for (int i = row + 1, j = col - 1; i < gradeSize && j >= 0; i++, j--)
        {
            if (chessMap[i][j] == playerChess)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        if (count >= 5)
            return true;

        return false;
    }

    // 对应 Chess::getGradeSize()
    int getGradeSize() const
    {
        return gradeSize;
    }

    // 对应 Chess::getChessData()
    int getChessData(int row, int col) const
    {
        if (row < 0 || row >= gradeSize || col < 0 || col >= gradeSize)
        {
            return 0;
        }
        return chessMap[row][col];
    }

    // 获取棋盘状态（用于返回给前端）
    vector<vector<int>> getBoard() const
    {
        return chessMap;
    }

    ChessPos getLastPos() const
    {
        return lastPos;
    }
};

// ========================================
// AILogic 类 - 从你的 AI.cpp 改编
// ========================================
class AILogic
{
private:
    ChessLogic *chess;
    vector<vector<int>> scoreMap;

public:
    AILogic() : chess(nullptr) {}

    // 对应 AI::init()
    void init(ChessLogic *chess)
    {
        this->chess = chess;
        int size = chess->getGradeSize();

        scoreMap.clear();
        for (int i = 0; i < size; i++)
        {
            vector<int> row;
            for (int j = 0; j < size; j++)
            {
                row.push_back(0);
            }
            scoreMap.push_back(row);
        }
    }

    // 对应 AI::go()
    ChessPos go()
    {
        return think();
    }

    // 对应 AI::think()
    ChessPos think()
    {
        calculateScore();

        vector<ChessPos> maxPoints;
        int maxScore = 0;
        int size = chess->getGradeSize();

        for (int row = 0; row < size; row++)
        {
            for (int col = 0; col < size; col++)
            {
                if (chess->getChessData(row, col) != 0)
                    continue;

                if (scoreMap[row][col] > maxScore)
                {
                    maxScore = scoreMap[row][col];
                    maxPoints.clear();
                    maxPoints.push_back(ChessPos(row, col));
                }
                else if (scoreMap[row][col] == maxScore)
                {
                    maxPoints.push_back(ChessPos(row, col));
                }
            }
        }

        if (maxPoints.empty())
        {
            return ChessPos(-1, -1);
        }

        int ind = rand() % maxPoints.size();
        return maxPoints[ind];
    }

    // 对应 AI::calculateScore() - 100%保留你的算法
    void calculateScore()
    {
        int personNum = 0;
        int aiNum = 0;
        int emptyNum = 0;

        int size = chess->getGradeSize();

        // 清空分数
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                scoreMap[i][j] = 0;
            }
        }

        for (int row = 0; row < size; row++)
        {
            for (int col = 0; col < size; col++)
            {
                if (chess->getChessData(row, col))
                    continue;

                for (int y = -1; y <= 0; y++)
                {
                    for (int x = -1; x <= 1; x++)
                    {
                        if (x == 0 && y == 0)
                            continue;
                        if (x != 1 && y == 0)
                            continue;

                        personNum = 0;
                        aiNum = 0;
                        emptyNum = 0;

                        // 正向检查玩家（黑棋）
                        for (int i = 1; i <= 4; i++)
                        {
                            int curRow = row + i * y;
                            int curCol = col + i * x;

                            if (curRow >= 0 && curRow < size &&
                                curCol >= 0 && curCol < size &&
                                chess->getChessData(curRow, curCol) == 1)
                            {
                                personNum++;
                            }
                            else if (curRow >= 0 && curRow < size &&
                                     curCol >= 0 && curCol < size &&
                                     chess->getChessData(curRow, curCol) == 0)
                            {
                                emptyNum++;
                                break;
                            }
                            else
                            {
                                break;
                            }
                        }

                        // 反向检查
                        for (int i = 1; i <= 4; i++)
                        {
                            int curRow = row - i * y;
                            int curCol = col - i * x;

                            if (curRow >= 0 && curRow < size &&
                                curCol >= 0 && curCol < size &&
                                chess->getChessData(curRow, curCol) == 1)
                            {
                                personNum++;
                            }
                            else if (curRow >= 0 && curRow < size &&
                                     curCol >= 0 && curCol < size &&
                                     chess->getChessData(curRow, curCol) == 0)
                            {
                                emptyNum++;
                                break;
                            }
                            else
                            {
                                break;
                            }
                        }

                        // 玩家威胁评分
                        if (personNum == 1)
                        {
                            scoreMap[row][col] += 10;
                        }
                        else if (personNum == 2)
                        {
                            if (emptyNum == 1)
                            {
                                scoreMap[row][col] += 30;
                            }
                            else if (emptyNum == 2)
                            {
                                scoreMap[row][col] += 40;
                            }
                        }
                        else if (personNum == 3)
                        {
                            if (emptyNum == 1)
                            {
                                scoreMap[row][col] += 60;
                            }
                            else if (emptyNum == 2)
                            {
                                scoreMap[row][col] += 200;
                            }
                        }
                        else if (personNum == 4)
                        {
                            scoreMap[row][col] += 20000;
                        }

                        // 检查AI（白棋）
                        emptyNum = 0;

                        for (int i = 1; i <= 4; i++)
                        {
                            int curRow = row + i * y;
                            int curCol = col + i * x;

                            if (curRow >= 0 && curRow < size &&
                                curCol >= 0 && curCol < size &&
                                chess->getChessData(curRow, curCol) == -1)
                            {
                                aiNum++;
                            }
                            else if (curRow >= 0 && curRow < size &&
                                     curCol >= 0 && curCol < size &&
                                     chess->getChessData(curRow, curCol) == 0)
                            {
                                emptyNum++;
                                break;
                            }
                            else
                            {
                                break;
                            }
                        }

                        for (int i = 1; i <= 4; i++)
                        {
                            int curRow = row - i * y;
                            int curCol = col - i * x;

                            if (curRow >= 0 && curRow < size &&
                                curCol >= 0 && curCol < size &&
                                chess->getChessData(curRow, curCol) == -1)
                            {
                                aiNum++;
                            }
                            else if (curRow >= 0 && curRow < size &&
                                     curCol >= 0 && curCol < size &&
                                     chess->getChessData(curRow, curCol) == 0)
                            {
                                emptyNum++;
                                break;
                            }
                            else
                            {
                                break;
                            }
                        }

                        // AI进攻评分
                        if (aiNum == 0)
                        {
                            scoreMap[row][col] += 5;
                        }
                        else if (aiNum == 1)
                        {
                            scoreMap[row][col] += 10;
                        }
                        else if (aiNum == 2)
                        {
                            if (emptyNum == 1)
                            {
                                scoreMap[row][col] += 25;
                            }
                            else if (emptyNum == 2)
                            {
                                scoreMap[row][col] += 50;
                            }
                        }
                        else if (aiNum == 3)
                        {
                            if (emptyNum == 1)
                            {
                                scoreMap[row][col] += 55;
                            }
                            else if (emptyNum == 2)
                            {
                                scoreMap[row][col] += 10000;
                            }
                        }
                        else if (aiNum == 4)
                        {
                            scoreMap[row][col] += 30000;
                        }
                    }
                }
            }
        }
    }
};

// ========================================
// HTTP 服务器主程序
// ========================================
int main()
{
    srand(time(nullptr));

    httplib::Server svr;

    // 存储游戏会话
    map<string, shared_ptr<ChessLogic>> games;
    map<string, shared_ptr<AILogic>> aiInstances;
    int gameIdCounter = 0;

    cout << "========================================" << endl;
    cout << "   五子棋在线服务器" << endl;
    cout << "   基于你的 C++ 核心逻辑" << endl;
    cout << "========================================" << endl;

    // CORS 中间件（允许跨域）
    svr.set_base_dir("/home/haoW/GobangServer/");

    // API: 创建新游戏
    svr.Post("/api/new-game", [&](const httplib::Request &req, httplib::Response &res)
             {
        auto chess = make_shared<ChessLogic>(13, 44, 43, 67.3f);
        auto ai = make_shared<AILogic>();
        
        chess->init();
        ai->init(chess.get());
        
        string gameId = "game_" + to_string(++gameIdCounter);
        games[gameId] = chess;
        aiInstances[gameId] = ai;

        json response;
        response["gameId"] = gameId;
        response["gradeSize"] = 13;
        
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_content(response.dump(), "application/json");
        
        cout << "[新游戏] gameId=" << gameId << endl; });

    // API: 玩家落子
    svr.Post("/api/move", [&](const httplib::Request &req, httplib::Response &res)
             {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        
        try {
            auto body = json::parse(req.body);
            string gameId = body["gameId"];
            int row = body["row"];
            int col = body["col"];

            cout << "[玩家落子] gameId=" << gameId << ", pos=(" << row << "," << col << ")" << endl;

            if (games.find(gameId) == games.end()) {
                json error;
                error["error"] = "Game not found";
                res.set_content(error.dump(), "application/json");
                return;
            }

            auto chess = games[gameId];
            auto ai = aiInstances[gameId];
            
            // 玩家落子（黑棋）
            if (!chess->chessDown(row, col, CHESS_BLACK)) {
                json error;
                error["error"] = "Invalid move";
                res.set_content(error.dump(), "application/json");
                return;
            }

            json response;
            response["success"] = true;
            
            // 检查玩家是否胜利
            if (chess->checkWin()) {
                response["gameOver"] = true;
                response["winner"] = "black";
                cout << "[游戏结束] 黑棋获胜" << endl;
                res.set_content(response.dump(), "application/json");
                return;
            }

            // AI落子（白棋）
            ChessPos aiPos = ai->go();
            if (aiPos.row >= 0 && aiPos.col >= 0) {
                chess->chessDown(aiPos.row, aiPos.col, CHESS_WHITE);
                response["aiMove"] = {{"row", aiPos.row}, {"col", aiPos.col}};
                
                cout << "[AI落子] pos=(" << aiPos.row << "," << aiPos.col << ")" << endl;

                // 检查AI是否胜利
                if (chess->checkWin()) {
                    response["gameOver"] = true;
                    response["winner"] = "white";
                    cout << "[游戏结束] 白棋（AI）获胜" << endl;
                }
            }

            res.set_content(response.dump(), "application/json");
        } catch (const exception& e) {
            json error;
            error["error"] = "Invalid request";
            error["message"] = e.what();
            res.set_content(error.dump(), "application/json");
        } });

    // API: 获取棋盘状态
    svr.Get("/api/board/:gameId", [&](const httplib::Request &req, httplib::Response &res)
            {
        res.set_header("Access-Control-Allow-Origin", "*");
        
        string gameId = req.path_params.at("gameId");
        if (games.find(gameId) == games.end()) {
            json error;
            error["error"] = "Game not found";
            res.set_content(error.dump(), "application/json");
            return;
        }

        json response;
        response["board"] = games[gameId]->getBoard();
        res.set_content(response.dump(), "application/json"); });

    // 处理OPTIONS请求（CORS预检）
    svr.Options(R"(/api/.*)", [](const httplib::Request &, httplib::Response &res)
                {
                    res.set_header("Access-Control-Allow-Origin", "*");
                    res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
                    res.set_header("Access-Control-Allow-Headers", "Content-Type");
                    res.status = 204; });

    // 启动服务器
    cout << "\n服务器启动中..." << endl;
    cout << "监听地址: http://0.0.0.0:8888" << endl;
    cout << "按 Ctrl+C 停止服务器\n"
         << endl;

    if (!svr.listen("0.0.0.0", 8888))
    {
        cerr << "错误：无法启动服务器，端口8888可能被占用" << endl;
        return 1;
    }

    return 0;
}
