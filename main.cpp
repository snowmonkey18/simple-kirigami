#include <igl/opengl/glfw/Viewer.h>
#include <math.h>

int main(int argc, char* argv[])
{
    Eigen::MatrixXd V = (Eigen::MatrixXd(3, 3) <<
        0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f
        ).finished();
    Eigen::MatrixXi F = (Eigen::MatrixXi(1, 3) <<
        0, 1, 2
        ).finished();
    igl::opengl::glfw::Viewer viewer;

    viewer.callback_mouse_move =
        [&V, &F](igl::opengl::glfw::Viewer& viewer, int, int)->bool
    {
        int fid;
        Eigen::Vector3f bc;
        // Cast a ray in the view direction starting from the mouse position
        double x = (viewer.current_mouse_x - viewer.core().viewport[0]) / viewer.core().viewport[2];
        double y = 1 - (viewer.current_mouse_y + viewer.core().viewport[1]) / viewer.core().viewport[3];
        double z = 0.f;

        std::cout << "hello: " << x << ' ' << y << ' ' << z << std::endl;

        V = (Eigen::MatrixXd(3, 3) <<
            0.f, 0.f, z,
            y, x, z,
            x, y, z
            ).finished();

        viewer.data().set_vertices(V);
        return false;
    };

    // show
    viewer.data().set_mesh(V, F);
    viewer.data().set_face_based(true);
    viewer.launch();
}
