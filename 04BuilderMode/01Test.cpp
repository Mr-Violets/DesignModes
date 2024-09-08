#include <iostream>
#include <string>
using namespace std;

// 产品类：Car
class Car {
public:
    string engine;  // 发动机
    string seats;   // 座位
    string GPS;     // GPS

    void showSpecifications() {
        cout << "Engine: " << engine << endl;
        cout << "Seats: " << seats << endl;
        cout << "GPS: " << GPS << endl;
    }
};

// 抽象建造者类：CarBuilder
class CarBuilder {
public:
    virtual void buildEngine() = 0;
    virtual void buildSeats() = 0;
    virtual void buildGPS() = 0;
    virtual Car* getCar() = 0;
};

// 具体建造者类：SportsCarBuilder
class SportsCarBuilder : public CarBuilder {
private:
    Car* car;
public:
    SportsCarBuilder() {
        car = new Car();
    }

    void buildEngine() override {
        car->engine = "V8 Engine";
    }

    void buildSeats() override {
        car->seats = "2 Sports Seats";
    }

    void buildGPS() override {
        car->GPS = "Advanced GPS";
    }

    Car* getCar() override {
        return car;
    }
};

// 具体建造者类：LuxuryCarBuilder
class LuxuryCarBuilder : public CarBuilder {
private:
    Car* car;
public:
    LuxuryCarBuilder() {
        car = new Car();
    }

    void buildEngine() override {
        car->engine = "V12 Engine";
    }

    void buildSeats() override {
        car->seats = "5 Leather Seats";
    }

    void buildGPS() override {
        car->GPS = "Luxury GPS";
    }

    Car* getCar() override {
        return car;
    }
};

// 指挥者类：Director
class Director {
public:
    void construct(CarBuilder* builder) {
        builder->buildEngine();
        builder->buildSeats();
        builder->buildGPS();
    }
};

// 客户端代码
int main() {
    Director director;

    // 建造一辆运动型汽车
    SportsCarBuilder sportsCarBuilder;
    director.construct(&sportsCarBuilder);
    Car* sportsCar = sportsCarBuilder.getCar();
    cout << "Sports Car Specifications:" << endl;
    sportsCar->showSpecifications();

    delete sportsCar;

    // 建造一辆豪华型汽车
    LuxuryCarBuilder luxuryCarBuilder;
    director.construct(&luxuryCarBuilder);
    Car* luxuryCar = luxuryCarBuilder.getCar();
    cout << "\nLuxury Car Specifications:" << endl;
    luxuryCar->showSpecifications();

    delete luxuryCar;

    return 0;
}
