#include <igl/opengl/glfw/Viewer.h>

void get_squares(int n, std::vector<std::vector<float>> *squares) {
    /*
    * n [>= 1]: size of grid (for nxn grid of 1x1 squares)
    * return: vector of coordinates of locations of squares (by bottom - left vertex)
    */
    std::vector<std::vector<float>> squares(1, { 0.f, 0.f, 0.f }); // first square at origin
    for (int i = 1; i < n; i++) {
        std::vector<float> prev = (*squares)[squares->size() - 1];
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

void squares_to_data(std::vector<std::vector<float>> *squares, std::vector<std::vector<float>> *vertices, std::vector<std::vector<int>> *faces) {
    std::vector<std::vector<float>> vertices;
    for (int i = 0; i < squares->size(); i++) {
        float x = (*squares)[i][0];
        float y = (*squares)[i][1];
        float z = (*squares)[i][2];
        std::vector<float> v0;
        std::vector<float> v1;
        std::vector<float> v2;
        std::vector<float> v3;

        // origin square has all new vertices
        // 
        v0 = { x, y, z };
        v1 = { x + 1, y, z };
        v2 = { x + 1, y + 1, z };
        v3 = { x, y + 1, z };


        // 1st triangle
        vertices->push_back(v0);
        vertices->push_back(v1);
        vertices->push_back(v3);
        // 2nd triangle
        vertices->push_back(v1);
        vertices->push_back(v2);
        vertices->push_back(v3);
    }
}

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
    int n = 1;
    std::vector<std::vector<float>> squares;
    std::vector<std::vector<float>> vertices;
    std::vector<std::vector<int>> faces;
    get_squares(n, &squares);
    squares_to_data(&squares, &vertices, &faces);

//    std::vector<std::vector<float>> pre_V;
//    std::vector<std::vector<int>> pre_F;
//    const Eigen::MatrixXd V(12, 3);
//    const Eigen::MatrixXi F(8, 3);
//    
//    // show
//    igl::opengl::glfw::Viewer viewer;
//    viewer.data().set_mesh(V, F);
//    viewer.data().set_face_based(true);
//    viewer.launch();
}
