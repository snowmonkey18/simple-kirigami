#include <igl/opengl/glfw/Viewer.h>
#include <math.h>

int main(int argc, char* argv[])
{
    float theta = .5;
    float s = sin(theta);
    float c = cos(theta);
    float z = 0.f;

    float x00 = 0.f;
    float y00 = 0.f;
    float x01 = x00 + 1.f;
    float y01 = y00;
    float x02 = x01;
    float y02 = y01 + 1.f;
    float x03 = x02 - 1.f;
    float y03 = y02;

    float x10 = x02 + s;
    float y10 = y02 - c;
    float x11 = x10 + c;
    float y11 = y10 + s;
    float x12 = x11 - s;
    float y12 = y11 + c;

    float x20 = x12 - 1.f;
    float y20 = y12 + 0.f;
    float x22 = x12 + 0.f;
    float y22 = y12 + 1.f;
    float x23 = x22 - 1.f;
    float y23 = y22 + 0.f;

    float x32 = x20 - s;
    float y32 = y20 + c;
    float x33 = x32 - c;
    float y33 = y32 - s;
    
    const Eigen::MatrixXd V = (Eigen::MatrixXd(12, 3) << 
        x00, y00, z,
        x01, y01, z,
        x02, y02, z,
        x03, y03, z,

        x10, y10, z,
        x11, y11, z,
        x12, y12, z,
        //xy13 = xy02

        x20, y20, z,
        //xy21 = xy12
        x22, y22, z,
        x23, y23, z,

        //xy30 = xy13
        //xy31 = xy20
        x32, y32, z,
        x33, y33, z).finished();

    const Eigen::MatrixXi F = (Eigen::MatrixXi(8, 3) <<
        0, 1, 3,
        1, 2, 3,
        4, 5, 2,
        5, 6, 2,
        7, 6, 9,
        6, 8, 9,
        3, 7, 11,
        7, 10, 11).finished();

    // show
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    viewer.data().set_face_based(true);
    viewer.launch();
}
