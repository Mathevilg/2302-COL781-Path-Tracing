#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// Program with perspective correct interpolation of vertex attributes.

// namespace R = COL781::Software;
namespace R = COL781::Hardware;
using namespace glm;

class RayTracer {
public:

    struct Plane {
        glm::vec3 point;
        glm::vec3 normal;
    };

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct Sphere {
        glm::vec3 center;
        float radius;
    };

    RayTracer() {
        // Initialize the ray tracer.
        camera = vec3(0.0f, 0.0f, 0.0f);
        cameraDirection = vec3(0.0f, 0.0f, -1.0f);
        verticalFieldOfView = 60.0f;
        aspectRatio = 4.0f / 3.0f;
        frameWidth = 640;
        frameHeight = 480;
        spheres.push_back(Sphere{vec3(0.0f, 0.0f, -5.0f), 1.0f});
        spheres.push_back(Sphere{vec3(1.0f, 0.0f, -5.0f), 1.0f});
        spheres.push_back(Sphere{vec3(0.0f, 1.0f, -5.0f), 1.0f});
        planes.push_back(Plane{vec3(0.0f, -5.0f, -10.0f), vec3(0.0f, 1.0f, 0.0f)});
        aabbs.push_back(AABB{vec3(-2.0f, -1.0f, -5.0f), vec3(-1.0f, 1.0f, -3.0f)});
    }

    void render(){
        // Render the scene.
        // Clear the framebuffer
        Uint32 *pixels = (Uint32*)framebuffer->pixels;
        SDL_PixelFormat *format = framebuffer->format;
        for (int i = 0; i < frameWidth; i++) {
            for (int j = 0; j < frameHeight; j++) {
                float x = (i + 0.5)/frameWidth;  // [0, w] -> [0, 1]
                x = 2*x - 1;                     // [0, 1] -> [-1, 1]
                float y = (j + 0.5)/frameHeight; // [0, h] -> [0, 1]
                y = 1 - 2*y;                     // [0, 1] -> [1, -1]
                vec3 origin = camera;
                vec3 direction = normalize(cameraDirection + vec3(x, y, 0.0f));
                float t = INFINITY;
                for (int k = 0; k < spheres.size(); k++) {
                    t = min(t, intersectSphere(origin, direction, spheres[k]));
                }
                float tPlane = INFINITY;
                for (int k = 0; k < planes.size(); k++) {
                    tPlane = min(tPlane, intersectPlane(origin, direction, planes[k]));
                }

                float tBox = INFINITY;
                for (int k = 0; k < aabbs.size(); k++) {
                    tBox = min(tBox, intersectAABB(origin, direction, aabbs[k]));
                }

                Uint32 color;
                if (tBox < INFINITY || tPlane < INFINITY || t < INFINITY) {
                    if(tBox < t && tBox < tPlane){
                        float l = 255;
                        color = SDL_MapRGBA(format, l, 0, 0, 255); // red
                    }
                    else if(tPlane < t){
                        float l = 255;
                        color = SDL_MapRGBA(format, l, 100, l, 255); // grey proportional to t
                    }
                    else{
                        float l = 255*(1.0f - t);
                        color = SDL_MapRGBA(format, l, l, l, 255); // grey proportional to t
                    }
                } else {
                    color = SDL_MapRGBA(format, 0, 0, 0, 255); // black
                }
                pixels[i + frameWidth*j] = color;
            }
        }

        // Update screen to apply the changes
        SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
        SDL_UpdateWindowSurface(window);
    }
    // void render(){  
    //     // Render the scene.
    //     // Clear the framebuffer



    //     // Uint32 *pixels = (Uint32*)framebuffer->pixels;
    //     // SDL_PixelFormat *format = framebuffer->format;
    //     // for (int i = 0; i < frameWidth; i++) {
    //     //     for (int j = 0; j < frameHeight; j++) {
    //     //         float x = (i + 0.5)/frameWidth;  // [0, w] -> [0, 1]
    //     //         x = 2*x - 1;                     // [0, 1] -> [-1, 1]
    //     //         float y = (j + 0.5)/frameHeight; // [0, h] -> [0, 1]
    //     //         y = 1 - 2*y;                     // [0, 1] -> [1, -1]
    //     //         float cx = 0, cy = 0, r = 0.8;
    //     //         Uint32 color;
    //     //         if ((x-cx)*(x-cx) + (y-cy)*(y-cy) <= r*r) { // inside circle
    //     //             color = SDL_MapRGBA(format, 0, 153, 0, 255); // green
    //     //         } else {
    //     //             float l = 255*(y*0.5 + 0.5);
    //     //             color = SDL_MapRGBA(format, l, l, l, 255); // grey proportional to y
    //     //         }
    //     //         pixels[i + frameWidth*j] = color;
    //     //     }
    //     // }

    //     // // Update screen to apply the changes
    //     // SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
    //     // SDL_UpdateWindowSurface(window);
    // }

    float intersectSphere(const vec3& origin, const vec3& direction, const Sphere& sphere) {
        // Compute the intersection of a ray with a sphere.
        vec3 oc = origin - sphere.center;
        float a = dot(direction, direction);
        float b = 2.0f * dot(oc, direction);
        float c = dot(oc, oc) - (sphere.radius * sphere.radius);
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return INFINITY;
        return (-b - sqrt(discriminant)) / (2.0f * a);
    }

    float intersectPlane(const glm::vec3& origin, const glm::vec3& direction, const Plane& plane) {
        float denom = dot(plane.normal, direction);
        if (abs(denom) > 1e-6) {
            glm::vec3 p0l0 = plane.point - origin;
            float t = dot(p0l0, plane.normal) / denom;
            if (t >= 0) {
                return t;
           }
        }
        return INFINITY;
    }

    float intersectAABB(const glm::vec3& origin, const glm::vec3& direction, const AABB& aabb) {
        float tmin = -INFINITY;
        float tmax = INFINITY;
    
        for (int i = 0; i < 3; ++i) {
            if (abs(direction[i]) < 1e-6) {
                if (origin[i] < aabb.min[i] || origin[i] > aabb.max[i]) {
                    return false;
                }
            } else {
                float t1 = (aabb.min[i] - origin[i]) / direction[i];
                float t2 = (aabb.max[i] - origin[i]) / direction[i];
    
                tmin = glm::max(tmin, glm::min(t1, t2));
                tmax = glm::min(tmax, glm::max(t1, t2));
            }
        }
    
        if (tmax >= tmin && tmax >= 0) {
            if(tmin<0){
                return tmax;
            }
            else{
                return tmin;
            }            
        }
    
        return INFINITY;
    }

    bool initialize() {
        bool success = true;
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            printf("SDL could not initialize! SDL_Error: %s", SDL_GetError());
            success = false;
        } else {
            int screenWidth = frameWidth * displayScale;
            int screenHeight = frameHeight * displayScale;
            window = SDL_CreateWindow("COL781", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
            if (window == NULL) {
                printf("Window could not be created! SDL_Error: %s", SDL_GetError());
                success = false;
            } else {
                windowSurface = SDL_GetWindowSurface(window);
                framebuffer = SDL_CreateRGBSurface(0, frameWidth, frameHeight, 32, 0, 0, 0, 0);
            }
        }
        return success;
    }

    // Handle window exit 
    void handleEvents() {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }
            
    void saveFramebuffer() {
        // Save the image
        // IMG_SavePNG(framebuffer, outputFile);
    }

    void terminate() {
        // Free resources and close SDL
        SDL_FreeSurface(framebuffer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }



    std::vector<Sphere> spheres;
    std::vector<float> radii;
    std::vector<Plane> planes;
    std::vector<AABB> aabbs;
    glm::vec3 camera;
    glm::vec3 cameraDirection;
    float verticalFieldOfView;
    float aspectRatio;
    int frameWidth = 640;
    int frameHeight = 480;
    float displayScale = 1.0f;
    SDL_Surface* framebuffer = NULL;

    /* SDL parameters */

    SDL_Window* window = NULL;
    SDL_Surface *windowSurface = NULL;
    bool quit = false;

    /* Output file */

    const char* outputFile = "out.png";
};

int main() {
    RayTracer rayTracer;
    if (!rayTracer.initialize()) {
        return 1;
    }
    while (!rayTracer.quit) {
        rayTracer.handleEvents();
        rayTracer.render();
    }
    rayTracer.saveFramebuffer();
    rayTracer.terminate();
    return 0;
}
