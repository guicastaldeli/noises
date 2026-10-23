#include <input.hpp>

Input::Input(Window& window) : window(window) {};
Input::~Input() {};

/********
 * 
 * Action Queries
 * 
 */
// Down
bool Input::down(const std::string& a) const {
    return m_actionsDown.count(norm(a)) > 0;
}

bool Input::downMouse(int b) const {
    return m_mouseDown.count(b) > 0;
}

// Pressed
bool Input::pressed(const std::string& a) const {
    return m_actionsPressed.count(norm(a)) > 0;
}

bool Input::pressedMouse(int b) const {
    return m_mousePressed.count(b) > 0;
}

// Released
bool Input::released(const std::string& a) const {
    return m_actionsReleased.count(norm(a)) > 0;
}

bool Input::releasedMouse(int b) const {
    return m_mouseReleased.count(b) > 0;
}
/***
 ****/

/********
 * 
 * Mouse
 * 
 */
// Mouse X
double Input::mouseX() const {
    return m_mouseX;
}

// Mouse Y
double Input::mouseY() const {
    return m_mouseY;
}

// Mouse Delta X
double Input::mouseDeltaX() const {
    return m_mouseDX;
}

// Mouse Delta Y
double Input::mouseDeltaY() const {
    return m_mouseDY;
}

/**
 * Bind
 */
void Input::bind(const std::string& action, int key) {
    const std::string k = norm(action);

    auto old = m_bindings.find(k);
    if (old != m_bindings.end()) {
        auto& set = m_keyToActions[old->second];
        set.erase(k);
        if (set.empty()) m_keyToActions.erase(old->second);
    }

    m_bindings[k] = key;
    m_keyToActions[key].insert(k);
}

/**
 * Unbind
 */
void Input::unbind(const std::string& action) {
    const std::string key = norm(action);
    auto it = m_bindings.find(key);
    if(it == m_bindings.end()) return;

    auto& set = m_keyToActions[it->second];
    set.erase(key);
    if(set.empty()) m_keyToActions.erase(it->second);
    m_bindings.erase(it);
}

/**
 * Key For
 */
int Input::keyFor(const std::string& action) const {
    auto it = m_bindings.find(norm(action));
    return it == m_bindings.end() ? GLFW_KEY_UNKNOWN : it->second;
}

/**
 * 
 * Set Bindings
 * 
 */
void Input::setBindings() {
    bind(Bindings::FORWARD,     GLFW_KEY_W);
    bind(Bindings::BACKWARD,    GLFW_KEY_S);
    bind(Bindings::LEFT,        GLFW_KEY_A);
    bind(Bindings::RIGHT,       GLFW_KEY_D);
    bind(Bindings::UP,          GLFW_KEY_SPACE);
    bind(Bindings::DOWN,        GLFW_KEY_LEFT_CONTROL);
}

/**
 * On Key
 */
void Input::onKey(int key, int action) {
    auto it = m_keyToActions.find(key);
    if(it == m_keyToActions.end()) return;
    if(action == GLFW_PRESS) {
        for(const auto& name : it->second) {
            m_actionsDown.insert(name);
            m_actionsPressed.insert(name);
        }
    } else if(action == GLFW_RELEASE) {
        for(const auto& name : it->second) {
            m_actionsDown.erase(name);
            m_actionsReleased.insert(name);
        }
    }
}

/**
 * On Mouse Button
 */
void Input::onMouseButton(int button, int action) {
    if(action == GLFW_PRESS) {
        m_mouseDown.insert(button);
        m_mousePressed.insert(button);
    } else if(action = GLFW_RELEASE) {
        m_mouseDown.erase(button);
        m_mouseReleased.insert(button);
    }
}

/**
 * On Cursor Pos
 */
void Input::onCursorPos(double x, double y) {
    if(m_firstMouseSample) {
        m_mouseX = x; m_mouseY = y;
        m_firstMouseSample = false;
        return;
    }
    m_rawDX += x - m_mouseX;
    m_rawDY += y - m_mouseY;
    m_mouseX = x;
    m_mouseY = y;
}

/**
 * Set Cursor Captured
 */
void Input::setCursorCaptured(bool captured) {
    if(m_cursorCaptured == captured) return;
    m_cursorCaptured = captured;
    if(window.window) {
        glfwSetInputMode(window.window,
                            GLFW_CURSOR,
                            captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    if(captured) m_firstMouseSample = true;
}

/**
 * Is Cursor Captured
 */
bool Input::isCursorCaptured() const {
    return m_cursorCaptured;
}