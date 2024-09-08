#include <iostream>
#include <string>
using namespace std;

// Product 产品基类：Shape
class Shape {
public:
    virtual void draw() = 0;  // 纯虚函数，用于绘制图形
};

// ConcreteProduct 具体产品类：Circle
class Circle : public Shape {
public:
    void draw() override {
        cout << "Drawing Circle" << endl;
    }
};

// ConcreteProduct 具体产品类：Square
class Square : public Shape {
public:
    void draw() override {
        cout << "Drawing Square" << endl;
    }
};

// Factory 工厂类：ShapeFactory
class ShapeFactory {
public:
    Shape* createShape(const string& shapeType) {
        if (shapeType == "Circle")
            return new Circle();
        else if (shapeType == "Square")
            return new Square();
        else
            return nullptr;
    }
};

// 客户端代码
int main() {
    ShapeFactory factory;

    // 创建一个圆形
    Shape* shape1 = factory.createShape("Circle");
    if (shape1 != nullptr) {
        shape1->draw();
        delete shape1;  // 记得删除对象，避免内存泄漏
    }

    // 创建一个正方形
    Shape* shape2 = factory.createShape("Square");
    if (shape2 != nullptr) {
        shape2->draw();
        delete shape2;
    }

    return 0;
}
