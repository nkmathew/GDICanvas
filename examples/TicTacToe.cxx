#include "Canvas.h"
#include <ctime>

// Adapted from the Python code found here http://pastebin.com/C9rZiPsc

int winningCombos[8][3] = {
  {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6},
  {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}
};

// Provides the logic
struct TicTacToe {
  // cppcheck-suppress unusedStructMember
  char board[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
  explicit TicTacToe(const char *board_) {
    for (unsigned i = 0; (i < std::strlen(board_)) && (i < 9); i++) {
      board[i] = board_[i];
    }
  }

  TicTacToe() {
  }

  // XXX: It's used to achieve the same effect as disabling a button in Tkinter
  bool squareEmpty(int square) {
    return board[square] == ' ';
  }

  // cppcheck-suppress unusedFunction
  std::string repr() {
    char currState[100];
    snprintf(
      currState, 100,
      " %c | %c | %c \n"
      "---+---+---\n"
      " %c | %c | %c \n"
      "---+---+---\n"
      " %c | %c | %c \n",
      board[0], board[1], board[2],
      board[3], board[4], board[5],
      board[6], board[7], board[8]
    );
    return currState;
  }

  // Returns the positions of all empty squares
  std::vector<int> emptySquares() {
    std::vector<int> squares;
    for (int i = 0; i < 9; i++) {
      if (board[i] == ' ') {
        squares.push_back(i);
      }
    }
    return squares;
  }

  // Counts the number of times the character occurs in the three positions on the
  // board
  int squareCount(int first, int second, int third, char ch) {
    int count = 0;
    std::vector<char> squares = {board[first], board[second], board[third]};
    for (char square : squares) {
      if (square == ch) {
        count++;
      }
    }
    return count;
  }

  // Returns the position that'll win the game.
  // Returns -1 if there's no square to move to
  int winningPos(char side) {
    for (int square : emptySquares()) {
      for (int *winner : winningCombos) {
        int w1 = winner[0],
            w2 = winner[1],
            w3 = winner[2];
        bool inWinner = (w1 == square) || (w2 == square) || (w3 == square);
        if (inWinner && squareCount(w1, w2, w3, side) == 2) {
          return square;
        }
      }
    }
    return -1;
  }

  int forkPos(char side) {
    for (int square : emptySquares()) {
      int count = 0;
      for (int *winner : winningCombos) {
        int w1 = winner[0],
            w2 = winner[1],
            w3 = winner[2];
        bool inWinner = (w1 == square) || (w2 == square) || (w3 == square);
        if (inWinner &&
            ((squareCount(w1, w2, w3, ' ') == 2) &&
             (squareCount(w1, w2, w3, side) == 1))) {
          count++;
        }
        if (count >= 2) {
          return square;
        }
      }
    }
    return -1;
  }

  // Makes the move to that square and returns true if there's a winner
  std::string moveTo(int pos, char side) {
    board[pos] = side;
    std::string threeSquares = ""; // Will be used for drawing a line across them
    for (int *winner : winningCombos) {
      int w1 = winner[0],
          w2 = winner[1],
          w3 = winner[2];
      bool inWinner = (w1 == pos) || (w2 == pos) || (w3 == pos);
      if (inWinner && (squareCount(w1, w2, w3, side) == 3)) {
        char nums[10];
        snprintf(nums, 10, "%d%d%d", w1, w2, w3);
        threeSquares = nums;
      }
    }
    return threeSquares;
  }

  // Returns the position that'll avoid the opponents fork
  int forkBlockerPos(char side, int trapSpot) {
    for (int square : emptySquares()) {
      for (int *winner : winningCombos) {
        int w1 = winner[0],
            w2 = winner[1],
            w3 = winner[2];
        bool inWinner = (w1 == square) || (w2 == square) || (w3 == square);
        bool inTrap = (w1 == trapSpot) || (w2 == trapSpot) || (w3 == trapSpot);
        if (!inTrap && inWinner &&
            (squareCount(w1, w2, w3, ' ') == 2) &&
            (squareCount(w1, w2, w3, side) == 1)) {
          return square;
        }
      }
    }
    return -1;
  }
};

struct Player {
  char side;
  char opponent;
  std::vector<int> positions = {8, 0, 2, 6, 4, 1, 3, 5, 7};
  explicit Player(char side_) {
    side = side_;
    opponent = (side == 'X') ? 'O' : 'X';
    shuffle();
  }

  Player() {
    shuffle();
  }

  void shuffle() {
    std::srand(std::time(0));
    std::random_shuffle(positions.begin(), positions.end());
  }

  int nextMove(TicTacToe board) {
    if (board.emptySquares().size() == 1) {
      return board.emptySquares()[0];
    }
    if (board.winningPos(side) != -1) {
      return board.winningPos(side);
    }
    if (board.winningPos(opponent) != -1) {
      return board.winningPos(opponent);
    }
    if (board.forkPos(side) != -1) {
      return board.forkPos(side);
    }
    if (board.forkPos(opponent) != -1) {
      if (board.forkBlockerPos(side, board.forkPos(opponent)) != -1) {
        int i = board.forkBlockerPos(side, board.forkPos(opponent));
        return i;
      } else {
        return board.forkPos(opponent);
      }
    }
    for (int pos : positions) {
      for (int emptySquare : board.emptySquares()) {
        if (pos == emptySquare) {
          return pos;
        }
      }
    }
    return -1;
  }
};

struct GUI;
struct Button : GC::EventHandler {
  GUI *gui;
  int number;
  explicit Button(GUI *gui, int number) : gui(gui), number(number) {}
  virtual void handle(GC::Mouse) override;
};

struct StartButton : GC::EventHandler {
  GUI *gui;
  explicit StartButton(GUI *gui) : gui(gui) {}
  virtual void handle(GC::Mouse) override;
};

struct GUI {
  bool gameOver = false;
  bool inPlay = false;
  bool disabled = false;
  Player player;
  TicTacToe tic_tac_toe = TicTacToe();
  GC::Canvas *canv;
  int squareWidth = 120;
  int squareLength = 120;
  POINT origin = {0, 0};
  POINT msgPos;
  int startButton = -1;
  int msgLabel = -1;
  int buttonLabel = -1;
  explicit GUI(GC::Canvas *canv_, POINT start_ = {100, 50}) {
    origin = {start_.x, start_.y + 50};
    canv = canv_;
    msgPos = {origin.x, origin.y + (squareLength * 3 + 10)};
    POINT start = origin;
    for (int i = 1; i <= 9; i++) {
      POINT end = {start.x + squareLength, start.y + squareWidth};
      int id = canv->rectangle(start.x, start.y, end.x, end.y);
      Button button(this, id);
      canv->bind("<Mouse-1>", button, id);
      canv->fillColor(id, "white");
      start.x += squareLength;
      if ((i % 3) == 0) {
        origin.y += squareWidth;
        start = origin;
      }
    }
    int x2 = start_.x + 90;
    int y2 = start_.y + 40;
    startButton = canv->rectangle(start_.x, start_.y, x2, y2);
    canv->fillColor(startButton, "grey");
    canv->bind("<Mouse-1>", StartButton(this), startButton);
    buttonLabel = canv->text(start_.x + 15, start_.y + 10, "Switch");
    canv->setFont(buttonLabel, "bold");
    msgLabel = canv->text(msgPos.x, msgPos.y, "");
    canv->penColor(msgLabel, "blue");
    canv->setFont(msgLabel, "bold", 15);
  }

  void goFirst() {
    if (inPlay) {
      // Reset the board and start a new game
      canv->removeShape("line");
      canv->removeShape("oval");
      gameOver = false;
      inPlay = false;
      tic_tac_toe = TicTacToe();
      canv->setText(buttonLabel, "Switch");
    } else {
      displayGoFirst();
      player = Player('X');
      play();
    }
  }

  void displayGoFirst() {
    canv->setText(buttonLabel, "Play");
    canv->hideShape(buttonLabel);
    canv->hideShape(startButton);
    disabled = true;
    inPlay = true;
  }

  void play() {
    int move = player.nextMove(tic_tac_toe);
    makeMove(move, player.side);
  }

  void show() {
    canv->showShape(buttonLabel);
    canv->showShape(startButton);
  }

  void declareDraw() {
    disabled = false;
    canv->setText(msgLabel, "Drawn Game!!");
    canv->setText(buttonLabel, "Restart");
    show();
  }

  void declareWin(char winner, const std::string& combo) {
    disabled = false;
    char text[10];
    snprintf(text, 10, "%c Wins", winner);
    canv->setText(msgLabel, text);
    canv->setText(buttonLabel, "Restart");
    int length = squareLength / 2,
        width  = squareWidth / 2;
    POINT start = origin,
          end = origin;
    if (combo == "012") {
      start = {origin.x, origin.y-length * 5};
      end = {origin.x + squareWidth * 3, origin.y - length * 5};
    }
    if (combo == "345") {
      start = {origin.x, origin.y-length * 3};
      end = {origin.x + squareWidth * 3, origin.y - length * 3};
    }
    if (combo == "678") {
      start = {origin.x, origin.y-length};
      end = {origin.x + squareWidth * 3, origin.y - length};
    }
    if (combo == "036") {
      start = {origin.x+width, origin.y};
      end = {origin.x+width, origin.y - squareLength * 3};
    }
    if (combo == "147") {
      start = {origin.x+width * 3, origin.y};
      end = {origin.x+width * 3, origin.y - squareLength * 3};
    }
    if (combo == "258") {
      start = {origin.x+width * 5, origin.y};
      end = {origin.x+width * 5, origin.y - squareLength * 3};
    }
    if (combo == "246") {
      start = origin;
      end = {origin.x + squareWidth * 3, origin.y - squareLength * 3};
    }
    if (combo == "048") {
      start = {origin.x + squareWidth * 3, origin.y};
      end = {origin.x, origin.y - squareLength * 3};
    }
    int crossLine = canv->line({start, end});
    canv->circle(start.x, start.y, 5);
    canv->circle(end.x, end.y, 5);
    canv->fillColor("circle", "yellow");
    canv->penSize("circle", 2);
    canv->penSize(crossLine, 10);
    show();
  }

  bool makeMove(int move, char side) {
    if (gameOver) {
      return false;
    }
    GS::Box box = canv->BBox(move);
    int x1 = static_cast<int>(box.x1 + 30);
    int y1 = static_cast<int>(box.y1 + 30);
    int x2 = static_cast<int>(box.x2 - 30);
    int y2 = static_cast<int>(box.y2 - 30);
    int id;
    if (side == 'X') {
      id = canv->line({{x1, y1}, {x2, y2}});
      canv->penSize(id, 6);
      id = canv->line({{x2, y1}, {x1, y2}});
      canv->penSize(id, 6);
    } else if (side == 'O') {
      id = canv->oval(x1, y1, x2, y2);
      canv->penSize(id, 6);
    }
    std::string combo = tic_tac_toe.moveTo(move, side);
    if (combo != "") {
      declareWin(side, combo);
      gameOver = true;
      return gameOver;
    }
    if (tic_tac_toe.emptySquares().size() == 0) {
      declareDraw();
      gameOver = true;
      return gameOver;
    }
    return false;
  }
};

void StartButton::handle(GC::Mouse) {
  if (gui->disabled) {
    return;
  }
  gui->canv->setText(gui->msgLabel, "");
  gui->goFirst();
}

void Button::handle(GC::Mouse) {
  if (!gui->tic_tac_toe.squareEmpty(number)) {
    // Stop attempt to play a square twice, akin to disabling a button
    return;
  }
  if (!gui->inPlay) {
    gui->displayGoFirst();
    gui->player = Player('O');
  }
  bool lostGame = gui->makeMove(number, gui->player.opponent);
  if (!lostGame) {
    gui->play();
  }
}

struct Exit : GC::EventHandler {
  GC::Canvas *canv;
  explicit Exit(GC::Canvas *canv) : canv(canv) {}
  virtual void handle(GCanvas::Mouse) override {
    canv->kill();
  }
};

int main() {
  GC::Canvas canv(600, 600, "Tic Tac Toe");
  canv.init();
  canv.background("tan");
  canv.noMaximize();
  canv.bind("<Key-Esc>", Exit(&canv));
  GUI gui(&canv);
  return canv.loop();
}
