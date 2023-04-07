#include <Eigen/StdVector>
#include <igl/min_quad_with_fixed.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/snap_points.h>

// Undoable
struct State
{
    // Rest and transformed control points
    Eigen::MatrixXd CV, CU; // <Eigen/StdVector>
    bool placing_handles = true;
} s;

int main(int argc, char* argv[]) //? necessary?
{
    Eigen::MatrixXd V, U; // rest and transformed vertices
    Eigen::MatrixXi F;
    long sel = -1;
    Eigen::RowVector3f last_mouse;
    igl::min_quad_with_fixed_data<double> arap_data; // <igl/min_quad_with_fixed.h>
    Eigen::SparseMatrix<double> arap_K;

    // Load input meshes
    V = (Eigen::MatrixXd(12, 3) <<
        0.f, 0.f, 0.f,
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
        0.f, 2.f, 0.f
        ).finished();
    F = (Eigen::MatrixXi(8, 3) <<
        0, 1, 3,
        1, 2, 3,
        4, 5, 2,
        5, 6, 2,
        7, 6, 9,
        6, 8, 9,
        3, 7, 11,
        7, 10, 11
        ).finished();
    U = V;
    igl::opengl::glfw::Viewer viewer; // #include <igl/opengl/glfw/Viewer.h>
    std::cout << R"(
[click]  To place new control point
[drag]   To move control point
[space]  Toggle whether placing control points or deforming
U,u      Update deformation (i.e., run another iteration of solver)
R,r      Reset control points
)";

    const auto& update = [&]()
    {
        // predefined colors
        const Eigen::RowVector3d orange(1.0, 0.7, 0.2);
        const Eigen::RowVector3d blue(0.2, 0.3, 0.8);
        if (s.placing_handles)
        {
            viewer.data().set_vertices(V); // rest mesh vertices
            viewer.data().set_colors(blue); // face color
            viewer.data().set_points(s.CV, orange); // rest control points
        }
        else
        {
            // SOLVE FOR ARAP DEFORMATION
            //arap_single_iteration(arap_data, arap_K, s.CU, U);
            

            viewer.data().set_vertices(U); // transformed mesh vertices
            viewer.data().set_colors(orange); // face color
            viewer.data().set_points(s.CU, blue); // transformed control points
        }
        viewer.data().compute_normals();
    };
    viewer.callback_mouse_down = [&](igl::opengl::glfw::Viewer&, int, int)->bool
    {
        last_mouse = Eigen::RowVector3f(
            viewer.current_mouse_x, viewer.core().viewport(3) - viewer.current_mouse_y, 0);
        std::cout << "last_mouse: " << last_mouse[0] << ' ' << last_mouse[1] << ' ' << last_mouse[2] << std::endl;
        if (s.placing_handles)
        {
            // Find closest point on mesh to mouse position
            int fid; // face index in V
            Eigen::Vector3f bary; // wrt fid face of V
            if (igl::unproject_onto_mesh( // <igl/unproject_onto_mesh.h>
                last_mouse.head(2),
                viewer.core().view,
                viewer.core().proj,
                viewer.core().viewport,
                V, F,
                fid, bary))
            {
                std::cout << "bary: " << bary[0] << ' ' << bary[1] << ' ' << bary[2] << std::endl;
                long c; // vertex index in face (0, 1, 2)
                bary.maxCoeff(&c);
                std::cout << "c: " << c << std::endl;
                Eigen::RowVector3d new_c = V.row(F(fid, c)); // x,y,z coords of vertex
                std::cout << "new_c: " << new_c[0] << ' ' << new_c[1] << ' ' << new_c[2] << std::endl;
                if (s.CV.size() == 0 || (s.CV.rowwise() - new_c).rowwise().norm().minCoeff() > 0)
                {
                    s.CV.conservativeResize(s.CV.rows() + 1, 3);
                    // Snap to closest vertex on hit face
                    s.CV.row(s.CV.rows() - 1) = new_c;
                    update();
                    return true;
                }
            }
        }
        else
        {
            // Move closest control point
            Eigen::MatrixXf CP;
            igl::project(
                Eigen::MatrixXf(s.CU.cast<float>()),
                viewer.core().view,
                viewer.core().proj, viewer.core().viewport, CP);
            Eigen::VectorXf D = (CP.rowwise() - last_mouse).rowwise().norm();
            sel = (D.minCoeff(&sel) < 30) ? sel : -1;
            std::cout << "sel: " << sel << std::endl;
            if (sel != -1)
            {
                last_mouse(2) = CP(sel, 2);
                std::cout << "last_mouse: " << last_mouse[0] << ' ' << last_mouse[1] << ' ' << last_mouse[2] << std::endl;
                update();
                return true;
            }
        }
        return false;
    };

    viewer.callback_mouse_move = [&](igl::opengl::glfw::Viewer&, int, int)->bool
    {
        if (sel != -1)
        {
            Eigen::RowVector3f drag_mouse(
                viewer.current_mouse_x,
                viewer.core().viewport(3) - viewer.current_mouse_y,
                last_mouse(2));
            Eigen::RowVector3f drag_scene, last_scene;
            igl::unproject(
                drag_mouse,
                viewer.core().view,
                viewer.core().proj,
                viewer.core().viewport,
                drag_scene);
            igl::unproject(
                last_mouse,
                viewer.core().view,
                viewer.core().proj,
                viewer.core().viewport,
                last_scene);
            s.CU.row(sel) += (drag_scene - last_scene).cast<double>();
            last_mouse = drag_mouse;
            update();
            return true;
        }
        return false;
    };
    viewer.callback_mouse_up = [&](igl::opengl::glfw::Viewer&, int, int)->bool
    {
        sel = -1;
        return false;
    };
    viewer.callback_key_pressed =
        [&](igl::opengl::glfw::Viewer&, unsigned int key, int mod)
    {
        switch (key)
        {
            case 'R':
            case 'r':
            {
                s.CU = s.CV;
                break;
            }
            case 'U':
            case 'u':
            {
                // Just trigger an update
                break;
            }
            case ' ':
                s.placing_handles ^= 1;
                if (!s.placing_handles && s.CV.rows() > 0)
                {
                    // Switching to deformation mode
                    s.CU = s.CV;
                    Eigen::VectorXi b;
                    igl::snap_points(s.CV, V, b);
                    // PRECOMPUTATION FOR DEFORMATION
                    //igl::arap_precomputation(V, F, V.cols(), b, arap_data);
                }
                break;
            default:
                return false;
        }
        update();
        return true;
    };
    viewer.callback_pre_draw =
        [&](igl::opengl::glfw::Viewer&)->bool
    {
        if (viewer.core().is_animating && !s.placing_handles)
        {
            //arap_single_iteration(arap_data, arap_K, s.CU, U);
            update();
        }
        return false;
    };
    viewer.data().set_mesh(V, F);
    viewer.data().show_lines = true;
    viewer.core().is_animating = true;
    viewer.data().face_based = true;
    update();
    viewer.launch();
    return EXIT_SUCCESS;
}
