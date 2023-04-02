//#include "biharmonic_precompute.h"
//#include "biharmonic_solve.h"
//#include "arap_precompute.h"
//#include "arap_single_iteration.h"
#include <igl/min_quad_with_fixed.h>
#include <igl/read_triangle_mesh.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/project.h>
#include <igl/unproject.h>
#include <igl/snap_points.h>
#include <igl/unproject_onto_mesh.h>
#include <Eigen/Core>
#include <iostream>
#include <stack>

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

// Undoable
struct State
{
    // Rest and transformed control points
    Eigen::MatrixXd CV, CU;
    bool placing_handles = true;
} s;

int main(int argc, char* argv[])
{
    // Undo Management
    std::stack<State> undo_stack, redo_stack;
    const auto push_undo = [&](State& _s = s) // after making changes, can undo, can't redo
    {
        undo_stack.push(_s);
        // clear
        redo_stack = std::stack<State>();
    };
    const auto undo = [&]() // after undoing, we can redo
    {
        if (!undo_stack.empty())
        {
            redo_stack.push(s);
            s = undo_stack.top();
            undo_stack.pop();
        }
    };
    const auto redo = [&]() // after redoing, we can undo
    {
        if (!redo_stack.empty())
        {
            undo_stack.push(s);
            s = redo_stack.top();
            redo_stack.pop();
        }
    };

    Eigen::MatrixXd V, U; // rest and transformed vertices
    Eigen::MatrixXi F;
    long sel = -1; //?
    Eigen::RowVector3f last_mouse; //?
    igl::min_quad_with_fixed_data<double> biharmonic_data, arap_data;
    Eigen::SparseMatrix<double> arap_K;

    // Load input meshes
    float z = 0.f;

    float x00 = 0.f;
    float y00 = 0.f;
    float x01 = x00 + 1.f;
    float y01 = y00;
    float x02 = x01;
    float y02 = y01 + 1.f;
    float x03 = x02 - 1.f;
    float y03 = y02;

    float x10 = x02;
    float y10 = y02 - 1.f;
    float x11 = x10 + 1.f;
    float y11 = y10;
    float x12 = x11;
    float y12 = y11 + 1.f;

    float x20 = x12 - 1.f;
    float y20 = y12 + 0.f;
    float x22 = x12 + 0.f;
    float y22 = y12 + 1.f;
    float x23 = x22 - 1.f;
    float y23 = y22 + 0.f;

    float x32 = x20;
    float y32 = y20 + 1.f;
    float x33 = x32 - 1.f;
    float y33 = y32;

    V = (Eigen::MatrixXd(12, 3) <<
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
        x33, y33, z
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
    igl::opengl::glfw::Viewer viewer;
    std::cout << R"(
[click]  To place new control point
[drag]   To move control point
[space]  Toggle whether placing control points or deforming
M,m      Switch deformation methods
U,u      Update deformation (i.e., run another iteration of solver)
R,r      Reset control points 
⌘ Z      Undo
⌘ ⇧ Z    Redo
)";
    enum Method
    {
        BIHARMONIC = 0,
        ARAP = 1,
        NUM_METHODS = 2,
    } method = ARAP;

    const auto& update = [&]()
    {
        // predefined colors
        const Eigen::RowVector3d orange(1.0, 0.7, 0.2);
        const Eigen::RowVector3d yellow(1.0, 0.9, 0.2);
        const Eigen::RowVector3d blue(0.2, 0.3, 0.8);
        const Eigen::RowVector3d green(0.2, 0.6, 0.3);
        if (s.placing_handles)
        {
            viewer.data().set_vertices(V); // all triangle vertices
            viewer.data().set_colors(blue); // face color
            viewer.data().set_points(s.CV, orange); // control points
        }
        else
        {
            // SOLVE FOR DEFORMATION
            switch (method)
            {
                default:
                    case BIHARMONIC:
                    {
                        Eigen::MatrixXd D;
                        //biharmonic_solve(biharmonic_data, s.CU - s.CV, D);
                        U = V + D;
                        break;
                    }
                    case ARAP:
                    {
                        //arap_single_iteration(arap_data, arap_K, s.CU, U);
                        break;
                    }
            }
            viewer.data().set_vertices(U);
            viewer.data().set_colors(method == ARAP ? orange : yellow);
            viewer.data().set_points(s.CU, method == ARAP ? blue : green);
        }
        viewer.data().compute_normals();
    };
    viewer.callback_mouse_down = [&](igl::opengl::glfw::Viewer&, int, int)->bool
    {
        last_mouse = Eigen::RowVector3f(
            viewer.current_mouse_x, viewer.core().viewport(3) - viewer.current_mouse_y, 0);
        if (s.placing_handles)
        {
            // Find closest point on mesh to mouse position
            int fid; // face index in V
            Eigen::Vector3f bary;
            if (igl::unproject_onto_mesh(
                last_mouse.head(2),
                viewer.core().view,
                viewer.core().proj,
                viewer.core().viewport,
                V, F,
                fid, bary))
            {
                long c; //? long type // vertex index in face (0, 1, 2)
                bary.maxCoeff(&c);
                Eigen::RowVector3d new_c = V.row(F(fid, c)); // x,y,z coords of vertex
                std::cout << "hello: " << new_c[0] << new_c[1] << new_c[2] << std::endl;
                if (s.CV.size() == 0 || (s.CV.rowwise() - new_c).rowwise().norm().minCoeff() > 0)
                {
                    push_undo();
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
            if (sel != -1)
            {
                last_mouse(2) = CP(sel, 2);
                push_undo();
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
        case 'M':
        case 'm':
        {
            method = (Method)(((int)(method)+1) % ((int)(NUM_METHODS)));
            break;
        }
        case 'R':
        case 'r':
        {
            push_undo();
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
            push_undo();
            s.placing_handles ^= 1;
            if (!s.placing_handles && s.CV.rows() > 0)
            {
                // Switching to deformation mode
                s.CU = s.CV;
                Eigen::VectorXi b;
                igl::snap_points(s.CV, V, b);
                // PRECOMPUTATION FOR DEFORMATION
                //biharmonic_precompute(V, F, b, biharmonic_data);
                igl::arap_precomputation(V, F, V.cols(), b, arap_data);
            }
            break;
        default:
            return false;
        }
        update();
        return true;
    };

    // Special callback for handling undo
    viewer.callback_key_down =
        [&](igl::opengl::glfw::Viewer&, unsigned char key, int mod)->bool
    {
        if (key == 'Z' && (mod & GLFW_MOD_SUPER))
        {
            (mod & GLFW_MOD_SHIFT) ? redo() : undo();
            update();
            return true;
        }
        return false;
    };
    viewer.callback_pre_draw =
        [&](igl::opengl::glfw::Viewer&)->bool
    {
        if (viewer.core().is_animating && !s.placing_handles && method == ARAP)
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
