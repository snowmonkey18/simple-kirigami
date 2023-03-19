#include <igl/opengl/glfw/Viewer.h>
//#include <../../../../../../ShapeOp.0.1.0/libShapeOp/src/Constraint.h>
//#include <../../../../../../ShapeOp.0.1.0/libShapeOp/src/Force.h>
//
//#include <Eigen/Dense>
//#include <Eigen/Sparse>
//#include <Solver.h>
//#include <Constraint.h>
//#include <Force.h>

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
    //Matrix3X V_new = Eigen::MatrixXd(3, 12);

    //// fix upper left vertex
    //const Vector3 position(-1.f, 1.f, 0.f);
    //ClosenessConstraint closeness_constraint = ClosenessConstraint(std::vector<int>{0}, 1.f, V.transpose());
    //closeness_constraint.setPosition(position);
    //closeness_constraint.project(V.transpose(), V_new); //? diff btwn project and add_constraint?

    //// preserve angles
    //RectangleConstraint rectangle_constraint_ul = RectangleConstraint(std::vector<int>{0, 3, 4, 1}, 1.f, V.transpose());
    //RectangleConstraint rectangle_constraint_ur = RectangleConstraint(std::vector<int>{1, 5, 6, 2}, 1.f, V.transpose());
    //RectangleConstraint rectangle_constraint_dl = RectangleConstraint(std::vector<int>{7, 9, 10, 4}, 1.f, V.transpose());
    //RectangleConstraint rectangle_constraint_dr = RectangleConstraint(std::vector<int>{5, 10, 11, 6}, 1.f, V.transpose());
    //rectangle_constraint_ul.project(V.transpose(), V_new);
    //rectangle_constraint_ur.project(V.transpose(), V_new);
    //rectangle_constraint_dl.project(V.transpose(), V_new);
    //rectangle_constraint_dr.project(V.transpose(), V_new);

    //// preserve areas
    //AreaConstraint area_constraint_ul0 = AreaConstraint(std::vector<int>{0, 4, 1}, 1.f, V.transpose(), .49, .51);
    //AreaConstraint area_constraint_ul1 = AreaConstraint(std::vector<int>{0, 3, 4}, 1.f, V.transpose(), .49, .51);
    //AreaConstraint area_constraint_ur0 = AreaConstraint(std::vector<int>{1, 6, 2}, 1.f, V.transpose(), .49, .51);
    //AreaConstraint area_constraint_ur1 = AreaConstraint(std::vector<int>{1, 5, 6}, 1.f, V.transpose(), .49, .51);
    //AreaConstraint area_constraint_dl0 = AreaConstraint(std::vector<int>{7, 10, 4}, 1.f, V.transpose(), .49, .51);
    //AreaConstraint area_constraint_dl1 = AreaConstraint(std::vector<int>{7, 9, 10}, 1.f, V.transpose(), .49, .51);
    //AreaConstraint area_constraint_dr0 = AreaConstraint(std::vector<int>{5, 11, 6}, 1.f, V.transpose(), .49, .51);
    //AreaConstraint area_constraint_dr1 = AreaConstraint(std::vector<int>{5, 10, 11}, 1.f, V.transpose(), .49, .51);
    //area_constraint_ul0.project(V.transpose(), V_new);
    //area_constraint_ul1.project(V.transpose(), V_new);
    //area_constraint_ur0.project(V.transpose(), V_new);
    //area_constraint_ur1.project(V.transpose(), V_new);
    //area_constraint_dl0.project(V.transpose(), V_new);
    //area_constraint_dl1.project(V.transpose(), V_new);
    //area_constraint_dr0.project(V.transpose(), V_new);
    //area_constraint_dr1.project(V.transpose(), V_new);

    // gravity force
  /*  const auto f = ShapeOp::Vector3(1.f, 0.f, 0.f);
    ShapeOp::GravityForce gravity_force = ShapeOp::GravityForce(f);*/

    // show
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    viewer.data().set_face_based(true);
    viewer.data().set_colors((Eigen::MatrixXd(1, 3) <<
        0.0, 1.0, 0.0).finished());
    viewer.launch();

    //return(0);
}
