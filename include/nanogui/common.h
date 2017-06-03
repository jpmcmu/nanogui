/*
    nanogui/common.h -- common definitions used by NanoGUI

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#ifndef NG_COMMON
#define NG_COMMON

#include <Eigen/Core>
#include <stdint.h>
#include <array>
#include <vector>

/* Set to 1 to draw boxes around widgets */
//#define NANOGUI_SHOW_WIDGET_BOUNDS 1

#if !defined(NAMESPACE_BEGIN) || defined(DOXYGEN_DOCUMENTATION_BUILD)
    /**
     * \brief Convenience macro for namespace declarations
     *
     * The macro ``NAMESPACE_BEGIN(nanogui)`` will expand to ``namespace
     * nanogui {``. This is done to hide the namespace scope from editors and
     * C++ code formatting tools that may otherwise indent the entire file.
     * The corresponding ``NAMESPACE_END`` macro also lists the namespace
     * name for improved readability.
     *
     * \param name
     *     The name of the namespace scope to open
     */
    #define NAMESPACE_BEGIN(name) namespace name {
#endif
#if !defined(NAMESPACE_END) || defined(DOXYGEN_DOCUMENTATION_BUILD)
    /**
     * \brief Convenience macro for namespace declarations
     *
     * Closes a namespace (counterpart to ``NAMESPACE_BEGIN``)
     * ``NAMESPACE_END(nanogui)`` will expand to only ``}``.
     *
     * \param name
     *     The name of the namespace scope to close
     */
    #define NAMESPACE_END(name) }
#endif

#if defined(NANOGUI_SHARED)
#  if defined(_WIN32)
#    if defined(NANOGUI_BUILD)
#      define NANOGUI_EXPORT __declspec(dllexport)
#    else
#      define NANOGUI_EXPORT __declspec(dllimport)
#    endif
#  elif defined(NANOGUI_BUILD)
#    define NANOGUI_EXPORT __attribute__ ((visibility("default")))
#  else
#    define NANOGUI_EXPORT
#  endif
#else
     /**
      * If the build flag ``NANOGUI_SHARED`` is defined, this directive will expand
      * to be the platform specific shared library import / export command depending
      * on the compilation stage.  If undefined, it expands to nothing. **Do not**
      * define this directive on your own.
      */
#    define NANOGUI_EXPORT
#endif

/* Force usage of discrete GPU on laptops (macro must be invoked in main application) */
#if defined(_WIN32)
#define NANOGUI_FORCE_DISCRETE_GPU() \
    extern "C" { \
        __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; \
        __declspec(dllexport) int NvOptimusEnablement = 1; \
    }
#else
/**
 * On Windows, exports ``AmdPowerXpressRequestHighPerformance`` and
 * ``NvOptimusEnablement`` as ``1``.
 */
#define NANOGUI_FORCE_DISCRETE_GPU()
#endif

#if defined(_WIN32)
#if defined(NANOGUI_BUILD)
/* Quench a few warnings on when compiling NanoGUI on Windows */
#pragma warning(disable : 4127) // warning C4127: conditional expression is constant
#pragma warning(disable : 4244) // warning C4244: conversion from X to Y, possible loss of data
#endif
#pragma warning(disable : 4251) // warning C4251: class X needs to have dll-interface to be used by clients of class Y
#pragma warning(disable : 4714) // warning C4714: function X marked as __forceinline not inlined
#endif

// These will produce broken links in the docs build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

struct NVGcontext { /* Opaque handle type, never de-referenced within NanoGUI */ };
struct GLFWwindow { /* Opaque handle type, never de-referenced within NanoGUI */ };

struct NVGcolor;
struct NVGglyphPosition;
struct GLFWcursor;

#endif // DOXYGEN_SHOULD_SKIP_THIS

// Define command key for windows/mac/linux
#ifdef __APPLE__
/// If on OSX, maps to ``NG_MOD_SUPER``.  Otherwise, maps to ``NG_MOD_CONTROL``.
#define SYSTEM_COMMAND_MOD NG_MOD_SUPER
#else
/// If on OSX, maps to ``NG_MOD_SUPER``.  Otherwise, maps to ``NG_MOD_CONTROL``.
#define SYSTEM_COMMAND_MOD NG_MOD_CONTROL
#endif

NAMESPACE_BEGIN(nanogui)

/// Cursor shapes available to use in GLFW.
enum class Cursor {
    Arrow = 0,
    IBeam,
    Crosshair,
    Hand,
    HResize,
    VResize,
    /// Not a cursor --- should always be last: enables a loop over the cursor types.
    CursorCount
};

/* Import some common Eigen types */
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Vector2i;
using Eigen::Vector3i;
using Eigen::Vector4i;
using Eigen::Matrix3f;
using Eigen::Matrix4f;
using Eigen::VectorXf;
using Eigen::MatrixXf;

/**
 * Convenience typedef for things like index buffers.  You would use it the same
 * as ``Eigen::MatrixXf``, only it is storing ``uint32_t`` instead of ``float``.
 */
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;

/**
 * \class Color common.h nanogui/common.h
 *
 * \brief Stores an RGBA floating point color value.
 *
 * This class simply wraps around an ``Eigen::Vector4f``, providing some convenient
 * methods and terminology for thinking of it as a color.  The data operates in the
 * same way as ``Eigen::Vector4f``, and the following values are identical:
 *
 * \rst
 * +---------+-------------+-----------------------+-------------+
 * | Channel | Array Index | Eigen::Vector4f Value | Color Value |
 * +=========+=============+=======================+=============+
 * | Red     | ``0``       | x()                   | r()         |
 * +---------+-------------+-----------------------+-------------+
 * | Green   | ``1``       | y()                   | g()         |
 * +---------+-------------+-----------------------+-------------+
 * | Blue    | ``2``       | z()                   | b()         |
 * +---------+-------------+-----------------------+-------------+
 * | Alpha   | ``3``       | w()                   | w()         |
 * +---------+-------------+-----------------------+-------------+
 *
 * .. note::
 *    The method for the alpha component is **always** ``w()``.
 * \endrst
 *
 * You can and should still use the various convenience methods such as ``any()``,
 * ``all()``, ``head<index>()``, etc provided by Eigen.
 */
class Color : public Eigen::Vector4f {
    typedef Eigen::Vector4f Base;
public:
    /// Default constructor: represents black (``r, g, b, a = 0``)
    Color() : Color(0, 0, 0, 0) {}

    /**
     * Makes an exact copy of the data represented by the input parameter.
     *
     * \param color
     * The four dimensional float vector being copied.
     */
    Color(const Eigen::Vector4f &color) : Eigen::Vector4f(color) { }

    /**
     * Copies (x, y, z) from the input vector, and uses the value specified by
     * the ``alpha`` parameter for this Color object's alpha component.
     *
     * \param color
     * The three dimensional float vector being copied.
     *
     * \param alpha
     * The value to set this object's alpha component to.
     */
    Color(const Eigen::Vector3f &color, float alpha)
        : Color(color(0), color(1), color(2), alpha) { }

    /**
     * Copies (x, y, z) from the input vector, casted as floats first and then
     * divided by ``255.0``, and uses the value specified by the ``alpha``
     * parameter, casted to a float and divided by ``255.0`` as well, for this
     * Color object's alpha component.
     *
     * \param color
     * The three dimensional integer vector being copied, will be divided by ``255.0``.
     *
     * \param alpha
     * The value to set this object's alpha component to, will be divided by ``255.0``.
     */
    Color(const Eigen::Vector3i &color, int alpha)
        : Color(color.cast<float>() / 255.f, alpha / 255.f) { }

    /**
     * Copies (x, y, z) from the input vector, and sets the alpha of this color
     * to be ``1.0``.
     *
     * \param color
     * The three dimensional float vector being copied.
     */
    Color(const Eigen::Vector3f &color) : Color(color, 1.0f) {}

    /**
     * Copies (x, y, z) from the input vector, casting to floats and dividing by
     * ``255.0``.  The alpha of this color will be set to ``1.0``.
     *
     * \param color
     * The three dimensional integer vector being copied, will be divided by ``255.0``.
     */
    Color(const Eigen::Vector3i &color)
        : Color((Vector3f)(color.cast<float>() / 255.f)) { }

    /**
     * Copies (x, y, z, w) from the input vector, casting to floats and dividing
     * by ``255.0``.
     *
     * \param color
     * The three dimensional integer vector being copied, will be divided by ``255.0``.
     */
    Color(const Eigen::Vector4i &color)
        : Color((Vector4f)(color.cast<float>() / 255.f)) { }

    /**
     * Creates the Color ``(intensity, intensity, intensity, alpha)``.
     *
     * \param intensity
     * The value to be used for red, green, and blue.
     *
     * \param alpha
     * The alpha component of the color.
     */
    Color(float intensity, float alpha)
        : Color(Vector3f::Constant(intensity), alpha) { }

    /**
     * Creates the Color ``(intensity, intensity, intensity, alpha) / 255.0``.
     * Values are casted to floats before division.
     *
     * \param intensity
     * The value to be used for red, green, and blue, will be divided by ``255.0``.
     *
     * \param alpha
     * The alpha component of the color, will be divided by ``255.0``.
     */
    Color(int intensity, int alpha)
        : Color(Vector3i::Constant(intensity), alpha) { }

    /**
     * Explicit constructor: creates the Color ``(r, g, b, a)``.
     *
     * \param r
     * The red component of the color.
     *
     * \param g
     * The green component of the color.
     *
     * \param b
     * The blue component of the color.
     *
     * \param a
     * The alpha component of the color.
     */
    Color(float r, float g, float b, float a) : Color(Vector4f(r, g, b, a)) { }

    /**
     * Explicit constructor: creates the Color ``(r, g, b, a) / 255.0``.
     * Values are casted to floats before division.
     *
     * \param r
     * The red component of the color, will be divided by ``255.0``.
     *
     * \param g
     * The green component of the color, will be divided by ``255.0``.
     *
     * \param b
     * The blue component of the color, will be divided by ``255.0``.
     *
     * \param a
     * The alpha component of the color, will be divided by ``255.0``.
     */
    Color(int r, int g, int b, int a) : Color(Vector4i(r, g, b, a)) { }

    /// Construct a color vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> Color(const Eigen::MatrixBase<Derived>& p)
        : Base(p) { }

    /// Assign a color vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> Color &operator=(const Eigen::MatrixBase<Derived>& p) {
        this->Base::operator=(p);
        return *this;
    }

    /// Return a reference to the red channel
    float &r() { return x(); }
    /// Return a reference to the red channel (const version)
    const float &r() const { return x(); }
    /// Return a reference to the green channel
    float &g() { return y(); }
    /// Return a reference to the green channel (const version)
    const float &g() const { return y(); }
    /// Return a reference to the blue channel
    float &b() { return z(); }
    /// Return a reference to the blue channel (const version)
    const float &b() const { return z(); }

    /**
     * Computes the luminance as ``l = 0.299r + 0.587g + 0.144b + 0.0a``.  If
     * the luminance is less than 0.5, white is returned.  If the luminance is
     * greater than or equal to 0.5, black is returned.  Both returns will have
     * an alpha component of 1.0.
     */
    Color contrastingColor() const {
        float luminance = cwiseProduct(Color(0.299f, 0.587f, 0.144f, 0.f)).sum();
        return Color(luminance < 0.5f ? 1.f : 0.f, 1.f);
    }

    /// Allows for conversion between this Color and NanoVG's representation.
    inline operator const NVGcolor &() const;
};

// skip the forward declarations for the docs
#ifndef DOXYGEN_SHOULD_SKIP_THIS

/* Forward declarations */
template <typename T> class ref;
class AdvancedGridLayout;
class BoxLayout;
class Button;
class CheckBox;
class ColorWheel;
class ColorPicker;
class ComboBox;
class GLFramebuffer;
class GLShader;
class GridLayout;
class GroupLayout;
class ImagePanel;
class ImageView;
class Label;
class Layout;
class MessageDialog;
class Object;
class Popup;
class PopupButton;
class ProgressBar;
class Screen;
class Serializer;
class Slider;
class StackedWidget;
class TabHeader;
class TabWidget;
class TextBox;
class GLCanvas;
class Theme;
class ToolButton;
class VScrollPanel;
class Widget;
class Window;

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * Static initialization; should be called once before invoking **any** NanoGUI
 * functions **if** you are having NanoGUI manage OpenGL / GLFW.  This method
 * is effectively a wrapper call to ``glfwInit()``, so if you are managing
 * OpenGL / GLFW on your own *do not call this method*.
 *
 * \rst
 * Refer to :ref:`nanogui_example_3` for how you might go about managing OpenGL
 * and GLFW on your own, while still using NanoGUI's classes.
 * \endrst
 */
//extern NANOGUI_EXPORT void init();

/// Static shutdown; should be called before the application terminates.
//extern NANOGUI_EXPORT void shutdown();

/**
 * \brief Enter the application main loop
 *
 * \param refresh
 *     NanoGUI issues a redraw call whenever an keyboard/mouse/.. event is
 *     received. In the absence of any external events, it enforces a redraw
 *     once every ``refresh`` milliseconds. To disable the refresh timer,
 *     specify a negative value here.
 *
 * \param detach
 *     This pararameter only exists in the Python bindings. When the active
 *     \c Screen instance is provided via the \c detach parameter, the
 *     ``mainloop()`` function becomes non-blocking and returns
 *     immediately (in this case, the main loop runs in parallel on a newly
 *     created thread). This feature is convenient for prototyping user
 *     interfaces on an interactive Python command prompt. When
 *     ``detach != None``, the function returns an opaque handle that
 *     will release any resources allocated by the created thread when the
 *     handle's ``join()`` method is invoked (or when it is garbage
 *     collected).
 *
 * \remark
 *     Unfortunately, Mac OS X strictly requires all event processing to take
 *     place on the application's main thread, which is fundamentally
 *     incompatible with this type of approach. Thus, NanoGUI relies on a
 *     rather crazy workaround on Mac OS (kudos to Dmitriy Morozov):
 *     ``mainloop()`` launches a new thread as before but then uses
 *     libcoro to swap the thread execution environment (stack, registers, ..)
 *     with the main thread. This means that the main application thread is
 *     hijacked and processes events in the main loop to satisfy the
 *     requirements on Mac OS, while the thread that actually returns from this
 *     function is the newly created one (paradoxical, as that may seem).
 *     Deleting or ``join()``ing the returned handle causes application to
 *     wait for the termination of the main loop and then swap the two thread
 *     environments back into their initial configuration.
 */
//extern NANOGUI_EXPORT void mainloop(int refresh = 50);

/// Request the application main loop to terminate (e.g. if you detached mainloop).
//extern NANOGUI_EXPORT void leave();

/**
 * \brief Open a native file open/save dialog.
 *
 * \param filetypes
 *     Pairs of permissible formats with descriptions like
 *     ``("png", "Portable Network Graphics")``.
 *
 * \param save
 *     Set to ``true`` if you would like subsequent file dialogs to open
 *     at whatever folder they were in when they close this one.
 */
//extern NANOGUI_EXPORT std::string
//file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes,
//            bool save);

#if defined(__APPLE__) || defined(DOXYGEN_DOCUMENTATION_BUILD)
/**
 * \brief Move to the application bundle's parent directory
 *
 * This is function is convenient when deploying .app bundles on OSX. It
 * adjusts the file path to the parent directory containing the bundle.
 */
extern NANOGUI_EXPORT void chdir_to_bundle_parent();
#endif

/**
 * \brief Convert a single UTF32 character code to UTF8.
 *
 * \rst
 * NanoGUI uses this to convert the icon character codes
 * defined in :ref:`file_include_nanogui_entypo.h`.
 * \endrst
 *
 * \param c
 *     The UTF32 character to be converted.
 */
extern NANOGUI_EXPORT std::array<char, 8> utf8(int c);

/// Load a directory of PNG images and upload them to the GPU (suitable for use with ImagePanel)
//extern NANOGUI_EXPORT std::vector<std::pair<int, std::string>>
//    loadImageDirectory(NVGcontext *ctx, const std::string &path);

/// Convenience function for instanting a PNG icon from the application's data segment (via bin2c)
#define nvgImageIcon(ctx, name) nanogui::__nanogui_get_image(ctx, #name, name##_png, name##_png_size)
/// Helper function used by nvgImageIcon
extern NANOGUI_EXPORT int __nanogui_get_image(NVGcontext *ctx, const std::string &name, uint8_t *data, uint32_t size);

extern NANOGUI_EXPORT const char* (*ngGetClipboardString)(Screen*);
extern NANOGUI_EXPORT void (*ngSetClipboardString)(Screen*,const char* string);
extern NANOGUI_EXPORT double (*ngGetTime)(void);
extern NANOGUI_EXPORT void (*ngSetCursor)(void* platformWindow,Cursor cursor);
extern NANOGUI_EXPORT void (*ngGetWindowSize)(void* platformWindow,int* x, int* y);
extern NANOGUI_EXPORT void (*ngGetFramebufferSize)(void* platformWindow,int* x, int* y);
extern NANOGUI_EXPORT void (*ngSwapBuffers)(void* platformWindow);
extern NANOGUI_EXPORT void (*ngMakeContextCurrent)(void* platformWindow);

/// Internal key & mouse event codes (These are the same as GLFW codes for easy translation)

#define NG_RELEASE                0
#define NG_PRESS                  1
#define NG_REPEAT                 2

#define NG_MOUSE_BUTTON_1         0
#define NG_MOUSE_BUTTON_2         1
#define NG_MOUSE_BUTTON_3         2
#define NG_MOUSE_BUTTON_4         3
#define NG_MOUSE_BUTTON_5         4
#define NG_MOUSE_BUTTON_6         5
#define NG_MOUSE_BUTTON_7         6
#define NG_MOUSE_BUTTON_8         7
#define NG_MOUSE_BUTTON_LAST      NG_MOUSE_BUTTON_8
#define NG_MOUSE_BUTTON_LEFT      NG_MOUSE_BUTTON_1
#define NG_MOUSE_BUTTON_RIGHT     NG_MOUSE_BUTTON_2
#define NG_MOUSE_BUTTON_MIDDLE    NG_MOUSE_BUTTON_3

/* The unknown key */
#define NG_KEY_UNKNOWN            -1

/* Printable keys */
#define NG_KEY_SPACE              32
#define NG_KEY_APOSTROPHE         39  /* ' */
#define NG_KEY_COMMA              44  /* , */
#define NG_KEY_MINUS              45  /* - */
#define NG_KEY_PERIOD             46  /* . */
#define NG_KEY_SLASH              47  /* / */
#define NG_KEY_0                  48
#define NG_KEY_1                  49
#define NG_KEY_2                  50
#define NG_KEY_3                  51
#define NG_KEY_4                  52
#define NG_KEY_5                  53
#define NG_KEY_6                  54
#define NG_KEY_7                  55
#define NG_KEY_8                  56
#define NG_KEY_9                  57
#define NG_KEY_SEMICOLON          59  /* ; */
#define NG_KEY_EQUAL              61  /* = */
#define NG_KEY_A                  65
#define NG_KEY_B                  66
#define NG_KEY_C                  67
#define NG_KEY_D                  68
#define NG_KEY_E                  69
#define NG_KEY_F                  70
#define NG_KEY_G                  71
#define NG_KEY_H                  72
#define NG_KEY_I                  73
#define NG_KEY_J                  74
#define NG_KEY_K                  75
#define NG_KEY_L                  76
#define NG_KEY_M                  77
#define NG_KEY_N                  78
#define NG_KEY_O                  79
#define NG_KEY_P                  80
#define NG_KEY_Q                  81
#define NG_KEY_R                  82
#define NG_KEY_S                  83
#define NG_KEY_T                  84
#define NG_KEY_U                  85
#define NG_KEY_V                  86
#define NG_KEY_W                  87
#define NG_KEY_X                  88
#define NG_KEY_Y                  89
#define NG_KEY_Z                  90
#define NG_KEY_LEFT_BRACKET       91  /* [ */
#define NG_KEY_BACKSLASH          92  /* \ */
#define NG_KEY_RIGHT_BRACKET      93  /* ] */
#define NG_KEY_GRAVE_ACCENT       96  /* ` */
#define NG_KEY_WORLD_1            161 /* non-US #1 */
#define NG_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define NG_KEY_ESCAPE             256
#define NG_KEY_ENTER              257
#define NG_KEY_TAB                258
#define NG_KEY_BACKSPACE          259
#define NG_KEY_INSERT             260
#define NG_KEY_DELETE             261
#define NG_KEY_RIGHT              262
#define NG_KEY_LEFT               263
#define NG_KEY_DOWN               264
#define NG_KEY_UP                 265
#define NG_KEY_PAGE_UP            266
#define NG_KEY_PAGE_DOWN          267
#define NG_KEY_HOME               268
#define NG_KEY_END                269
#define NG_KEY_CAPS_LOCK          280
#define NG_KEY_SCROLL_LOCK        281
#define NG_KEY_NUM_LOCK           282
#define NG_KEY_PRINT_SCREEN       283
#define NG_KEY_PAUSE              284
#define NG_KEY_F1                 290
#define NG_KEY_F2                 291
#define NG_KEY_F3                 292
#define NG_KEY_F4                 293
#define NG_KEY_F5                 294
#define NG_KEY_F6                 295
#define NG_KEY_F7                 296
#define NG_KEY_F8                 297
#define NG_KEY_F9                 298
#define NG_KEY_F10                299
#define NG_KEY_F11                300
#define NG_KEY_F12                301
#define NG_KEY_F13                302
#define NG_KEY_F14                303
#define NG_KEY_F15                304
#define NG_KEY_F16                305
#define NG_KEY_F17                306
#define NG_KEY_F18                307
#define NG_KEY_F19                308
#define NG_KEY_F20                309
#define NG_KEY_F21                310
#define NG_KEY_F22                311
#define NG_KEY_F23                312
#define NG_KEY_F24                313
#define NG_KEY_F25                314
#define NG_KEY_KP_0               320
#define NG_KEY_KP_1               321
#define NG_KEY_KP_2               322
#define NG_KEY_KP_3               323
#define NG_KEY_KP_4               324
#define NG_KEY_KP_5               325
#define NG_KEY_KP_6               326
#define NG_KEY_KP_7               327
#define NG_KEY_KP_8               328
#define NG_KEY_KP_9               329
#define NG_KEY_KP_DECIMAL         330
#define NG_KEY_KP_DIVIDE          331
#define NG_KEY_KP_MULTIPLY        332
#define NG_KEY_KP_SUBTRACT        333
#define NG_KEY_KP_ADD             334
#define NG_KEY_KP_ENTER           335
#define NG_KEY_KP_EQUAL           336
#define NG_KEY_LEFT_SHIFT         340
#define NG_KEY_LEFT_CONTROL       341
#define NG_KEY_LEFT_ALT           342
#define NG_KEY_LEFT_SUPER         343
#define NG_KEY_RIGHT_SHIFT        344
#define NG_KEY_RIGHT_CONTROL      345
#define NG_KEY_RIGHT_ALT          346
#define NG_KEY_RIGHT_SUPER        347
#define NG_KEY_MENU               348

#define NG_KEY_LAST               NG_KEY_MENU

#define NG_MOD_SHIFT           0x0001
#define NG_MOD_CONTROL         0x0002
#define NG_MOD_ALT             0x0004
#define NG_MOD_SUPER           0x0008

NAMESPACE_END(nanogui)

#endif
