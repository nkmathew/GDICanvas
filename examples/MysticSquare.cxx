#include "Canvas.h"
#include <ctime>

/*!
 * started: 1st July 2014
 * completed: 7th July 2014
 *
 * Move the squares using the arrow keys or left mouse.
 */

struct Exit : GC::EventHandler {
  GC::Canvas *canv;
  explicit Exit(GC::Canvas *canv) : canv(canv) {}
  virtual void handle(GCanvas::Mouse) override {
    canv->kill();
  }
};

struct GUI;
struct Scrambler : GC::EventHandler {
  GUI *gui;
  explicit Scrambler(GUI *gui) : gui(gui) {}
  virtual void handle(GC::Mouse) override;
};

struct Click : GC::EventHandler {
  GUI *gui;
  int squareID;
  Click(GUI *gui, int square) : gui(gui), squareID(square) {}
  virtual void handle(GC::Mouse) override;
};

// The adjacent postions of a square
struct Adjacent {
  int above = 0;
  int below = 0;
  int left  = 0;
  int right = 0;
};

// Indicates the arrow key pressed
enum Direction {RIGHT, LEFT, UP, DOWN};

struct GUI {
  GC::Canvas *canv;
  int width = 80;
  int length = 80;
  POINT origin = {0, 0};
  int side = 4;
  int emptySquare = 0;
  std::map<int, POINT> squareCoord;
  // Stores the id of the square at a certain position
  std::map<int, int> squareID;
  // Stores the position of the square with the id
  std::map<int, int> squarePos;
  int movesLabel = 0;
  int movesMade = 0;
  std::map<int, int> squareValues;
  GUI(GC::Canvas *canv, int side = 4, POINT start_ = {80, 20})
    : canv(canv), side(side) {
    origin = {start_.x, start_.y + 50};
    int count = std::pow(side, 2);
    POINT start = origin;
    POINT top = origin;
    POINT bottom = {top.x + width * side, top.y + length * side};
    int border = canv->rectangle(top.x, top.y, bottom.x, bottom.y);
    canv->fillColor(border, "tan");
    int number = 0;
    int id = 0;
    for (int i = 1; i <= count; i++) {
      POINT end = {start.x + length, start.y + width};
      GS::Box box = {start.x, start.y, end.x, end.y};
      if (i != count) {
        number++;
        id = canv->rectangle(box);
        char text[5];
        snprintf(text, 5, "%d", number);
        int txt;
        if (number < 10) {
          txt = canv->text(start.x + 30, start.y + 20, text);
        } else {
          txt = canv->text(start.x + 20, start.y + 20, text);
        }
        canv->setFont(txt, "", 25);
        Click click(this, id);
        canv->bind("<Mouse-1>", click, id);
        canv->fillColor(id, "white");
        int pos = i;
        squarePos[id] = pos;
        squareID[pos] = id;
        squareValues[id] = pos;
      } else {
        emptySquare = i;
      }
      squareCoord[i] = {start.x, start.y};
      start.x += length;
      if ((i % side) == 0) {
        origin.y += width;
        start = origin;
      }
    }
    movesLabel = canv->text(start.x, start.y + 5, "Moves: 0");
    canv->setFont(movesLabel, "bold", 15);
    canv->raiseShape("text", id); // Makes the numbers be on top
    canv->penSize(border, 4);
    scramble();
    int x2 = start_.x + 100;
    int y2 = start_.y + 40;
    int button = canv->rectangle(start_.x, start_.y, x2, y2);
    canv->fillColor(button, "grey");
    canv->bind("<Mouse-1>", Scrambler(this), button);
    int label = canv->text(start_.x + 15, start_.y + 10, "Scramble");
    canv->setFont(label, "bold");
  }

  // Scrambles by playing random moves starting from an already solved board
  void scramble() {
    std::vector<int> positions = genNumbers();
    for (unsigned i = 1; i < positions.size(); i++) {
      int direction = positions[i];
      Adjacent adj = adjacents(emptySquare);
      int move = 0;
      switch (direction) {
        case 1: {
          move = adj.left;
          break;
        }
        case 2: {
          move = adj.above;
          break;
        }
        case 3: {
          move = adj.right;
          break;
        }
        case 4: {
          move = adj.below;
          break;
        }
      };
      if (move) {
        int id = squareID[move];
        moveToEmptySquare(id, move);
      }
    }
    movesMade = 0;
    canv->setText(movesLabel, "Moves: 0");
  }

  // Generates the numbers indicating the direction of the moves to be made.
  std::vector<int> genNumbers() {
    std::srand(std::time(0));
    std::vector<int> lst;
    int val = 1;
    int threshold = 0;
    for (int i = 0; i < 100; i++) {
      if (threshold > 25) {
        val++;
        threshold = 0;
      }
      threshold++;
      lst.push_back(val);
    }
    std::random_shuffle(lst.begin(), lst.end());
    return lst;
  }

  // Returns the squares adjacent to the empty square
  Adjacent adjacents(int square) {
    int squareCount = side * side;
    Adjacent adjacent;
    if ((square > squareCount) || (square < 1)) {
      // Out of bounds value
      return adjacent;
    }
    int above = square + side;
    int below = square - side;
    if (above <= squareCount) {
      adjacent.above = above;
    }
    if (below >= 1) {
      adjacent.below = below;
    }
    int left = square - 1;
    int right = square + 1;
    int before = square - 1;
    if ((left >= 1) || (right <= squareCount)) {
      if (!(square % side) && (square != 1)) {
        // The square is at the right edge
        adjacent.left = left;
        return adjacent;
      } else if (!(before % side) && (before != 1)) {
        // The square is at the left edge
        adjacent.right = right;
        return adjacent;
      }
      adjacent.left = left;
      adjacent.right = right;
    }
    return adjacent;
  }

  bool areAdjacent(int square, int test) {
    Adjacent adjacent = adjacents(square);
    return (adjacent.left == test) ||
           (adjacent.right == test) ||
           (adjacent.above == test) ||
           (adjacent.below == test);
  }

  void moveToEmptySquare(int shapeID, int fromPos) {
    if (!areAdjacent(emptySquare, fromPos)) {
      return;
    }
    bool solved = moveSquare(shapeID, fromPos, emptySquare);
    emptySquare = fromPos;
    char text[30];
    if (solved) {
      snprintf(text, 30, "Solved in %d moves", movesMade);
      canv->setText(movesLabel, text);
      movesMade = 0;
      return;
    }
    movesMade++;
    snprintf(text, 30, "Moves: %d", movesMade);
    canv->setText(movesLabel, text);
  }

  bool solved() {
    bool res = true;
    for (auto str : squareValues) {
      int value = squareValues[str.first];
      int pos = squarePos[str.first];
      res &= pos == value;
    }
    return res;
  }

  // Moves the empty square if they are adjacent.
  bool moveSquare(int shapeID, int fromPos, int toPos) {
    POINT pointFrom = squareCoord[fromPos];
    POINT pointTo = squareCoord[toPos];
    POINT vec = {pointTo.x - pointFrom.x, pointTo.y - pointFrom.y};
    canv->moveShape(shapeID, vec.x, vec.y);
    canv->moveShape(shapeID + 1, vec.x, vec.y);
    squareID.erase(fromPos);
    squareID[toPos] = shapeID;
    squarePos[shapeID] = toPos;
    if (solved()) {
      return true;
    }
    return false;
  }

  // Moves the square in response to an arrow key movement
  void moveSquare(Direction moveDirection) {
    Adjacent adj = adjacents(emptySquare);
    int pos = 0;
    switch (moveDirection) {
      case UP:
        pos = adj.above;
        break;
      case DOWN:
        pos = adj.below;
        break;
      case LEFT:
        pos = adj.right;
        break;
      case RIGHT:
        pos = adj.left;
    }
    if (pos) {
      int id = squareID[pos];
      moveToEmptySquare(id, pos);
    }
  }

  // Returns a list of all the squares to the left up to the clicked square
  std::vector<int> leftSquares(int clickPos) {
    int pos = adjacents(emptySquare).left;
    std::vector<int> squares;
    while (pos) {
      if (pos == clickPos) {
        squares.push_back(clickPos);
        return squares;
      } else {
        squares.push_back(pos);
        pos = adjacents(pos).left;
      }
    }
    return {};
  }

  // Returns a list of all the squares to the right up to the clicked square
  std::vector<int> rightSquares(int clickPos) {
    int pos = adjacents(emptySquare).right;
    std::vector<int> squares;
    while (pos) {
      if (pos == clickPos) {
        squares.push_back(clickPos);
        return squares;
      } else {
        squares.push_back(pos);
        pos = adjacents(pos).right;
      }
    }
    return {};
  }

  // Returns a list of all the squares below up to the clicked square
  std::vector<int> bottomSquares(int clickPos) {
    int pos = adjacents(emptySquare).below;
    std::vector<int> squares;
    while (pos) {
      if (pos == clickPos) {
        squares.push_back(clickPos);
        return squares;
      } else {
        squares.push_back(pos);
        pos = adjacents(pos).below;
      }
    }
    return {};
  }

  // Returns a list of all the squares above up to the clicked square
  std::vector<int> topSquares(int clickPos) {
    int pos = adjacents(emptySquare).above;
    std::vector<int> squares;
    while (pos) {
      if (pos == clickPos) {
        squares.push_back(clickPos);
        return squares;
      } else {
        squares.push_back(pos);
        pos = adjacents(pos).above;
      }
    }
    return {};
  }

  void moveMultiple(int clickedSquare) {
    // Shifts multple squares at once.
    // XXX: Bends the rules of the original game.
    std::vector<int> tops = topSquares(clickedSquare);
    std::vector<int> lefts = leftSquares(clickedSquare);
    std::vector<int> rights = rightSquares(clickedSquare);
    std::vector<int> bottoms = bottomSquares(clickedSquare);
    std::vector<int> squares;
    if (std::find(tops.begin(), tops.end(), clickedSquare) != tops.end()) {
      squares = tops;
    } else if (std::find(bottoms.begin(), bottoms.end(), clickedSquare) !=
               bottoms.end()) {
      squares = bottoms;
    } else if (std::find(lefts.begin(), lefts.end(), clickedSquare) !=
               lefts.end()) {
      squares = lefts;
    } else if (std::find(rights.begin(), rights.end(), clickedSquare) !=
               rights.end()) {
      squares = rights;
    }
    for (int square : squares) {
      int id = squareID[square];
      moveToEmptySquare(id, square);
    }
  }
};

void Click::handle(GC::Mouse) {
  Sleep(100);
  int pos = gui->squarePos[squareID];
  if (gui->areAdjacent(pos, gui->emptySquare)) {
    gui->moveToEmptySquare(squareID, pos);
  } else {
    gui->moveMultiple(pos);
  }
}

// Handles arrow key presses
struct ArrowKey : GC::EventHandler {
  Direction direction;
  GUI *gui;
  ArrowKey(GUI *gui, Direction key) : direction(key), gui(gui) {}
  virtual void handle(GC::Mouse) override {
    Sleep(100);
    gui->moveSquare(direction);
  }
};

void Scrambler::handle(GC::Mouse) {
  gui->scramble();
}

int main() {
  GC::Canvas canv(500, 500, "Mystic Square");
  canv.init();
  canv.background("tan");
  canv.noMaximize();
  GUI gui(&canv, 3);
  canv.bind("<Key-Esc>", Exit(&canv));
  canv.bind("<Key-Up>", ArrowKey(&gui, UP));
  canv.bind("<Key-Down>", ArrowKey(&gui, DOWN));
  canv.bind("<Key-Left>", ArrowKey(&gui, LEFT));
  canv.bind("<Key-Right>", ArrowKey(&gui, RIGHT));
  return canv.loop();
}
