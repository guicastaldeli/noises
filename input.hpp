#pragma once

#include <window.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

class Input {
private:
    Window& window;

    std::unordered_map<std::string, int> m_bindings;
    std::unordered_map<int, std::unordered_set<std::string>> m_keyToActions;
    
    std::unordered_set<std::string> m_actionsDown;
    std::unordered_set<std::string> m_actionsPressed;
    std::unordered_set<std::string> m_actionsReleased;

    std::unordered_set<int> m_mouseDown;
    std::unordered_set<int> m_mousePressed;
    std::unordered_set<int> m_mouseReleased;

    double m_mouseX = 0.0, m_mouseY = 0.0;
    double m_mouseDX = 0.0, m_mouseDY = 0.0;
    double m_rawDX = 0.0, m_rawDY = 0.0;

    bool m_cursorCaptured = false;
    bool m_firstMouseSample = true;

public:
    /*************
     * 
     * Bindings
     * 
     *  ****/
    #define BINDING(name) \
        static constexpr char* name = #name;

    struct Bindings {
        BINDING(FORWARD)
        BINDING(BACKWARD)
        BINDING(LEFT)
        BINDING(RIGHT)
        BINDING(UP)
        BINDING(DOWN)
    };

    #undef BINDING

    void setBindings();

    static std::string norm(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for(char c : s) {
            if(c >= 'A' && c <= 'Z') out.push_back(char(c - 'A' + 'a'));
            else out.push_back(c);
        }
        return out;
    }
    /**
     * 
     */

    Input(Window& window);
    ~Input();

    bool down(const std::string& action) const;
    bool pressed(const std::string& action) const;
    bool released(const std::string& action) const;

    double mouseX() const;
    double mouseY() const;
    double mouseDeltaX() const;
    double mouseDeltaY() const;
    bool downMouse(int button) const;
    bool pressedMouse(int button) const;
    bool releasedMouse(int button) const;

    void bind(const std::string& action, int key);
    void unbind(const std::string& action);
    int keyFor(const std::string& action) const;
    
    void onKey(int key, int action);
    void onMouseButton(int button, int action);
    void onCursorPos(double x, double y);
    void onScroll(double xoffset, double yoffset);

    void setCursorCaptured(bool captured);
    bool isCursorCaptured() const;

    /********** 
     * Begin Frame
     ******/
    void beginFrame() {
        m_actionsPressed.clear();
        m_actionsReleased.clear();
        m_mousePressed.clear();
        m_mouseReleased.clear();
    }

    /********** 
     * End Frame
     ******/
    void endFrame() {
        m_mouseDX = m_rawDX;
        m_mouseDY = m_rawDY;
        m_rawDX = m_rawDX = 0.0f;
    }
};