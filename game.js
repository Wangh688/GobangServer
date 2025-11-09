// ==========================================
// 服务器配置
// ==========================================
const SERVER_URL = 'http://123.56.77.88:8888';
console.log('游戏初始化, 服务器地址:', SERVER_URL);

// ==========================================
// ChessPos 结构体
// ==========================================
class ChessPos {
    constructor(r = 0, c = 0) {
        this.row = r;
        this.col = c;
    }
}

// ==========================================
// chess_kind 枚举
// ==========================================
const chess_kind = {
    CHESS_WHITE: -1,
    CHESS_BLACK: 1
};

// ==========================================
// Chess 类（简化版，仅用于测试）
// ==========================================
class Chess {
    constructor(gradeSize, marginX, marginY, chessSize) {
        this.gradeSize = gradeSize;
        this.margin_x = marginX;
        this.margin_y = marginY;
        this.chessSize = chessSize;
        this.playerFlag = chess_kind.CHESS_BLACK;

        this.chessMap = [];
        for (let i = 0; i < gradeSize; i++) {
            const row = [];
            for (let j = 0; j < gradeSize; j++) {
                row.push(0);
            }
            this.chessMap.push(row);
        }

        this.lastPos = new ChessPos(-1, -1);
        this.canvas = document.getElementById('chessboard');
        this.ctx = this.canvas.getContext('2d');
    }

    init() {
        this.drawBoard();
        for (let i = 0; i < this.gradeSize; i++) {
            for (let j = 0; j < this.gradeSize; j++) {
                this.chessMap[i][j] = 0;
            }
        }
        this.playerFlag = chess_kind.CHESS_BLACK;
        this.lastPos = new ChessPos(-1, -1);
    }

    drawBoard() {
        const ctx = this.ctx;
        const size = this.chessSize;
        const margin = this.margin_x;

        ctx.fillStyle = '#DCB35C';
        ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

        ctx.strokeStyle = '#000';
        ctx.lineWidth = 1.5;

        for (let i = 0; i < this.gradeSize; i++) {
            ctx.beginPath();
            ctx.moveTo(margin, this.margin_y + i * size);
            ctx.lineTo(margin + (this.gradeSize - 1) * size, this.margin_y + i * size);
            ctx.stroke();

            ctx.beginPath();
            ctx.moveTo(margin + i * size, this.margin_y);
            ctx.lineTo(margin + i * size, this.margin_y + (this.gradeSize - 1) * size);
            ctx.stroke();
        }

        const stars = [[3, 3], [3, 9], [9, 3], [9, 9], [6, 6]];
        ctx.fillStyle = '#000';
        stars.forEach(([row, col]) => {
            ctx.beginPath();
            ctx.arc(margin + col * size, this.margin_y + row * size, 5, 0, 2 * Math.PI);
            ctx.fill();
        });
    }

    clickBoard(x, y, pos) {
        let col = Math.floor((x - this.margin_x) / this.chessSize);
        let row = Math.floor((y - this.margin_y) / this.chessSize);
        let leftTopPosX = this.margin_x + this.chessSize * col;
        let leftTopPosY = this.margin_y + this.chessSize * row;
        let offset = this.chessSize * 0.4;

        let len;
        let ret = false;

        do {
            len = Math.sqrt((x - leftTopPosX) ** 2 + (y - leftTopPosY) ** 2);
            if (len < offset) {
                pos.row = row;
                pos.col = col;
                if (this.chessMap[pos.row][pos.col] === 0) {
                    ret = true;
                }
                break;
            }

            let x2 = leftTopPosX + this.chessSize;
            let y2 = leftTopPosY;
            len = Math.sqrt((x - x2) ** 2 + (y - y2) ** 2);
            if (len < offset) {
                pos.row = row;
                pos.col = col + 1;
                if (this.chessMap[pos.row][pos.col] === 0) {
                    ret = true;
                }
                break;
            }

            x2 = leftTopPosX;
            y2 = leftTopPosY + this.chessSize;
            len = Math.sqrt((x - x2) ** 2 + (y - y2) ** 2);
            if (len < offset) {
                pos.row = row + 1;
                pos.col = col;
                if (this.chessMap[pos.row][pos.col] === 0) {
                    ret = true;
                }
                break;
            }

            x2 = leftTopPosX + this.chessSize;
            y2 = leftTopPosY + this.chessSize;
            len = Math.sqrt((x - x2) ** 2 + (y - y2) ** 2);
            if (len < offset) {
                pos.row = row + 1;
                pos.col = col + 1;
                if (this.chessMap[pos.row][pos.col] === 0) {
                    ret = true;
                }
                break;
            }
        } while (false);

        return ret;
    }

    chessDown(pos, kind) {
        let x = this.margin_x + this.chessSize * pos.col - 0.5 * this.chessSize;
        let y = this.margin_y + this.chessSize * pos.row - 0.5 * this.chessSize;

        this.drawChessPiece(x, y, kind);
        this.updateGameMap(pos);
    }

    drawChessPiece(x, y, kind) {
        const centerX = x + this.chessSize / 2;
        const centerY = y + this.chessSize / 2;
        const radius = this.chessSize * 0.4;

        this.ctx.shadowBlur = 8;
        this.ctx.shadowColor = 'rgba(0, 0, 0, 0.5)';
        this.ctx.shadowOffsetX = 3;
        this.ctx.shadowOffsetY = 3;

        this.ctx.beginPath();
        this.ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI);

        if (kind === chess_kind.CHESS_BLACK) {
            const gradient = this.ctx.createRadialGradient(
                centerX - 6, centerY - 6, 2, centerX, centerY, radius
            );
            gradient.addColorStop(0, '#555');
            gradient.addColorStop(1, '#000');
            this.ctx.fillStyle = gradient;
        } else {
            const gradient = this.ctx.createRadialGradient(
                centerX - 6, centerY - 6, 2, centerX, centerY, radius
            );
            gradient.addColorStop(0, '#fff');
            gradient.addColorStop(1, '#ccc');
            this.ctx.fillStyle = gradient;
        }

        this.ctx.fill();
        this.ctx.shadowBlur = 0;
    }

    updateGameMap(pos) {
        this.chessMap[pos.row][pos.col] = this.playerFlag ?
            chess_kind.CHESS_BLACK : chess_kind.CHESS_WHITE;
        this.playerFlag = !this.playerFlag;
        this.lastPos = pos;
    }

    getGradeSize() {
        return this.gradeSize;
    }

    getChessData(rowOrPos, col) {
        if (typeof rowOrPos === 'object') {
            return this.chessMap[rowOrPos.row][rowOrPos.col];
        } else {
            return this.chessMap[rowOrPos][col];
        }
    }
}

// ==========================================
// 在线游戏类（连接服务器）
// ==========================================
class OnlineGobangGame {
    constructor() {
        this.gameId = null;
        this.chess = new Chess(13, 44, 43, 67.3);
        this.gameStarted = false;
        this.aiThinking = false;

        console.log('OnlineGobangGame 初始化');
        this.init();
    }

    init() {
        console.log('开始初始化...');
        this.chess.init();
        this.setupEventListeners();
        this.checkServerConnection();
    }

    // 检查服务器连接
    async checkServerConnection() {
        const statusEl = document.getElementById('server-status');
        console.log('=== 开始测试服务器连接 ===');
        console.log('服务器地址:', SERVER_URL);

        try {
            console.log('发送请求到:', `${SERVER_URL}/api/new-game`);

            const response = await fetch(`${SERVER_URL}/api/new-game`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                mode: 'cors'
            });

            console.log('收到响应:', response);
            console.log('响应状态:', response.status);
            console.log('响应OK:', response.ok);

            if (response.ok) {
                const data = await response.json();
                console.log('响应数据:', data);

                this.gameId = data.gameId;
                statusEl.textContent = '🟢 服务器已连接';
                statusEl.style.color = 'green';
                console.log('✅ 服务器连接成功! gameId:', this.gameId);
            } else {
                console.error('❌ 响应不OK, 状态码:', response.status);
                statusEl.textContent = '🔴 服务器连接失败 (状态码:' + response.status + ')';
                statusEl.style.color = 'red';
            }
        } catch (error) {
            console.error('❌ 连接失败!');
            console.error('错误类型:', error.name);
            console.error('错误信息:', error.message);
            console.error('完整错误:', error);

            statusEl.textContent = '🔴 服务器连接失败: ' + error.message;
            statusEl.style.color = 'red';
        }

        console.log('=== 服务器连接测试完成 ===');
    }

    setupEventListeners() {
        console.log('设置事件监听...');

        document.getElementById('new-game-btn').addEventListener('click', () => {
            console.log('点击了"开始游戏"按钮');
            this.newGame();
        });

        document.getElementById('restart-btn').addEventListener('click', () => {
            console.log('点击了"重新开始"按钮');
            this.newGame();
        });

        document.getElementById('modal-new-game').addEventListener('click', () => {
            console.log('点击了弹窗"再来一局"按钮');
            document.getElementById('result-modal').classList.add('hidden');
            this.newGame();
        });

        this.chess.canvas.addEventListener('click', (e) => this.handleClick(e));
        console.log('事件监听设置完成');
    }

    async newGame() {
        console.log('=== 开始新游戏 ===');
        try {
            console.log('发送新游戏请求...');
            const response = await fetch(`${SERVER_URL}/api/new-game`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' }
            });

            console.log('新游戏响应:', response);

            const data = await response.json();
            console.log('新游戏数据:', data);

            this.gameId = data.gameId;
            this.gameStarted = true;
            this.aiThinking = false;

            this.chess.init();
            document.getElementById('game-status').textContent = '游戏开始！请落子';
            document.getElementById('current-turn').innerHTML = '当前：<strong>黑棋（玩家）</strong>';
            document.getElementById('result-modal').classList.add('hidden');

            console.log('✅ 新游戏创建成功, gameId:', this.gameId);
        } catch (error) {
            console.error('❌ 创建游戏失败:', error);
            alert('创建游戏失败: ' + error.message);
        }
    }

    async handleClick(event) {
        if (!this.gameStarted) {
            console.log('游戏未开始，忽略点击');
            alert('请先点击"开始游戏"按钮！');
            return;
        }

        if (this.aiThinking) {
            console.log('AI思考中，忽略点击');
            return;
        }

        const rect = this.chess.canvas.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;

        console.log('点击坐标:', x, y);

        const pos = new ChessPos();
        if (this.chess.clickBoard(x, y, pos)) {
            console.log('有效落子位置:', pos.row, pos.col);
            await this.sendMove(pos.row, pos.col);
        } else {
            console.log('无效落子位置');
        }
    }

    async sendMove(row, col) {
        console.log('=== 发送落子请求 ===');
        console.log('位置:', row, col);

        document.getElementById('game-status').textContent = '正在发送落子请求...';

        try {
            const requestBody = {
                gameId: this.gameId,
                row: row,
                col: col
            };
            console.log('请求体:', requestBody);

            const response = await fetch(`${SERVER_URL}/api/move`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(requestBody)
            });

            console.log('落子响应:', response);

            const data = await response.json();
            console.log('落子数据:', data);

            if (data.error) {
                console.error('服务器返回错误:', data.error);
                alert(data.error);
                return;
            }

            // 绘制玩家落子
            this.chess.chessDown(new ChessPos(row, col), chess_kind.CHESS_BLACK);
            console.log('✅ 玩家落子完成');

            if (data.gameOver) {
                console.log('游戏结束, 获胜方:', data.winner);
                this.gameStarted = false;
                const winner = data.winner === 'black' ? '黑棋（玩家）' : '白棋（AI）';
                this.showResult(`🎉 ${winner}获胜！`);
                return;
            }

            // AI落子
            if (data.aiMove) {
                this.aiThinking = true;
                document.getElementById('game-status').textContent = 'AI正在思考...';
                document.getElementById('current-turn').innerHTML = '当前：<strong>白棋（AI）</strong>';

                console.log('AI将在0.8秒后落子:', data.aiMove);

                setTimeout(() => {
                    this.chess.chessDown(
                        new ChessPos(data.aiMove.row, data.aiMove.col),
                        chess_kind.CHESS_WHITE
                    );
                    console.log('✅ AI落子完成');

                    this.aiThinking = false;
                    document.getElementById('game-status').textContent = 'AI已落子';
                    document.getElementById('current-turn').innerHTML = '当前：<strong>黑棋（玩家）</strong>';

                    if (data.gameOver) {
                        console.log('游戏结束, 获胜方:', data.winner);
                        this.gameStarted = false;
                        const winner = data.winner === 'black' ? '黑棋（玩家）' : '白棋（AI）';
                        this.showResult(`${winner}获胜！`);
                    }
                }, 800);
            }

        } catch (error) {
            console.error('❌ 网络错误:', error);
            alert('网络错误: ' + error.message);
        }
    }

    showResult(text) {
        console.log('显示结果:', text);
        document.getElementById('result-title').textContent = text;
        document.getElementById('result-text').textContent = '点击按钮开始新游戏';
        document.getElementById('result-modal').classList.remove('hidden');
    }
}

// ==========================================
// 启动游戏
// ==========================================
console.log('=== 页面加载完成，开始启动游戏 ===');
window.onload = () => {
    console.log('执行 window.onload');
    new OnlineGobangGame();
};
console.log('=== 游戏脚本加载完成 ===');