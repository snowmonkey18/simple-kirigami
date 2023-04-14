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
std::vector<float> get_quad_vertices(std::vector<float> bl_vertex) {
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
    std::vector<std::vector<float>> squares = { {0.f,0.f,0.f} };
    int n = 4;
    get_squares(n, &squares);
    std::cout << "squares.size(): " << squares.size() << std::endl;
    for (int i = 0; i < squares.size(); i++) {
        std::cout << i << ' ' << squares[i][0] << ' ' << squares[i][1] << ' ' << squares[i][2] << std::endl;
    }

    std::vector<float> vertices = {0.f, 0.f, 0.f, 
                                   0.f, 1.f, 0.f,
                                   1.f, 1.f, 0.f,
                                   1.f, }
    return EXIT_SUCCESS;
}
