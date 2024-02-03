#include <iostream>

using namespace std;

// Subsystem 1
class Engine {
public:
	void Start()
	{
		cout << "Engine started" << endl;
	}

	void Stop()
	{
		cout << "Engine stopped" << endl;
	}
};

// Subsystem 2
class Lights {
public:
	void TurnOn() { cout << "Lights on" << endl; }

	void TurnOff()
	{
		cout << "Lights off" << endl;
	}
};

// Facade
class Car {
private:
	Engine engine;
	Lights lights;
	string car;

public:
    Car(string pCar){
		car = pCar;
	}

	void StartCar()
	{
		engine.Start();
		lights.TurnOn();
		cout << car <<" is ready to drive" << endl;
	}

	void StopCar()
	{
		lights.TurnOff();
		engine.Stop();
		cout << car<<" has stopped" << endl;
	}
};

int main()
{
	// Using the Facade to start and stop the car
	Car* car = new Car("Honda");
	car->StartCar();
	// Simulate some driving
	car->StopCar();
	return 0;
}
