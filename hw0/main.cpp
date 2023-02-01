#include <Eigen/Core>
#include <Eigen/Dense>
#include <cmath>
#include <iostream>
using namespace std;
using namespace Eigen;
#define M_PI 3.14159265358979323846
int main() {
    // define p(2,1) by homogeneous coordinate
    Vector3f p(2.0f, 1.0f, 1.0f);
    // define rotation matrix
    Matrix3f R;
    float theta = M_PI / 4.0;
    R << cos(theta), -sin(theta), 0, sin(theta), cos(theta), 0, 0, 0, 1;
    // define translation matrix
    Matrix3f T;
    T << 1, 0, 1, 0, 1, 2, 0, 0, 1;
    // define transformation matrix
    Matrix3f M;
    M = T * R;
    // transform p
    p = M * p;
    cout << p << endl;
    return 0;
}