#include <igl/opengl/glfw/Viewer.h>
#include <math.h>
//void add_quad_face(int i0, int i1, int i2, int i3, std::vector<int> *faces) {
//    faces->push_back(i0);
//    faces->push_back(i1);
//    faces->push_back(i3);
//    faces->push_back(i1);
//    faces->push_back(i2);
//    faces->push_back(i3);
//}
//void add_vertex(float x, float y, float z, std::vector<float>* vertices) {
//    vertices->push_back(x);
//    vertices->push_back(y);
//    vertices->push_back(z);
//}
//int get_vertex_index(int square_index, int corner_index) {
//    return -1;
//}
////void get_data(int n, std::vector<float> *vertices, std::vector<int> *faces) {
//void get_data(int n, std::vector<std::vector<float>> *squares) {
//    /*
//    * n [>= 1]: size of grid (for nxn grid of 1x1 squares)
//    * return: vector of coordinates of locations of squares (by bottom - left vertex)
//    */
//    // first square at origin
//    /*add_vertex(0.f, 0.f, 0.f, vertices);
//    add_vertex(1.f, 0.f, 0.f, vertices);
//    add_vertex(1.f, 1.f, 0.f, vertices);
//    add_vertex(0.f, 1.f, 0.f, vertices);
//    add_quad_face(0, 1, 2, 3, faces);*/
//    squares->push_back({ 0.f, 0.f, 0.f });
//    for (int i = 1; i < n; i++) {
//        std::vector<float> prev = (*squares)[squares->size() - 1];
//        // for (i=1) and subsequent odd iterations: go "out right" -> up -> left
//        if (i % 2 == 1) {
//            squares->push_back({ prev[0] + 1, prev[1], prev[2] }); // go "out right"
//            for (int j = 1; j <= i; j++) {
//                squares->push_back({ prev[0] + 1, prev[1] + j, prev[2] }); // go up
//            }
//            for (int j = 0; j < i; j++) {
//                squares->push_back({ prev[0] - j, prev[1] + i, prev[2] }); // go left
//            }
//        }
//        // on (i=2) and subsequent even iterations: go "out up" -> right -> down
//        else {
//            squares->push_back({ prev[0], prev[1] + 1, prev[2] }); // go "out up"
//            for (int j = 1; j <= i; j++) {
//                squares->push_back({ prev[0] + j, prev[1] + 1, prev[2] }); // go right
//            }
//            for (int j = 0; j < i; j++) {
//                squares->push_back({ prev[0] + i, prev[1] - j, prev[2] }); // go down
//            }
//        }
//    }
//}
//
//void squares_to_data(std::vector<std::vector<float>> *squares, std::vector<std::vector<float>> *vertices, std::vector<std::vector<int>> *faces) {
//    for (int i = 0; i < squares->size(); i++) {
//        float x = (*squares)[i][0];
//        float y = (*squares)[i][1];
//        float z = (*squares)[i][2];
//        std::vector<float> v0;
//        std::vector<float> v1;
//        std::vector<float> v2;
//        std::vector<float> v3;
//
//        // origin square has all new vertices
//        // 
//        v0 = { x, y, z };
//        v1 = { x + 1, y, z };
//        v2 = { x + 1, y + 1, z };
//        v3 = { x, y + 1, z };
//
//
//        // 1st triangle
//        vertices->push_back(v0);
//        vertices->push_back(v1);
//        vertices->push_back(v3);
//        // 2nd triangle
//        vertices->push_back(v1);
//        vertices->push_back(v2);
//        vertices->push_back(v3);
//    }
//}

int main(int argc, char* argv[])
{
    //// mesh [old]
    //const Eigen::MatrixXd V = (Eigen::MatrixXd(12, 3) <<
    //    -1.0, 1.0, 0.0,//0
    //    0.0, 1.0, 0.0,//1
    //    1.0, 1.0, 0.0,//2
    //    -1.0, 0.0, 0.0,//3
    //    0.0, 0.0, 0.0,//4
    //    0.0, 0.0, 0.0,//5
    //    1.0, 0.0, 0.0,//6
    //    -1.0, 0.0, 0.0,//7
    //    1.0, 0.0, 0.0,//8
    //    -1.0, -1.0, 0.0,//9
    //    0.0, -1.0, 0.0,//10
    //    1.0, -1.0, 0.0//11
    //    ).finished();

    //const Eigen::MatrixXi F = (Eigen::MatrixXi(8, 3) <<
    //    0, 4, 1,
    //    0, 3, 4,
    //    1, 6, 2,
    //    1, 5, 6,
    //    5, 11, 8,
    //    5, 10, 11,
    //    7, 10, 4,
    //    7, 9, 10).finished();

    // mesh [new]: nxn grid of 1x1 squares
    /*int n = 1;
    std::vector<std::vector<float>> squares;
    std::vector<std::vector<float>> vertices;
    std::vector<std::vector<int>> faces;
    get_squares(n, &squares);
    squares_to_data(&squares, &vertices, &faces);*/

    /*Eigen::VectorXf v0 = (Eigen::VectorXf(3) << 
        0, 0, 0).finished();
    Eigen::VectorXf v1 = (Eigen::VectorXf(3) <<
        1, 0, 0).finished();
    Eigen::VectorXf v2 = (Eigen::VectorXf(3) <<
        1, 1, 0).finished();
    Eigen::VectorXf v3 = (Eigen::VectorXf(3) <<
        0, 1, 0).finished();*/
 

//    std::vector<std::vector<float>> pre_V;
//    std::vector<std::vector<int>> pre_F;
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
//    
//    // show
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    viewer.data().set_face_based(true);
    viewer.launch();
}
