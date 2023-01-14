#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

constexpr double MY_PI = 3.1415926;

inline float deg2rad(float degree){
    return degree/180.0f*MY_PI;
}
bool ex = true;
Eigen::Matrix4f get_rotation(Eigen::Vector3f,float);
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
                 0, 1, 0, -eye_pos[1], 
                 0, 0, 1, -eye_pos[2], 
                 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    if(!ex)
    {float radian_rotation_angle = deg2rad(rotation_angle);
    model << cos(radian_rotation_angle),-sin(radian_rotation_angle),0,0,
            sin(radian_rotation_angle),cos(radian_rotation_angle),0,0,
            0,0,1,0,
            0,0,0,1;}
    else 
        model = get_rotation({0,1,0},rotation_angle);
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.
    eye_fov = deg2rad(eye_fov);
    float t = fabs(zNear)*tan(eye_fov/2),b = -t;
    float r = t*aspect_ratio,l = -r;
    float n = -zNear,f = -zFar;
    Eigen::Matrix4f p2o;
    p2o<<n,0,0,0,
         0,n,0,0,
         0,0,n+f,-n*f,
         0,0,1,0;
    Eigen::Matrix4f translation;
    translation << 1,0,0,-(l+r)/2,
                   0,1,0,-(b+t)/2,
                   0,0,1,-(n+f)/2,
                   0,0,0,1;
    Eigen::Matrix4f zoom;
    zoom << 2/(r-l),0,0,0,
            0,2/(t-b),0,0,
            0,0,2/(n-f),0,
            0,0,0,1;
    auto ortho = zoom*translation;
    projection = ortho*p2o;
    return projection;
}

Eigen::Matrix4f get_rotation(Vector3f axis,float angle){
    /*Rodrigues' rotation formula*/
    /* R = Icos+(1-cos)axis*trans(axis)+sin balabala */
    angle = deg2rad(angle);
    Eigen::Matrix3f rotation3f;
    Eigen::Matrix3f I3=Eigen::Matrix3f::Identity(),temp;
    temp << 0,-axis[2],axis[1],
            axis[2],0,-axis[0],
            -axis[1],axis[0],0;
    float cos_v = cos(angle),sin_v = sin(angle);
    rotation3f = cos_v*I3+(1-cos_v)*axis*axis.transpose()+sin_v * temp;
    //rewrite it with homogenous coordinate.
    Eigen::Matrix4f rotation;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            rotation(i,j)=rotation3f(i,j);
    rotation.col(3).setZero();
    rotation.row(3).setZero();
    rotation(3,3)=1;
    
    return rotation;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
