#include <Eigen/StdVector>
#include <igl/arap.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/snap_points.h>
#include <igl/colon.h>
#include <algorithm>

int v_ = 40;
int f_ = 32;

Eigen::MatrixXd CV, CU; // rest and transformed control points
Eigen::MatrixXd V_(v_, 3), V(v_, 3), U(v_, 3); // rest and transformed vertices
Eigen::MatrixXi F(f_, 3);
Eigen::VectorXi S(v_), b;
bool placing_handles = true;
long sel = -1;
Eigen::RowVector3f last_mouse;
igl::ARAPData arap_data;

//void add_vertex(float x, float y, float z, std::vector<float>* vertices) {
//    vertices->push_back(x);
//    vertices->push_back(y);
//    vertices->push_back(z);
//}
//void get_quad_vertices(std::vector<float> bl_vertex) {
//    std::vector<float> vertices = { bl_vertex };
//    add_vertex(bl_vertex[0] + 1.f, bl_vertex[1], bl_vertex[2], &vertices);
//    add_vertex(bl_vertex[0], bl_vertex[1] + 1.f, bl_vertex[2], &vertices);
//    add_vertex(bl_vertex[0] + 1.f, bl_vertex[1] + 1.f, bl_vertex[2], &vertices);
//}
//void add_quad_vertices(std::vector<float> bl_vertex, std::vector<int> indices, std::vector<float>* vertices) {
//    for (int index : indices) {
//        vertices->push_back(0);
//    }
//}
//void get_squares(int n, std::vector<std::vector<float>>* squares) {
//    
//}
//void get_data(std::vector<std::vector<float>>* squares, std::vector<float>* vertices, std::vector<int>* faces) {
//}

int main(int argc, char* argv[])
{
    // Load input meshes
    V_ << 0.0, 0.0, 0.0, //0
        0.0, 1.0, 0.0,
        0.0, 2.0, 0.0,
        0.0, 3.0, 0.0,
        0.0, 4.0, 0.0,

        1.0, 0.0, 0.0, //5
        1.0, 1.0, 0.0,
        1.0, 2.0, 0.0,
        1.0, 3.0, 0.0,
        1.0, 4.0, 0.0,

        2.0, 0.0, 0.0, //10
        2.0, 1.0, 0.0,
        2.0, 2.0, 0.0,
        2.0, 3.0, 0.0,
        2.0, 4.0, 0.0,

        3.0, 0.0, 0.0, //15
        3.0, 1.0, 0.0,
        3.0, 2.0, 0.0,
        3.0, 3.0, 0.0,
        3.0, 4.0, 0.0,

        4.0, 0.0, 0.0, //20
        4.0, 1.0, 0.0,
        4.0, 2.0, 0.0,
        4.0, 3.0, 0.0,
        4.0, 4.0, 0.0,

        //0.0, 0.0, 0.0, 
        //0.0, 1.0, 0.0, 
        0.0, 2.0, 0.0, //25
        //0.0, 3.0, 0.0, 
        //0.0, 4.0, 0.0, 

        1.0, 0.0, 0.0, //26
        1.0, 1.0, 0.0,
        1.0, 2.0, 0.0,
        1.0, 3.0, 0.0,
        1.0, 4.0, 0.0,

        //2.0, 0.0, 0.0, 
        2.0, 1.0, 0.0, //31
        2.0, 2.0, 0.0,
        2.0, 3.0, 0.0,
        //2.0, 4.0, 0.0, 

        3.0, 0.0, 0.0, //34
        3.0, 1.0, 0.0,
        3.0, 2.0, 0.0,
        3.0, 3.0, 0.0,
        3.0, 4.0, 0.0,

        //4.0, 0.0, 0.0, 
        //4.0, 1.0, 0.0, 
        4.0, 2.0, 0.0; //39
        //4.0, 3.0, 0.0, 
        //4.0, 4.0, 0.0

    V<< -0.04424410632398201, 0.7360605652501859, 0.7101911935424106, 
        1.6694447137186184, 1.1276442750843145, 0.20240363331094313, 
        -1.7793720818775305, 0.4432333242597135, -1.5194107305460052, 
        -0.20770251459560107, 1.2705689221508076, -1.6215791650540239, 
        -1.2243473907444673, 4.31761401180085, -1.552032895423395, 
        
        -0.5351786244388181, 0.6378360411290545, 1.098609103234483, 
        2.0986115248454644, 2.182027243687119, 1.4764629353942644, 
        2.6824935654349518, 2.51211439215529, -1.0279559272937466, 
        1.3344728064718954, 2.6898213809164915, -1.1877592533269468, 
        1.269666928884197, 3.279206128175854, 0.9426165576856089, 
        
        2.796822673364784, 0.4849298885223243, -0.5411666298828268, 
        3.560375177060934, -0.7538287204469918, -0.6715997794831949, 
        3.537169821096846, 2.302957113256939, -1.2208623662428182, 
        1.9173215703232676, 1.1629440941290108, -1.2206021304201213, 
        0.8588220369601918, 4.301449277939312, 1.8885554919204814, 
        
        3.778815026906152, -1.922657336300876, 0.916743155178918, 
        1.7039783643486204, 2.1043721433015348, -1.706751866264836, 
        1.3735502359269463, 0.9298841517959677, 1.3061387927533508, 
        4.4928366080415465, 4.145282874711803, 1.6594217810943257, 
        1.2246765376903808, 4.579154686820638, 0.7536753747839322, 
        
        3.8522420750827813, 1.5610311868636266, -0.8584169568066788, 
        3.6430210591272596, 1.26443522281415, 1.5046934791346214, 
        4.517511744873926, 3.7722708395928257, 0.162936648471951, 
        3.241295658440135, 4.696159363782488, 1.3601374440068832, 
        3.3234455398248803, 4.060893426465757, -1.1156566457433246, 
        
        //-0.04424410632398201, 0.7360605652501859, 0.7101911935424106, 
        //1.6694447137186184, 1.1276442750843145, 0.20240363331094313, 
        -1.7793720818775305, 0.4432333242597135, -1.5194107305460052, 
        //-0.20770251459560107, 1.2705689221508076, -1.6215791650540239, 
        //-1.2243473907444673, 4.31761401180085, -1.552032895423395, 
        
        -0.5351786244388181, 0.6378360411290545, 1.098609103234483, 
        2.0986115248454644, 2.182027243687119, 1.4764629353942644, 
        2.6824935654349518, 2.51211439215529, -1.0279559272937466, 
        1.3344728064718954, 2.6898213809164915, -1.1877592533269468, 
        1.269666928884197, 3.279206128175854, 0.9426165576856089, 
        
        //2.796822673364784, 0.4849298885223243, -0.5411666298828268, 
        3.560375177060934, -0.7538287204469918, -0.6715997794831949, 
        3.537169821096846, 2.302957113256939, -1.2208623662428182, 
        1.9173215703232676, 1.1629440941290108, -1.2206021304201213, 
        //0.8588220369601918, 4.301449277939312, 1.8885554919204814, 
        
        3.778815026906152, -1.922657336300876, 0.916743155178918, 
        1.7039783643486204, 2.1043721433015348, -1.706751866264836, 
        1.3735502359269463, 0.9298841517959677, 1.3061387927533508, 
        4.4928366080415465, 4.145282874711803, 1.6594217810943257, 
        1.2246765376903808, 4.579154686820638, 0.7536753747839322, 
        
        //3.8522420750827813, 1.5610311868636266, -0.8584169568066788, 
        //3.6430210591272596, 1.26443522281415, 1.5046934791346214, 
        4.517511744873926, 3.7722708395928257, 0.162936648471951, 
        //3.241295658440135, 4.696159363782488, 1.3601374440068832, 
        //3.3234455398248803, 4.060893426465757, -1.1156566457433246

    F << 0, 5, 1, 1, 5, 6,
        1, 27, 2, 2, 27, 7,
        25, 7, 3, 3, 7, 8,
        3, 29, 4, 4, 29, 9,
        26, 10, 6, 6, 10, 11,
        27, 11, 28, 28, 11, 12,
        28, 32, 8, 8, 32, 13,
        29, 13, 30, 30, 13, 14,
        10, 15, 31, 31, 15, 16,
        31, 35, 12, 12, 35, 17,
        32, 17, 33, 33, 17, 18,
        33, 37, 14, 14, 37, 19,
        34, 20, 16, 16, 20, 21,
        35, 21, 36, 36, 21, 22,
        36, 39, 18, 18, 39, 23,
        37, 23, 38, 38, 23, 24;
        
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
    /*MatrixXd V(12, 3);
    for (int r = 0; r < 12; ++r) {
        for (int c = 0; c < 3; ++c) {
            V(r, c) = param(r * n + c);
        }
    }*/
    return EXIT_SUCCESS;
}
