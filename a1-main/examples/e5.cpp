#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
// Program with perspective correct interpolation of vertex attributes.

// namespace R = COL781::Software;
namespace R = COL781::Hardware;
using namespace glm;

class RayTracer {
public:

    class Camera{
    public:

        vec3 Eye;
        vec3 ViewDir;
        vec3 UpVec;
        
        Camera(){
            Eye = vec3(0.0f, 0.0f, 0.0f);
            ViewDir = vec3(0.0f, 0.0f, -1.0f);
            UpVec = vec3(0.0f, 1.0f, 0.0f);
        }

        Camera(vec3 eye, vec3 viewDir, vec3 upVec){
            Eye = eye;
            ViewDir = viewDir;
            UpVec = upVec;
        }

        mat4 getViewMatrix() const{
            return lookAt(Eye, ViewDir, UpVec);
        }

        void moveForward(float delta){
            Eye += delta * ViewDir;
        }

        void moveRight(float delta){
            Eye += delta * cross(ViewDir, UpVec);
        }

        void moveUp(float delta){
            Eye += delta * UpVec;
        }

        void rotateRight(float angle){
            ViewDir = rotate(mat4(1.0f), -radians(angle), UpVec) * vec4(ViewDir, 0.0f);
        }

        void rotateUp(float angle){
            vec3 right = cross(ViewDir, UpVec);
            ViewDir = rotate(mat4(1.0f), -radians(angle), right) * vec4(ViewDir, 0.0f);
            UpVec = rotate(mat4(1.0f), -radians(angle), right) * vec4(UpVec, 0.0f);
        }

};

    struct Plane {
        glm::vec3 point;
        glm::vec3 normal;
        glm::mat4 transform;

        // void applyTransformation() {
        //     // Apply transformation to the point on the plane and its normal
        //     vec4 transformedPoint = transform * vec4(point, 1.0f);
        //     vec4 transformedNormal = glm::transpose(glm::inverse(transform)) * vec4(normal, 0.0f);
        //     point = vec3(transformedPoint);
        //     normal = vec3(transformedNormal);
        //     transform = glm::mat4(1.0f);
        // }

        void translateShape(const glm::vec3& translation) {
            // Update the transformation matrix of the shape
            transform = glm::translate(transform, translation);
        }
    
        void rotateShape(float angle, const glm::vec3& axis) {
            // Update the transformation matrix of the shape
            transform = glm::rotate(transform, angle, axis);
        }
        
        void scaleShape( const glm::vec3& scale) {
            // Update the transformation matrix of the shape
            transform = glm::scale(transform, scale);
        }
    };

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
        glm::mat4 transform;

        // void applyTransformation() {
        //     // Apply transformation to the minimum and maximum points of the AABB
        //     vec4 transformedMin = transform * vec4(min, 1.0f);
        //     vec4 transformedMax = transform * vec4(max, 1.0f);
        //     min = vec3(transformedMin);
        //     max = vec3(transformedMax);
        //     transform = glm::mat4(1.0f);
        // }

        void translateShape(const glm::vec3& translation) {
            // Update the transformation matrix of the shape
            transform = glm::translate(transform, translation);
        }
    
        void rotateShape(float angle, const glm::vec3& axis) {
            // Update the transformation matrix of the shape
            transform = glm::rotate(transform, angle, axis);
        }
        
        void scaleShape( const glm::vec3& scale) {
            // Update the transformation matrix of the shape
            transform = glm::scale(transform, scale);
        }
    };

    struct Sphere {
        glm::vec3 center;
        float radius;
        glm::mat4 transform;

        void translateShape(const glm::vec3& translation) {
            // Update the transformation matrix of the shape
            transform = glm::translate(transform, translation);
        }
    
        void rotateShape(float angle, const glm::vec3& axis) {
            // Update the transformation matrix of the shape
            transform = glm::rotate(transform, angle, axis);
        }
        
        void scaleShape( const glm::vec3& scale) {
            // Update the transformation matrix of the shape
            transform = glm::scale(transform, scale);
        }
    };

    RayTracer() {
        // Initialize the ray tracer.
        // camera = vec3(0.0f, 0.0f, 0.0f);
        // cameraDirection = vec3(0.0f, 0.0f, -1.0f);
        verticalFieldOfView = 60.0f;
        aspectRatio = 4.0f / 3.0f;
        frameWidth = 640;
        frameHeight = 640;
        spheres.push_back(Sphere{vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::mat4(1.0f)});
        // spheres[0].scaleShape(vec3(2.0f, 2.0f, 1.0f));
        spheres[0].translateShape(vec3(0.0f, 0.0f, -5.0f));
        // spheres[0].rotateShape(radians(-45.0f), vec3(0.0f, 1.0f, 0.0f));
        // spheres.push_back(Sphere{vec3(1.0f, 0.0f, -5.0f), 1.0f});
        // spheres.push_back(Sphere{vec3(0.0f, 1.0f, -5.0f), 1.0f});
        // spheres.push_back(Sphere{vec3(-1.0f, 0.0f, -5.0f), 1.0f});
        aabbs.push_back(AABB{vec3(-1.0f, -1.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f), glm::mat4(1.0f)});
        aabbs[0].translateShape(vec3(-2.5f, 0.0f, -6.0f));
        aabbs[0].rotateShape(radians(45.0f), vec3(1.0f, 0.0f, 0.0f));
        aabbs[0].scaleShape(vec3(1.0f, 2.0f, 1.0f));
        planes.push_back(Plane{vec3(0.0f, -5.0f, -10.0f), vec3(0.0f, 1.0f, 0.0f), glm::mat4(1.0f)});
        // planes[0].translateShape(vec3(0.0f, 4.0f, 0.0f));
        // planes[0].rotateShape(radians(45.0f), vec3(0.0f, 0.0f, 1.0f));
        // aabbs.push_back(AABB{vec3(-2.0f, -1.0f, -5.0f), vec3(-1.0f, 1.0f, -3.0f)});
    }

    // void render(){
    //     // Render the scene.
    //     // Clear the framebuffer
    //     Uint32 *pixels = (Uint32*)framebuffer->pixels;
    //     SDL_PixelFormat *format = framebuffer->format;
    //     for (int i = 0; i < frameWidth; i++) {
    //         for (int j = 0; j < frameHeight; j++) {
    //             float x = (i + 0.5)/frameWidth;  // [0, w] -> [0, 1]
    //             x = 2*x - 1;                     // [0, 1] -> [-1, 1]
    //             float y = (j + 0.5)/frameHeight; // [0, h] -> [0, 1]
    //             y = 1 - 2*y;                     // [0, 1] -> [1, -1]
    //             vec3 origin = camera;
    //             vec3 direction = normalize(cameraDirection + vec3(x, y, 0.0f));
    //             float t = INFINITY;
    //             for (int k = 0; k < spheres.size(); k++) {
    //                 t = min(t, intersectSphere(origin, direction, spheres[k]));
    //             }
    //             float tPlane = INFINITY;
    //             for (int k = 0; k < planes.size(); k++) {
    //                 tPlane = min(tPlane, intersectPlane(origin, direction, planes[k]));
    //             }

    //             float tBox = INFINITY;
    //             for (int k = 0; k < aabbs.size(); k++) {
    //                 tBox = min(tBox, intersectAABB(origin, direction, aabbs[k]));
    //             }

    //             Uint32 color;
    //             if (tBox < INFINITY || tPlane < INFINITY || t < INFINITY) {
    //                 if(tBox < t && tBox < tPlane){
    //                     float l = 255;
    //                     color = SDL_MapRGBA(format, l, 0, 0, 255); // red
    //                 }
    //                 else if(tPlane < t){
    //                     float l = 255;
    //                     color = SDL_MapRGBA(format, l, 100, l, 255); // grey proportional to t
    //                 }
    //                 else{
    //                     float l = 255*(1.0f - t);
    //                     color = SDL_MapRGBA(format, l, l, l, 255); // grey proportional to t
    //                 }
    //             } else {
    //                 color = SDL_MapRGBA(format, 0, 0, 0, 255); // black
    //             }
    //             pixels[i + frameWidth*j] = color;
    //         }
    //     }

    //     // Update screen to apply the changes
    //     SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
    //     SDL_UpdateWindowSurface(window);
    // }

    vec3 computeRadiance(const vec3& point, const vec3& normal, const vec3& viewDirection, const std::vector<Sphere>& spheres, const std::vector<Plane>& planes, const std::vector<AABB>& aabbs, const std::vector<vec3>& lightSources) {
        vec3 radiance(0.0f);
        // // print point, normal, viewDirection
        // std::cout << "point: " << point.x << " " << point.y << " " << point.z << std::endl;
        // std::cout << "normal: " << normal.x << " " << normal.y << " " << normal.z << std::endl;
        // std::cout << "viewDirection: " << viewDirection.x << " " << viewDirection.y << " " << viewDirection.z << std::endl;

        // // print lightSources
        // for (const auto& lightSource : lightSources) {
        //     std::cout << "lightSource: " << lightSource.x << " " << lightSource.y << " " << lightSource.z << std::endl;
        // }

        // // print spheres
        // for (const auto& sphere : spheres) {
        //     std::cout << "sphere: " << sphere.center.x << " " << sphere.center.y << " " << sphere.center.z << " " << sphere.radius << std::endl;
        // }

        // // print planes
        // for (const auto& plane : planes) {
        //     std::cout << "plane: " << plane.point.x << " " << plane.point.y << " " << plane.point.z << " " << plane.normal.x << " " << plane.normal.y << " " << plane.normal.z << std::endl;
        // }

        // // print aabbs
        // for (const auto& aabb : aabbs) {
        //     std::cout << "aabb: " << aabb.min.x << " " << aabb.min.y << " " << aabb.min.z << " " << aabb.max.x << " " << aabb.max.y << " " << aabb.max.z << std::endl;
        // }



        for (const auto& lightSource : lightSources) {
            vec3 lightDirection = normalize(lightSource - point);
            bool isVisible = true;
            for (const auto& sphere : spheres) {
                // float t = intersectSphere(point + 0.001f * normal, lightDirection, sphere);
                float t = intersectSphere(point + 0.001f * normal, lightDirection, sphere);
                if (t < INFINITY && t > 0.0f) {
                    isVisible = false;
                    break;
                }
            }
            if (isVisible) {
                for (const auto& plane : planes) {
                    float t = intersectPlane(point + 0.001f * normal, lightDirection, plane);
                    if (t < INFINITY && t > 0.0f) {
                        isVisible = false;
                        break;
                    }
                }
            }
            if (isVisible) {
                for (const auto& aabb : aabbs) {
                    float t = intersectAABB(point + 0.001f * normal, lightDirection, aabb);
                    if (t < INFINITY && t > 0.0f) {
                        isVisible = false;
                        break;
                    }
                }
            }
            // isVisible = true;
            if (isVisible) {
                float irradiance = dot(normal, lightDirection);
                vec3 diffuseAlbedo = vec3(0.8f, 0.8f, 0.8f); // Example diffuse albedo
                vec3 exitantRadiance = irradiance * diffuseAlbedo;
                radiance += exitantRadiance;
                // print here
                // std::cout << "here" << std::endl;
            }
        }
        // print radiance
        // std::cout << "radiance: " << radiance.x << " " << radiance.y << " " << radiance.z << std::endl;
        return radiance;
    }

    void render() {
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
                vec3 origin = camera.Eye;
                vec3 direction = normalize(camera.ViewDir + vec3(x, y, 0.0f));
                float t = INFINITY;
                int k_t = -1;
                int k_plane = -1;
                int k_box = -1;
                // k_t , k_plane, k_box are the minimum indices of the sphere, plane and aabb respectively
                for (int k = 0; k < spheres.size(); k++) {
                    float t_temp = intersectSphere(origin, direction, spheres[k]);
                    if(t_temp < t){
                        t = t_temp;
                        k_t = k;
                    }
                }
                float tPlane = INFINITY;
                // k_t , k_plane, k_box are the minimum indices of the sphere, plane and aabb respectively
                for (int k = 0; k < planes.size(); k++) {
                    float t_temp = intersectPlane(origin, direction, planes[k]);
                    if(t_temp < tPlane){
                        tPlane = t_temp;
                        k_plane = k;
                    }
                }

                float tBox = INFINITY;
                // k_t , k_plane, k_box are the minimum indices of the sphere, plane and aabb respectively

                for (int k = 0; k < aabbs.size(); k++) {
                    float t_temp = intersectAABB(origin, direction, aabbs[k]);
                    if(t_temp < tBox){
                        tBox = t_temp;
                        k_box = k;
                    }
                }

                Uint32 color;
                if ((tBox < INFINITY && tBox > 0.0f) || (tPlane < INFINITY && tPlane > 0.0f) || (t < INFINITY && t > 0.0f)) {
                    if(tBox < t && tBox < tPlane){
                        float l = 255;
                        color = SDL_MapRGBA(format, l, 0, 0, 255); // red
                    }
                    else if(tPlane < t){
                        float l = 255;
                        color = SDL_MapRGBA(format, l, 100, l, 255); // grey proportional to t
                    }
                    else{
                        vec3 point = origin + t * direction;
                        vec3 normal;
                        int k; // Declare the variable 'k' here
                        if(k_t != -1){
                            point = origin + t * direction;
                            normal = computeNormalSphere(point, spheres[k_t]);
                        }
                        else if(k_plane != -1){
                            point = origin + tPlane * direction;
                            normal = computeNormalPlane(point, planes[k_plane]);
                        }
                        else if(k_box != -1){
                            point = origin + tBox * direction;
                            normal = computeNormalAABB(point, aabbs[k_box]);
                        }
                        vec3 viewDirection = normalize(camera.Eye - point);
                        vec3 radiance = computeRadiance(point, normal, viewDirection, spheres, planes, aabbs, lightSources);
                        // std::cout << radiance.r << " " << radiance.g << " " << radiance.b << std::endl;
                        // radiance = gammaCorrection(radiance);
                        // // std::cout << radiance.r << " " << radiance.g << " " << radiance.b << std::endl;
                        float r = 255 * radiance.r;
                        float g = 255 * radiance.g;
                        float b = 255 * radiance.b;
                        color = SDL_MapRGBA(format, r, g, b, 255); // grey proportional to t
                        // float l = 255*(1.0f - t);
                        // color = SDL_MapRGBA(format, l, l, l, 255); // grey proportional to t
                    }
                } else {
                    color = SDL_MapRGBA(format, 0, 255, 0, 255); // black
                }
                pixels[i + frameWidth*j] = color;
            }
        }

        // Update screen to apply the changes
        SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
        SDL_UpdateWindowSurface(window);
    }

    vec3 computeNormalSphere(const vec3& point, const Sphere& sphere) {
        return normalize(point - sphere.center);
    }

    vec3 computeNormalAABB(const vec3& point, const AABB& aabb) {
        vec3 normal(0.0f);
        for (int i = 0; i < 3; ++i) {
            if (point[i] < aabb.min[i] + 1e-6) {
                normal[i] = -1.0f;
            } else if (point[i] > aabb.max[i] - 1e-6) {
                normal[i] = 1.0f;
            }
        }
        return normal;
    }

    vec3 computeNormalPlane(const vec3& point, const Plane& plane) {
        return plane.normal;
    }

    vec3 gammaCorrection(const vec3& radiance) {
        return pow(radiance, vec3(1.0f/2.2f));
    }

    std::vector<vec3> lightSources = {vec3(0.0f, 10.0f, -5.0f)};


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
        glm::vec3 transformedOrigin = glm::inverse(sphere.transform) * glm::vec4(origin, 1.0f);
        glm::vec3 transformedDirection = glm::inverse(sphere.transform) * glm::vec4(direction, 0.0f);
    
        vec3 oc = transformedOrigin - sphere.center;
        float a = dot(transformedDirection, transformedDirection);
        float b = 2.0f * dot(oc, transformedDirection);
        float c = dot(oc, oc) - (sphere.radius * sphere.radius);
        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f) return INFINITY;
        return (-b - sqrt(discriminant)) / (2.0f * a);
    }

    float intersectPlane(const glm::vec3& origin, const glm::vec3& direction, const Plane& plane) {
        glm::vec3 transformedOrigin = glm::inverse(plane.transform) * glm::vec4(origin, 1.0f);
        glm::vec3 transformedDirection = glm::inverse(plane.transform) * glm::vec4(direction, 0.0f);

        vec3 transformedNormal = vec3(glm::transpose(glm::inverse(plane.transform)) * vec4(plane.normal, 0.0f));

        float denom = dot(plane.normal, transformedDirection);
        if (abs(denom) > 1e-6) {
            glm::vec3 p0l0 = plane.point - transformedOrigin;
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
        glm::vec3 transformedOrigin = glm::inverse(aabb.transform) * glm::vec4(origin, 1.0f);
        glm::vec3 transformedDirection = glm::inverse(aabb.transform) * glm::vec4(direction, 0.0f);
        // vec3 transformedMin = vec3(aabb.transform * vec4(aabb.min, 1.0f));
        // vec3 transformedMax = vec3(aabb.transform * vec4(aabb.max, 1.0f));

        for (int i = 0; i < 3; ++i) {
            if (abs(transformedDirection[i]) < 1e-6) {
                if (transformedOrigin[i] < aabb.min[i] || transformedOrigin[i] > aabb.max[i]) {
                    return false;
                }
            } else {
                float t1 = (aabb.min[i] - transformedOrigin[i]) / transformedDirection[i];
                float t2 = (aabb.max[i] - transformedOrigin[i]) / transformedDirection[i];
    
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
    Camera camera;
    // glm::vec3 camera;
    // glm::vec3 cameraDirection;
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
