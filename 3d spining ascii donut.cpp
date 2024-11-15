#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int width = 80;
const int height = 22;
const float theta_spacing = 0.07;
const float phi_spacing = 0.02;
const int R1 = 1;
const int R2 = 2;
const int K2 = 5;
const float K1 = width * K2 * 3 / (8 * (R1 + R2));

void render_frame(float A, float B) {
    char output[width * height];
    float zbuffer[width * height];
    std::fill_n(output, width * height, ' ');
    std::fill_n(zbuffer, width * height, 0.0f);

    for (float theta = 0; theta < 2 * M_PI; theta += theta_spacing) {
        for (float phi = 0; phi < 2 * M_PI; phi += phi_spacing) {
            float costheta = cos(theta), sintheta = sin(theta);
            float cosphi = cos(phi), sinphi = sin(phi);
            float circlex = R2 + R1 * costheta;
            float circley = R1 * sintheta;
            float x = circlex * (cos(B) * cosphi + sin(A) * sin(B) * sinphi) - circley * cos(A) * sin(B);
            float y = circlex * (sin(B) * cosphi - sin(A) * cos(B) * sinphi) + circley * cos(A) * cos(B);
            float z = K2 + cos(A) * circlex * sinphi + circley * sin(A);
            float ooz = 1 / z;
            int xp = static_cast<int>(width / 2 + K1 * ooz * x);
            int yp = static_cast<int>(height / 2 - K1 * ooz * y);
            float L = cosphi * costheta * sin(B) - cos(A) * costheta * sinphi - sin(A) * sintheta + cos(B) * (cos(A) * sintheta - costheta * sin(A) * sinphi);
            if (L > 0) {
                int luminance_index = std::min(9, std::max(0, static_cast<int>(L * 8)));
                int idx = xp + yp * width;
                if (idx >= 0 && idx < width * height) {
                    if (ooz > zbuffer[idx]) {
                        zbuffer[idx] = ooz;
                        output[idx] = ".,-~:;=!*#$@"[luminance_index];
                    }
                }
            }
        }
    }

    std::cout << "\x1b[H";
    for (int k = 0; k < width * height; k++) {
        std::putchar(k % width ? output[k] : '\n');
    }
}

int main() {
    std::cout << "\x1b[2J\x1b[H";
    float A = 0, B = 0;
    while (true) {
        render_frame(A, B);
        A += 0.04;
        B += 0.02;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    return 0;
}