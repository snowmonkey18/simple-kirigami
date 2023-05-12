import numpy as np
n = 4

def display_array(array):
    print(*(array.flatten()), sep=', ')
    return array

def uniform_grid():
    x = np.arange(0.0, n+1, 1.0, dtype=float)
    y = np.arange(0.0, n+1, 1.0, dtype=float)
    xv, yv = np.meshgrid(x, y)
    assert(xv.shape == yv.shape)
    zv = np.zeros(xv.shape)
    grid = np.stack((yv, xv, zv), axis=2) # so indices are nice
    grid += np.random.uniform(-2.0, 2.0, grid.shape)
    return grid


def real_grid4(grid):
    grid = np.stack((grid, grid), axis=0)
    #np.delete(grid, 1, 0)
    return grid
    
#double_grid = 
print(display_array(real_grid4(uniform_grid())))

def _real_grid(grid): # with repeats
    ref = dict()
    # initialize
    for j in range(n):
        for i in range(n):
            ref[(j,i)] = (j*n+i, j*n+i + n*n)
    # fix boundaries
    for j in (0, n-1):
        for i in (0, n-1):
            ref[(j,i)] = (ref[(j,i)][0], ref[(j,i)][0])
            
    for j in (0, n-1):
        for i in range(1, n-1, 2):
            ref[(j,i)] = (ref[(j,i)][0], ref[(j,i)][0])

    for j in range(2, n-1, 2):
        for i in (0, n-1):
            ref[(j,i)] = (ref[(j,i)][0], ref[(j,i)][0])
            
            
    #inds = np.arange(0, grid.size/3, 1, dtype=int)
    #inds = np.stack((inds, inds), axis=1)
    #dict(inds)
    grid = np.concatenate((grid, grid))
    
    return ref


#print(real_grid4(uniform_grid()))

def quad_vertices(grid):
    
    return quad_vertices

def triangle_vertices(quad_vertices):
    return triangle_vertices

def test_vertices(vertices):
    return vertices == [0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 1.0, 0.0, 
        0.0, 1.0, 0.0, 
        1.0, 0.0, 0.0,
        2.0, 0.0, 0.0,
        2.0, 1.0, 0.0, 
        1.0, 1.0, 0.0, 
        2.0, 2.0, 0.0,
        1.0, 2.0, 0.0,
        1.0, 2.0, 0.0,
        0.0, 2.0, 0.0]
                        
