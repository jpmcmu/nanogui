/*
    nanogui/screen.h -- Top-level widget and interface between NanoGUI and GLFW

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Screen screen.h nanogui/screen.h
 *
 * \brief Represents a display surface (i.e. a full-screen or windowed GLFW window)
 * and forms the root element of a hierarchy of nanogui widgets.
 */
class NANOGUI_EXPORT Screen : public Widget {
    friend class Widget;
    friend class Window;
public:
    /**
     * \brief Default constructor
     *
     * Performs no initialization at all. Use this if the application is
     * responsible for setting up GLFW, OpenGL, etc.
     *
     */
    Screen();

    /// Release all resources
    virtual ~Screen();

    /// Initialize the \ref Screen
    void initialize(void* platformWindow);

    /// Return the screen's background color
    const Color &background() const { return mBackground; }

    /// Set the screen's background color
    void setBackground(const Color &background) { mBackground = background; }

    /// Draw the Screen contents
    virtual void drawAll();

    /// Draw the window contents --- put your OpenGL draw calls here
    virtual void drawContents() { /* To be overridden */ }

    /// Return the ratio between pixel and device coordinates (e.g. >= 2 on Mac Retina displays)
    float pixelRatio() const { return mPixelRatio; }

    /// Handle a file drop event
    virtual bool dropEvent(const std::vector<std::string> & /* filenames */) { return false; /* To be overridden */ }

    /// Default keyboard event handler
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

    /// Text input event handler: codepoint is native endian UTF-32 format
    virtual bool keyboardCharacterEvent(unsigned int codepoint);

    /// Window resize event handler
    virtual bool resizeEvent(const Vector2i &) { return false; }

    /// Return the last observed mouse position value
    Vector2i mousePos() const { return mMousePos; }

    /// Return a pointer to the underlying platform window
    void* platformWindow() {return mPlatformWindow; }

    /// Return a pointer to the underlying nanoVG draw context
    NVGcontext *nvgContext() { return mNVGContext; }

    using Widget::performLayout;

    /// Compute the layout of all widgets
    void performLayout() {
        Widget::performLayout(mNVGContext);
    }

    /* Event handlers */
    bool cursorPosCallbackEvent(double x, double y);
    bool mouseButtonCallbackEvent(int button, int action, int modifiers);
    bool keyCallbackEvent(int key, int scancode, int action, int mods);
    bool charCallbackEvent(unsigned int codepoint);
    bool dropCallbackEvent(int count, const char **filenames);
    bool scrollCallbackEvent(double x, double y);
    bool resizeCallbackEvent(int width, int height);

    /* Internal helper functions */
    void updateFocus(Widget *widget);
    void disposeWindow(Window *window);
    void centerWindow(Window *window);
    void moveWindowToFront(Window *window);
    void drawWidgets();

protected:
    void *mPlatformWindow;
    NVGcontext *mNVGContext;
    Cursor mCursor;
    std::vector<Widget *> mFocusPath;
    Vector2i mFBSize;
    float mPixelRatio;
    int mMouseState, mModifiers;
    Vector2i mMousePos;
    bool mDragActive;
    Widget *mDragWidget = nullptr;
    double mLastInteraction;
    bool mProcessEvents;
    Color mBackground;
    std::string mCaption;
    bool mFullscreen;
};

NAMESPACE_END(nanogui)
