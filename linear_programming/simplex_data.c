typedef struct Tableu{
    void* z_vector;             // c-Transpose
    void* a_matrix;             // A
    void* b_vector;             // b
    void* basis_headers;        // c_B-T
    void* non_basis_headers;    // c_N-T
    void* basis_matrix;         // A_B
    void* non_basis_matrix;     // A_N

    void* reduces_costs;

}Tableu;