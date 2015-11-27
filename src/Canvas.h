/**
 * \file Canvas.h
 */

#ifndef Canvas_H_
#define Canvas_H_

#define GDICANVAS_VERSION "1.0.0"
#define _WIN32_WINNT 0x501

#include <windows.h>
#include <cstring>
#include <string>
#include <map>
#include <cstdio>
#include <cstdarg>
#include <vector>
#include <algorithm>
#include <memory>
#include "./Colors.h"
#include "./Vec2D.h"
#include "./Shapes.h"
#include "./logo.h"
#include "./VirtualKeys.h"

namespace GS = GShape;

/*!
 * \namespace GCanvas
 * \brief Canvas stuff
 */
namespace GCanvas {

/*!
 * \enum EventType
 * \brief Lists all events handled in the canvas.
 *
 * The event strings will be parsed by in Canvas::parseEventString
 */
enum EventType {
  //! specified as `<timer>`
  TIMER,
  //! specified as `<hover>`
  HOVER,
  //! specified as `<Mouse-1>`
  LEFT_CLICK,
  //! specified as `<Ctrl-Mouse-1>`
  CTRL_LEFT_CLICK,
  //! specified as `<Alt-Mouse-1>`
  ALT_LEFT_CLICK,
  //! specified as `<Mouse-2>`
  RIGHT_CLICK,
  //! specified as `<Wheel-Click>`
  WHEEL_CLICK,
  //! specified as `<Wheel-Spin>`
  WHEEL_ROLL,
  //! Unmodified key press, e.g `<Key-Q>`
  BARE_KEY,
  //! e.g `<Alt-W>`
  ALT_KEY,
  //! e.g `<Alt-Shift-R>`
  ALT_SHIFT_KEY,
  //! e.g `<Ctrl-U>`
  CTRL_KEY,
  //! e.g `<Ctrl-Shift-S>`
  CTRL_SHIFT_KEY,
  //! Used to indicate an invalid event when parsing the event string.
  INVALID_EVENT
};

/*!
 * \class Mouse
 *
 * \brief Contains methods used to query the cursor position
 */
class Mouse {
    POINT cursorCoord;
    HWND winHandle;
    //! Indicates the direction of the wheel roll. Either -1 or 1
    int wheelDelta = 0;
  public:
    explicit Mouse(HWND winHandle_, int wheelDelta_ = 0) {
      winHandle = winHandle_;
      wheelDelta = wheelDelta_;
    }

    //! Returns the wheel roll's direction
    int delta();

    //! Cursor position from top left of screen
    int xRoot();
    int yRoot();

    //! Cursor position from top left of window
    int x();
    int y();
};

/*!
 * \interface EventHandler
 *
 * \brief The super class of which all event handlers must inherit from.
 *
 * It's basically used to implement the heterogeneous container of event handlers
 */
class EventHandler {
  public:
    virtual void handle(Mouse mouse) = 0;
    virtual ~EventHandler() {}
};

/*!
 * \struct Event
 *
 * \brief Contains the necessary information used to identify an event
 */
struct Event {
  //! ASCII code of the key that invokes this event.
  int keyToHandle = -999;
  // A shape id and tag are needed in mouse events. The mouse event handler will
  // be called only if the mouse position is within that shape.
  int shapeID = -1;
  int timerID = -1;
  std::string shapeTag = "";
  std::shared_ptr<EventHandler> handler = std::shared_ptr<EventHandler>(nullptr);
  EventType eventType = INVALID_EVENT;
  Event(EventHandler *functor, EventType type) :
      handler(std::shared_ptr<EventHandler>(functor)) {
    eventType = type;
  }
};

//! Checks if any of the shift keys have been pressed
bool shiftKeyDown();

//! Checks if any of the control keys(CTRL) have been pressed
bool ctrlKeyDown();

//! Checks if any of the alternate keys(ALT) have been pressed
bool altKeyDown();

/*!
 * \class Canvas
 * \brief Main Canvas class.
 *
 * Manages all the items in the canvas/window and interfaces with native WinAPI.
 */
class Canvas {
  public:
    //! Adds \b newTag to all items with a \b shapeID above the specified one.
    bool tagAbove(const std::string &newTag, int shapeID);

    //! Adds \b newTag to all items with a \b shapeID below the specified one.
    bool tagBelow(const std::string &newTag, int shapeID);

    //! Adds \b newTag to all item in the canvas
    bool tagAll(const std::string &newTag);

    //! Adds \b newTag to the item closest to screen coordinate `(x, y)`
    bool tagClosest(const std::string &newTag, int x, int y);

    /*!
     * \brief Adds \b newTag to all objects that occur completely within the
     * rectangle whose top left corner is `(x1, y1)` and whose bottom right
     * corner is `(x2,y2)`.
     */
    bool tagEnclosed(const std::string &newTag, int x1, int y1, int x2, int y2);

    //! \overload tagEnclosed(const std::string&, int, int, int, int)
    bool tagEnclosed(const std::string &newTag, GS::Box region);

    /*! Adds \b newTag to all elements that share at least one point with the
     * rectangle specified.
     */
    bool tagOverlapping(const std::string &newTag, int x1, int y1, int x2, int y2);

    //! \overload tagOverlapping(const std::string&, int, int, int, int)
    bool tagOverlapping(const std::string &newTag, GS::Box region);

    //! Adds \b newTag to the items with the specified \b newTag
    bool tagWithTag(const std::string &tagName, const std::string &newTag);

    //! Adds \b newTag to the items with the specified \b shapeID
    bool tagWithTag(int shapeID, const std::string &newTag);

    /*!
     * \brief Removes the tag \b tagToDelete from the shape with the specified
     * \b shapeID
     */
    bool deleteTag(int shapeID, const std::string &tagToDelete);

    /*!
     * \brief Returns a structure `{x1, y1, x2, y2}` describing a rectangle
     * that encloses all the objects specified by tagOrId.
     *
     * Returns {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX} of failure
     */
    GS::Box BBox(const std::vector<int> &shapes);

    //! \overload BBox(const std::vector<int>& shapes)
    GS::Box BBox(const std::vector<std::string> &shapes);

    //! \overload BBox(const std::vector<int>& shapes)
    GS::Box BBox(int shapeID);

    /*!
     * \brief Returns the shape's coordinates
     */
    std::vector<POINT> coords(int id);

    //! Changes the shape's coordinates. Ovewrites the previous ones.
    bool coords(int shapeID, const std::vector<POINT> &newCoords);

    /*!
     * \brief Returns all the shapes with an id above \p id
     */
    std::vector<int> findAbove(int id);

    /*!
     * \brief Pause execution for the specified milliseconds
     */
    void sleep(int millSecs);

    /*!
     * \brief Finds all shape's with an id above the specified one
     * \see findAbove() for an example
     */
    std::vector<int> findBelow(int id);

    /*!
     * \brief Returns ids of all shapes in the canvas
     */
    std::vector<int> findAll();

    /*!
     * \brief Finds all items that occur completely within region
     * `{x1, y1, x2, y2}`
     */
    std::vector<int> findEnclosed(int x1, int y1, int x2, int y2);

    /*!
     * \brief Finds all items that share a point with region `{x1, y1, x2, y2}`
     */
    std::vector<int> findOverlapping(int x1, int y1, int x2, int y2);

    /*!
     * \brief Finds the closest item to pixel `(X, Y)`
     */
    std::vector<int> findClosest(int x, int y);

    /*!
     * \brief Finds all items with the specified tag.
     */
    std::vector<int> findWithTag(const std::string &tag);

    /*!
     * \brief Returns all the specified shape's tags
     *
     */
    std::vector<std::string> getTags(int id);

    /*!
     * Moves all the shapes with the specified tag by through vector `(x, y)`.
     */
    bool moveShape(const std::string &shapeTag, int xAmount, int yAmount);

    //! \overload moveShape(const std::string, int, int)
    bool moveShape(int shapeID, int xAmount, int yAmount);

    //! Makes the shape invisible by not redrawing it
    bool hideShape(const std::string &tagName, bool visible = false);

    //! \overload (const std::string &tagName, bool visible = false)
    bool hideShape(int shapeID, bool visible = false);

    //! Makes the shape with the tag visible
    bool showShape(const std::string &tagName);

    //! \overload showShape(const std::string)
    bool showShape(int shapeID);

    //! Returns true if the shape is visible on the screen
    bool isVisible(int shapeID);

    /*!
     * \brief Binds the key sequence to the event.
     *
     * \note Eliding the `id` or `tag` when specifying adding a mouse event
     * handler causes the handler to be called when the event happens anywhere
     * within the window. E.g
     *
     * \code
     *   struct Robot : EventHandler {
     *     void handler(Mouse) {
     *       MessageGS::Box("Doing the robot...");
     *     }
     *   };
     *
     *   Canvas canv;
     *   int rect = canv.rectangle(10, 100, 400, 500);
     *   canv.fillColor("tan");
     *   canv.bind("<hover>", Robot(), rect); // fired when above the rectangle
     *   canv.bind("<hover>", Robot()); // fired when over the whole window
     * \endcode
     *
     */
    template<typename FunctorType>
    bool bind(const std::string &eventString,
              FunctorType funcType,
              int shapeID) {
      std::string keyStr("");
      EventType type = parseEventString(eventString, &keyStr);
      FunctorType *func_ = new FunctorType(funcType);
      Event event(func_, type);
      event.shapeID = shapeID;
      return addHandler(event, keyStr);
    }

    //! \overload bind(std::string, FunctorType, int)
    template<typename FunctorType>
    bool bind(const std::string &eventString,
              FunctorType funcType,
              const std::string &tagName = "") {
      std::string keyStr("");
      EventType type = parseEventString(eventString, &keyStr);
      FunctorType *func_ = new FunctorType(funcType);
      Event event(func_, type);
      event.shapeTag = tagName;
      return addHandler(event, keyStr);
    }

    /*!
     * \brief Removes all the event handlers associated with the specified object
     *
     * \code
     *   GC::Canvas canv;
     *   int id = canv.circle(400, 500, 50);
     *   canv.unbind("<Mouse-1>", id);
     * \endcode
     *
     */
    bool unbind(const std::string &eventString, int shapeID);

    //! \overload unbind(const std::string, EventHandler, int)
    bool unbind(const std::string &eventString, const std::string &tag = "");

    //! Add a timer event. The function will be called once.
    template<typename FunctorType>
    bool timer(int millSecs, FunctorType func) {
      int timerID = ++timerCount;
      SetTimer(winHandle, timerID, millSecs, NULL);
      FunctorType *func_ = new FunctorType(func);
      Event event(func_, TIMER);
      event.timerID = timerID;
      return addHandler(event, "<timer>");
    }

    //! Returns the shape type of the shape with the specified id.
    GS::ShapeType shapeType(int id);

    /*!
     * \brief Calculates the correct bounding box coordinates in the case where
     * the first point is ahead of the second.
     *
     * \param[out] [x1, y1, x2, y2] The coordinates of the two points.
     *
     * \code
     *   int x1 = 400;
     *   int y2 = 700;
     *   int x2 = 500;
     *   int y1 = 600;
     *   fixBBoxCoord(x1, y1, x2, y2);
     *   printf("{%d, %d, %d, %d}\n", x1, y1, x2, y2); // {400, 600, 500, 700}
     * \endcode
     */
    void fixBBoxCoord(int *x1, int *y1, int *x2, int *y2);

    /*!
     * \brief Draws an arc on the screen.
     *
     * \param[in] box   The top left and bottom right coordinates.
     * \param[in] arcType A choice between a plain arc, a chord or a pie
     * \param[in] pieSize The size of the arc in degrees measures from the
     *                  right half of the center line.
     * \param[in] tiltAngle The starting position of the arc. Can take negative
     *                  angles
     */
    int arc(GS::Box box,
            GS::ArcType arcType = GS::CHORD,
            float pieSize = 60.0f,
            float tiltAngle = 0.0f);

    /*!
     * \overload arc(GS::Box, GS:ArcType, float, float)
     */
    int arc(int x1,
            int y1,
            int x2,
            int y2,
            GS::ArcType arcType = GS::CHORD,
            float pieSize = 60.0f,
            float tiltAngle = 0.0f);

    /*!
     * \brief Draws a line on the screen.
     *
     * \code
     *   Canvas canv;
     *   canv.init()
     *   canv.line({{490, 173}, {359, 351}, {597, 337}, {488, 171}, {325, 185}});
     * \endcode
     */
    int line(const std::vector<POINT> &lineCoords);

    /*!
     * \brief Draws a rectangle on the screen
     */
    int rectangle(int x1, int y1, int x2, int y2);

    /*!
     * \overload (int x1, int y1, int x2, int y2)
     * \param box A struct `{left, top, right, bottom}` representing the top
     *  left and bottom right coordinates.
     */
    int rectangle(GS::Box box);

    /*!
     * \brief Draws an ellipse(oval)
     *
     * \param[in] [x1, y1, x2, y2] The oval's bounding box coordinates
     */
    int oval(int x1, int y1, int x2, int y2);

    //! \overload oval(int, int, int, int)
    int oval(GS::Box box);

    //! Draws a circle
    int circle(int x, int y, int radius);

    //! \overload circle(int, int, int)
    int circle(POINT point, int radius);

    /*!
     * \brief Draws text on the screen
     *
     * Specifying the width will cause the text to be truncated in case it doesn't
     * fit within the specified region.
     *
     * \note It won't wrap the text
     */
    int text(int x, int y, const std::string &txtStr, int width = 0);

    //! \overload text(int, int, const std::string&, int)
    int text(POINT point, const std::string &txtStr, int width = 0);

    //! Returns the shape's text
    std::string getText(int shapeID);

    //! Changes the shape's text
    void setText(const std::string &tagName, const std::string &text);

    //! \overload setText(int, const std::string&)
    void setText(int shapeID, const std::string &text);

    /*!
     * \brief Draws a polygon on the screen.
     */
    int polygon(const std::vector<POINT> &lineCoords);

    /*! Moves the item with id \p first above the item with id \p second in the
     * display list
     *
     * \return __true__  If the element was moved
     * \return __false__ If one of the elements wasn't found or the first is
     * already at a higher position in the display list.
     */
    bool raiseShape(int first, int second);

    /*!
     * \brief Moves all the items with the specified tag above the shape with
     * tag id \p target
     */
    bool raiseShape(const std::string &others, int target);

    //! Moves the item with id \p first below the item with id \p second in the
    //! display list
    bool lowerShape(int first, int second);

    /*!
     * \brief Moves all items with the specified tag below the shape with the id
     */
    bool lowerShape(const std::string &others, int target);

    /*!
     * \brief Registers the window and displays it
     *
     * Some canvas methods that change the window's behaviour will not work when
     * invoked before init() is called.
     *
     * \param[in] hInstance A handle to the current program's instance, passed
     *          as an argument to WinMain.
     *
     * \param[in] cmdShow Determines how the program will be displayed on startup
     * depending on settings made in the system by the user(in *explorer*), e.g
     * *maximized*, *minimized*, *normal*. This value is also obtained from
     * `WinMain`
     *
     * \returns The program's exit code.
     *
     * \code
     *   int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int show) {
     *     Canvas canv;
     *     canv.init()
     *     canv.init(inst, show);
     *     return canv.loop();
     *   }
     * \endcode
     *
     */
    int init(HINSTANCE hInstance, int cmdShow = SW_SHOWNORMAL);

    /*!
     * \brief Displays the window normally(with the set dimensions and positions).
     *
     * The created window will be centered by default.
     */
    int init();

    //! Removes the shape from the screen.
    bool removeShape(const std::string &tagName);

    //! \overload removeShape(const std::string&)
    bool removeShape(int shapeID);

    //! Change the border size/pen size
    bool penSize(int shapeID, int width);

    //! \overload (int shapeID, int width)
    bool penSize(const std::string &tagName, int width);

    /*!
     * \brief returns the shape's fill color
     */
    std::string fillColor(int shapeID);

    /*!
     *  \brief Changes a shape's fill color
     *
     *  The color can be specified in three ways(using the overloads):
     *  \code
     *    Canvas canv;
     *    int rect = canv.rectangle(12, 55, 100, 654); // Unfilled rectangle
     *    canv.fillColor(rect, "yellow");
     *    canv.fillColor("all", "#FFFF00");
     *    canv.fillColor("all", 255, 252, 0);
     *  \endcode
     */
    bool fillColor(const std::string &tagName, std::string colorString);

    //! \overload fillColor(const std::string&, std::string)
    bool fillColor(const std::string &tagName, int red, int green, int blue);

    //! \overload fillColor(const std::string&, int, int, int)
    bool fillColor(int shapeID, std::string colorString);

    //! \overload fillColor(int, int, int, int)
    bool fillColor(int shapeID, int red, int green, int blue);

    /*!
     * \brief returns the shape's pen color
     */
    std::string penColor(int shapeID);

    //! Changes a shape's border color
    bool penColor(const std::string &tagName, std::string colorString);

    //! \overload penColor(const std::string&, std::string)
    bool penColor(const std::string &tagName, int red, int green, int blue);

    //! \overload penColor(const std::string, int, int, int)
    bool penColor(int shapeID, int red, int green, int blue);

    //! \overload penColor(int, int, int, int)
    bool penColor(int shapeID, std::string colorString);

    //! Returns the shape's border style.
    GS::BorderStyle borderStyle(int shapeID);

    //! Set border style
    bool borderStyle(const std::string &tag, GS::BorderStyle style);

    //! \overload borderStyle(const std::string&, GS:BorderStyle)
    bool borderStyle(int id, GS::BorderStyle style);

    /*!
     * \brief Changes the shape's font attributes
     * \return **true** if any shape with the id was found
     *
     * \code
     *   canv.setFont(id, "strikeout underline bold", 15, "Fixedsys");
     * \endcode
     */
    bool setFont(int shapeID,
                 const std::string &fontStyle,
                 int size = 12,
                 const std::string &fontFamily = "");

    //! \overload setFont(int, const std::string&, const std::string&, int)
    bool setFont(const std::string &tag,
                 const std::string &fontStyle,
                 int size = 12,
                 const std::string &fontFamily = "");

    //! Returns the shape's font attributes
    GS::FontAttr getFont(int shapeID);

    //! Returns the width and height of the screen, `{width, height}`
    //! You must call it after the screen has been registered.
    POINT screenSize();

    //! Brings up a console window enabling trace logging with IO functions
    //! It reopens `stderr` and `stdout`
    //! \note Showing the console will cause the icon to disappear.
    bool showConsole();

    //! Closes the console window.
    bool killConsole();

    /*!
     * \brief Sets an icon to the program at runtime.
     *
     * \param[in] iconPath Path to the image
     *
     * The choice of path separators is inconsequential.
     *
     * \note
     * It must be invoked after init() has been called.
     * \code
     *   Canvas canv;
     *   canv.init();
     *   canv.icon("src/logo.ico");
     *   canv.icon("src\logo.ico");
     * \endcode
     *  An alternative way of setting an icon(permanently) is through a resource
     *  file. You can create your own or use the one that comes with the
     *  library.
     *  The icon is added by linking with the object file:
     *  \code
     *    windres icon.rc icon.o
     *    g++ -o test.exe test.cxx icon.o
     *  \endcode
     */
    bool icon(const char *iconPath);

    //! Change window background color
    void background(int red, int green, int blue);

    //! \overload background(int red, int green, int blue)
    void background(std::string colorString);

    //! Disable maximize button
    void noMaximize();

    //! Disable minimize button
    void noMinimize();

    //! Returns the path to the icon file being used be the program currently.
    //! It'll only be true if the icon was set using the setIcon method.
    std::string icon();

    //! Changes window title
    bool title(const char *title);

    //! Returns current window title
    std::string title();

    //! Handles all window messages sent by the system
    LRESULT CALLBACK handleMessage(const HWND &winHandle,
                                   unsigned message,
                                   WPARAM wParam,
                                   LPARAM lParam);

    /*!
     * \brief Displays the window and runs the message loop.
     *
     * The method is usually placed last in the `main`/`WinMain` function
     *
     * \code
     *   int main(){
     *     Canvas canv;
     *     canv.init();
     *     return canv.loop();
     *   }
     * \endcode
     * \return An integer returned by the window message handler.
     */
    int loop();

    //! Returns the window's dimensions. `x` is width and `y` length
    POINT windowSize();

    //! Changes the window position such that the coordinate of the top left corner
    //! is at `{x, y}`
    void moveWindow(int newX, int newY);

    //! Changes the window's dimensions
    void resizeWindow(int width, int height);

    //! Returns the coordinate of the top left corner of the window
    POINT windowPos();

    //! Maximizes the window
    void maximize();

    //! Minimizes the window
    void minimize();

    //! Destroys the window
    bool kill();

    //! Creates the window with the default dimensions
    Canvas() {}

    //! Creates a window with the given size and position.
    Canvas(int x, int y, int width, int height);

    //! Creates a window of the specified size
    //! The extra parameter is just there to make this ctor overload
    Canvas(int width, int height, const char *title = "Canvas");

    //! Creates window positioned at `(x, y)`
    Canvas(int x, int y);

    /*!
     * \brief Returns the window's handle.
     *
     * Useful when displaying message boxes especially when you don't wan't the
     * message box to be ignored.
     */
    HWND handle();

  private:
    Canvas(const Canvas &);
    Canvas &operator=(const Canvas &);

    //! Callback function needed to be invoked by the system
    static LRESULT CALLBACK windowProcedure(HWND winHandle,
                                            unsigned message,
                                            WPARAM wParam,
                                            LPARAM lParam);

    // Supposed to cause the change made by SetWindowLongPtr to take effect
    void refreshWindow();

    //! Calls all the functions in the events map that have the same event
    //! type and handle the same key.
    bool callHandlers(EventType type, int key = 0);

    /*!
     * \brief Parses the event string and adds the event to the events map.
     *
     * The string must start and end with an angle bracket.
     *
     * Event's like \b <Ctrl-Shift-Q> and \b <Shift-Ctrl-Q> are considered to
     * be same.
     *
     * \see EventType for accepted format.
     *
     * \param[in] eventString The event specifier, in angle brackets.
     * \param[out] keyString A variable to hold the key. The key will be used to
     * lookup the keycode. The variable will be empty on invalid event string.
     *
     * \code
     *   std::string keyStr;
     *   parseEventString("<Ctrl-Shift-A>", keyStr); // Returns CTRL_SHIFT_KEY
     *   std::cout << keyStr; "A"
     * \endcode
     *
     */
    EventType parseEventString(std::string eventString, std::string *keyString);

    bool addHandler(Event event, const std::string &keyStr);

    // Makes the program listen for mouse move messages. Needed for <hover> event
    bool trackMouse();

    /*!
     * \brief Adds the shape to the list.
     * \returns -1 if the shape already exists. Otherwise a value greater than -1.
     */
    int addShape(GS::Shape *newShape);

    int timerCount = 0;
    int cmdShow = SW_SHOWNORMAL;
    int winHeight = 700;
    int winWidth = 700;
    int xPos = 100;
    int yPos = 50;
    std::string iconFile = "logo.ico";
    std::string windowTitle = "Canvas";
    unsigned classStyle = CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    unsigned windowStyle = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
                           WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
    WNDCLASSEX windowClassEx;
    HWND winHandle;
    HINSTANCE winInst = GetModuleHandle(NULL);
    MSG windowMessage;
    std::map<EventType, std::vector<Event>> events;
    std::vector<std::shared_ptr<GS::Shape>> shapeList;
};

}

namespace GC = GCanvas;

#endif
