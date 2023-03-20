#include <igl/opengl/glfw/Viewer.h>

int main(int argc, char* argv[])
{
    // mesh
    const Eigen::MatrixXd V = (Eigen::MatrixXd(12, 3) <<
        -1.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        -1.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        1.0, -1.0, 0.0).finished();
    const Eigen::MatrixXi F = (Eigen::MatrixXi(8, 3) <<
        0, 4, 1,
        0, 3, 4,
        1, 6, 2,
        1, 5, 6,
        5, 11, 8,
        5, 10, 11,
        7, 10, 4,
        7, 9, 10).finished();
    
    // show
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    viewer.data().set_face_based(true);
    viewer.launch();
}
