#include <igl/colon.h>
#include <igl/directed_edge_orientations.h>
#include <igl/directed_edge_parents.h>
#include <igl/forward_kinematics.h>
#include <igl/PI.h>
#include <igl/lbs_matrix.h>
#include <igl/deform_skeleton.h>
#include <igl/dqs.h>
#include <igl/readDMAT.h>
#include <igl/readOFF.h>
#include <igl/arap.h>
#include <igl/opengl/glfw/Viewer.h>

#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <vector>
#include <algorithm>
#include <iostream>

#include <direct.h>
#include <limits.h>

typedef
std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >
RotationList;

const Eigen::RowVector3d sea_green(70. / 255., 252. / 255., 167. / 255.);
Eigen::MatrixXd V(12, 3), U(12, 3);
Eigen::MatrixXi F(8, 3);
Eigen::VectorXi S(12), b;
Eigen::RowVector3d mid;
double anim_t = 0.0;
double anim_t_dir = 0.3;
igl::ARAPData arap_data;

bool pre_draw(igl::opengl::glfw::Viewer& viewer)
{
    using namespace Eigen;
    using namespace std;
    MatrixXd bc(b.size(), V.cols());
    for (int i = 0;i < b.size();i++)
    {
        bc.row(i) = V.row(b(i));
        switch (S(b(i)))
        {
        case 0:
        {
            /*const double r = mid(0) * 0.25;
            bc(i, 0) += r * sin(0.5 * anim_t * 2. * igl::PI);
            bc(i, 1) -= r + r * cos(igl::PI + 0.5 * anim_t * 2. * igl::PI);*/
            break;
        }
        case 1:
        {
            const double r = mid(1) * 0.15;
            bc(i, 0) += r + r * cos(igl::PI + 0.15 * anim_t * 2. * igl::PI);
            bc(i, 1) -= r * sin(0.15 * anim_t * 2. * igl::PI);
            break;
        }
        case 2:
        {
            const double r = mid(1) * 0.15;
            bc(i, 2) += r + r * cos(igl::PI + 0.35 * anim_t * 2. * igl::PI);
            bc(i, 0) += r * sin(0.35 * anim_t * 2. * igl::PI);
            break;
        }
        default:
            break;
        }
    }
    igl::arap_solve(bc, arap_data, U);
    viewer.data().set_vertices(U);
    viewer.data().compute_normals();
    if (viewer.core().is_animating)
    {
        anim_t += anim_t_dir;
    }
    return false;
}

bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int mods)
{
    switch (key)
    {
    case ' ':
        viewer.core().is_animating = !viewer.core().is_animating;
        return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    using namespace Eigen;
    using namespace std;
    /*char cwd[1000];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working directory is: " << cwd << '\n';
    }
    else {
        std::cerr << "Failed to get current working directory.\n";
    }*/
    //igl::readOFF("../../../decimated-knight.off", V, F);
    V << 0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,
        1.f, 1.f, 0.f, //p1
        0.f, 1.f, 0.f, //p3

        1.f, 0.f, 0.f,
        2.f, 0.f, 0.f,
        2.f, 1.f, 0.f, //p2
        //p1

        1.f, 1.f, 0.f, //p4
        //p2
        2.f, 2.f, 0.f,
        1.f, 2.f, 0.f,

        //p3
        //p4
        1.f, 2.f, 0.f,
        0.f, 2.f, 0.f;
    F << 0, 1, 3,
        1, 2, 3,
        4, 5, 2,
        5, 6, 2,
        7, 6, 9,
        6, 8, 9,
        3, 7, 11,
        7, 10, 11;
    std::cout << "V.cols(): " << V.cols() << std::endl;
    U = V;
    //igl::readDMAT("../../../decimated-knight-selection.dmat", S);
    S.setConstant(-1);
    S(0) = 0;
    S(8) = 1;
    std::cout << "S.size(): " << S.size() << std::endl;

    // vertices in selection
    igl::colon<int>(0, V.rows() - 1, b);
    b.conservativeResize(stable_partition(b.data(), b.data() + b.size(),
        [](int i)->bool {return S(i) >= 0;}) - b.data());
    std::cout << "b.size(): " << b.size() << std::endl;
    std::cout << 'b' << b[0] << ' ' << b[1] << std::endl;

    // Centroid
    mid = 0.5 * (V.colwise().maxCoeff() + V.colwise().minCoeff());
    // Precomputation
    arap_data.max_iter = 100;
    igl::arap_precomputation(V, F, V.cols(), b, arap_data);

    // Set color based on selection
    MatrixXd C(F.rows(), 3);
    RowVector3d purple(80.0 / 255.0, 64.0 / 255.0, 255.0 / 255.0);
    RowVector3d gold(255.0 / 255.0, 228.0 / 255.0, 58.0 / 255.0);
    for (int f = 0;f < F.rows();f++)
    {
        if (S(F(f, 0)) >= 0 && S(F(f, 1)) >= 0 && S(F(f, 2)) >= 0)
        {
            C.row(f) = purple;
        }
        else
        {
            C.row(f) = gold;
        }
    }

    // Plot the mesh with pseudocolors
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(U, F);
    viewer.data().set_colors(C);
    viewer.callback_pre_draw = &pre_draw;
    viewer.callback_key_down = &key_down;
    viewer.core().is_animating = false;
    viewer.core().animation_max_fps = 30.;
    cout <<
        "Press [space] to toggle animation" << endl;
    viewer.launch();
}
