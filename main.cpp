#include <Eigen/StdVector>
#include <igl/arap.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/snap_points.h>
#include <igl/colon.h>
#include <algorithm>

Eigen::MatrixXd CV, CU; // rest and transformed control points
Eigen::MatrixXd V(12, 3), U(12, 3); // rest and transformed vertices
Eigen::MatrixXi F(8, 3);
Eigen::VectorXi S(12), b;
bool placing_handles = true;
long sel = -1;
Eigen::RowVector3f last_mouse;
igl::ARAPData arap_data;
void add_vertex(float x, float y, float z, std::vector<float>* vertices) {
    vertices->push_back(x);
    vertices->push_back(y);
    vertices->push_back(z);
}
void get_quad_vertices(std::vector<float> bl_vertex) {
    std::vector<float> vertices = { bl_vertex };
    add_vertex(bl_vertex[0] + 1.f, bl_vertex[1], bl_vertex[2], &vertices);
    add_vertex(bl_vertex[0], bl_vertex[1] + 1.f, bl_vertex[2], &vertices);
    add_vertex(bl_vertex[0] + 1.f, bl_vertex[1] + 1.f, bl_vertex[2], &vertices);
}
void add_quad_vertices(std::vector<float> bl_vertex, std::vector<int> indices, std::vector<float>* vertices) {
    for (int index : indices) {
        vertices->push_back(0);
    }
}
void get_squares(int n, std::vector<std::vector<float>>* squares) {
    /*
    * n [>= 1]: size of grid (for nxn grid of 1x1 squares)
    * squares: initialized with first element (ie first square at origin)
    * return: vector of coordinates of locations of squares (by bottom - left vertex)
    */
    for (int i = 1; i < n; i++) {
        std::vector<float> prev = squares->back();
        // for (i=1) and subsequent odd iterations: go "out right" -> up -> left
        if (i % 2 == 1) {
            squares->push_back({ prev[0] + 1, prev[1], prev[2] }); // go "out right"
            for (int j = 1; j <= i; j++) {
                squares->push_back({ prev[0] + 1, prev[1] + j, prev[2] }); // go up
            }
            for (int j = 0; j < i; j++) {
                squares->push_back({ prev[0] - j, prev[1] + i, prev[2] }); // go left
            }
        }
        // on (i=2) and subsequent even iterations: go "out up" -> right -> down
        else {
            squares->push_back({ prev[0], prev[1] + 1, prev[2] }); // go "out up"
            for (int j = 1; j <= i; j++) {
                squares->push_back({ prev[0] + j, prev[1] + 1, prev[2] }); // go right
            }
            for (int j = 0; j < i; j++) {
                squares->push_back({ prev[0] + i, prev[1] - j, prev[2] }); // go down
            }
        }
    }
}
void get_data(std::vector<std::vector<float>>* squares, std::vector<float>* vertices, std::vector<int>* faces) {
}

int main(int argc, char* argv[])
{
    // Load input meshes
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
        /*0, 1, 2,
        0, 2, 3,*/
        4, 5, 2,
        5, 6, 2,
        7, 6, 9,
        6, 8, 9,
        3, 7, 11,
        7, 10, 11;
    U = V;
    S.setConstant(-1);
    // vertices in selection
    igl::colon<int>(0, V.rows() - 1, b);
    b.conservativeResize(std::stable_partition(b.data(), b.data() + b.size(),
        [](int i)->bool {return S(i) >= 0;}) - b.data());
    // Precomputation
    arap_data.max_iter = 100;
    arap_data.energy = igl::ARAP_ENERGY_TYPE_ELEMENTS;;
    igl::arap_precomputation(V, F, V.cols(), b, arap_data);

    igl::opengl::glfw::Viewer viewer;
    std::cout << R"(
        [click]  To place new control point
        [drag]   To move control point
        [space]  Toggle whether placing control points or deforming
    )";

    const auto& update = [&]()
    {
        // predefined colors
        const Eigen::RowVector3d orange(1.0, 0.7, 0.2);
        const Eigen::RowVector3d blue(0.2, 0.3, 0.8);
        if (placing_handles)
        {
            viewer.data().set_vertices(V); // rest mesh vertices
            viewer.data().set_colors(blue); // face color
            viewer.data().set_points(CV, orange); // rest control points
        }
        else
        {
            // SOLVE FOR ARAP DEFORMATION
            igl::arap_solve(CU, arap_data, U);
            viewer.data().set_vertices(U); // transformed mesh vertices
            viewer.data().set_colors(orange); // face color
            viewer.data().set_points(CU, blue); // transformed control points
        }
        viewer.data().compute_normals();
    };
    viewer.callback_mouse_down = [&](igl::opengl::glfw::Viewer&, int, int)->bool
    {
        last_mouse = Eigen::RowVector3f(
            viewer.current_mouse_x, viewer.core().viewport(3) - viewer.current_mouse_y, 0);
        if (placing_handles)
        {
            // Find closest point on mesh to mouse position
            int fid; // face index in V
            Eigen::Vector3f bary; // wrt fid face of V
            if (igl::unproject_onto_mesh(
                last_mouse.head(2),
                viewer.core().view,
                viewer.core().proj,
                viewer.core().viewport,
                V, F, fid, bary))
            {
                long c; // vertex index in face (0, 1, 2)
                bary.maxCoeff(&c);
                int vid = F(fid, c);
                S(vid) = 1;
                Eigen::RowVector3d new_c = V.row(vid); // x,y,z coords of vertex
                if (CV.size() == 0 || (CV.rowwise() - new_c).rowwise().norm().minCoeff() > 0)
                {
                    CV.conservativeResize(CV.rows() + 1, 3);
                    // Snap to closest vertex on hit face
                    CV.row(CV.rows() - 1) = new_c;
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
                Eigen::MatrixXf(CU.cast<float>()),
                viewer.core().view,
                viewer.core().proj, viewer.core().viewport, CP);
            Eigen::VectorXf D = (CP.rowwise() - last_mouse).rowwise().norm();
            sel = (D.minCoeff(&sel) < 30) ? sel : -1;
            if (sel != -1)
            {
                last_mouse(2) = CP(sel, 2);
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
            CU.row(sel) += (drag_scene - last_scene).cast<double>();
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
    viewer.callback_key_pressed = [&](igl::opengl::glfw::Viewer&, unsigned int key, int mod)
    {
        switch (key)
        {
        case ' ':
            placing_handles ^= 1;
            if (!placing_handles && CV.rows() > 0)
            {
                // Switching to deformation mode
                CU = CV;
                Eigen::VectorXi b;
                igl::snap_points(CV, V, b);
                // PRECOMPUTATION FOR DEFORMATION
                igl::arap_precomputation(V, F, V.cols(), b, arap_data);
            }
            break;
        default:
            return false;
        }
        update();
        return true;
    };
    viewer.callback_pre_draw = [&](igl::opengl::glfw::Viewer&)->bool
    {
        if (viewer.core().is_animating && !placing_handles)
        {
            igl::arap_solve(CU, arap_data, U);
            viewer.data().set_vertices(U);
            viewer.data().compute_normals();
            return false;
        }
        return false;
    };
    viewer.data().set_mesh(V, F);
    viewer.data().show_lines = true;
    viewer.core().is_animating = true;
    viewer.data().face_based = true;
    update();
    viewer.launch();
    /*std::vector<std::vector<float>> squares = { {0.f,0.f,0.f} };
    int n = 4;
    get_squares(n, &squares);
    std::cout << "squares.size(): " << squares.size() << std::endl;
    for (int i = 0; i < squares.size(); i++) {
        std::cout << i << ' ' << squares[i][0] << ' ' << squares[i][1] << ' ' << squares[i][2] << std::endl;
    }

    std::vector<float> vertices = {0.f, 0.f, 0.f,
                                   0.f, 1.f, 0.f,
                                   1.f, 1.f, 0.f,
                                   1.f, }*/
    return EXIT_SUCCESS;
}
