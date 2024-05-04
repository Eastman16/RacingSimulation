#include <GLFW/glfw3.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <stdio.h>
#include <thread>
#include <vector>
#include <iostream>

struct Car{
    public:
    float car_x, car_y;
    float speed;
    int lapCount=0;
    float lastDistance = 0.0f;
    bool draw = true;
    bool firstRoad;
    float colorR, colorG, colorB;
    std::chrono::milliseconds delay=std::chrono::milliseconds(0);
    Car(float x, float y, bool r) : car_x(x), car_y(y), firstRoad(r) {
        colorR = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        colorG = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        colorB = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if (firstRoad) {
            speed = 10 + static_cast<float>(rand() % 91);
            car_x = -50.0f;
            car_y = 18.0f;
            delay = std::chrono::milliseconds(rand() % 5000);
        } else {
            speed = 30;
        }
    }
};
bool stopThreads = false;
std::vector <Car> cars;
std::vector <Car> cars2;
std::vector <std::thread> threads;

void update_viewport(GLFWwindow* window, int width, int height) {
    if (height == 0) height = 1;
    if (width == 0) width = 1;
    float aspectRatio = (float)width / (float)height;

    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, width, height);
    glLoadIdentity();

    if (width <= height) {
        glOrtho(-100.0, 100.0, -100.0 / aspectRatio, 100.0 / aspectRatio, 1.0, -1.0);
    } else {
        glOrtho(-100.0 * aspectRatio, 100.0 * aspectRatio, -100.0, 100.0, 1.0, -1.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw_rectangle(float x, float y, float width, float height) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x, y + height);
    glVertex2f(x + width, y + height);

    glVertex2f(x, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x + width, y);
    glEnd();
}

void draw_hollow_rectangle(float x, float y, float width, float height, float thickness) {
    float half_thickness = thickness / 2.0f;

    glBegin(GL_TRIANGLES);
    // Bottom side
    glVertex2f(x - half_thickness, y - half_thickness);
    glVertex2f(x - half_thickness, y + half_thickness);
    glVertex2f(x + width + half_thickness, y + half_thickness);

    glVertex2f(x - half_thickness, y - half_thickness);
    glVertex2f(x + width + half_thickness, y + half_thickness);
    glVertex2f(x + width + half_thickness, y - half_thickness);

    // Right side
    glVertex2f(x + width + half_thickness, y - half_thickness);
    glVertex2f(x + width + half_thickness, y + height + half_thickness);
    glVertex2f(x + width - half_thickness, y - half_thickness);

    glVertex2f(x + width - half_thickness, y - half_thickness);
    glVertex2f(x + width + half_thickness, y + height + half_thickness);
    glVertex2f(x + width - half_thickness, y + height + half_thickness);

    // Top side
    glVertex2f(x + width + half_thickness, y + height + half_thickness);
    glVertex2f(x - half_thickness, y + height + half_thickness);
    glVertex2f(x + width + half_thickness, y + height - half_thickness);

    glVertex2f(x - half_thickness, y + height - half_thickness);
    glVertex2f(x - half_thickness, y + height + half_thickness);
    glVertex2f(x + width + half_thickness, y + height - half_thickness);

    // Left side
    glVertex2f(x - half_thickness, y + height + half_thickness);
    glVertex2f(x - half_thickness, y - half_thickness);
    glVertex2f(x + half_thickness, y + height + half_thickness);

    glVertex2f(x + half_thickness, y + height + half_thickness);
    glVertex2f(x - half_thickness, y - half_thickness);
    glVertex2f(x + half_thickness, y - half_thickness);
    glEnd();
}

void draw_car(Car &car) {
    float car_width = 8.0f; 
    float car_height = 5.0f; 
    float wheel_width = 2.0f;
    float wheel_height = 2.0f;
    float wheel_offset = 1.0f; 

    glColor3f(car.colorR, car.colorG, car.colorB);
    draw_rectangle(car.car_x - car_width / 2, car.car_y, car_width, car_height);

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_rectangle(car.car_x - car_width / 2 + wheel_offset, car.car_y - wheel_height, wheel_width, wheel_height);

    draw_rectangle(car.car_x + car_width / 2 - wheel_width - wheel_offset, car.car_y - wheel_height, wheel_width, wheel_height);


}

void move_car(Car &car) {
    std::this_thread::sleep_for(car.delay);
    
    float fixedDistancePerUpdate = 1.0f;
    float updateIntervalBase = 10;

    float rect_x = -50.0f;
    float rect_y = -22.0f;
    float rect_width = 100.0f;
    float rect_height = 40.0f;
    float perimeter = 2 * (rect_width + rect_height);

    car.car_x = rect_x;
    car.car_y = rect_y + rect_height;
    car.lastDistance = 0.0f;

    bool wasOnBottom = false;

    while (!stopThreads) {
        car.lastDistance += fixedDistancePerUpdate;
        if (car.lastDistance >= perimeter) {
            car.lastDistance -= perimeter;
            wasOnBottom = false; 
        }

        if (car.lastDistance < rect_width) {
            car.car_x = rect_x + car.lastDistance;
            car.car_y = rect_y + rect_height;
        } else if (car.lastDistance < rect_width + rect_height) {
            car.car_x = rect_x + rect_width;
            car.car_y = rect_y + rect_height - (car.lastDistance - rect_width);
        } else if (car.lastDistance < 2 * rect_width + rect_height) {
            car.car_x = rect_x + rect_width - (car.lastDistance - (rect_width + rect_height)); 
            car.car_y = rect_y;
            wasOnBottom = true;
        } else {
            car.car_x = rect_x;
            car.car_y = rect_y + (car.lastDistance - (2 * rect_width + rect_height));
            if (wasOnBottom) { 
                car.lapCount++;
                printf("Lap %d completed at bottom-left corner by car\n", car.lapCount);
                if (car.lapCount == 3) {
                    car.draw = false;
                    return;
                }
                wasOnBottom = false;
            }
        }

        float updateInterval = updateIntervalBase / (car.speed / 50);
        std::this_thread::sleep_for(std::chrono::milliseconds((int)updateInterval));
    }
}


void move_car2(Car &car) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    float rect_x = -20.0f;
    float rect_y = -52.0f;
    float rect_width = 40.0f;
    float rect_height = 100.0f;

    float fixedDistancePerUpdate = 1.0f;
    float updateIntervalBase = 10;

    float perimeter = 2 * (rect_width + rect_height);

    float random_start = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * perimeter;
    car.lastDistance = random_start;

    if (random_start < rect_width) {
        car.car_x = rect_x + random_start;
        car.car_y = rect_y;
    } else if (random_start < rect_width + rect_height) {
        car.car_x = rect_x + rect_width;
        car.car_y = rect_y + (random_start - rect_width);
    } else if (random_start < 2 * rect_width + rect_height) {
        car.car_x = rect_x + rect_width - (random_start - (rect_width + rect_height));
        car.car_y = rect_y + rect_height;
    } else {
        car.car_x = rect_x;
        car.car_y = rect_y + rect_height - (random_start - (2 * rect_width + rect_height));
    }

    while(!stopThreads){
        car.lastDistance += fixedDistancePerUpdate;
        if (car.lastDistance >= perimeter) car.lastDistance -= perimeter;

        float distance = car.lastDistance;

        if (distance < rect_width) {
            car.car_x = rect_x + distance;
            car.car_y = rect_y;
        } else if (distance < rect_width + rect_height) {
            car.car_x = rect_x + rect_width;
            car.car_y = rect_y + (distance - rect_width);
        } else if (distance < 2 * rect_width + rect_height) {
            car.car_x = rect_x + rect_width - (distance - (rect_width + rect_height));
            car.car_y = rect_y + rect_height;
        } else {
            car.car_x = rect_x;
            car.car_y = rect_y + rect_height - (distance - (2 * rect_width + rect_height));
        }

        float updateInterval = updateIntervalBase / (car.speed / 50);
        std::this_thread::sleep_for(std::chrono::milliseconds((int)updateInterval));
    }
}



void render(double time) {
    
    glClear(GL_COLOR_BUFFER_BIT);

    //first road
    glColor3f(0.0f / 255.0f, 16.0f / 255.0f, 117.0f / 255.0f);
    draw_hollow_rectangle(-50.0f, -20.0f, 100.0f, 40.0f, 10.0f);

    for (Car &car : cars2) {
        if(car.draw && car.firstRoad){
            draw_car(car);
        }
    }

    //second road
    glColor3f(44.0f / 255.0f, 66.0f / 255.0f, 209.0f / 255.0f);
    draw_hollow_rectangle(-20.0f, -50.0f, 40.0f, 100.0f, 10.0f);

    for (Car &car : cars) {
        if(car.draw && !car.firstRoad){
            draw_car(car);
        }
    }


    glFlush();
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Racing Simulation", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int windowPosX = (mode->width - 1000) / 2;
    int windowPosY = (mode->height - 1000) / 2;

    glfwSetWindowPos(window, windowPosX, windowPosY);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, update_viewport);
    update_viewport(window, 1000, 1000);
    glfwSwapInterval(1);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    int numOfCarsFirstRoad = 2;
    int numOfCarsSecondRoad = 2;

    //loop to create cars
    for (int i = 0; i < numOfCarsFirstRoad; ++i) {
        cars2.emplace_back(-50.0f, 18.0f, true);
    }
    for (int i = 0; i < numOfCarsSecondRoad; ++i) {
        cars.emplace_back(0.0f, 0.0f, false);
    }

    //create threads
    for (size_t i = 0; i < cars2.size(); ++i) {
        threads.emplace_back(&move_car, std::ref(cars2[i]));
    }
    for (size_t i = 0; i < cars.size(); ++i) {
        threads.emplace_back(&move_car2, std::ref(cars[i]));
    } 

    //main loop
    while (!glfwWindowShouldClose(window)) {  
        render(glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            break;
        }
    }
    stopThreads = true;

    //join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    glfwTerminate();
    return 0;
}


