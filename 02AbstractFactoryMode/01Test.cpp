#include <iostream>
using namespace std;

// 抽象产品类：Button
class Button {
public:
    virtual void paint() = 0;
};

// 具体产品类：WindowsButton
class WindowsButton : public Button {
public:
    void paint() override {
        cout << "Rendering a button in Windows style." << endl;
    }
};

// 具体产品类：MacButton
class MacButton : public Button {
public:
    void paint() override {
        cout << "Rendering a button in MacOS style." << endl;
    }
};

// 抽象产品类：TextBox
class TextBox {
public:
    virtual void paint() = 0;
};

// 具体产品类：WindowsTextBox
class WindowsTextBox : public TextBox {
public:
    void paint() override {
        cout << "Rendering a text box in Windows style." << endl;
    }
};

// 具体产品类：MacTextBox
class MacTextBox : public TextBox {
public:
    void paint() override {
        cout << "Rendering a text box in MacOS style." << endl;
    }
};

// 抽象工厂类：UIFactory
class UIFactory {
public:
    virtual Button* createButton() = 0;
    virtual TextBox* createTextBox() = 0;
};

// 具体工厂类：WindowsFactory
class WindowsFactory : public UIFactory {
public:
    Button* createButton() override {
        return new WindowsButton();
    }

    TextBox* createTextBox() override {
        return new WindowsTextBox();
    }
};

// 具体工厂类：MacFactory
class MacFactory : public UIFactory {
public:
    Button* createButton() override {
        return new MacButton();
    }

    TextBox* createTextBox() override {
        return new MacTextBox();
    }
};

// 客户端代码
class Application {
private:
    UIFactory* factory;  // 抽象工厂
    Button* button;
    TextBox* textBox;

public:
    Application(UIFactory* f) : factory(f) {
        button = factory->createButton();
        textBox = factory->createTextBox();
    }

    void render() {
        button->paint();
        textBox->paint();
    }

    ~Application() {
        delete button;
        delete textBox;
    }
};

int main() {
    UIFactory* factory;
    Application* app;

    // 假设我们运行在 Windows 平台
    factory = new WindowsFactory();
    app = new Application(factory);
    app->render();

    delete app;
    delete factory;

    // 假设我们运行在 MacOS 平台
    factory = new MacFactory();
    app = new Application(factory);
    app->render();

    delete app;
    delete factory;

    return 0;
}
